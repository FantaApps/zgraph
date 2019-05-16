#ifndef _ODPS_GRAPH_ALGO_
#define _ODPS_GRAPH_ALGO_

#include <memory>
#include <stack>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <omp.h>
#include "bitset.h"
#include "graph.h"

namespace apsara {
namespace odps {
namespace graph {
namespace query {

template <typename VType>
class GraphAlgo
{
public:
    GraphAlgo(const std::shared_ptr<Graph<VType>> &g)
    { 
        mG = g; 
        size_t sizeBitSet = g->GetNumV();
        mVisited = std::shared_ptr<BitSet>(new BitSet(sizeBitSet));
        mOddFrontier = std::shared_ptr<BitSet>(new BitSet(sizeBitSet));
        mEvenFrontier = std::shared_ptr<BitSet>(new BitSet(sizeBitSet));
    }
    virtual ~GraphAlgo() {}

    void SetNumThreads(int numThreads) 
    { 
        mNumThreads = numThreads;
        mVisited->SetNumThreads(numThreads);
        mOddFrontier->SetNumThreads(numThreads);
        mEvenFrontier->SetNumThreads(numThreads);
    }

    size_t BFS(const VType &vQuery);
   
    void StronglyCC(const std::shared_ptr<Graph<VType>> &g,
                    std::map<VType, std::vector<VType>> &sCC); 

    void Visit(const VType                         &v,
               VType                               &index,
               std::vector<VType>                  &vIndex,
               std::vector<VType>                  &vLowLink,
               std::vector<bool>                   &vOnStack,
               std::stack<VType>                   &S,
               const std::shared_ptr<Graph<VType>> &g,
               std::map<VType, std::vector<VType>> &sCC);

private:
    void TopDownStep(std::shared_ptr<BitSet> &first,
                     std::shared_ptr<BitSet> &second);
    void BottomUpStep(std::shared_ptr<BitSet> &first,
                      std::shared_ptr<BitSet> &second);
    size_t GetNumEdgesToCheck(const std::shared_ptr<BitSet> &set,
                              bool                           testSet);

private:
    int                            mNumThreads;
    std::shared_ptr<Graph<VType>>  mG;
    std::shared_ptr<BitSet>        mVisited;
    std::shared_ptr<BitSet>        mOddFrontier;
    std::shared_ptr<BitSet>        mEvenFrontier;
};

template <typename VType>
size_t GraphAlgo<VType>::BFS(const VType &vQuery)
{
    int alpha = 12;
    mVisited->ReSet();
    mOddFrontier->ReSet();
    mEvenFrontier->ReSet();
    mOddFrontier->Set(vQuery);
    int step = 0;

    while((step %2 == 0 && mOddFrontier->Any()) ||
          (step %2 == 1 && mEvenFrontier->Any()) )
    {
        if(step %2 == 0)
        {
            if(GetNumEdgesToCheck(mOddFrontier, true) * alpha < 
               GetNumEdgesToCheck(mVisited, false))
            {
                TopDownStep(mOddFrontier, mEvenFrontier);
            }
            else
            {
                BottomUpStep(mOddFrontier, mEvenFrontier);
            }
        }
        if(step %2 == 1)
        {
            if(GetNumEdgesToCheck(mEvenFrontier, true) * alpha < 
               GetNumEdgesToCheck(mVisited, false))
            {
                TopDownStep(mEvenFrontier, mOddFrontier);
            }
            else
            {
                BottomUpStep(mEvenFrontier, mOddFrontier);
            }
        }
        step++;
    }

    return mVisited->IsSet(vQuery) ? mVisited->Count() - 1 :
                                     mVisited->Count();
}

template <typename VType>
size_t GraphAlgo<VType>::GetNumEdgesToCheck(const std::shared_ptr<BitSet> &set,
                                            bool                           testSet)
{
    size_t ret = 0;
    size_t i;
#pragma omp parallel for num_threads(mNumThreads) private(i) reduction(+:ret)
    for (i=0; i<mG->GetNumV(); ++i)
    {
        if(testSet && set->IsSet(i)) 
        {
            ret += mG->GetDegree(i);
        }
        else if(!testSet && !set->IsSet(i)) 
        {
            ret += mG->GetRevDegree(i);
        }
    }
    return ret;
}

template <typename VType>
void GraphAlgo<VType>::TopDownStep(std::shared_ptr<BitSet> &first,
                                   std::shared_ptr<BitSet> &second)
{
    second->ReSet();
    size_t i;
#pragma omp parallel for num_threads(mNumThreads) private(i)
    for(i=0; i<first->GetBuckets(); i++)
    {
        if(first->Test(i))
        {
            for(VType j=i*8; j<(i+1)*8; j++)
            {
                if(first->IsSet(j))
                {
                    size_t from, to;
                    std::shared_ptr<VType> &edges = mG->GetEdges(j, from, to); 
                    for(size_t k=from; k<to; k++)
                    {
                        VType edge = edges.get()[k];
                        if(!mVisited->IsSet(edge))
                        {
                            mVisited->Set(edge);
                            second->Set(edge);
                        }
                    }
                }
            }
        }
    }
}

template <typename VType>
void GraphAlgo<VType>::BottomUpStep(std::shared_ptr<BitSet> &first,
                                    std::shared_ptr<BitSet> &second)
{
    second->ReSet();
    VType i;
#pragma omp parallel for num_threads(mNumThreads) private(i)
    for(i=0; i<mVisited->GetSize(); i++)
    {
        if(!mVisited->IsSet(i))
        {
            size_t from, to;
            std::shared_ptr<VType> &edges = mG->GetRevEdges(i, from, to); 
            for(size_t k=from; k<to; k++)
            {
                VType edge = edges.get()[k];
                if(first->IsSet(edge))
                {
                    mVisited->Set(i);
                    second->Set(i);
                    break;
                }
            }
        }
    }
}

template <typename VType>
void GraphAlgo<VType>::StronglyCC(const std::shared_ptr<Graph<VType>> &g,
                                  std::map<VType, std::vector<VType>> &sCC)
{
    std::stack<VType> S;
    VType index;
    std::vector<VType> vIndex; 
    std::vector<VType> vLowLink;
    std::vector<bool>  vOnStack;
    std::vector<VType> vertices = g->GetAllVertices();

    vIndex.resize(vertices.size()+1);
    vLowLink.resize(vertices.size()+1);
    vOnStack.resize(vertices.size()+1);

    size_t i;
//#pragma omp parallel for num_threads(mNumThreads) private(i)
    for(i=0; i<vertices.size(); ++i)
    {
        vIndex[i] = std::numeric_limits<VType>::max();
        vLowLink[i] = std::numeric_limits<VType>::max();
        vOnStack[i] = false; 
    }

    for(size_t i=0; i<vertices.size(); ++i)
    {
        if(vIndex[i] == std::numeric_limits<VType>::max())
            Visit(i, index, vIndex, vLowLink, vOnStack, S, g, sCC);
    }

    std::cout<<"Number of vertices: "<<vertices.size()<<" number of strongly CC: "<<sCC.size()<<std::endl;
}

template <typename VType>
void GraphAlgo<VType>::Visit(const VType                         &v,
                             VType                               &index,
                             std::vector<VType>                  &vIndex,
                             std::vector<VType>                  &vLowLink,
                             std::vector<bool>                   &vOnStack,
                             std::stack<VType>                   &S,
                             const std::shared_ptr<Graph<VType>> &g,
                             std::map<VType, std::vector<VType>> &sCC)
{
    vIndex[v] = index;
    vLowLink[v] = index;
    index++;
    S.push(v);
    vOnStack[v] = true;


    size_t from, to;
    std::shared_ptr<VType> edges = g->GetEdges(v, from, to);
    for(size_t j=from; j<to; j++)
    {
        VType w = edges.get()[j];
        if(vIndex[w] == std::numeric_limits<VType>::max())
        {
            Visit(w, index, vIndex, vLowLink, vOnStack, S, g, sCC); 
            vLowLink[v] = std::min(vLowLink[v], vLowLink[w]);
        }
        else
        {
            vLowLink[v] = std::min(vLowLink[v], vIndex[w]);
        }
    }

    if(vLowLink[v] == vIndex[v])
    {
        VType w=v;
        while(!S.empty()) 
        {
            w = S.top(); S.pop();
            vOnStack[w] = false;
            sCC[v].push_back(w);
            if(v == w) break;
        }
    }
}

} // namespace query
} // namespace graph
} // namespace odps
} // namespace apsara

#endif

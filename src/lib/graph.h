#ifndef _ODPS_GRAPH_
#define _ODPS_GRAPH_

#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <omp.h>
#include <cstddef>
#include "graph_tool.h"

namespace apsara {
namespace odps {
namespace graph {
namespace query {

template<typename VType>
class Graph
{
public: 
    Graph() {}
    virtual ~Graph() {}
    virtual void Init(const std::string &folder) = 0;

    virtual std::shared_ptr<VType>& GetEdges(const VType &vertex,
                                             size_t      &from,
                                             size_t      &to) = 0;

    virtual std::shared_ptr<VType>& GetRevEdges(const VType &vertex,
                                                size_t      &from,
                                                size_t      &to) = 0;

    //virtual void GetCommonEdges(const std::vector<VType> &vertices,
    //                            const std::vector<VType> &common) = 0;

    //virtual void GetCommonRevEdges(const std::vector<VType> &vertices,
    //                               const std::vector<VType> &common) = 0;

    virtual std::vector<VType> GetAllVertices() = 0;

    virtual size_t GetDegree(const VType &vertex) = 0;

    virtual size_t GetRevDegree(const VType &vertex) = 0;

    virtual size_t GetNumV() = 0;

    virtual size_t GetNumE() = 0; 

protected:
    VType mNumE;
    VType mNumV;
};

template<typename VType>
class GraphAdj : public Graph<VType> 
{
public:
    GraphAdj() : mIsInitialized(false) {}
    ~GraphAdj() {}
    virtual void Init(const std::string &folder);
    virtual std::shared_ptr<VType>& GetEdges(const VType &vertex, size_t &from, size_t &to);
    virtual std::shared_ptr<VType>& GetRevEdges(const VType &vertex, size_t &from, size_t &to);
    virtual std::vector<VType> GetAllVertices();
    virtual size_t GetDegree(const VType &vertex);
    virtual size_t GetRevDegree(const VType &vertex);
    virtual size_t GetNumV() { return mNumV; };
    virtual size_t GetNumE() { return mNumE; }; 

private:
    std::map<VType, std::shared_ptr<VType>> mGraph;    
    std::map<VType, std::shared_ptr<VType>> mRevGraph;    
    VType mNumE;
    VType mNumV;
    bool mIsInitialized;
    std::shared_ptr<VType> mNull;
};

template<typename VType>
class GraphCSR : public Graph<VType> 
{
public:
    GraphCSR() : mIsInitialized(false) {}
    ~GraphCSR() {}
    virtual void Init(const std::string &folder);
    virtual std::shared_ptr<VType>& GetEdges(const VType &vertex, size_t &from, size_t &to);
    virtual std::shared_ptr<VType>& GetRevEdges(const VType &vertex, size_t &from, size_t &to);
    virtual std::vector<VType> GetAllVertices();
    virtual size_t GetDegree(const VType &vertex);
    virtual size_t GetRevDegree(const VType &vertex);
    virtual size_t GetNumV() { return mNumV; };
    virtual size_t GetNumE() { return mNumE; }; 

private:
    VType ReadOneFile(const std::string &file, std::shared_ptr<VType> &data);
 
private:
    std::shared_ptr<VType> mVIdx;
    std::shared_ptr<VType> mEIdx;
    std::shared_ptr<VType> mRevVIdx;
    std::shared_ptr<VType> mRevEIdx;
    VType mNumE;
    VType mNumV;
    bool mIsInitialized;
};

template<typename VType>
void GraphAdj<VType>::Init(const std::string &folder)
{
    if(mIsInitialized) return;
    double start = omp_get_wtime();
#pragma omp parallel num_threads(2) 
    {
        int tid = omp_get_thread_num();
        if(tid == 0)
        {
            GraphTool<VType>::ReadOneFile(folder+"/graph.bin", folder+"/graphLen.bin", mGraph);
        }

        if(tid == 1)
        {
            GraphTool<VType>::ReadOneFile(folder+"/revGraph.bin", folder+"/revGraphLen.bin", mRevGraph);
        }
    }

    double end = omp_get_wtime();
    mNumV = 0;
    for(auto iter = mGraph.begin(); iter != mGraph.end(); ++iter)
    {
        mNumV = std::max(mNumV, iter->first+1);
        mNumE += iter->second.get()[0];
    }
    std::cout<<"Graph load time:"<<end-start<<" numV: "<<mNumV<<" numE: "<<mNumE<<std::endl;
    mIsInitialized = true;
}

template<typename VType>
std::shared_ptr<VType>& GraphAdj<VType>::GetEdges(const VType &vertex, size_t &from, size_t &to)
{
    from =0; to = 0;
    auto iter = mGraph.find(vertex);
    if(iter == mGraph.end()) return mNull; 
    from = 1;
    to = iter->second.get()[0] + 1;
    return iter->second; 
}

template<typename VType>
std::shared_ptr<VType>& GraphAdj<VType>::GetRevEdges(const VType &vertex, size_t &from, size_t &to)
{
    from =0; to = 0;
    auto iter = mRevGraph.find(vertex);
    if(iter == mRevGraph.end()) return mNull; 
    from = 1;
    to = iter->second.get()[0] + 1;
    return iter->second; 
}

template<typename VType>
std::vector<VType> GraphAdj<VType>::GetAllVertices()
{
    std::vector<VType> ret;
    for(auto iter = mGraph.begin(); iter != mGraph.end(); ++iter)
        ret.push_back(iter->first);
    return ret;
}

template<typename VType>
size_t GraphAdj<VType>::GetDegree(const VType &vertex)
{
    auto iter = mGraph.find(vertex);
    if(iter == mGraph.end()) return 0;
    return (size_t)mGraph.find(vertex)->second.get()[0];
}

template<typename VType>
size_t GraphAdj<VType>::GetRevDegree(const VType &vertex)
{
    auto iter = mRevGraph.find(vertex);
    if(iter == mRevGraph.end()) return 0;
    return (size_t)mRevGraph.find(vertex)->second.get()[0];
}


template<typename VType>
void GraphCSR<VType>::Init(const std::string &folder)
{
    if(mIsInitialized) return;
    double start = omp_get_wtime();
#pragma omp parallel num_threads(4) 
    {
        int tid = omp_get_thread_num();
        if(tid == 0)
        {
            mNumV = GraphTool<VType>::ReadOneFile(folder+"/vIdx.bin", mVIdx);
        }

        if(tid == 1)
        {
            mNumE = GraphTool<VType>::ReadOneFile(folder+"/eIdx.bin", mEIdx);
        }

        if(tid == 2)
        {
            mNumV = GraphTool<VType>::ReadOneFile(folder+"/revVIdx.bin", mRevVIdx);
        }

        if(tid == 3)
        {
            mNumE = GraphTool<VType>::ReadOneFile(folder+"/revEIdx.bin", mRevEIdx);
        }
    }
    double end = omp_get_wtime();
    std::cout<<"Graph load time:"<<end-start<<" numV: "<<mNumV<<" numE: "<<mNumE<<std::endl;
    mIsInitialized = true;
}



template<typename VType>
std::shared_ptr<VType>& GraphCSR<VType>::GetEdges(const VType &vertex, size_t &from, size_t &to)
{
    from = vertex == 0 ? 0 : mVIdx.get()[vertex-1]; 
    to = mVIdx.get()[vertex];
    return mEIdx;
}

template<typename VType>
std::shared_ptr<VType>& GraphCSR<VType>::GetRevEdges(const VType &vertex, size_t &from, size_t &to)
{
    from = vertex == 0 ? 0 : mRevVIdx.get()[vertex-1]; 
    to = mRevVIdx.get()[vertex];
    return mRevEIdx;
}

template<typename VType>
std::vector<VType> GraphCSR<VType>::GetAllVertices()
{
    std::vector<VType> ret;
    for(size_t i=0; i<mNumV; i++)
        ret.push_back(mVIdx.get()[i]);
    return ret;
}

template<typename VType>
size_t GraphCSR<VType>::GetDegree(const VType &vertex)
{
    VType from = vertex == 0 ? 0 : mVIdx.get()[vertex-1]; 
    VType to = mVIdx.get()[vertex];
    return (size_t)(to-from);
}

template<typename VType>
size_t GraphCSR<VType>::GetRevDegree(const VType &vertex)
{
    VType from = vertex == 0 ? 0 : mRevVIdx.get()[vertex-1]; 
    VType to = mRevVIdx.get()[vertex];
    return (size_t)(to-from);
}

} // namespace query
} // namespace graph
} // namespace odps
} // namespace apsara

#endif

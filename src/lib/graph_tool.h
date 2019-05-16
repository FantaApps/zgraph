#ifndef _GRAPH_TOOL_H
#define _GRAPH_TOOL_H

#include <omp.h>
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <limits>
#include <cstdint>

namespace apsara {
namespace odps {
namespace graph {
namespace query {

template <typename VType>
class GraphTool 
{
public:
    GraphTool() {}
    virtual ~GraphTool() {}

    void PrepareCSR(const std::string &fileList, const std::string &csrFolder);
    void PrepareAdj(const std::string &fileList, const std::string &adjFolder);

    void CheckSumOrig(const std::string &fileList);

    void CheckSumCSR(const std::string &folder);

    void CheckSumAdj(const std::string &folder);

    void FillCSR(const std::map<VType, std::vector<VType>> &graph,
                 const size_t                              &numV,
                 std::shared_ptr<VType>                    &vIdx,
                 std::shared_ptr<VType>                    &eIdx);

    static size_t ReadOneFile(const std::string      &file,
                              std::shared_ptr<VType> &data);

    static void ReadOneFile(const std::string                       &file,
                            const std::string                       &fileLen,
                            std::map<VType, std::shared_ptr<VType>> &graph);

private:
    void WriteOneFile(const std::string      &file,
                      std::shared_ptr<VType> &data,
                      const size_t           &len);

    void WriteOneFile(const std::string                   &file,
                      const std::string                   &fileLen,
                      std::map<VType, std::vector<VType>> &graph);

};

template<typename VType>
void GraphTool<VType>::PrepareCSR(const std::string &fileList,
                                  const std::string &csrFolder)
{
    VType maxV = 0;
    VType minV = std::numeric_limits<VType>::max();

    std::shared_ptr<VType>         vIdx;
    std::shared_ptr<VType>         eIdx;
    std::shared_ptr<VType>         revVIdx;
    std::shared_ptr<VType>         revEIdx;
    std::map<VType, std::vector<VType>> graph;
    std::map<VType, std::vector<VType>> revGraph;

    std::ifstream ifs(fileList);
    std::string file;

    size_t numV=0;
    size_t numE=0;

    while(getline(ifs, file))
    { 
        std::cout<<"Process: "<<file<<std::endl;
        std::ifstream ifs1(file);
        std::string line;
        while(getline(ifs1, line))
        {
            std::stringstream ss(line);
            VType from, to;
            ss>>from;
            ss>>to;
            maxV = std::max((VType)std::max(from,to), maxV);
            minV = std::min((VType)std::min(from,to), minV);
            graph[from].push_back(to);
            revGraph[to].push_back(from);
            numE++;
        }
    }

    for(auto iter = graph.begin(); iter != graph.end(); ++iter)
        sort(iter->second.begin(), iter->second.end());

    for(auto iter = revGraph.begin(); iter != revGraph.end(); ++iter)
        sort(iter->second.begin(), iter->second.end());

    numV = (maxV-minV) + 1;

    std::cout<<"numV: "<<numV<<" numE: "<<numE<<std::endl;

    vIdx    = std::shared_ptr<VType>(new VType[numV]);
    eIdx    = std::shared_ptr<VType>(new VType[numE+1]); 

    revVIdx = std::shared_ptr<VType>(new VType[numV]); 
    revEIdx = std::shared_ptr<VType>(new VType[numE+1]); 


    FillCSR(graph,    numV, vIdx,    eIdx);
    FillCSR(revGraph, numV, revVIdx, revEIdx);

    WriteOneFile(csrFolder+"/vIdx.bin",    vIdx,    (size_t)numV*sizeof(VType));
    WriteOneFile(csrFolder+"/eIdx.bin",    eIdx,    (size_t)numE*sizeof(VType));
    WriteOneFile(csrFolder+"/revVIdx.bin", revVIdx, (size_t)numV*sizeof(VType));
    WriteOneFile(csrFolder+"/revEIdx.bin", revEIdx, (size_t)numE*sizeof(VType));
}

template<typename VType>
void GraphTool<VType>::FillCSR(const std::map<VType, std::vector<VType>> &graph,
                               const size_t                              &numV,
                               std::shared_ptr<VType>                    &vIdx,
                               std::shared_ptr<VType>                    &eIdx)
{
    VType sum=0;
    for(VType i=0; i<numV; i++)
    {
        VType from = sum;
        auto iter = graph.find(i);
        if(iter == graph.end())
        {
            vIdx.get()[i] = sum;
            continue;
        }
        VType to = sum+iter->second.size();
        
        sum += iter->second.size();
        vIdx.get()[i] = sum;

        for(VType j=from, k=0; j<to; j++, k++)
        {
            iter->second[k];
            eIdx.get()[j] = iter->second[k];
        }
    }
}

template<typename VType>
void GraphTool<VType>::WriteOneFile(const std::string      &file,
                                    std::shared_ptr<VType> &data,
                                    const size_t           &len)
{
    std::ofstream of(file);
    char *buf = (char*)(data.get());
    of.write(buf, len);
    of.close();
}

template<typename VType>
void GraphTool<VType>::PrepareAdj(const std::string &fileList,
                                  const std::string &adjFolder)
{
    VType maxV = 0;
    VType minV = std::numeric_limits<VType>::max();

    std::map<VType, std::vector<VType>> graph;
    std::map<VType, std::vector<VType>> revGraph;

    std::ifstream ifs(fileList);
    std::string file;

    size_t numV=0;
    size_t numE=0;

    while(getline(ifs, file))
    { 
        std::cout<<"Process: "<<file<<std::endl;
        std::ifstream ifs1(file);
        std::string line;
        while(getline(ifs1, line))
        {
            std::stringstream ss(line);
            VType from, to;
            ss>>from;
            ss>>to;
            maxV = std::max((VType)std::max(from,to), maxV);
            minV = std::min((VType)std::min(from,to), minV);
            graph[from].push_back(to);
            revGraph[to].push_back(from);
            numE++;
        }
    }

    for(auto iter = graph.begin(); iter != graph.end(); ++iter)
        sort(iter->second.begin(), iter->second.end());

    for(auto iter = revGraph.begin(); iter != revGraph.end(); ++iter)
        sort(iter->second.begin(), iter->second.end());

    numV = (maxV-minV)+1;

    std::cout<<"numV: "<<numV<<" numE: "<<numE<<std::endl;

    WriteOneFile(adjFolder+"/graph.bin", adjFolder+"/graphLen.bin", graph);
    WriteOneFile(adjFolder+"/revGraph.bin", adjFolder+"/revGraphLen.bin", revGraph);
}

template<typename VType>
void GraphTool<VType>::WriteOneFile(const std::string                   &file,
                                    const std::string                   &fileLen,
                                    std::map<VType, std::vector<VType>> &graph)
{
    std::ofstream of(file);
    std::ofstream ofLen(fileLen);

    size_t *dataLen = new size_t[graph.size()];

    size_t count = 0;
    for(auto iter = graph.begin(); iter != graph.end(); ++iter)
    {
        size_t len = iter->second.size()+1;
        VType *data = new VType[len];
        data[0] = iter->first;
        for(size_t i=0; i<iter->second.size(); ++i)
            data[i+1] = iter->second[i];
        of.write((char*)data, len*sizeof(VType));

        dataLen[count++] = len*sizeof(VType);
    }

    ofLen.write((char*)dataLen, graph.size()*sizeof(size_t));

    of.close();
    ofLen.close();
}

template<typename VType>
void GraphTool<VType>::CheckSumOrig(const std::string &fileList)
{
    size_t sumFrom=0;
    size_t sumTo=0;

    std::ifstream ifs(fileList);
    std::string file;

    while(getline(ifs, file))
    {
        std::cout<<"Process: "<<file<<std::endl;
        std::ifstream ifs1(file);
        std::string line;
        while(getline(ifs1, line))
        {
            std::stringstream ss(line);
            VType from, to;
            ss>>from;
            ss>>to;
            sumFrom += from;
            sumTo += to;
        }
    }
    
    std::cout<<"Sum from:"<<sumFrom<<" Sum to:"<<sumTo<<std::endl;
}

template<typename VType>
void GraphTool<VType>::CheckSumCSR(const std::string &folder)
{
    size_t sumFrom=0;
    size_t sumTo=0;

    VType                  numE;
    VType                  numV;

    std::shared_ptr<VType> vIdx;
    std::shared_ptr<VType> eIdx;
    std::shared_ptr<VType> revVIdx;
    std::shared_ptr<VType> revEIdx;

#pragma omp parallel num_threads(4) 
    {
        int tid = omp_get_thread_num();
        if(tid == 0)
        {
            numV = GraphTool<VType>::ReadOneFile(folder+"/vIdx.bin", vIdx);
        }

        if(tid == 1)
        {
            numE = GraphTool<VType>::ReadOneFile(folder+"/eIdx.bin", eIdx);
        }

        if(tid == 2)
        {
            numV = GraphTool<VType>::ReadOneFile(folder+"/revVIdx.bin", revVIdx);
        }

        if(tid == 3)
        {
            numE = GraphTool<VType>::ReadOneFile(folder+"/revEIdx.bin", revEIdx);
        }
    }

    std::cout<<"numV: "<<numV<<" numE: "<<numE<<std::endl;

    for(size_t i=0; i<numV; i++)
    {
        uint64_t from = i==0 ? 0 :vIdx.get()[i-1];
        uint64_t to = vIdx.get()[i];
        sumFrom += (to-from) * i;
        for(size_t j=from; j<to; j++)
        {
            sumTo += eIdx.get()[j];
        }
    }

    std::cout<<"Sum from:"<<sumFrom<<" Sum to:"<<sumTo<<std::endl;

    sumFrom=0;
    sumTo=0;

    for(size_t i=0; i<numV; i++)
    {
        uint64_t from = i==0 ? 0 :revVIdx.get()[i-1];
        uint64_t to = revVIdx.get()[i];
        sumFrom += (to-from) * i;
        for(size_t j=from; j<to; j++)
        {
            sumTo += revEIdx.get()[j];
        }
    }

    std::cout<<"Sum from:"<<sumFrom<<" Sum to:"<<sumTo<<std::endl;
}

template<typename VType>
size_t GraphTool<VType>::ReadOneFile(const std::string      &file,
                                     std::shared_ptr<VType> &data)
{
    std::ifstream if1(file);

    if1.seekg (0, if1.end);
    size_t length = if1.tellg();
    if1.seekg (0, if1.beg);

    char *buf;
    { // parallel malloc will hurdle performance
        #pragma omp critical
        buf = new char[length];
    }

    if1.read(buf, length);
    data = std::shared_ptr<VType>((VType*)buf);

    return length/sizeof(VType);
}

template<typename VType>
void GraphTool<VType>::CheckSumAdj(const std::string &folder)
{
    size_t sumFrom=0;
    size_t sumTo=0;

    std::map<VType, std::shared_ptr<VType>> graph;
    std::map<VType, std::shared_ptr<VType>> revGraph;

    ReadOneFile(folder+"/graph.bin", folder+"/graphLen.bin", graph);
    ReadOneFile(folder+"/revGraph.bin", folder+"/revGraphLen.bin", revGraph);

    for(auto iter = graph.begin(); iter != graph.end(); ++iter)
    {
        VType num = iter->second.get()[0];
        sumFrom += iter->first * num;
        for(size_t i=1; i<=num; i++)
        {
            sumTo += iter->second.get()[i];
        }
    }

    std::cout<<"sumFrom: "<<sumFrom<<" sumTo: "<<sumTo<<std::endl;

    sumFrom = 0;
    sumTo   = 0;

    for(auto iter = revGraph.begin(); iter != revGraph.end(); ++iter)
    {
        VType num = iter->second.get()[0];
        sumFrom += iter->first * num;
        for(size_t i=1; i<=num; i++)
            sumTo += iter->second.get()[i];
    }

    std::cout<<"sumFrom: "<<sumFrom<<" sumTo: "<<sumTo<<std::endl;

}

template<typename VType>
void GraphTool<VType>::ReadOneFile(const std::string                       &file,
                                   const std::string                       &fileLen,
                                   std::map<VType, std::shared_ptr<VType>> &graph)
{
    std::ifstream ifs(file);
    std::ifstream ifsLen(fileLen);

    ifsLen.seekg (0, ifsLen.end);
    size_t length = ifsLen.tellg();
    ifsLen.seekg (0, ifsLen.beg);

    char *buf = new char[length];
    ifsLen.read(buf, length);
    ifsLen.close();
    

    size_t *lens = (size_t*)buf;
    for(size_t i=0; i<length/sizeof(size_t); i++) 
    {
        size_t sz = lens[i];

        char *buf; 

        {
            #pragma omp critical
            buf = new char[sz];
        }

        ifs.read(buf, sz);

        graph[((VType*)buf)[0]] = std::shared_ptr<VType>((VType*)buf);
        ((VType*)buf)[0] = sz/sizeof(VType) -1;
    }

    ifs.close();
}

} // namespace query
} // namespace graph
} // namespace odps
} // namespace apsara



#endif

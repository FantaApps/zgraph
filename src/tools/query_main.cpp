#include "../lib/graph.h"
#include "../lib/graph_algo.h"
#include "../lib/graph_tool.h"
#include <omp.h>
#include <vector>
#include <fstream>

using namespace std;
using namespace apsara::odps::graph::query;

int main(int argc, char* argv[])
{
    if(argc < 5)
    {
        cout<<"Usage: graphQuery <query_file> <graph_folder> <CSR|Adj> <uint32|uint64> [num_threads]"<<endl;
        return 0;
    }

    if(argc > 5)
    {
        omp_set_dynamic(0);
        omp_set_num_threads(stoi(argv[5]));
    }

    vector<uint64_t> queries;
    ifstream ifs(argv[1]);
    string line;
    while(getline(ifs, line))
    {
        stringstream ss(line);
        uint64_t q;
        ss>>q;
        queries.push_back(q);
    }

    if(string(argv[3]) == "CSR")
    {
        if(string(argv[4]) == "uint32")
        {
            shared_ptr<Graph<uint32_t>> g = 
                    shared_ptr<Graph<uint32_t>>(new GraphCSR<uint32_t>());
            g->Init(argv[2]);
            GraphAlgo<uint32_t> algo(g);
            algo.SetNumThreads(stoi(argv[5]));
            double start = omp_get_wtime();
            for(size_t i=0; i<queries.size(); ++i)
            {
                cout<<"Query: "<<queries[i]<<" result:"<<algo.BFS((uint32_t)queries[i])<<endl;
            }
            double end = omp_get_wtime();
            cout<<" time taken:"<<end-start<<endl;
        }
        else if(string(argv[4]) == "uint64")
        {
            shared_ptr<Graph<uint64_t>> g = 
                    shared_ptr<Graph<uint64_t>>(new GraphCSR<uint64_t>());
            g->Init(argv[2]);
            GraphAlgo<uint64_t> algo(g);
            algo.SetNumThreads(stoi(argv[5]));
            double start = omp_get_wtime();
            for(size_t i=0; i<queries.size(); ++i)
            {
                cout<<"Query: "<<queries[i]<<" result:"<<algo.BFS((uint64_t)queries[i])<<endl;
            }
            double end = omp_get_wtime();
            cout<<" time taken:"<<end-start<<endl;
        }
    }

    if(string(argv[3]) == "Adj")
    {
        if(string(argv[4]) == "uint32")
        {
            shared_ptr<Graph<uint32_t>> g = 
                    shared_ptr<Graph<uint32_t>>(new GraphAdj<uint32_t>());
            g->Init(argv[2]);
            GraphAlgo<uint32_t> algo(g);
            algo.SetNumThreads(stoi(argv[5]));
            double start = omp_get_wtime();
            for(size_t i=0; i<queries.size(); ++i)
            {
                cout<<"Query: "<<queries[i]<<" result:"<<algo.BFS((uint32_t)queries[i])<<endl;
            }
            double end = omp_get_wtime();
            cout<<" time taken:"<<end-start<<endl;
        }
        else if(string(argv[4]) == "uint64")
        {
            shared_ptr<Graph<uint64_t>> g = 
                    shared_ptr<Graph<uint64_t>>(new GraphAdj<uint64_t>());
            g->Init(argv[2]);
            GraphAlgo<uint64_t> algo(g);
            algo.SetNumThreads(stoi(argv[5]));
            double start = omp_get_wtime();
            for(size_t i=0; i<queries.size(); ++i)
            {
                cout<<"Query: "<<queries[i]<<" result:"<<algo.BFS((uint64_t)queries[i])<<endl;
            }
            double end = omp_get_wtime();
            cout<<" time taken:"<<end-start<<endl;
        }
    }

    return 0;
}

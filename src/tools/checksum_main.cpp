#include "../lib/graph.h"
#include "../lib/graph_tool.h"
#include <omp.h>

using namespace std;
using namespace apsara::odps::graph::query;

int main(int argc, char* argv[])
{
    if(argc < 4)
    {
        cout<<"Usage: graphCheckSum <CSR|Adj|Orig> <folder> <uint32|uint64>"<<endl;
        return 0;
    }

    if(string(argv[1])=="Orig")
    {
        if(string(argv[3])=="uint32")
        {
            GraphTool<uint32_t> g;
            g.CheckSumOrig(argv[2]);
        }
        if(string(argv[3])=="uint64")
        {
            GraphTool<uint64_t> g;
            g.CheckSumOrig(argv[2]);
        }
    }

    if(string(argv[1])=="CSR")
    {
        if(string(argv[3])=="uint32")
        {
            GraphTool<uint32_t> g;
            g.CheckSumCSR(argv[2]);
        }
        if(string(argv[3])=="uint64")
        {
            GraphTool<uint64_t> g;
            g.CheckSumCSR(argv[2]);
        }
    }

    if(string(argv[1])=="Adj")
    {
        if(string(argv[3])=="uint32")
        {
            GraphTool<uint32_t> g;
            g.CheckSumAdj(argv[2]);
        }
        if(string(argv[3])=="uint64")
        {
            GraphTool<uint64_t> g;
            g.CheckSumAdj(argv[2]);
        }
    }

    return 0;
}

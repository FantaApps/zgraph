#include "../lib/graph.h"
#include "../lib/graph_tool.h"
#include <omp.h>

using namespace std;
using namespace apsara::odps::graph::query;

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        cout<<"Usage: graphPrepare <file_list> <tgtFolder> <CSR|Adj> <uint32|uint64>"<<endl;
        return 0;
    }

    string fileList = argv[2];
    string tgtFolder = argv[3];

    if(string(argv[3])=="CSR")
    {
        if(string(argv[4])=="uint32")
        {
            GraphTool<uint32_t> g;
            g.PrepareCSR(argv[1], argv[2]);
        }
        if(string(argv[4])=="uint64")
        {
            GraphTool<uint64_t> g;
            g.PrepareCSR(argv[1], argv[2]);
        }
    }

    if(string(argv[3])=="Adj")
    {
        if(string(argv[4])=="uint32")
        {
            GraphTool<uint32_t> g;
            g.PrepareAdj(argv[1], argv[2]);
        }
        if(string(argv[4])=="uint64")
        {
            GraphTool<uint64_t> g;
            g.PrepareAdj(argv[1], argv[2]);
        }
    }

    return 0;
}

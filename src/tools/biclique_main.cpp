#include "../lib/graph.h"
#include "../lib/graph_bnb.h"
#include <omp.h>
#include <sys/resource.h>

#include <map>
#include <vector>

using namespace std;
using namespace apsara::odps::graph::query;

int main(int argc, char* argv[])
{
    if(argc < 5)
    {
        cout<<"Usage: graphBiClique <CSR|Adj> <folder> <uint32|uint64> <recover|raw>"<<endl;
        return 0;
    }

    bool isRecover = false;
    if(string(argv[4])=="recover")
        isRecover = true;
    else if(string(argv[4])=="raw")
        isRecover = false;

    if(string(argv[1])=="CSR")
    {
        if(string(argv[3])=="uint32")
        {
            shared_ptr<Graph<uint32_t>> g = 
                shared_ptr<Graph<uint32_t>>(new GraphCSR<uint32_t>());
            g->Init(argv[2]);
            GraphBnB<uint32_t> bnb(g, argv[2], isRecover);
            bnb.SetNumThreads(16);
            bnb.EnumerateBiCliqueBasic();
        }
    }

    return 0;
}

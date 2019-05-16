#include "../lib/graph.h"
#include "../lib/graph_algo.h"
#include <omp.h>
#include <sys/resource.h>

#include <map>
#include <vector>

using namespace std;
using namespace apsara::odps::graph::query;

int main(int argc, char* argv[])
{

    const rlim_t kStackSize = 2 * 1024 * 1024 * 1024;   // min stack size = 16 MB
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        cout<<"Adjusting stack size......" <<endl;
        if (rl.rlim_cur < kStackSize)
        {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }

    string graphFolder = "/apsarapangu/disk8/twitter-2010/bfs_query/test/CSR/uint32/";
    shared_ptr<Graph<uint32_t>> g = 
        shared_ptr<Graph<uint32_t>>(new GraphCSR<uint32_t>());
    g->Init(graphFolder);
    GraphAlgo<uint32_t> algo(g);
    algo.SetNumThreads(16);
    map<uint32_t, vector<uint32_t>> sCC;
    algo.StronglyCC(g, sCC);

    return 0;
}

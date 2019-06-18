# Graph Query Toolkit  
Author: Zhaoming Yin  
  
## Brief  
This toolkit is used for querying the number of vertices that a input query vertex can reach. The underlying algorthm utilizes the Beamer BFS algorithm [1]. The major consideration for the design and implementation of this toolkit involves three bullet points:  
* Correctness:  
    - Check sum to ensure the graph is correct;  
    - Small unit test to ensure the BFS algorithm is correct.   
* Performance:  
    - Loading graph (parallel + binary storage);  
    - Using (Compressed sparse row) CSR;  
    - Edge list is sorted;  
    - Using beamer algorithm (top-down + bottom-up step).  
    - shared_ptr has big overhead for ctor/dtor [2], remember to reference it.
* Product:   
    - Support both CSR/Adjacency list, such that we can trade off between performance and graph update;   
    - Support both uint32 bit and uint64 bit of vertex/edge encoding;  
  
## How to build  
You need to install:  
* python2.7+  
* Scons  
* C++11 support  
  
In the local folder, please run:
```
/usr/local/bin/python2.7 /usr/local/bin/scons
```
  
## How to run
* You need to configure the environment variables first
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/:/usr/local/gcc-4.9.2/lib64/:/apsarapangu/disk8/twitter-2010/bfs_query/build/release/src/lib/
```  

* Build the graph in CSR or Adjacency list format  
  
1) Graph Prepareation  
  
```
graphPrepare <file_list> <tgtFolder> <CSR|Adj> <uint32|uint64>  
```
  
2) Graph Checksum
```
graphCheckSum <CSR|Adj|Orig> <folder> <uint32|uint64>  
```
   
* Run the graph query program  

1) Graph Query 
```
graphQuery <query_file> <graph_folder> <CSR|Adj> <uint32|uint64> [num_threads]
```

  
## Reference 

```
[1] S. Beamer et. al Direction-Optimizing Breadth-First Search  
[2] shared_ptr performnce: https://stackoverflow.com/questions/22295665/how-much-is-the-overhead-of-smart-pointers-compared-to-normal-pointers-in-c
[3] Y. Zhang et. al Consensus algorithms for the generation of all maximal bicliques
[4] Y. Zhang et. al On Finding Bicliques in Bipartite Graphs: a Novel Algorithm with Application to the Integration of Diverse Biological Data Types
[5] V. Batagelj et. al An O(m) Algorithm for Cores Decomposition of Networks
[6] Tarjan Depth-first search and linear graph algorithms
```

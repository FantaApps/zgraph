# Graph Query Toolkit  
Author: [Zhaoming Yin](https://stplaydog.github.io/)  
  
## Brief  
This package can do following things:

1) BFS (beamer's algorithm);
2) graph query the number of vertices that a input query vertex can reach;
3) Biclique computation.
  
## How to build  
### Linux
You need to install:  
* python2.7+
* Scons
* C++11 support
* install zstd

### MAC
* install homebrew
* install scons
* install zstd
  
In the local folder, please run:
```
scons
```

## Graph format
Our graph format is compatible with Prof. Jure Leskovec's graph collection format.
In which we call it as .gr file.
  
## How to run
### You need to configure the environment variables first
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/:/usr/local/gcc-4.9.2/lib64/:/apsarapangu/disk8/twitter-2010/bfs_query/build/release/src/lib/
```

### Build the graph in CSR or Adjacency list format  
  
1) Graph Prepareation
  
```
graphPrepare <file_list> <tgtFolder> <CSR|Adj> <uint32|uint64>
```
  
2) Graph Checksum
```
graphCheckSum <CSR|Adj|Orig> <folder> <uint32|uint64>
```

3) If you want to run biclique program, you might need to convert the graph format in [8] to .gr file 

```
python biclique_convert.py <from_file> <.gr file>
```

Or, you might need to convert the graph format in [4] to the .gr file.

```
python el2gr.py <from_file> <.gr file>
```
   
### Run the graph query program  

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
[7] https://snap.stanford.edu/data/index.html
[8] http://konect.uni-koblenz.de/networks/amazon-ratings
```

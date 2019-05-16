PREFIX=/apsarapangu/disk8/twitter-2010/bfs_query/
FILELIST=sampleFile.txt
TESTLIST=bigQuery.txt

$PREFIX/build/release/src/graphPrepare $PREFIX/test/$FILELIST $PREFIX/test/sample/CSR/uint32/ CSR uint32

PREFIX=/Users/stplaydog/gitlocal/bfs_query/
FILELIST=amazonFile.txt
TESTLIST=bigQuery.txt

$PREFIX/build/release/src/tools/graphPrepare $PREFIX/test/$FILELIST $PREFIX/test/amazon/CSR/uint32/ CSR uint32

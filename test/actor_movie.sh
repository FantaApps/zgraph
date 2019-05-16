PREFIX=/Users/stplaydog/gitlocal/bfs_query/
FILELIST=actor_movieFile.txt
TESTLIST=bigQuery.txt

$PREFIX/build/release/src/tools/graphPrepare $PREFIX/test/$FILELIST $PREFIX/test/actor_movie/CSR/uint32/ CSR uint32

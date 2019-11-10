PREFIX=/Users/zhaomingyin/gitlocal/zgraph/
FILELIST=graph_prepare_scripts/actor_movieFile.txt
TESTLIST=bigQuery.txt

$PREFIX/build/release/src/tools/graphPrepare $PREFIX/test/$FILELIST $PREFIX/test/graph_bin/actor_movie/CSR/uint32/ CSR uint32

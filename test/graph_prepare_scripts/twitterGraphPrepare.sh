PREFIX=/apsarapangu/disk8/twitter-2010/bfs_query/
FILELIST=origFileList.txt
TESTLIST=bigQuery.txt

#$PREFIX/build/release/src/graphPrepare $PREFIX/test/$FILELIST $PREFIX/test/CSR/uint32/ CSR uint32
#$PREFIX/build/release/src/graphPrepare $PREFIX/test/$FILELIST $PREFIX/test/CSR/uint64/ CSR uint64
#$PREFIX/build/release/src/graphPrepare $PREFIX/test/$FILELIST $PREFIX/test/Adj/uint32/ Adj uint32
#$PREFIX/build/release/src/graphPrepare $PREFIX/test/$FILELIST $PREFIX/test/Adj/uint64/ Adj uint64

#$PREFIX/build/release/src/graphCheckSum Orig $PREFIX/test/$FILELIST  uint32
#$PREFIX/build/release/src/graphCheckSum Orig $PREFIX/test/$FILELIST  uint64
#$PREFIX/build/release/src/graphCheckSum CSR $PREFIX/test/CSR/uint32/ uint32
#$PREFIX/build/release/src/graphCheckSum CSR $PREFIX/test/CSR/uint64/ uint64
#$PREFIX/build/release/src/graphCheckSum Adj $PREFIX/test/Adj/uint32/ uint32
#$PREFIX/build/release/src/graphCheckSum Adj $PREFIX/test/Adj/uint64/ uint64

#$PREFIX/build/release/src/graphQuery $PREFIX/test/$TESTLIST $PREFIX/test/CSR/uint32/ CSR uint32 16
#$PREFIX/build/release/src/graphQuery $PREFIX/test/$TESTLIST $PREFIX/test/CSR/uint64/ CSR uint64 16

#$PREFIX/build/release/src/graphQuery $PREFIX/test/$TESTLIST $PREFIX/test/Adj/uint32/ Adj uint32 16
$PREFIX/build/release/src/graphQuery $PREFIX/test/$TESTLIST $PREFIX/test/Adj/uint64/ Adj uint64 16

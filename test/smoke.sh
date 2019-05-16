PREFIX=/apsarapangu/disk8/twitter-2010/bfs_query/

#$PREFIX/build/release/src/graphPrepare $PREFIX/test/testFileList.txt $PREFIX/test/tCSR/uint32/ CSR uint32
#$PREFIX/build/release/src/graphPrepare $PREFIX/test/testFileList.txt $PREFIX/test/tCSR/uint64/ CSR uint64
#$PREFIX/build/release/src/graphPrepare $PREFIX/test/testFileList.txt $PREFIX/test/tAdj/uint32/ Adj uint32
#$PREFIX/build/release/src/graphPrepare $PREFIX/test/testFileList.txt $PREFIX/test/tAdj/uint64/ Adj uint64
#
#$PREFIX/build/release/src/graphCheckSum Orig $PREFIX/test/testFileList.txt  uint32
#$PREFIX/build/release/src/graphCheckSum Orig $PREFIX/test/testFileList.txt  uint64
#$PREFIX/build/release/src/graphCheckSum CSR $PREFIX/test/tCSR/uint32/ uint32
#$PREFIX/build/release/src/graphCheckSum CSR $PREFIX/test/tCSR/uint64/ uint64
#$PREFIX/build/release/src/graphCheckSum Adj $PREFIX/test/tAdj/uint32/ uint32
#$PREFIX/build/release/src/graphCheckSum Adj $PREFIX/test/tAdj/uint64/ uint64

$PREFIX/build/release/src/graphQuery $PREFIX/test/testQueryList.txt $PREFIX/test/tCSR/uint32/ CSR uint32 16
$PREFIX/build/release/src/graphQuery $PREFIX/test/testQueryList.txt $PREFIX/test/tCSR/uint64/ CSR uint64 16
$PREFIX/build/release/src/graphQuery $PREFIX/test/testQueryList.txt $PREFIX/test/tAdj/uint32/ Adj uint32 16
$PREFIX/build/release/src/graphQuery $PREFIX/test/testQueryList.txt $PREFIX/test/tAdj/uint64/ Adj uint64 16

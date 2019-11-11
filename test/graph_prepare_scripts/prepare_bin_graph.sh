GRAPH_FILE=$1

# Prepare parameters
PREFIX=/Users/zhaomingyin/gitlocal/zgraph/
FILELIST=graph_prepare_scripts/${GRAPH_FILE}File.txt
TESTLIST=bigQuery.txt

# Some pre action
mkdir -p $PREFIX/test/graph_bin/${GRAPH_FILE}/CSR/uint32/
echo ${PREFIX}test/data/${GRAPH_FILE}.gr > $PREFIX/test/$FILELIST

# Prepare graph
$PREFIX/build/release/src/tools/graphPrepare $PREFIX/test/$FILELIST $PREFIX/test/graph_bin/${GRAPH_FILE}/CSR/uint32/ CSR uint32

# Post action
rm $PREFIX/test/$FILELIST

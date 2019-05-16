curl http://localhost:8414/graphQuery -d '{"format":"CSR", "encode":"uint64","file":"bigQuery.txt","threadnum":16,"query":15}' -X GET -o - -s -w "%{http_code}\n"

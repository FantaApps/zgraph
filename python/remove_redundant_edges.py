# Remove redundant edges in a graph file

import sys

class RemoveRedundant(object):
    def __init__(self, graphFile, outFile):
       self.gf = open(graphFile)
       self.of = open(outFile, 'w')
       self.graph = {}
       self.readGraph(' ')
       self.writeToFile()

    def readGraph(self, delimiter):
       for line in self.gf:
           items = line.split(delimiter)
           if len(items) == 3:
               # these values are not useful now
               cnt_from = int(items[0])
               cnt_to = int(items[1])
               cnt_edges = int(items[2])
           else:
               if items[0] in self.graph:
                   if items[1] not in self.graph[items[0]]:
                       self.graph[items[0]].append(items[1])
               else:
                   self.graph[items[0]] = []
                   self.graph[items[0]].append(items[1])

    def writeToFile(self):
        for key in self.graph:
            vFrom = str(key)
            for val in self.graph[key]:
                vTo = str(val)
                self.of.write(vFrom + ' ' + vTo)
        self.gf.close()
        self.of.close()

if __name__ == '__main__':
    redundant = RemoveRedundant(sys.argv[1], sys.argv[2])

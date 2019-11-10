# Convert graph format to biclique graph format

import sys

class BiCliqueConvert(object):
    def __init__(self, graphFile, outFile):
        self.graph = {}
        self.graphMap = {}
        self.revGraph = {}
        self.revGraphMap = {}
        self.gf = open(graphFile)
        self.of = open(outFile, 'w')
        self.readGraph()
        self.mapId()
        self.writeToFile()

    def readGraph(self):
        for line in self.gf:
            items = line.split(" ")
            if items[0] in self.graph:
                self.graph[items[0]].append(items[1])
            else:
                self.graph[items[0]] = []
                self.graph[items[0]].append(items[1])
            if items[1] in self.revGraph:
                self.revGraph[items[1]].append(items[0])
            else:
                self.revGraph[items[1]] = []
                self.revGraph[items[1]].append(items[0])

    def mapId(self):
        cnt = 0
        for key in self.graph:
            self.graphMap[key] = cnt
            cnt += 1
        for key in self.revGraph:
            self.revGraphMap[key] = cnt
            cnt += 1

    def writeToFile(self):
        for key in self.graph:
            vFrom = str(self.graphMap[key])
            for val in self.graph[key]:
                vTo = str(self.revGraphMap[val])
                self.of.write(vFrom + ' ' + vTo + '\n')
        self.gf.close()
        self.of.close()

if __name__ == '__main__':
    convert = BiCliqueConvert(sys.argv[1], sys.argv[2])


# Convert graph format to biclique graph format

import sys

# conver graph format to biclique format now
class BiCliqueConvert(object):
    def __init__(self, graphFile, outFile, delimiter):
        self.graph = {}
        self.graphMap = {}
        self.revGraph = {}
        self.revGraphMap = {}
        self.gf = open(graphFile)
        self.of = open(outFile, 'w')
        if delimiter == "WHITE_SPACE":
            self.readGraph(" ")
        elif delimiter == "TAB":
            self.readGraph("\t")
        self.mapId()
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
    convert = BiCliqueConvert(sys.argv[1], sys.argv[2], sys.argv[3])


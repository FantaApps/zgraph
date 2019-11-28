# Convert graph format to biclique graph format

import sys

# Example
# python bipartite_connected_component.py graph.file outgraph.folder
class BiCliqueCC(object):
    def __init__(self, graphFile, outFolder):
       self.gf = open(graphFile)
       self.outFolder = outFolder
       self.graph = {}
       self.color = {}
       self.colorSet = {}
       self.colorId = 0
       self.readGraph(' ')
       self.markColor()
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

    def markColor(self):
        for key in self.graph:
            if key in self.color:
                continue
            else:
                setColor = False
                for val in self.graph[key]:
                    color = self.hasColor(self.graph[key])
                    if color != -1:
                        self.setColor(self.graph[key], color)
                        self.color[val] = color
                        self.insertColorSet(key, color)
                        setColor = True
                        break
                if setColor == False:
                    self.color[val] = self.colorId
                    self.insertColorSet(key, self.colorId)
                    self.setColor(self.graph[key], self.colorId)
                    self.colorId += 1

    def hasColor(self, valSet):
        for val in valSet:
             if val in self.color:
               return self.color[val]
        return -1

    def setColor(self, valSet, color):
        for val in valSet:
            self.color[val] = color

    def insertColorSet(self, key, color):
      if color in self.colorSet:
          self.colorSet[color].add(key)
      else:
          self.colorSet[color] = set([])
          self.colorSet[color].add(key)

    def writeToFile(self):
        for c in range(0, self.colorId):
            of = open(self.outFolder + "CC_" +str(c), 'w')
            for key in self.colorSet[c]:
                for val in self.graph[key]:
                    of.write(key + " " + val)

if __name__ == '__main__':
    convert = BiCliqueCC(sys.argv[1], sys.argv[2])


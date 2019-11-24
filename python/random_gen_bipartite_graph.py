# Generate random bi-partite graph 

import sys
import random

# Example:
# python random_gen_bipartite_graph.py 10 20 50 rand1.gr 
class BipartiteRandGen(object):
    def __init__(self, leftV, rightV, numE, outFile):
        self.edgeExisted = {}
        self.graph = {}
        self.leftV = int(leftV)
        self.rightV = int(rightV)
        self.numE = int(numE)
        self.of = open(outFile, 'w')
        self.generateGraph()
        self.writeToFile()

    def generateGraph(self):
        for i in range(0, self.numE):
            left = 0
            right = 0
            while True:
                left = random.randint(0, self.leftV-1)
                right = random.randint(self.leftV, self.leftV + self.rightV-1) 
                key = str(left) + "," + str(right)
                if key not in self.edgeExisted:
                    self.edgeExisted[key] = True
                    break
            if left in self.graph:
                self.graph[left].append(right)
            else:
                self.graph[left] = []
                self.graph[left].append(right)

    def writeToFile(self):
        print self.leftV, self.rightV
        for key in self.graph:
            vFrom = str(key)
            for val in self.graph[key]:
                vTo = str(val)
                if(self.leftV < self.rightV):
                    self.of.write(vFrom + ' ' + vTo + '\n')
                else:
                    self.of.write(vTo + ' ' + vFrom + '\n')
        self.of.close()

if __name__ == '__main__':
    convert = BipartiteRandGen(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])

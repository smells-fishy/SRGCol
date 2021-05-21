from sage.graphs.graph_coloring import edge_coloring
from sage.graphs.graph_list import from_graph6
import sys

def triangles(filename):
  with open(filename, 'r') as f:
    L = f.readlines()
  f.close()
  l = from_graph6(L)
  outname = filename[0:(len(filename) - 3)] + "tri1.txt"
  sys.stdout = open(outname, 'w')
  count = 1
  for G in l:
    print("Graph " + str(count))
    edges = G.edges(labels=false)
    for i in range(0, len(edges) - 1):
      for j in range(i + 1, len(edges)):
        e1 = edges[i]
        e2 = edges[j]
        if e1[0] == e2[0]:
            if (e1[1], e2[1]) in edges:
                print(str(e1[0]) + ", " + str(e1[1]) + ", " + str(e2[1]))
        else:
            break
    count = count + 1
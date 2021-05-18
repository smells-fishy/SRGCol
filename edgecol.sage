from sage.graphs.graph_coloring import edge_coloring
from sage.graphs.graph_list import from_graph6
import sys

def edgecolor(filename):
  with open(filename, 'r') as f:
    L = f.readlines()
  f.close()
  l = from_graph6(L)
  outname = filename[0:(len(filename) - 3)] + "color.txt"
  out = open(outname, 'w')
  for G in l:
    out.write(str(edge_coloring(G, value_only=False, vizing=False, solver="GLPK")) + "\n")

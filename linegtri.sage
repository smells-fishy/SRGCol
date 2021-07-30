import sys
from sage.graphs.graph_list import from_graph6
from sage.graphs.line_graph import line_graph

def lineg(filename):
  with open(filename, 'r') as f:
    L = f.readlines()
  f.close()
  l = from_graph6(L)
  pos = filename.find('/')
  outname = "Lineg" + filename[pos:(len(filename) - 3)] + "LGadjstat.txt"
  sys.stdout = open(outname, 'w')
  count = 1
  triangleset = set()
  for G in l:
    print("Graph " + str(count))
    lg = G.line_graph()
    v0 = lg.vertices()[0] 
    v0_nghl = lg.neighbors(v0)
    for v1 in v0_nghl:
      if v0[0] == 
  print(triangleset, flush = True)
import sys
from sage.graphs.graph_list import from_graph6
from sage.graphs.line_graph import line_graph

def amNoBracket (am, size):
  for i in range(0, size):
    for j in range(0, size):
      print(str(am[i][j]), end = ' ', flush = True)
    print(' ')
  print(' ')

def crossg(filename):
  with open(filename, 'r') as f:
    L = f.readlines()
  f.close()
  l = from_graph6(L)
  pos = filename.find('/')
  outname = "Lineg" + filename[pos:(len(filename) - 3)] + "CrossG.txt"
  sys.stdout = open(outname, 'w')
  count = 1
  for G in l:
    lg = G.line_graph()
    GuLg = G.disjoint_union(lg)
    GuLg.delete_edges(GuLg.edge_iterator())
    print("Graph " + str(count) + " Order " + str(GuLg.order()), flush = True)
    vert = list(GuLg.vertex_iterator())
    upper = len(vert)
    for i in range(0, upper - 1):
      for j in range(i + 1,upper):
        if (vert[i][0] != vert[j][0]):
          if (vert[i][0] == 0):
            if (vert[i][1] == vert[j][1][0] or vert[i][1] == vert[j][1][1]):
              GuLg.add_edge(vert[i], vert[j])
          else:
            if (vert[j][1] == vert[i][1][0] or vert[j][1] == vert[i][1][1]):
              GuLg.add_edge(vert[i], vert[j])
    amNoBracket(GuLg.adjacency_matrix(), upper)
    del lg
    del GuLg
    del vert
    del upper
    gc.collect()
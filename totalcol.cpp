#include <algorithm>
#include <cstdio>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <new>
#include <string>
#include <vector>

#if DEBUG == 1
  std::ofstream log("log.txt");
#endif

class Graph {
  private:
    int  order_v;
    std::list<int> *adjlist;
    std::list<int> *ecolors;
    int * vcolors;
  public:
    Graph(int, int);
    ~Graph();
    void adj_insert(int, int);
    void color(int ,int);
    void ecolor_insert(int, int);
    void insert_or_eq(std::list<int>, std::list<int>::iterator, int);
    void relabel();
};

Graph::Graph(int o, int col) {
  order_v = o;
  adjlist = new std::list<int>[o];
  ecolors = new std::list<int>[o];
  vcolors = new int[o];
}

Graph::~Graph() {
  delete [] adjlist;
  delete [] ecolors;
  delete [] vcolors;
}

void Graph::adj_insert(int ind, int d) {
  adjlist[ind].insert(adjlist[ind].end(), d);
#if DEBUG == 1
  std::streambuf *coutbuf = std::cout.rdbuf(); 
  std::cout.rdbuf(log.rdbuf()); 
  std::cout << "Inserted edge (" << ind + 1 << ", " << d + 1 << ")\n"; 
  std::cout.rdbuf(coutbuf);
#endif
}

void Graph::insert_or_eq(std::list<int> L, std::list<int>::iterator p, int d) {
  std::list<int>::iterator j;
  for(j = L.begin(); j != L.end(); j++) {
    if(*j == d) return;
  }
  L.insert(p, d);
}

void Graph::color(int k, int r) {
  int count = 0;
  while(count < r) {
    //Edge coloring with k colors first
    for(int i = 0; i < order_v; i++) {
      std::list<int>::iterator j;
      bool cavail[k];
      for(int u = 0; u < k; u++) {
        cavail[u] = true;
      }
      for(j = ecolors[i].begin(); j != ecolors[i].end(); j++) {
        cavail[*j] = false;
      }
      j = adjlist[i].begin();
      for(int v = 0; v < i + 1; i++) {
        j = j++;
        for(int m = 0; m < k; m++) {
          if(cavail[m]) {
            insert_or_eq(ecolors[i], ecolors[i].end(), m);
            insert_or_eq(ecolors[*j], ecolors[*j].end(), m);
          }
        }
      }
    }

    //Vertex coloring with k colors now
    for(int i = 0; i < order_v; i++) {
      std::list<int>::iterator j;
      bool cavail[k];
      for(int u = 0; u < k; u++) {
        cavail[u] = true;
      }
      for(j = ecolors[i].begin(); j != ecolors[i].end(); j++) {
        cavail[*j] = false;
      }
      for(j = adjlist[i].begin(); j != adjlist[i].end(); j++) {
        cavail[vcolors[*j]] = false;
      }
      for(int m = 0; m < k; m++) {
          if(cavail[m]) {
            vcolors[i] = m;
          }
        }
    }
    count++;
  }
}

int parse() {
  //Initial parsing of the showg output to find the graph order
  std::string first;
  std::getline(std::cin, first);
  std::getline(std::cin, first);

  int res = 0;
  int base = 1;
  int n = first.length() - 2;
  while(n > 0 && first[n] - '0' >= 0 && '9' - first[n] >= 0) {
    int x = first[n] - 48;
    res += x * base;
    base *= 10;
    n--;
  }

  return res;

}

int main(int argc, char *argv[]) {

  std::ios::sync_with_stdio(false);

  //Function takes a filename as a command-line input
  freopen(argv[1], "r", stdin);

  std::string outname = argv[1];
  outname.erase(outname.find("g6") - 1, outname.length());
  outname.append("col.txt");

  freopen(outname.c_str(), "w", stdout);

  int order = parse();
  int col = std::stoi(argv[2]);
  int runs = 100;
  std::vector<Graph*> graphs;
  while(!std::cin.eof() && std::cin.good()) {
    Graph *graph = new Graph(order, col);
    graphs.insert(graphs.end(), graph);
    for(int i = 0; i < order; i++) {
      for(int j = 0; j < order; j++) {
        int x;
        std::cin >> x;
        if(x) graph->adj_insert(i, j);
      }
    }

    std::string skip;
    std::getline(std::cin, skip);
    std::getline(std::cin, skip);
    std::getline(std::cin, skip);
  }

  for(Graph *i : graphs) {
    i->color(col, runs);
  }
  
}
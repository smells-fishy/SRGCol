#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <new>
#include <string>
#include <time.h>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#if DEBUG == 1
  std::ofstream log("log.txt");
#endif

const int HASH_CONST = rand(); 

namespace std {
  template <> struct hash<std::pair<int, int> > {
    size_t operator()(const std::pair<int, int> &x) const {
      std::hash<std::string> h;
      return (x.first + x.second) * HASH_CONST;
    }
  };

  template <> struct hash<std::tuple<int, int, int> > {
    size_t operator()(const std::tuple<int, int, int> &x) const {
      std::hash<std::string> h;
      return (std::get<0>(x) + std::get<1>(x) + std::get<2>(x)) * HASH_CONST;
    }
  };
}

class Graph {
  private:
    int  order_v;
    std::list<int> *adjlist;
    std::unordered_set<int> *ecolors;
    std::unordered_set<std::pair<int, int> > edge;
    std::unordered_set<std::tuple<int, int, int> > triangle;
    int * vcolors;
  public:
    Graph(int, int);
    ~Graph();
    void adj_insert(int, int);
    void color(int ,int);
    void edges();
    void print_adj();
    void print_edges();
    void relabel();
    void triangles();
};

Graph::Graph(int o, int col) {
  order_v = o;
  adjlist = new std::list<int>[o];
  ecolors = new std::unordered_set<int>[o];
  vcolors = new int[o];
  for(int i = 0; i < o; i++) vcolors[i] = -1;
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

void Graph::edges() {
  for(int i = 0; i < order_v; i++) {
    std::list<int>::iterator j = adjlist[i].begin();
    for(int k = 0; k < i; k++) {
      edge.insert(edge.end(), std::make_pair(std::min(i, *j), std::max(i, *j)));
      j++;
      if(j == adjlist[i].end()) break;
    }
  }
}

void Graph::triangles() {
  for(auto const &p : edge) { 
    for(auto const &r : edge) {
      if(p == r) ;
      else {
        if(p.first == r.first) {
          if(edge.find(std::make_pair(p.second, r.second)) != edge.end()) {
            auto tri = std::make_tuple(p.first, p.second, r.second);
            triangle.insert(triangle.end(), tri);
          }
        }
      }
    }
  }
}

void Graph::print_adj() {
  for(int i = 0; i < order_v; i++) {
    std::cout << i << ": ";
    for(std::list<int>::iterator j = adjlist[i].begin(); j != adjlist[i].end(); j++) {
      if(j == --adjlist[i].end()) std::cout << *j << "\n";
      else std::cout << *j << ", ";
    }
  }
}

void Graph::print_edges() {
  int count = 0;
  for(auto const &p : edge) {
    std::cout << "(" << p.first << ", " << p.second << ")\n";
    count++;
  }
  std::cout << "Edges: " << count << "\n";
}

void Graph::relabel() {
  std::vector<int> avail(order_v);
  for(int i = 0; i < order_v; i++) avail[i] = i;

  //Set up the random bijection between vertices
  std::vector<int> perm(order_v);
  for(int i = 0; i < order_v; i++) {
    int n = rand() % avail.size();
    perm[i] = avail[n];
    avail.erase(avail.begin()+n);
  }

  //Reassign the edges
  std::unordered_set<std::pair<int, int> > new_edge;
  for(auto &p : edge) {
    new_edge.insert(new_edge.end(), std::make_pair(std::min(perm[p.first], perm[p.second]), std::max(perm[p.first], perm[p.second])));
  }
  edge = new_edge;

  //Reassign the adjlist
  for(int i = 0; i < order_v; i++) {
    std::list<int>::iterator j;
    for(j = adjlist[i].begin(); j != adjlist[i].end(); j++) {
      *j = perm[*j];
    }
  }

  //Create new adjlist
  std::list<int> *adjlist_new = new std::list<int>[order_v];
  for(int i = 0; i < order_v; i++) {
    adjlist[i].sort();
    adjlist_new[perm[i]] = adjlist[i];
  }
  delete [] adjlist;
  adjlist = adjlist_new;

#if DEBUG == 1
  std::streambuf *coutbuf = std::cout.rdbuf(); 
  std::cout.rdbuf(log.rdbuf()); 
  print_adj();
  print_edges();
  std::cout.rdbuf(coutbuf);
#endif
}

void Graph::color(int k, int r) {
  int count = 0;
  int edges;
  std::vector<std::pair<int, int> > *color_classes = new std::vector<std::pair<int, int> >[k];
  while(count < r) {
    //Edge coloring first
    
    //Before each attempt, clear the color classes and color lists
    edges = 0;
    for(int i = 0; i < k; i++) {
      color_classes[i].clear();
    }
    for(int i = 0; i < order_v; i++) {
      ecolors[i].clear();
    }
    for(auto const &p : edge) {
      bool cavail[k];
      for(int u = 0; u < k; u++) {
        cavail[u] = true;
      }
      for(std::unordered_set<int>::iterator j = ecolors[p.first].begin(); j != ecolors[p.first].end(); j++) {
        cavail[*j] = false;
      }
      for(std::unordered_set<int>::iterator j = ecolors[p.second].begin(); j != ecolors[p.second].end(); j++) {
        cavail[*j] = false;
      }
      for(int m = 0; m < k; m++) {
        if(cavail[m]) {
          color_classes[m].insert(color_classes[m].end(), p);
          ecolors[p.first].insert(ecolors[p.first].end(), m);
          ecolors[p.second].insert(ecolors[p.second].end(), m);
          edges++;
          break;
        }
      }
    }
    if(edges != edge.size()) {
      relabel();
    } else {
      int vertct = 0;
      std::vector<int> order(order_v);
      for(int i = 0; i < order_v; i++) order[i] = i;
      while(vertct < r) {
        int vert = 0;
        //Reset the vertex colorings
        for(int i = 0; i < order_v; i++) vcolors[i] = -1;
        for(int i = 0; i < order_v; i++) {
          bool cavail[k];
          for(int u = 0; u < k; u++) {
            cavail[u] = true;
          }
          for(std::unordered_set<int>::iterator j = ecolors[order[i]].begin(); j != ecolors[order[i]].end(); j++) {
            cavail[*j] = false;
          }
          for(std::list<int>::iterator j = adjlist[order[i]].begin(); j != adjlist[order[i]].end(); j++) {
            if(vcolors[*j] != -1) cavail[vcolors[*j]] = false;
          }
          for(int m = 0; m < k; m++) {
            if(cavail[m]) {
              vcolors[order[i]] = m;
              cavail[m] = false;
              vert++;
              break;
            }
          }
        }
        if(vert != order_v) {
          std::random_shuffle(order.begin(), order.end());
        } else {
          break;
        }
        vertct++;
      }
    }
    count++;
  }

  //Print color classes to text file
  std::cout << "{\n";
  edges = 0;
  for(int i = 0; i < k; i++) {
    std::cout << i << ": ";
    for(auto const &p : color_classes[i]) {
      std::cout << "(" << p.first << ", " << p.second << "), ";
      edges++;
    }
    std::cout << "\n";
  }
  std::cout << "}\n";
  std::cout << edges << "\n";
  
  //Print vertex colorings
  for(int i = 0; i < order_v; i++) {
    std::cout << "Vert " << i << ": " << vcolors[i] << "\n";
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
  srand (time(NULL));

  //Function takes a filename as a command-line input
  freopen(argv[1], "r", stdin);

  std::string outname = argv[1];
  outname.erase(outname.find("g6") - 1, outname.length());
  outname.append("col.txt");

  freopen(outname.c_str(), "w", stdout);

  int order = parse();
  int col = std::stoi(argv[2]);
  int runs = std::stoi(argv[3]);
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
#if DDEBUG == 1
  graphs->print_adj();
#endif
    std::string skip;
    std::getline(std::cin, skip);
    std::getline(std::cin, skip);
    std::getline(std::cin, skip);
  }

  for(Graph *i : graphs) {
    i->edges();
    i->color(col, runs);
    std::cout << "\n";
  }
}
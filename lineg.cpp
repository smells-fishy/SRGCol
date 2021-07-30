#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <new>
#include <set>
#include <string>
#include <time.h>
#include <tuple>
#include <unistd.h>
#include <utility>
#include <vector>

#ifdef DEBUG
  std::ofstream log("log.txt");
#endif

struct pair_cmp {
  bool operator() (const std::pair<int, int> &a, const std::pair<int, int> &b) const {
    if(a.first == b.first) return a.second < b.second;
    else return a.first < b.first;
  }
};

struct tupl_cmp {
  bool operator() (const std::tuple<int, int, int> &a, const std::tuple<int, int, int> &b) const {
    if(std::get<0>(a) == std::get<0>(b)) {
      if(std::get<1>(a) == std::get<1>(b)) return std::get<2>(a) < std::get<2>(b);
      else return std::get<1>(a) < std::get<1>(b);
    }
    else return std::get<0>(a) < std::get<0>(b);
  }
};

class Graph {
  private:
    int order_v;
    std::list<int> *adjlist;
    std::set<std::pair<int, int>, pair_cmp> edge;
    std::set<std::tuple<int, int, int>, tupl_cmp> triangle;
  public:
    Graph(int);
    ~Graph();
    void adjlist_insert(int, int);
    std::vector<int> clique();
    void edges();
    int  edgesize();
    bool isClique(std::vector<int> vec);
    inline int order();
    void print_adj();
    void print_edges();
    void triangles();
    int  trianglesize();

    friend void col(Graph, int, bool, bool, bool);
};

Graph::Graph(int o) {
  order_v = o;
  adjlist = new std::list<int>[o];
}

Graph::~Graph() {
  delete [] adjlist;
}

void Graph::adjlist_insert(int ind, int d) {
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
#if DEBUG == 1
  std::streambuf *coutbuf = std::cout.rdbuf(); 
  std::cout.rdbuf(log.rdbuf()); 
  print_edges(); 
  std::cout.rdbuf(coutbuf);
#endif
}

int Graph::edgesize() {
  return this->edge.size();
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

int Graph::trianglesize() {
  return this->triangle.size();
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

inline int Graph::order() {
  return this->order_v;
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

  freopen(argv[1], "r", stdin);

  std::ios::sync_with_stdio(false);

  int order = parse();
  int count = 1;
  std::vector<Graph*> graphs;
  while(!std::cin.eof() && std::cin.good()) {
    Graph *graph = new Graph(order);
    graphs.insert(graphs.end(), graph);
    for(int i = 0; i < order; i++) {
      for(int j = 0; j < order; j++) {
        int x;
        std::cin >> x;
        if(x) graph->adjlist_insert(i, j);
      }
    }
#if DDEBUG == 1
  graphs->print_adj();
#endif

    //This trick is unique to showg, will generalize later
    std::string skip;
    std::getline(std::cin, skip);
    std::getline(std::cin, skip);
    std::getline(std::cin, skip);
  }

}
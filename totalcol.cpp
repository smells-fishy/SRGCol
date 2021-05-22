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
#include <utility>
#include <vector>

#if DEBUG == 1
  std::ofstream log("log.txt");
#endif

struct comp
{
  template<typename T>
  bool operator()(const T &l, const T &r) const
  {
    if (l.first == r.first) {
      return l.second < r.second;
    }
    return l.first < r.first;
  }
};

class Graph {
  private:
    int  order_v;
    std::list<int> *adjlist;
    std::set<int> *ecolors;
    std::set<std::pair<int, int>, comp> edge;
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
};

Graph::Graph(int o, int col) {
  order_v = o;
  adjlist = new std::list<int>[o];
  ecolors = new std::set<int>[o];
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
  for(auto const &p : edge) {
    std::cout << "(" << p.first << ", " << p.second << ")\n";
  }
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
  for(auto p : edge) {
    p.first = perm[p.first];
    p.second = perm[p.second];
  }

  //Reassign the adjlist
  for(int i = 0; i < order_v; i++) {
    std::list<int>::iterator j;
    for(j = adjlist[i].begin(); j != adjlist[i].end(); j++) {
      *j = perm[*j];
    }
  }

  //Create new adjlist
  std::list<int> *adjlist_new;
  for(int i = 0; i < order_v; i++) {
    adjlist_new[perm[i]] = adjlist[i];
  }
  delete [] adjlist;
  adjlist = adjlist_new;
}

void Graph::color(int k, int r) {
  int count = 0;
  std::vector<std::pair<int, int> > *color_classes = new std::vector<std::pair<int, int> >[k];
  while(count < r) {
    //Edge coloring first
    for(auto const &p : edge) {
      bool cavail[k];
      for(int u = 0; u < k; u++) {
        cavail[u] = true;
      }
      for(std::set<int>::iterator j = ecolors[p.first].begin(); j != ecolors[p.first].end(); j++) {
        cavail[*j] = false;
      }
      for(std::set<int>::iterator j = ecolors[p.second].begin(); j != ecolors[p.second].end(); j++) {
        cavail[*j] = false;
      }
      int edges = 0;
      for(int m = 0; m < k; m++) {
        if(cavail[m]) {
          color_classes[m].insert(color_classes[m].end(), p);
          ecolors[p.first].insert(ecolors[p.first].end(), m);
          ecolors[p.second].insert(ecolors[p.second].end(), m);
          edges++;
          break;
        }
      }
      if(edges != 130) {
        //relabel();
      }
    }
    
    //Vertex coloring with k colors now
    /*
    for(int i = 0; i < order_v; i++) {
      bool cavail[k];
      for(int u = 0; u < k; u++) {
        cavail[u] = true;
      }
      for(std::set<int>::iterator j = ecolors[i].begin(); j != ecolors[i].end(); j++) {
        cavail[*j] = false;
      }
      for(std::list<int>::iterator j = adjlist[i].begin(); j != adjlist[i].end(); j++) {
        if(vcolors[*j] != -1) cavail[vcolors[*j]] = false;
      }
      for(int m = 0; m < k; m++) {
        if(cavail[m]) {
          vcolors[i] = m;
          cavail[m] = false;
          break;
        }
      }
    }
    */
    count++;
  }
  //Print color classes to text file
  std::cout << "{\n";
  int edges = 0;
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
  int runs = 1;
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
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
#include <utility>
#include <vector>

#if DEBUG == 1
  std::ofstream log("log.txt");
#endif

struct pair_cmp {
  bool operator() (const std::pair<int, int> &a, const std::pair<int, int> &b) {
    if(a.first == b.first) return a.second < b.second;
    else return a.first < b.first;
  }
};

struct tupl_cmp {
  bool operator() (const std::tuple<int, int, int> &a, const std::tuple<int, int, int> &b) {
    if(std::get<0>(a) == std::get<0>(b)) {
      if(std::get<1>(a) == std::get<1>(b)) return std::get<2>(a) < std::get<2>(b);
      else return std::get<1>(a) < std::get<1>(b);
    }
    else return std::get<0>(a) < std::get<0>(b);
  }
};

class Graph {
  private:
    int  order_v;
    std::list<int> *adjlist;
    std::set<std::pair<int, int>, pair_cmp> edge;
    std::set<std::tuple<int, int, int>, tupl_cmp> triangle;
  public:
    Graph(int);
    ~Graph();
    void adj_insert(int, int);
    void color(int ,int);
    void edges();
    int  edgesize();
    inline int order();
    void print_adj();
    void print_edges();
    void triangles();

    friend void col(Graph, int, int*);
};

Graph::Graph(int o) {
  order_v = o;
  adjlist = new std::list<int>[o];
}

Graph::~Graph() {
  delete [] adjlist;
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

/*
*
*   Helper fucntions which allows us to cleanly calculate the variable "number" corresponding
*   to certain structures
*
*/

inline int vindex(int j, int k) {
  //Returns first variable encoding data for vertex j
  return 2 * j * k + 1;
}

inline int eindex(int j, int k, int vert) {
  //Returns first variable encoding data for edge j 
  //Edges are lexicographically ordered by std::set
  return 2 * j * k + 2 * vert * k + 1;
}

inline int sindex(int j, int k, int counter) {
  return counter + k + j;
}

/*
*
*   In traditional vertex coloring, we want to encode at-most-one for the variables v_(i,j)
*   where i is the "vertex number" and j is the "color number." Moreover, we add additional
*   clauses to express that two neighbors cannot have the same color. We will use this idea
*   to efficiently encode total coloring, where we instead use the at-most-one encoding for
*   each vertex and edge. 
*   
*   We start counting the indices at 1, and we increment by k after every row, since we need 
*   k - 1 auxillary variables S_(i, 1) to S_(i, k - 1)
*
*/

void amo(int k, int counter, int* max, int* clause) {
  
  //First, we write that for each v_(i, j) that NOT v_(i, j) OR S_(i, j)
  for(int j = 1; j < k + 1; j++) {
    std::cout << "-" << counter + j << " " << sindex(j, k, counter) << " " << 0 << std::endl;
    *clause = *clause + 1;
    *max = std::max(sindex(j, k, counter), *max);
  }

  //Next, we write that for each v_(i, j) that NOT S_(i, j) OR NOT v_(i, j + 1) 
  for(int j = 1; j < k + 1; j++) {
    std::cout << "-" << sindex(j, k, counter) << " " << "-" << counter + j + 1 << " " << 0 << std::endl;
    *clause = *clause + 1;
    *max = std::max(sindex(j, k, counter), *max);
  }

  //Next, we write that for each S_(i, j) that NOT S_(i, j) OR S_(i, j + 1)
  for(int j = 1; j < k; j++) {
    std::cout << "-" << sindex(j, k, counter) << " " << sindex(j + 1, k, counter) << " " << 0 << std::endl;
    *clause = *clause + 1;
    *max = std::max(sindex(j + 1, k, counter), *max);
  }
  
  //Last, we write the unit clause S_(i, j) so that there is exactly one true
  std::cout << sindex(k, k, counter) << " " << 0 << std::endl;
  *clause = *clause + 1;
}

/*
*
*   We now encode the coloring conditions: if two vertices are neighbors, then they cannot
*   be coloring with the same color, if two edges are incident to the same vertex then they 
*   cannot have the same color, and if an edge is incient to a vertex then that edges and the
*   vertex cannot have the same color.
*
*   We use the trick of iterating through the edges, and adding these constraints via the
*   endpoints rather than iterating seperately.
*
*/

void col(Graph G, int k, int* clause) {

  //Encodes adjacency condition for each pair of adjacent vertices
  for(const auto &e : G.edge) {
    int first = e.first;
    int second = e.second;
    for(int j = 0; j < k; j++) {
      std::cout << "-" << vindex(first, k) + j << " " << "-" << vindex(second, k) + j << " " << 0 << std::endl;
      *clause = *clause + 1;
    }
  }

  //Encodes adjacency condition for vertex-edge pairs
  int ind = 1;
  for(const auto &e : G.edge) {
    int first = e.first;
    int second = e.second;
    for(int j = 0; j < k; j++) {
      std::cout << "-" << vindex(first, k) + j << " " << "-" << eindex(ind, k, G.order()) + j << " " << 0 << std::endl;
      std::cout << "-" << vindex(second, k) + j << " " << "-" << eindex(ind, k, G.order()) + j << " " << 0 << std::endl;
      *clause = *clause + 2;
    }
    ind++;
  }

  //Encodes adjacency condition for edge-edge pairs
  ind = 1;
  for(const auto &e : G.edge) {
    int sind = ind + 1;
    for(std::set<std::pair<int, int>, pair_cmp>::iterator j = ++G.edge.find(e); j != G.edge.end(); j++) {
      if(e.first == (*j).first || e.first == (*j).second || e.second == (*j).second || e.second == (*j).second) { 
        std::cout << "-" << eindex(ind, k, G.order()) << " " << "-" << eindex(sind, k, G.order()) << " " << 0 << std::endl;
        *clause = *clause + 1;
      }
      sind++;
    }
    ind++;
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

  //Program takes inputs <filename> k
  if(argc < 3) throw 1;

  int k = std::stoi(argv[2]);

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
        if(x) graph->adj_insert(i, j);
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

  for(Graph *i : graphs) {
    std::string outname = argv[1];
    outname.erase(outname.find("g6") - 1, outname.length());
    outname.append("G");
    outname.append(std::to_string(count));
    outname.append("col");
    outname.append(argv[2]);
    outname.append(".cnf");

    freopen(outname.c_str(), "w", stdout);

    //Save a line for cnf header
    std::cout << std::endl;

    i->edges();
    i->triangles();
    int counter = 0;
    int clause = 0;
    int max = 0;

    /*
    *
    *  This formulation allows us to keep track of which variables are for which purpose: 
    *  in essence, if a variable is nk + 1, even n, then it the start of 10 non-sequential
    *  counting variables. The first i->order() of these represent vertices
    * 
    */

    for(int j = 0; j < i->order(); j++) {
      amo(k, counter, &max, &clause);
      counter += 2 * k;
    }
    for(int j = 0; j < i->edgesize(); j++) {
      amo(k, counter, &max, &clause);
      counter += 2 * k;
    }

    col(*i, k, &clause);

    fseek(stdout, 0L, SEEK_SET);
    //rewind(stdout);
    std::cout << "p cnf " << max << " " << clause;
    
    count++;
  }
}
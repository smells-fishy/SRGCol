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

inline int tindex(int j, int k, int vert, int edge) {
  return 2 * j * k + 2 * edge * k + 2 * vert * k + 1;
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

void equals_k(int k, int counter) {
  //Abysmal bandaid: we encode at least one
  for(int j = 1; j < k + 1; j++) {
    std::cout << counter + j << " ";
  }
  std::cout << 0 << std::endl;
  
  //First, we write that for each v_(i, j) that NOT v_(i, j) OR S_(i, j)
  for(int j = 1; j < k + 1; j++) {
    std::cout << "-" << counter + j << " " << sindex(j, k, counter) << " " << 0 << std::endl;
  }

  //Next, we write that for each v_(i, j) that NOT S_(i, j) OR NOT v_(i, j + 1) 
  for(int j = 1; j < k + 1; j++) {
    std::cout << "-" << sindex(j, k, counter) << " " << "-" << counter + j + 1 << " " << 0 << std::endl;
  }

  //Next, we write that for each S_(i, j) that NOT S_(i, j) OR S_(i, j + 1)
  for(int j = 1; j < k; j++) {
    std::cout << "-" << sindex(j, k, counter) << " " << sindex(j + 1, k, counter) << " " << 0 << std::endl;
  }
  
  //Last, we write the unit clause S_(i, j) so that there is exactly one true
  std::cout << sindex(k, k, counter) << " " << 0 << std::endl;
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

void col(Graph G, int k, bool vertex, bool edge, bool triangle) {

  //Copy edges into a vector to make things easier
  std::vector<std::pair<int, int> > edges;
  std::copy(G.edge.begin(), G.edge.end(), std::back_inserter(edges));

  //Copy triangles into a vector to make things easier
  std::vector<std::tuple<int, int, int> > triangles;
  std::copy(G.triangle.begin(), G.triangle.end(), std::back_inserter(triangles));

  //Minor optimizations
  int vert = G.order();
  int edgesize = edges.size();
  int triupper = triangles.size();

  if(vertex) {
    //Encodes adjacency condition for each pair of adjacent vertices
    for (const auto &e : G.edge) {
      int first = e.first;
      int second = e.second;
      for (int j = 0; j < k; j++) {
        std::cout << "-" << vindex(first, k) + j << " " << "-" << vindex(second, k) + j << " " << 0 << std::endl;
      }
    }
  }

  if(vertex && edge) {
    //Encodes adjacency condition for vertex-edge pairs
    for(int i = 0; i < edgesize; i++) {
      std::pair<int, int> e = edges[i];
      int first  = e.first;
      int second = e.second;
      for(int j = 0; j < k; j++) {
        std::cout << "-" << vindex(first, k) + j  << " " << "-" << eindex(i, k, vert) + j << " " << 0 << std::endl;
        std::cout << "-" << vindex(second, k) + j << " " << "-" << eindex(i, k, vert) + j << " " << 0 << std::endl;
      }
    }
  }

  if(edge){
    //Encodes adjacency condition for edge-edge pairs
    for(int i = 0; i < edgesize - 1; i++) {
      for(int j = i + 1; j < edgesize; j++) {
        std::pair<int, int> e1 = edges[i];
        std::pair<int, int> e2 = edges[j];
        if(e1.first == e2.first || e1.first == e2.second || e1.second == e2.second || e1.second == e2.first) { 
          for(int m = 0; m < k; m++) {
            std::cout << "-" << eindex(i, k, vert) + m << " " << "-" << eindex(j, k, vert) + m << " " << 0 << std::endl;
          }
        }
      }
    }
  }

  if(vertex && triangle) {
    //Encodes adjacency condition for triangle-vertex pairs
    for(int i = 0; i < triupper - 1; i++) {
      std::tuple<int, int, int> t = triangles[i];
      int first  = std::get<0>(t);
      int second = std::get<1>(t);
      int third  = std::get<2>(t);
      for(int j = 0; j < k; j++) {
        std::cout << "-" << vindex(first, k) + j  << " " << "-" << tindex(i, k, vert, edge) + j << " " << 0 << std::endl;
        std::cout << "-" << vindex(second, k) + j << " " << "-" << tindex(i, k, vert, edge) + j << " " << 0 << std::endl;
        std::cout << "-" << vindex(third, k) + j  << " " << "-" << tindex(i, k, vert, edge) + j << " " << 0 << std::endl;
      }
    }
  }

  if(triangle && edge) {
    //Encodes adjacency condition for triangle-edge pairs
    for(int i = 0; i < triupper - 1; i++) {
      std::tuple<int, int, int> t = triangles[i];
      int first  = std::get<0>(t);
      int second = std::get<1>(t);
      int third  = std::get<2>(t);
      std::pair<int, int> e1 = std::make_pair(first, second);
      std::pair<int, int> e2 = std::make_pair(first, third);
      std::pair<int, int> e3 = std::make_pair(second, third);
      int ind1 = std::find(edges.begin(), edges.end(), e1) - edges.begin();
      int ind2 = std::find(edges.begin(), edges.end(), e2) - edges.begin();
      int ind3 = std::find(edges.begin(), edges.end(), e3) - edges.begin();
      for(int j = 0; j < k; j++) {
        std::cout << "-" << eindex(ind1, k, vert) + j << " " << "-" << tindex(i, k, vert, edge) + j << " " << 0 << std::endl;
        std::cout << "-" << eindex(ind2, k, vert) + j << " " << "-" << tindex(i, k, vert, edge) + j << " " << 0 << std::endl;
        std::cout << "-" << eindex(ind3, k, vert) + j << " " << "-" << tindex(i, k, vert, edge) + j << " " << 0 << std::endl;
      }
    }
  }

  if(triangle) {
    //Encodes adjacency for triangle-triangle pairs
    //We say that two triangles are adjacent iff they share an edge or a vertex
    for(int i = 0; i < triupper - 1; i++) {
      for(int j = i + 1; j < triupper; j++) {
        std::tuple<int, int, int> t1 = triangles[i];
        std::tuple<int, int, int> t2 = triangles[j];
        int t1f = std::get<0>(t1);
        int t1s = std::get<1>(t1);
        int t1t = std::get<2>(t1);
        int t2f = std::get<0>(t2);
        int t2s = std::get<1>(t2);
        int t2t = std::get<2>(t2);
        if(t1f == t2f || t1s == t2s || t1t == t2t || t1s == t2f || t1t == t1s 
          || t1f == t2t || t1t == t2f || t1f == t2s || t1s == t2t) {
          for(int m = 0; m < k; m++) {
            std::cout << "-" << tindex(i, k, vert, edge) + m << " " << "-" << tindex(j, k, vert, edge) + m << " " << 0 << std::endl;
          }
        }
      }
    }
  }
}

void break_symmetry () {
  
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

  /*
  *
  *   Program takes following valid inputs:
  *   ./. -v <filename> k   vertex coloring
  *   ./. -e <filename> k   edge coloring
  *   ./. -t <filename> k   triangle coloring
  * 
  *   combinations of the above can be used to 
  *   generate different combinations of colorings
  */

  bool vertex, edge, triangle;

  //Thanks to StackOverflow
  int opt;
  while((opt = getopt(argc, argv, "vet")) != -1) {
    switch(opt) {
      case 'v':
        vertex = true;
        break;
      case 'e':
        edge = true;
        break;
      case 't':
        triangle = true;
        break;
      default:
        fprintf(stderr, "Usage: %s [-vet] [file] [colors]\n", argv[0]);
        exit(-1);
    }
  }
  if(optind == 1) {
    fprintf(stderr, "Usage: %s [-vet] [file] [colors]\n", argv[0]);
    exit(-1);
  }

  int k = std::stoi(argv[optind + 1]);

  freopen(argv[optind], "r", stdin);

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
    i->edges();
    i->triangles();

    std::string outname = argv[optind];
    outname.erase(0, outname.find('/'));
    outname = "CNF/" + outname;
    outname.erase(outname.find("."), outname.length());
    outname.append("G");
    outname.append(std::to_string(count));
    if(vertex) {
      outname.append("v");
    }
    if(edge) {
      outname.append("e");
    }
    if(triangle) {
      outname.append("t");
    }
    outname.append("col");
    outname.append(argv[optind + 1]);
    outname.append(".cnf");

    freopen(outname.c_str(), "w", stdout);

    int counter = 0;

    /*
    *
    *  This formulation allows us to keep track of which variables are for which purpose: 
    *  in essence, if a variable is nk + 1, even n, then it the start of 10 non-sequential
    *  counting variables. The first i->order() of these represent vertices, the rest are
    *  edges
    * 
    */

    //Small optimization
    int verts = i->order();
    int edges = i->edgesize();
    int tris =  i->trianglesize();

    if(vertex) {
      for(int j = 0; j < verts; j++) {
        equals_k(k, counter);
        counter += 2 * k;
      }
    }
    if(edge) {
      for(int j = 0; j < edges; j++) {
        equals_k(k, counter);
        counter += 2 * k;
      }
    }
    if(triangle) {
      for(int j = 0; j < tris; j++) {
        equals_k(k, counter);
        counter += 2 * k;
      }
    }

    col(*i, k, vertex, edge, triangle);

    std::system(("./cnffin " + outname).c_str());

    count++;
  }

}
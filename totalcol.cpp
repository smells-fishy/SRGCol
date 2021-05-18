#include <cstdio>
#include <forward_list>
#include <iostream>
#include <list>
#include <memory>
#include <new>
#include <string>
#include <vector>

class Graph{
  private:
    int  order;
    std::vector<std::list<int> > adjlist;
    std::vector<std::list<int> > colors;
  public:
    Graph(int, int);
    void adj_insert(int, int);
    void color_insert(int, int);
};

Graph::Graph(int o, int col) {
  order = o;
  adjlist[o];
  colors[o];
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

  //Function takes a filename as a command-line input
  freopen(argv[1], "r", stdin);

  std::string outname = argv[1];
  outname.erase(outname.find("g6") - 1, outname.length());
  outname.append("test.txt");

  freopen(outname.c_str(), "w", stdout);

  int order = parse();
  int col = std::stoi(argv[2]);
  std::vector<Graph> graphs;
  while(!std::cin.eof() && std::cin.good()) {
    Graph graph(order, col);
    graphs.insert(graphs.end(), graph);
    for(int i = 0; i < order; i++) {
      for(int j = 0; j < order; j++) {
        int x;
        std::cin >> x;
      }
    }

    std::string skip;
    std::getline(std::cin, skip);
    std::getline(std::cin, skip);
    std::getline(std::cin, skip);
  }

  
  for(Graph i : graphs) {
    
  }

}
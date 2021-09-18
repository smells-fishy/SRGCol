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

std::vector<std::pair<int, int> > edges;
int counter;
int edgec;

inline int seq_counter (int i, int j) {
  return i * j + counter;
}

void encode_regularity (int degmin, int degmax, int verts) {
  
  /* 
  * 
  *  Encoded via the Sinz encoding:
  *  (-x_1 or s_(1,1))
  *    For 1 < i < n:
  *    (-x_i or s_(i,1))
  *    (-s_(i-1,1) or s_(i, 1))
  *      For 1 < j <= k:
  *      (-x_i or -s_(i-1,j-1) or s_(i,j))
  *      (-s_(i-1,j) or s_(i,j))
  *    (-x_i or -s_(i-1,k))
  *  (-x_n or -s_(n-1,k))
  *  (-s_(1,j)) for 1 < j <= k
  * 
  *  The outer for loop encodes the maximal adjacencies of 
  *  vertex i.
  * 
  */
  
  /* Keeps track of if a clause is the first */
  bool first = true;

  /* The first set encodes at most degmax connections */
  for (int v = 1; v <= verts; v++) {
    std::cout << "-" << 1 << " " << seq_counter (1, 1) 
              << " " << 0 << std::endl;
    for (int j = 2; j <= degmax; j++) {
      std::cout << "-" << seq_counter (1, j) 
                << " " << 0 << std::endl;
    }
    for (int i = 2; i < edgec; i++) {
      if (edges[i].first == v || edges[i].second == v) {
        std::cout << "-" << i 
                  << " " << seq_counter (i, 1) 
                  << " " << 0 << std::endl;
        std::cout << "-" << seq_counter (i - 1, 1) 
                  << " " << seq_counter (i, 1) 
                  << " " << 0 << std::endl;
        for (int j = 2; j <= degmax; j++) {
          std::cout << "-" << i << " -" << seq_counter (i - 1, j - 1) 
                    << " " << seq_counter (i, j) 
                    << " " << 0 << std::endl;
          std::cout << "-" << seq_counter (i - 1, j)
                    << " " << seq_counter (i, j) 
                    << " " << 0 << std::endl;
        }
        std::cout << "-" << i + 1 << " "
                  << "-" << seq_counter (i, degmax) 
                  << " " << 0 << std::endl;
      }
      std::cout << "-" << edgec
                << " " << seq_counter (edgec - 1, degmax)
                << " " << 0 << std::endl;
    }
  }

  /* Now, we encode at most degmin non-connections */
  
}

int main (int argc, char *argv[]) {
  
  /*
  *
  *   Program takes following inputs:
  *   ./. m n order degmin degmax
  * 
  */
  
  if (argc != 6) abort();

  int m      = std::stoi(argv[1]); 
  int n      = std::stoi(argv[2]);
  int order  = std::stoi(argv[3]);
  int degmin = std::stoi(argv[4]);
  int degmax = std::stoi(argv[5]);

  /* Generate a vector of all possible edges on a graph of the given order */
  
  for (int i = 0; i < order - 1; i++) {
    for (int j = i + 1; j < order; j++) {
      edges.push_back(std::make_pair(i,j));
    }
  }

  edgec = edges.size();
  counter = edgec;
  
}
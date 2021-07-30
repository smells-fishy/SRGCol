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

void encode_regularity (int degmin, int degmax) {
  
  /* 
  * 
  *  We must encode both at most degmax connections 
  *  and at least degmin connections
  * 
  */

  

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


  
}
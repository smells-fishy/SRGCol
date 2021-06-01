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
#include <sstream>
#include <string>
#include <time.h>
#include <tuple>
#include <utility>
#include <vector>

int main(int argc, char *argv[]) {
  
  //Program takes inputs <filename>
  if(argc < 2) throw 1;

  freopen(argv[1], "r+", stdin);
  //Compute the maximum variable and the number of clauses

  //Copy the entire file
  std::ifstream c(argv[1]);
  std::stringstream buf;
  buf << c.rdbuf();

  int clause = 0;
  int max = 0;
  std::string curr;
  while(std::cin >> curr) {
    if(curr.compare("0") == 0) clause++;
    else{
      int abscurr = std::abs(std::stoi(curr));
      max = std::max(max, abscurr);
    }
  }

  freopen(argv[1], "r+", stdout);
  std::cout << "p cnf " << max << " " << clause << std::endl;

  std::cout << buf.str();
    
}
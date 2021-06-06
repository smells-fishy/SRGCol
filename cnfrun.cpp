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

//Thanks to StackOverflow
int exec(const char* cmd) {
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if(!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return std::stoi(result);
}

int main(int argc, char *argv[]) {

  //The program takes in the inputs ./. N k l m col
  //and runs all cnfs associated with the given parameters

  if(argc < 6) throw 1;

  int col = std::stoi(argv[5]);
  std::string searchname = "CNF/sr" + std::string(argv[1]) + argv[2] + argv[3] + argv[4] +
      + "*" + argv[5] + ".cnf";

  int c = exec(("ls -dq " + searchname + " | wc -l").c_str());

  int sat = 0;

  for(int i = 1; i < c + 1; i++) {
    std::string sourcename = "CNF/sr" + std::string(argv[1]) + argv[2] + argv[3] + argv[4] 
      + "G" + std::to_string(i) + "col" + argv[5] + ".cnf";

    std::string outname = "CNF/sr" + std::string(argv[1]) + argv[2] + argv[3] + argv[4] 
      + "G" + std::to_string(i) + "col" + argv[5] + ".out";
    
    std::system(("./cadical-master/build/cadical -q -w " + outname + " " + sourcename).c_str());
    
    std::ifstream c(outname.c_str());
    std::stringstream out;
    out << c.rdbuf();
    if (out.str().find("s SATISFIABLE") != std::string::npos) sat++;
    else std::cout << "GRAPH " << i << " UNSATSIFIABLE" << std::endl;
  }

  std::string statout = "CNF/sr" + std::string(argv[1]) + argv[2] + argv[3] + argv[4] 
      + "col" + argv[5] + "stat.txt";
  
  freopen(statout.c_str(), "w", stdout);
  std::cout << "SATISFIABLE: " << sat << "/" << c;
}
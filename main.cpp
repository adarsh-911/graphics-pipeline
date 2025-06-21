#include <iostream>
#include "local_test.hpp"

int main(int argc, char* argv[]) {
  
  if (argc != 7) { std::cout << "Format: ./run {position} {direction}" << "\n"; return 1; }
  
  run_localTest(argv);

  return 0;
}
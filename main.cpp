#include "localTest.h"
#include <iostream>

int main(int argc, char* argv[]) {
  
  if (argc != 7) { std::cout << "Format: {position} {direction}" << "\n"; return 1; }
  else run_localTest(argv);
  
  return 0;
}
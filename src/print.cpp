#include <iostream>
#include "print.h"

void print_hello() {
  std::cout << "Hello CMake\n";

#ifdef DEBUG_VERBOSE
  std::cout << "DEBUG_VERBOSE\n";
#endif // DEBUG_VERBOSE
}

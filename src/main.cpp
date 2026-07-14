#include "print.h"

int main() {
  print_hello();
#ifdef DEBUG_VERBOSE
  print_hello();
#endif // DEBUG_VERBOSE
  return 0;
}

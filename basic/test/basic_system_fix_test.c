#include <stdio.h>
#include "basic_num.h"

extern basic_num_t basic_system (const char *cmd);

int main (void) {
  basic_num_t r = basic_system ("exit 7");
  printf ("%ld\n", basic_num_to_int (r));
  return 0;
}

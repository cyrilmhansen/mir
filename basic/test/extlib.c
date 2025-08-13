#include "basic_num.h"
#include <stdio.h>

basic_num_t EXT_ADD (basic_num_t a, basic_num_t b) { return basic_num_add (a, b); }
void EXT_PRINT (basic_num_t x) {
  char buf[64];
  basic_num_to_chars (x, buf, sizeof (buf));
  printf ("%s\n", buf);
}

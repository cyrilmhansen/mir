#include "basic_num.h"
#include <stdio.h>

basic_num_t EXT_ADD (basic_num_t a, basic_num_t b) { return a + b; }
void EXT_PRINT (basic_num_t x) { printf ("%.0f\n", (double) x); }

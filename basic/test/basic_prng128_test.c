#include <assert.h>
#include <math.h>
#include "basic_num.h"

void basic_randomize (basic_num_t n, basic_num_t has_seed);
basic_num_t basic_rnd (basic_num_t n);

int main (void) {
  basic_randomize (basic_num_from_int (1), basic_num_from_int (1));
  assert (fabsl ((long double) basic_rnd (basic_num_from_int (1)) - 0.70292183315885049496L)
          < 1e-18L);
  return 0;
}

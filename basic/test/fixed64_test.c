#include <assert.h>
#include <stdio.h>

#include "fixed64/fixed64.h"

int main (void) {
  fixed64_t half = {.hi = 0, .lo = 1ULL << 63};
  fixed64_t quarter = {.hi = 0, .lo = 1ULL << 62};
  fixed64_t res = fixed64_add (half, quarter);
  assert (res.hi == 0 && res.lo == ((1ULL << 63) + (1ULL << 62)));

  fixed64_t two = fixed64_from_int (2);
  res = fixed64_sub (two, half);
  assert (res.hi == 1 && res.lo == (1ULL << 63));

  res = fixed64_mul (two, half);
  assert (res.hi == 1 && res.lo == 0);

  fixed64_t neg_two = fixed64_neg (two);
  res = fixed64_mul (neg_two, half);
  assert (res.hi == -1 && res.lo == 0);

  res = fixed64_div (two, half);
  assert (res.hi == 4 && res.lo == 0);

  res = fixed64_div (neg_two, half);
  assert (res.hi == -4 && res.lo == 0);

  printf ("fixed64 tests passed\n");
  return 0;
}

#include <assert.h>
#include <math.h>
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
  (void) res;

  fixed64_t zero = fixed64_from_int (0);
  fixed64_t half_pi = {.hi = 1, .lo = 0x921fb54442d18000ULL};
  fixed64_t pi = {.hi = 3, .lo = 0x243f6a8885a30000ULL};
  fixed64_t quarter_pi = {.hi = 0, .lo = 0xc90fdaa22168c000ULL};

  assert (fabs (fixed64_to_double (fixed64_sin (zero))) < 1e-6);
  assert (fabs (fixed64_to_double (fixed64_cos (zero)) - 1.0) < 1e-6);
  assert (fabs (fixed64_to_double (fixed64_sin (half_pi)) - 1.0) < 1e-6);
  assert (fabs (fixed64_to_double (fixed64_cos (half_pi))) < 1e-6);
  assert (fabs (fixed64_to_double (fixed64_sin (pi))) < 1e-6);
  assert (fabs (fixed64_to_double (fixed64_cos (pi)) + 1.0) < 1e-6);
  assert (fabs (fixed64_to_double (fixed64_tan (quarter_pi)) - 1.0) < 1e-6);
  (void) zero;
  (void) half_pi;
  (void) pi;
  (void) quarter_pi;

  printf ("fixed64 tests passed\n");
  return 0;
}

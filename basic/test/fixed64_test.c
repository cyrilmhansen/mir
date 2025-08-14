#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "fixed64/fixed64.h"

int main (void) {
  fixed64_t half = {.hi = 0, .lo = 1ULL << 63};
  fixed64_t quarter = {.hi = 0, .lo = 1ULL << 62};
  fixed64_t two = fixed64_from_int (2);
  assert (fixed64_cmp (fixed64_from_int (1), two) < 0);
  assert (fixed64_cmp (two, fixed64_from_int (1)) > 0);
  assert (fixed64_cmp (two, two) == 0);
  assert (fixed64_eq (two, fixed64_from_int (2)));
  assert (!fixed64_eq (two, fixed64_from_int (3)));
  assert (fixed64_ne (two, fixed64_from_int (3)));
  assert (!fixed64_ne (two, fixed64_from_int (2)));
  assert (fixed64_lt (fixed64_from_int (1), two));
  assert (fixed64_le (fixed64_from_int (1), two));
  assert (fixed64_le (two, two));
  assert (fixed64_gt (two, fixed64_from_int (1)));
  assert (fixed64_ge (two, fixed64_from_int (1)));
  assert (fixed64_ge (two, two));
  assert (fixed64_to_int (two) == 2);
  fixed64_t res = fixed64_add (half, quarter);
  assert (res.hi == 0 && res.lo == ((1ULL << 63) + (1ULL << 62)));
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
  fixed64_t four = fixed64_from_int (4);
  res = fixed64_sqrt (four);
  assert (res.hi == 2 && res.lo == 0);
  fixed64_t three = fixed64_from_int (3);
  res = fixed64_pow (two, three);
  assert (res.hi == 8 && res.lo == 0);

  fixed64_t five_half = fixed64_from_double (5.5);
  res = fixed64_ceil (five_half);
  assert (res.hi == 6 && res.lo == 0);
  fixed64_t neg_five_half = fixed64_from_double (-5.5);
  res = fixed64_ceil (neg_five_half);
  assert (res.hi == -5 && res.lo == 0);
  res = fixed64_trunc (five_half);
  assert (res.hi == 5 && res.lo == 0);
  res = fixed64_round (fixed64_from_double (2.5));
  assert (res.hi == 3 && res.lo == 0);
  res = fixed64_round (fixed64_from_double (-2.5));
  assert (res.hi == -3 && res.lo == 0);

  res = fixed64_fmod (fixed64_from_int (5), fixed64_from_int (2));
  assert (res.hi == 1 && res.lo == 0);

  res = fixed64_atan2 (fixed64_from_int (1), fixed64_from_int (1));
  assert (fabs (fixed64_to_double (res) - fixed64_to_double (quarter_pi)) < 1e-6);

  res = fixed64_log1p (zero);
  assert (res.hi == 0 && res.lo == 0);
  res = fixed64_log1p (fixed64_from_double (exp (1.0) - 1.0));
  assert (fabs (fixed64_to_double (res) - 1.0) < 1e-6);
  res = fixed64_expm1 (fixed64_from_int (1));
  assert (fabs (fixed64_to_double (res) - (exp (1.0) - 1.0)) < 1e-6);

  fixed64_t v = fixed64_from_double (3.75);
  res = fixed64_floor (v);
  assert (res.hi == 3 && res.lo == 0);

  res = fixed64_log (fixed64_from_int (1));
  assert (fabs (fixed64_to_double (res)) < 1e-6);

  res = fixed64_exp (zero);
  assert (res.hi == 1 && res.lo == 0);

  res = fixed64_asin (fixed64_from_double (0.5));
  assert (fabs (fixed64_to_double (res) - asin (0.5)) < 1e-6);

  (void) res;


  /* additional math helpers */
  res = fixed64_log (two);
  assert (fabs (fixed64_to_double (res) - log (2.0)) < 1e-6);
  res = fixed64_log2 (two);
  assert (fabs (fixed64_to_double (res) - 1.0) < 1e-6);
  res = fixed64_log10 (fixed64_from_int (100));
  assert (fabs (fixed64_to_double (res) - 2.0) < 1e-6);
  res = fixed64_exp (fixed64_from_int (1));
  assert (fabs (fixed64_to_double (res) - exp (1.0)) < 1e-6);
  res = fixed64_floor (fixed64_from_double (1.75));
  assert (res.hi == 1 && res.lo == 0);
  res = fixed64_floor (fixed64_from_double (-1.75));
  assert (res.hi == -2 && res.lo == 0);
  res = fixed64_ceil (fixed64_from_double (1.25));
  assert (res.hi == 2 && res.lo == 0);
  res = fixed64_ceil (fixed64_from_double (-1.25));
  assert (res.hi == -1 && res.lo == 0);
  res = fixed64_fmod (fixed64_from_double (5.5), two);
  assert (fabs (fixed64_to_double (res) - fmod (5.5, 2.0)) < 1e-6);

#ifndef _WIN32
  pid_t pid = fork ();
  if (pid == 0) {
    fixed64_stub_unary (two);
    _exit (0);
  } else {
    int status;
    waitpid (pid, &status, 0);
    assert (!(WIFEXITED (status) && WEXITSTATUS (status) == 0));
  }
#endif

  (void) zero;
  (void) half_pi;
  (void) pi;
  (void) quarter_pi;

  printf ("fixed64 tests passed\n");
  return 0;
}

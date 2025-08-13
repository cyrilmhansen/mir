#include "fixed64.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

fixed64_t fixed64_mul (fixed64_t a, fixed64_t b) {
  (void) a;
  (void) b;
  /* TODO: implement 128-bit multiplication using 64-bit parts */
  return fixed64_from_int (0);
}

fixed64_t fixed64_div (fixed64_t a, fixed64_t b) {
  (void) a;
  (void) b;
  /* TODO: implement 128-bit division using 64-bit parts */
  return fixed64_from_int (0);
}

#define FIX64_FRAC 18446744073709551616.0 /* 2^64 */

fixed64_t fixed64_from_double (double d) {
  int neg = d < 0;
  if (neg) d = -d;
  double intpart;
  double frac = modf (d, &intpart);
  fixed64_t r;
  r.hi = (int64_t) intpart;
  r.lo = (uint64_t) (frac * FIX64_FRAC);
  if (neg) r = fixed64_neg (r);
  return r;
}

double fixed64_to_double (fixed64_t x) {
  if (x.hi < 0) {
    fixed64_t t = fixed64_neg (x);
    return -((double) t.hi + (double) t.lo / FIX64_FRAC);
  }
  return (double) x.hi + (double) x.lo / FIX64_FRAC;
}

fixed64_t fixed64_from_string (const char *s, char **endptr) {
  double d = strtod (s, endptr);
  return fixed64_from_double (d);
}

int fixed64_to_string (fixed64_t x, char *buf, size_t size) {
  double d = fixed64_to_double (x);
  return snprintf (buf, size, "%.17g", d);
}

fixed64_t fixed64_abs (fixed64_t x) { return x.hi < 0 ? fixed64_neg (x) : x; }

fixed64_t fixed64_stub_unary (fixed64_t a) {
  (void) a;
  return fixed64_from_int (0);
}

fixed64_t fixed64_stub_binary (fixed64_t a, fixed64_t b) {
  (void) a;
  (void) b;
  return fixed64_from_int (0);
}

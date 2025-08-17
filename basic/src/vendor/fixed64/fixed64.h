#ifndef FIXED64_H
#define FIXED64_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint64_t lo; /* fractional part */
  int64_t hi;  /* integer part */
} fixed64_t;

fixed64_t fixed64_from_int (int64_t i);
fixed64_t fixed64_from_uint (uint64_t u);
int64_t fixed64_to_int (fixed64_t x);
int fixed64_cmp (fixed64_t a, fixed64_t b);
int fixed64_eq (fixed64_t a, fixed64_t b);
int fixed64_ne (fixed64_t a, fixed64_t b);
int fixed64_lt (fixed64_t a, fixed64_t b);
int fixed64_le (fixed64_t a, fixed64_t b);
int fixed64_gt (fixed64_t a, fixed64_t b);
int fixed64_ge (fixed64_t a, fixed64_t b);

static inline fixed64_t fixed64_neg (fixed64_t a) {
  fixed64_t r;
  r.lo = ~a.lo + 1;
  r.hi = ~a.hi + (r.lo == 0 ? 1 : 0);
  return r;
}

static inline fixed64_t fixed64_add (fixed64_t a, fixed64_t b) {
  fixed64_t r;
  r.lo = a.lo + b.lo;
  r.hi = a.hi + b.hi;
  if (r.lo < a.lo) r.hi++;
  return r;
}

static inline fixed64_t fixed64_sub (fixed64_t a, fixed64_t b) {
  fixed64_t r;
  r.lo = a.lo - b.lo;
  r.hi = a.hi - b.hi;
  if (a.lo < b.lo) r.hi--;
  return r;
}

fixed64_t fixed64_mul (fixed64_t a, fixed64_t b);
fixed64_t fixed64_div (fixed64_t a, fixed64_t b);

fixed64_t fixed64_from_double (double d);
double fixed64_to_double (fixed64_t x);
fixed64_t fixed64_from_string (const char *s, char **endptr);
int fixed64_to_string (fixed64_t x, char *buf, size_t size);
fixed64_t fixed64_abs (fixed64_t x);
fixed64_t fixed64_sin (fixed64_t angle);
fixed64_t fixed64_cos (fixed64_t angle);
fixed64_t fixed64_tan (fixed64_t angle);
fixed64_t fixed64_sqrt (fixed64_t a);
fixed64_t fixed64_pow (fixed64_t a, fixed64_t b);
fixed64_t fixed64_floor (fixed64_t a);
fixed64_t fixed64_ceil (fixed64_t a);
fixed64_t fixed64_trunc (fixed64_t a);
fixed64_t fixed64_round (fixed64_t a);
fixed64_t fixed64_fmod (fixed64_t a, fixed64_t b);
fixed64_t fixed64_sinh (fixed64_t a);
fixed64_t fixed64_cosh (fixed64_t a);
fixed64_t fixed64_tanh (fixed64_t a);
fixed64_t fixed64_asinh (fixed64_t a);
fixed64_t fixed64_acosh (fixed64_t a);
fixed64_t fixed64_atanh (fixed64_t a);
fixed64_t fixed64_asin (fixed64_t a);
fixed64_t fixed64_acos (fixed64_t a);
fixed64_t fixed64_atan (fixed64_t a);
fixed64_t fixed64_log (fixed64_t a);
fixed64_t fixed64_log2 (fixed64_t a);
fixed64_t fixed64_log10 (fixed64_t a);
fixed64_t fixed64_exp (fixed64_t a);
fixed64_t fixed64_atan2 (fixed64_t y, fixed64_t x);
fixed64_t fixed64_log1p (fixed64_t a);
fixed64_t fixed64_expm1 (fixed64_t a);

#ifdef __cplusplus
}
#endif

#endif /* FIXED64_H */

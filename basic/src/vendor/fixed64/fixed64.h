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

static inline fixed64_t fixed64_from_int (int64_t i) {
  fixed64_t r;
  r.hi = i;
  r.lo = 0;
  return r;
}

static inline int64_t fixed64_to_int (fixed64_t x) { return x.hi; }

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
fixed64_t fixed64_stub_unary (fixed64_t a);
fixed64_t fixed64_stub_binary (fixed64_t a, fixed64_t b);

#ifdef __cplusplus
}
#endif

#endif /* FIXED64_H */

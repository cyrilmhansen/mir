#ifndef BASIC_NUM_H
#define BASIC_NUM_H

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  BASIC_NUM_MODE_DOUBLE,
  BASIC_NUM_MODE_LONG_DOUBLE,
  BASIC_NUM_MODE_FIXED64,
} basic_num_mode_t;

static inline void basic_num_init (basic_num_mode_t mode) { (void) mode; }

#if defined(BASIC_USE_LONG_DOUBLE)

typedef long double basic_num_t;
#define BASIC_ZERO ((basic_num_t) 0.0L)
#define BASIC_FROM_INT(x) ((basic_num_t) (x))
#define BASIC_TO_INT(x) ((long) (x))
#define BASIC_STRTOF strtold
#define BASIC_NUM_SCANF "%Lf"
#define BASIC_NUM_PRINTF "%.18Lg"
#define BASIC_ADD(a, b) ((a) + (b))
#define BASIC_SUB(a, b) ((a) - (b))
#define BASIC_MUL(a, b) ((a) * (b))
#define BASIC_DIV(a, b) ((a) / (b))
#define BASIC_NEG(a) (-(a))
#define BASIC_EQ(a, b) ((a) == (b))
#define BASIC_NE(a, b) ((a) != (b))
#define BASIC_LT(a, b) ((a) < (b))
#define BASIC_LE(a, b) ((a) <= (b))
#define BASIC_GT(a, b) ((a) > (b))
#define BASIC_GE(a, b) ((a) >= (b))
#define BASIC_FABS fabsl
#define BASIC_SQRT sqrtl
#define BASIC_SIN sinl
#define BASIC_COS cosl
#define BASIC_TAN tanl
#define BASIC_SINH sinhl
#define BASIC_COSH coshl
#define BASIC_TANH tanhl
#define BASIC_ASINH asinhl
#define BASIC_ACOSH acoshl
#define BASIC_ATANH atanhl
#define BASIC_ASIN asinl
#define BASIC_ACOS acosl
#define BASIC_ATAN atanl
#define BASIC_LOG logl
#define BASIC_LOG2 log2l
#define BASIC_LOG10 log10l
#define BASIC_EXP expl
#define BASIC_POW powl
#define BASIC_FLOOR floorl

#elif defined(BASIC_USE_FIXED64)

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include "fixed64/fixed64.h"

typedef fixed64_t basic_num_t;
#define BASIC_ZERO ((basic_num_t) {0, 0})
#define BASIC_FROM_INT(x) fixed64_from_int (x)
#define BASIC_TO_INT(x) fixed64_to_int (x)
#define BASIC_STRTOF fixed64_from_string
#define BASIC_NUM_SCANF "%s"
#define BASIC_NUM_PRINTF "%s"
#define BASIC_ADD(a, b) fixed64_add ((a), (b))
#define BASIC_SUB(a, b) fixed64_sub ((a), (b))
#define BASIC_MUL(a, b) fixed64_mul ((a), (b))
#define BASIC_DIV(a, b) fixed64_div ((a), (b))
#define BASIC_NEG(a) fixed64_neg (a)
#define BASIC_EQ(a, b) (fixed64_cmp ((a), (b)) == 0)
#define BASIC_NE(a, b) (fixed64_cmp ((a), (b)) != 0)
#define BASIC_LT(a, b) (fixed64_cmp ((a), (b)) < 0)
#define BASIC_LE(a, b) (fixed64_cmp ((a), (b)) <= 0)
#define BASIC_GT(a, b) (fixed64_cmp ((a), (b)) > 0)
#define BASIC_GE(a, b) (fixed64_cmp ((a), (b)) >= 0)
#define BASIC_FABS fixed64_abs
#define BASIC_SQRT fixed64_sqrt
#define BASIC_SIN fixed64_sin
#define BASIC_COS fixed64_cos
#define BASIC_TAN fixed64_tan
#define BASIC_SINH fixed64_sinh
#define BASIC_COSH fixed64_cosh
#define BASIC_TANH fixed64_tanh
#define BASIC_ASINH fixed64_asinh
#define BASIC_ACOSH fixed64_acosh
#define BASIC_ATANH fixed64_atanh
#define BASIC_ASIN fixed64_asin
#define BASIC_ACOS fixed64_acos
#define BASIC_ATAN fixed64_atan
#define BASIC_LOG fixed64_log
#define BASIC_LOG2 fixed64_log2
#define BASIC_LOG10 fixed64_log10
#define BASIC_EXP fixed64_exp
#define BASIC_POW fixed64_pow
#define BASIC_FLOOR fixed64_floor

#else /* BASIC_USE_DOUBLE */

typedef double basic_num_t;
#define BASIC_ZERO ((basic_num_t) 0.0)
#define BASIC_FROM_INT(x) ((basic_num_t) (x))
#define BASIC_TO_INT(x) ((long) (x))
#define BASIC_STRTOF strtod
#define BASIC_NUM_SCANF "%lf"
#define BASIC_NUM_PRINTF "%.15g"
#define BASIC_ADD(a, b) ((a) + (b))
#define BASIC_SUB(a, b) ((a) - (b))
#define BASIC_MUL(a, b) ((a) * (b))
#define BASIC_DIV(a, b) ((a) / (b))
#define BASIC_NEG(a) (-(a))
#define BASIC_EQ(a, b) ((a) == (b))
#define BASIC_NE(a, b) ((a) != (b))
#define BASIC_LT(a, b) ((a) < (b))
#define BASIC_LE(a, b) ((a) <= (b))
#define BASIC_GT(a, b) ((a) > (b))
#define BASIC_GE(a, b) ((a) >= (b))
#define BASIC_FABS fabs
#define BASIC_SQRT sqrt
#define BASIC_SIN sin
#define BASIC_COS cos
#define BASIC_TAN tan
#define BASIC_SINH sinh
#define BASIC_COSH cosh
#define BASIC_TANH tanh
#define BASIC_ASINH asinh
#define BASIC_ACOSH acosh
#define BASIC_ATANH atanh
#define BASIC_ASIN asin
#define BASIC_ACOS acos
#define BASIC_ATAN atan
#define BASIC_LOG log
#define BASIC_LOG2 log2
#define BASIC_LOG10 log10
#define BASIC_EXP exp
#define BASIC_POW pow
#define BASIC_FLOOR floor

#endif

static inline basic_num_t basic_num_from_int (long x) { return BASIC_FROM_INT (x); }
static inline basic_num_t basic_num_from_string (const char *s, char **end) {
  return BASIC_STRTOF (s, end);
}
static inline long basic_num_to_int (basic_num_t x) { return BASIC_TO_INT (x); }
static inline basic_num_t basic_num_add (basic_num_t a, basic_num_t b) { return BASIC_ADD (a, b); }
static inline basic_num_t basic_num_sub (basic_num_t a, basic_num_t b) { return BASIC_SUB (a, b); }
static inline basic_num_t basic_num_mul (basic_num_t a, basic_num_t b) { return BASIC_MUL (a, b); }
static inline basic_num_t basic_num_div (basic_num_t a, basic_num_t b) { return BASIC_DIV (a, b); }
static inline basic_num_t basic_num_neg (basic_num_t a) { return BASIC_NEG (a); }
static inline int basic_num_eq (basic_num_t a, basic_num_t b) { return BASIC_EQ (a, b); }
static inline int basic_num_ne (basic_num_t a, basic_num_t b) { return BASIC_NE (a, b); }
static inline int basic_num_lt (basic_num_t a, basic_num_t b) { return BASIC_LT (a, b); }
static inline int basic_num_le (basic_num_t a, basic_num_t b) { return BASIC_LE (a, b); }
static inline int basic_num_gt (basic_num_t a, basic_num_t b) { return BASIC_GT (a, b); }
static inline int basic_num_ge (basic_num_t a, basic_num_t b) { return BASIC_GE (a, b); }
static inline basic_num_t basic_num_fabs (basic_num_t x) { return BASIC_FABS (x); }
static inline basic_num_t basic_num_sqrt (basic_num_t x) { return BASIC_SQRT (x); }
static inline basic_num_t basic_num_sin (basic_num_t x) { return BASIC_SIN (x); }
static inline basic_num_t basic_num_cos (basic_num_t x) { return BASIC_COS (x); }
static inline basic_num_t basic_num_tan (basic_num_t x) { return BASIC_TAN (x); }
static inline basic_num_t basic_num_sinh (basic_num_t x) { return BASIC_SINH (x); }
static inline basic_num_t basic_num_cosh (basic_num_t x) { return BASIC_COSH (x); }
static inline basic_num_t basic_num_tanh (basic_num_t x) { return BASIC_TANH (x); }
static inline basic_num_t basic_num_asinh (basic_num_t x) { return BASIC_ASINH (x); }
static inline basic_num_t basic_num_acosh (basic_num_t x) { return BASIC_ACOSH (x); }
static inline basic_num_t basic_num_atanh (basic_num_t x) { return BASIC_ATANH (x); }
static inline basic_num_t basic_num_asin (basic_num_t x) { return BASIC_ASIN (x); }
static inline basic_num_t basic_num_acos (basic_num_t x) { return BASIC_ACOS (x); }
static inline basic_num_t basic_num_atan (basic_num_t x) { return BASIC_ATAN (x); }
static inline basic_num_t basic_num_log (basic_num_t x) { return BASIC_LOG (x); }
static inline basic_num_t basic_num_log2 (basic_num_t x) { return BASIC_LOG2 (x); }
static inline basic_num_t basic_num_log10 (basic_num_t x) { return BASIC_LOG10 (x); }
static inline basic_num_t basic_num_exp (basic_num_t x) { return BASIC_EXP (x); }
static inline basic_num_t basic_num_pow (basic_num_t x, basic_num_t y) { return BASIC_POW (x, y); }
static inline basic_num_t basic_num_floor (basic_num_t x) { return BASIC_FLOOR (x); }

#if defined(BASIC_USE_FIXED64)
static inline int basic_num_scan (FILE *f, basic_num_t *out) {
  char buf[128], *end;
  if (fgets (buf, sizeof (buf), f) == NULL) return 0;
  errno = 0;
  double d = strtod (buf, &end);
  if (end == buf || errno == ERANGE) return 0;
  while (isspace ((unsigned char) *end)) end++;
  if (*end != '\0') return 0;
  if (d < (double) INT64_MIN || d > (double) INT64_MAX) return 0;
  *out = fixed64_from_double (d);
  return 1;
}
static inline void basic_num_print (FILE *f, basic_num_t x) {
  char buf[128];
  fixed64_to_string (x, buf, sizeof (buf));
  fputs (buf, f);
}
static inline int basic_num_to_chars (basic_num_t x, char *buf, size_t size) {
  return fixed64_to_string (x, buf, size);
}
#else
static inline int basic_num_scan (FILE *f, basic_num_t *out) {
  return fscanf (f, BASIC_NUM_SCANF, out) == 1;
}
static inline void basic_num_print (FILE *f, basic_num_t x) { fprintf (f, BASIC_NUM_PRINTF, x); }
static inline int basic_num_to_chars (basic_num_t x, char *buf, size_t size) {
  return snprintf (buf, size, BASIC_NUM_PRINTF, x);
}
#endif

#endif /* BASIC_NUM_H */

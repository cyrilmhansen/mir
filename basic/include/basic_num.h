#ifndef BASIC_NUM_H
#define BASIC_NUM_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(BASIC_USE_LONG_DOUBLE)
#include "ryu/ld2s.h"
typedef long double basic_num_t;
#define BASIC_NUM_SCANF "%Lf"
#define BASIC_NUM_PRINTF "%.21Lg"
#define BASIC_FROM_INT(x) ((basic_num_t) (x))
#define BASIC_TO_INT(x) ((long) (x))
#define BASIC_ZERO ((basic_num_t) 0.0L)
#define BASIC_ONE ((basic_num_t) 1.0L)
#define BASIC_ADD(a, b) ((a) + (b))
#define BASIC_SUB(a, b) ((a) - (b))
#define BASIC_MUL(a, b) ((a) * (b))
#define BASIC_DIV(a, b) ((a) / (b))
#define BASIC_NEG(a) (-(a))
#define BASIC_LT(a, b) ((a) < (b))
#define BASIC_LE(a, b) ((a) <= (b))
#define BASIC_GT(a, b) ((a) > (b))
#define BASIC_GE(a, b) ((a) >= (b))
#define BASIC_EQ(a, b) ((a) == (b))
#define BASIC_NE(a, b) ((a) != (b))
#define BASIC_STRTOF strtold
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
static inline int basic_num_to_chars (basic_num_t x, char *buf, size_t size) {
  (void) size;
  ld2s_buffered (x, buf);
  char *e = strchr (buf, 'e');
  if (e == NULL) e = strchr (buf, 'E');
  if (e != NULL) {
    long exp = strtol (e + 1, NULL, 10);
    int mant_len = (int) (e - buf);
    char digits[64];
    int digits_len = 0;
    for (int i = 0; i < mant_len; i++)
      if (buf[i] != '.') digits[digits_len++] = buf[i];
    if (exp >= 0) {
      if (exp >= digits_len - 1) {
        memcpy (buf, digits, digits_len);
        for (long k = 0; k < exp - (digits_len - 1); k++) buf[digits_len + k] = '0';
        buf[digits_len + exp - (digits_len - 1)] = '\0';
      } else {
        memcpy (buf, digits, exp + 1);
        buf[exp + 1] = '.';
        memcpy (buf + exp + 2, digits + exp + 1, digits_len - (exp + 1));
        buf[digits_len + 1] = '\0';
      }
    } else {
      long k = -exp;
      buf[0] = '0';
      buf[1] = '.';
      for (long j = 0; j < k - 1; j++) buf[2 + j] = '0';
      memcpy (buf + 1 + k, digits, digits_len);
      buf[1 + k + digits_len] = '\0';
    }
  }
  return (int) strlen (buf);
}

#elif defined(BASIC_USE_FIXED64)
#include "fixed64/fixed64.h"
typedef fixed64_t basic_num_t;
#define BASIC_FROM_INT(x) fixed64_from_int (x)
static inline long BASIC_TO_INT (basic_num_t x) { return fixed64_to_int (x); }
#define BASIC_ZERO ((basic_num_t) {.lo = 0, .hi = 0})
#define BASIC_ONE ((basic_num_t) {.lo = 0, .hi = 1})
static inline basic_num_t BASIC_ADD (basic_num_t a, basic_num_t b) { return fixed64_add (a, b); }
static inline basic_num_t BASIC_SUB (basic_num_t a, basic_num_t b) { return fixed64_sub (a, b); }
static inline basic_num_t BASIC_MUL (basic_num_t a, basic_num_t b) { return fixed64_mul (a, b); }
static inline basic_num_t BASIC_DIV (basic_num_t a, basic_num_t b) { return fixed64_div (a, b); }
static inline basic_num_t BASIC_NEG (basic_num_t a) { return fixed64_neg (a); }
static inline int BASIC_EQ (basic_num_t a, basic_num_t b) { return a.hi == b.hi && a.lo == b.lo; }
static inline int BASIC_NE (basic_num_t a, basic_num_t b) { return !BASIC_EQ (a, b); }
static inline int BASIC_LT (basic_num_t a, basic_num_t b) {
  return a.hi < b.hi || (a.hi == b.hi && a.lo < b.lo);
}
static inline int BASIC_LE (basic_num_t a, basic_num_t b) {
  return BASIC_LT (a, b) || BASIC_EQ (a, b);
}
static inline int BASIC_GT (basic_num_t a, basic_num_t b) { return BASIC_LT (b, a); }
static inline int BASIC_GE (basic_num_t a, basic_num_t b) { return BASIC_LE (b, a); }
#define BASIC_STRTOF fixed64_from_string
#define BASIC_FABS fixed64_abs
#define BASIC_SQRT fixed64_stub_unary
#define BASIC_SIN fixed64_sin
#define BASIC_COS fixed64_cos
#define BASIC_TAN fixed64_tan
#define BASIC_SINH fixed64_stub_unary
#define BASIC_COSH fixed64_stub_unary
#define BASIC_TANH fixed64_stub_unary
#define BASIC_ASINH fixed64_stub_unary
#define BASIC_ACOSH fixed64_stub_unary
#define BASIC_ATANH fixed64_stub_unary
#define BASIC_ASIN fixed64_stub_unary
#define BASIC_ACOS fixed64_stub_unary
#define BASIC_ATAN fixed64_stub_unary
#define BASIC_LOG fixed64_stub_unary
#define BASIC_LOG2 fixed64_stub_unary
#define BASIC_LOG10 fixed64_stub_unary
#define BASIC_EXP fixed64_stub_unary
#define BASIC_POW fixed64_stub_binary
#define BASIC_FLOOR fixed64_stub_unary
static inline int basic_num_to_chars (basic_num_t x, char *buf, size_t size) {
  return fixed64_to_string (x, buf, size);
}
static inline int BASIC_NUM_SCAN (FILE *f, basic_num_t *out) {
  char buf[128];
  if (fgets (buf, sizeof (buf), f) == NULL) return 0;
  *out = fixed64_from_string (buf, NULL);
  return 1;
}
static inline void BASIC_NUM_PRINT (FILE *f, basic_num_t x) {
  char buf[128];
  fixed64_to_string (x, buf, sizeof (buf));
  fputs (buf, f);
}

#elif defined(BASIC_USE_DECIMAL128)
#include <dfp/decimal128.h>
#include <dfp/math.h>
#include <dfp/stdlib.h>
typedef _Decimal128 basic_num_t;
#define BASIC_NUM_SCANF "%DDf"
#define BASIC_NUM_PRINTF "%.34DDg"
#define BASIC_FROM_INT(x) ((basic_num_t) (x))
#define BASIC_TO_INT(x) ((long) (x))
#define BASIC_ZERO ((basic_num_t) 0DD)
#define BASIC_ONE ((basic_num_t) 1DD)
#define BASIC_ADD(a, b) ((a) + (b))
#define BASIC_SUB(a, b) ((a) - (b))
#define BASIC_MUL(a, b) ((a) * (b))
#define BASIC_DIV(a, b) ((a) / (b))
#define BASIC_NEG(a) (-(a))
#define BASIC_LT(a, b) ((a) < (b))
#define BASIC_LE(a, b) ((a) <= (b))
#define BASIC_GT(a, b) ((a) > (b))
#define BASIC_GE(a, b) ((a) >= (b))
#define BASIC_EQ(a, b) ((a) == (b))
#define BASIC_NE(a, b) ((a) != (b))
#define BASIC_STRTOF strtod128
#define BASIC_FABS fabsd128
#define BASIC_SQRT sqrtd128
#define BASIC_SIN sind128
#define BASIC_COS cosd128
#define BASIC_TAN tand128
#define BASIC_SINH sinhd128
#define BASIC_COSH coshd128
#define BASIC_TANH tanhd128
#define BASIC_ASINH asinhd128
#define BASIC_ACOSH acoshd128
#define BASIC_ATANH atanhd128
#define BASIC_ASIN asind128
#define BASIC_ACOS acosd128
#define BASIC_ATAN atand128
#define BASIC_LOG logd128
#define BASIC_LOG2 log2d128
#define BASIC_LOG10 log10d128
#define BASIC_POW powd128
#define BASIC_EXP expd128
#define BASIC_FLOOR floord128
static inline int basic_num_to_chars (basic_num_t x, char *buf, size_t size) {
  (void) size;
  decimal128ToString ((decimal128 *) &x, buf);
  return (int) strlen (buf);
}

#else
#include "ryu/ryu.h"
typedef double basic_num_t;
#define BASIC_NUM_SCANF "%lf"
#define BASIC_NUM_PRINTF "%.15g"
#define BASIC_FROM_INT(x) ((basic_num_t) (x))
#define BASIC_TO_INT(x) ((long) (x))
#define BASIC_ZERO ((basic_num_t) 0.0)
#define BASIC_ONE ((basic_num_t) 1.0)
#define BASIC_ADD(a, b) ((a) + (b))
#define BASIC_SUB(a, b) ((a) - (b))
#define BASIC_MUL(a, b) ((a) * (b))
#define BASIC_DIV(a, b) ((a) / (b))
#define BASIC_NEG(a) (-(a))
#define BASIC_LT(a, b) ((a) < (b))
#define BASIC_LE(a, b) ((a) <= (b))
#define BASIC_GT(a, b) ((a) > (b))
#define BASIC_GE(a, b) ((a) >= (b))
#define BASIC_EQ(a, b) ((a) == (b))
#define BASIC_NE(a, b) ((a) != (b))
#define BASIC_STRTOF strtod
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
static inline int basic_num_to_chars (basic_num_t x, char *buf, size_t size) {
  (void) size;
  d2s_buffered (x, buf);
  char *e = strchr (buf, 'e');
  if (e == NULL) e = strchr (buf, 'E');
  if (e != NULL) {
    long exp = strtol (e + 1, NULL, 10);
    int mant_len = (int) (e - buf);
    char digits[32];
    int digits_len = 0;
    for (int i = 0; i < mant_len; i++)
      if (buf[i] != '.') digits[digits_len++] = buf[i];
    if (exp >= 0) {
      if (exp >= digits_len - 1) {
        memcpy (buf, digits, digits_len);
        for (long k = 0; k < exp - (digits_len - 1); k++) buf[digits_len + k] = '0';
        buf[digits_len + exp - (digits_len - 1)] = '\0';
      } else {
        memcpy (buf, digits, exp + 1);
        buf[exp + 1] = '.';
        memcpy (buf + exp + 2, digits + exp + 1, digits_len - (exp + 1));
        buf[digits_len + 1] = '\0';
      }
    } else {
      long k = -exp;
      buf[0] = '0';
      buf[1] = '.';
      for (long j = 0; j < k - 1; j++) buf[2 + j] = '0';
      memcpy (buf + 1 + k, digits, digits_len);
      buf[1 + k + digits_len] = '\0';
    }
  }
  return (int) strlen (buf);
}
#endif

#if !defined(BASIC_USE_FIXED64)
static inline int BASIC_NUM_SCAN (FILE *f, basic_num_t *out) {
  return fscanf (f, BASIC_NUM_SCANF, out) == 1;
}
static inline void BASIC_NUM_PRINT (FILE *f, basic_num_t x) { fprintf (f, BASIC_NUM_PRINTF, x); }
#endif

#endif /* BASIC_NUM_H */

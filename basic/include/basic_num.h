#ifndef BASIC_NUM_H
#define BASIC_NUM_H

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#include "fixed64/fixed64.h"

typedef union {
  double d;
  long double ld;
  fixed64_t f64;
  uint8_t bytes[16];
} basic_num_t;

typedef struct {
  basic_num_t (*from_int) (long);
  basic_num_t (*from_string) (const char *, char **);
  long (*to_int) (basic_num_t);
  basic_num_t (*add) (basic_num_t, basic_num_t);
  basic_num_t (*sub) (basic_num_t, basic_num_t);
  basic_num_t (*mul) (basic_num_t, basic_num_t);
  basic_num_t (*div) (basic_num_t, basic_num_t);
  basic_num_t (*neg) (basic_num_t);
  int (*eq) (basic_num_t, basic_num_t);
  int (*ne) (basic_num_t, basic_num_t);
  int (*lt) (basic_num_t, basic_num_t);
  int (*le) (basic_num_t, basic_num_t);
  int (*gt) (basic_num_t, basic_num_t);
  int (*ge) (basic_num_t, basic_num_t);
  int (*to_chars) (basic_num_t, char *, size_t);
  int (*scan) (FILE *, basic_num_t *);
  void (*print) (FILE *, basic_num_t);
  basic_num_t (*fabs) (basic_num_t);
  basic_num_t (*sqrt) (basic_num_t);
  basic_num_t (*sin) (basic_num_t);
  basic_num_t (*cos) (basic_num_t);
  basic_num_t (*tan) (basic_num_t);
  basic_num_t (*sinh) (basic_num_t);
  basic_num_t (*cosh) (basic_num_t);
  basic_num_t (*tanh) (basic_num_t);
  basic_num_t (*asinh) (basic_num_t);
  basic_num_t (*acosh) (basic_num_t);
  basic_num_t (*atanh) (basic_num_t);
  basic_num_t (*asin) (basic_num_t);
  basic_num_t (*acos) (basic_num_t);
  basic_num_t (*atan) (basic_num_t);
  basic_num_t (*log) (basic_num_t);
  basic_num_t (*log2) (basic_num_t);
  basic_num_t (*log10) (basic_num_t);
  basic_num_t (*exp) (basic_num_t);
  basic_num_t (*pow) (basic_num_t, basic_num_t);
  basic_num_t (*floor) (basic_num_t);
} basic_num_ops_t;

extern basic_num_ops_t basic_num;

typedef enum {
  BASIC_NUM_MODE_DOUBLE,
  BASIC_NUM_MODE_LONG_DOUBLE,
  BASIC_NUM_MODE_FIXED64,
  BASIC_NUM_MODE_MBASIC5
} basic_num_mode_t;

void basic_num_init (basic_num_mode_t mode);

#define BASIC_ZERO ((basic_num_t) {.bytes = {0}})

static inline basic_num_t basic_num_from_int (long x) { return basic_num.from_int (x); }
static inline basic_num_t basic_num_from_string (const char *s, char **end) {
  return basic_num.from_string (s, end);
}
static inline long basic_num_to_int (basic_num_t x) { return basic_num.to_int (x); }
static inline basic_num_t basic_num_add (basic_num_t a, basic_num_t b) {
  return basic_num.add (a, b);
}
static inline basic_num_t basic_num_sub (basic_num_t a, basic_num_t b) {
  return basic_num.sub (a, b);
}
static inline basic_num_t basic_num_mul (basic_num_t a, basic_num_t b) {
  return basic_num.mul (a, b);
}

#define BASIC_NUM_SCANF(f, out) basic_num_scan ((f), (out))
#define BASIC_NUM_PRINTF(f, x) basic_num_print ((f), (x))

#elif defined(BASIC_USE_DECIMAL128) || defined(BASIC_USE_MSFP)
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

#else /* BASIC_USE_DOUBLE or default */
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

static inline basic_num_t basic_num_div (basic_num_t a, basic_num_t b) {
  return basic_num.div (a, b);
}
static inline basic_num_t basic_num_neg (basic_num_t a) { return basic_num.neg (a); }
static inline int basic_num_eq (basic_num_t a, basic_num_t b) { return basic_num.eq (a, b); }
static inline int basic_num_ne (basic_num_t a, basic_num_t b) { return basic_num.ne (a, b); }
static inline int basic_num_lt (basic_num_t a, basic_num_t b) { return basic_num.lt (a, b); }
static inline int basic_num_le (basic_num_t a, basic_num_t b) { return basic_num.le (a, b); }
static inline int basic_num_gt (basic_num_t a, basic_num_t b) { return basic_num.gt (a, b); }
static inline int basic_num_ge (basic_num_t a, basic_num_t b) { return basic_num.ge (a, b); }

static inline int basic_num_to_chars (basic_num_t x, char *buf, size_t size) {
  return basic_num.to_chars (x, buf, size);
}
static inline int basic_num_scan (FILE *f, basic_num_t *out) { return basic_num.scan (f, out); }
static inline void basic_num_print (FILE *f, basic_num_t x) { basic_num.print (f, x); }

static inline basic_num_t basic_num_fabs (basic_num_t x) { return basic_num.fabs (x); }
static inline basic_num_t basic_num_sqrt (basic_num_t x) { return basic_num.sqrt (x); }
static inline basic_num_t basic_num_sin (basic_num_t x) { return basic_num.sin (x); }
static inline basic_num_t basic_num_cos (basic_num_t x) { return basic_num.cos (x); }
static inline basic_num_t basic_num_tan (basic_num_t x) { return basic_num.tan (x); }
static inline basic_num_t basic_num_sinh (basic_num_t x) { return basic_num.sinh (x); }
static inline basic_num_t basic_num_cosh (basic_num_t x) { return basic_num.cosh (x); }
static inline basic_num_t basic_num_tanh (basic_num_t x) { return basic_num.tanh (x); }
static inline basic_num_t basic_num_asinh (basic_num_t x) { return basic_num.asinh (x); }
static inline basic_num_t basic_num_acosh (basic_num_t x) { return basic_num.acosh (x); }
static inline basic_num_t basic_num_atanh (basic_num_t x) { return basic_num.atanh (x); }
static inline basic_num_t basic_num_asin (basic_num_t x) { return basic_num.asin (x); }
static inline basic_num_t basic_num_acos (basic_num_t x) { return basic_num.acos (x); }
static inline basic_num_t basic_num_atan (basic_num_t x) { return basic_num.atan (x); }
static inline basic_num_t basic_num_log (basic_num_t x) { return basic_num.log (x); }
static inline basic_num_t basic_num_log2 (basic_num_t x) { return basic_num.log2 (x); }
static inline basic_num_t basic_num_log10 (basic_num_t x) { return basic_num.log10 (x); }
static inline basic_num_t basic_num_exp (basic_num_t x) { return basic_num.exp (x); }
static inline basic_num_t basic_num_pow (basic_num_t x, basic_num_t y) {
  return basic_num.pow (x, y);
}
static inline basic_num_t basic_num_floor (basic_num_t x) { return basic_num.floor (x); }

#endif /* BASIC_NUM_H */

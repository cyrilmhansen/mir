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

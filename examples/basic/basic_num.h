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
#define BASIC_ATAN atanl
#define BASIC_LOG logl
#define BASIC_LOG2 log2l
#define BASIC_LOG10 log10l
#define BASIC_EXP expl
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
#elif defined(BASIC_USE_DECIMAL128)
#include <dfp/decimal128.h>
#include <dfp/math.h>
#include <dfp/stdlib.h>
typedef _Decimal128 basic_num_t;
#define BASIC_NUM_SCANF "%DDf"
#define BASIC_NUM_PRINTF "%.34DDg"
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
#define BASIC_ATAN atand128
#define BASIC_LOG logd128
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
#define BASIC_ATAN atan
#define BASIC_LOG log
#define BASIC_LOG2 log2
#define BASIC_LOG10 log10
#define BASIC_EXP exp
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

#endif /* BASIC_NUM_H */

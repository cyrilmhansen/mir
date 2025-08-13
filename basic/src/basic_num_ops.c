#include "basic_num_ops.h"
#include <math.h>
#include <string.h>
#include <stdint.h>

/* ---- double backend ---- */
static void dbl_from_int (long x, void *out) { *(double *) out = (double) x; }
static long dbl_to_int (const void *a) { return (long) *(const double *) a; }
static void dbl_add (void *r, const void *a, const void *b) {
  *(double *) r = *(const double *) a + *(const double *) b;
}
static void dbl_sub (void *r, const void *a, const void *b) {
  *(double *) r = *(const double *) a - *(const double *) b;
}
static void dbl_mul (void *r, const void *a, const void *b) {
  *(double *) r = *(const double *) a * *(const double *) b;
}
static void dbl_div (void *r, const void *a, const void *b) {
  *(double *) r = *(const double *) a / *(const double *) b;
}
static void dbl_neg (void *r, const void *a) { *(double *) r = -*(const double *) a; }
static int dbl_eq (const void *a, const void *b) {
  return *(const double *) a == *(const double *) b;
}
static int dbl_ne (const void *a, const void *b) { return !dbl_eq (a, b); }
static int dbl_lt (const void *a, const void *b) {
  return *(const double *) a < *(const double *) b;
}
static int dbl_le (const void *a, const void *b) {
  return *(const double *) a <= *(const double *) b;
}
static int dbl_gt (const void *a, const void *b) {
  return *(const double *) a > *(const double *) b;
}
static int dbl_ge (const void *a, const void *b) {
  return *(const double *) a >= *(const double *) b;
}
static int dbl_scan (FILE *f, void *out) { return fscanf (f, "%lf", (double *) out) == 1; }
static void dbl_print (FILE *f, const void *val) { fprintf (f, "%.15g", *(const double *) val); }
static const basic_num_ops dbl_ops = {
  sizeof (double), _Alignof (double), dbl_from_int, dbl_to_int, dbl_add, dbl_sub, dbl_mul,
  dbl_div,         dbl_neg,           dbl_eq,       dbl_ne,     dbl_lt,  dbl_le,  dbl_gt,
  dbl_ge,          dbl_scan,          dbl_print,
};

/* ---- long double backend ---- */
static void ldb_from_int (long x, void *out) { *(long double *) out = (long double) x; }
static long ldb_to_int (const void *a) { return (long) *(const long double *) a; }
static void ldb_add (void *r, const void *a, const void *b) {
  *(long double *) r = *(const long double *) a + *(const long double *) b;
}
static void ldb_sub (void *r, const void *a, const void *b) {
  *(long double *) r = *(const long double *) a - *(const long double *) b;
}
static void ldb_mul (void *r, const void *a, const void *b) {
  *(long double *) r = *(const long double *) a * *(const long double *) b;
}
static void ldb_div (void *r, const void *a, const void *b) {
  *(long double *) r = *(const long double *) a / *(const long double *) b;
}
static void ldb_neg (void *r, const void *a) { *(long double *) r = -*(const long double *) a; }
static int ldb_eq (const void *a, const void *b) {
  return *(const long double *) a == *(const long double *) b;
}
static int ldb_ne (const void *a, const void *b) { return !ldb_eq (a, b); }
static int ldb_lt (const void *a, const void *b) {
  return *(const long double *) a < *(const long double *) b;
}
static int ldb_le (const void *a, const void *b) {
  return *(const long double *) a <= *(const long double *) b;
}
static int ldb_gt (const void *a, const void *b) {
  return *(const long double *) a > *(const long double *) b;
}
static int ldb_ge (const void *a, const void *b) {
  return *(const long double *) a >= *(const long double *) b;
}
static int ldb_scan (FILE *f, void *out) { return fscanf (f, "%Lf", (long double *) out) == 1; }
static void ldb_print (FILE *f, const void *val) {
  fprintf (f, "%.21Lg", *(const long double *) val);
}
static const basic_num_ops ldb_ops = {
  sizeof (long double),
  _Alignof (long double),
  ldb_from_int,
  ldb_to_int,
  ldb_add,
  ldb_sub,
  ldb_mul,
  ldb_div,
  ldb_neg,
  ldb_eq,
  ldb_ne,
  ldb_lt,
  ldb_le,
  ldb_gt,
  ldb_ge,
  ldb_scan,
  ldb_print,
};

/* ---- fixed64 backend ---- */
static void fx_from_int (long x, void *out) { *(fixed64_t *) out = fixed64_from_int (x); }
static long fx_to_int (const void *a) { return fixed64_to_int (*(const fixed64_t *) a); }
static void fx_add (void *r, const void *a, const void *b) {
  *(fixed64_t *) r = fixed64_add (*(const fixed64_t *) a, *(const fixed64_t *) b);
}
static void fx_sub (void *r, const void *a, const void *b) {
  *(fixed64_t *) r = fixed64_sub (*(const fixed64_t *) a, *(const fixed64_t *) b);
}
static void fx_mul (void *r, const void *a, const void *b) {
  *(fixed64_t *) r = fixed64_mul (*(const fixed64_t *) a, *(const fixed64_t *) b);
}
static void fx_div (void *r, const void *a, const void *b) {
  *(fixed64_t *) r = fixed64_div (*(const fixed64_t *) a, *(const fixed64_t *) b);
}
static void fx_neg (void *r, const void *a) {
  *(fixed64_t *) r = fixed64_neg (*(const fixed64_t *) a);
}
static int fx_eq (const void *a, const void *b) {
  return fixed64_cmp (*(const fixed64_t *) a, *(const fixed64_t *) b) == 0;
}
static int fx_ne (const void *a, const void *b) {
  return fixed64_cmp (*(const fixed64_t *) a, *(const fixed64_t *) b) != 0;
}
static int fx_lt (const void *a, const void *b) {
  return fixed64_cmp (*(const fixed64_t *) a, *(const fixed64_t *) b) < 0;
}
static int fx_le (const void *a, const void *b) {
  return fixed64_cmp (*(const fixed64_t *) a, *(const fixed64_t *) b) <= 0;
}
static int fx_gt (const void *a, const void *b) {
  return fixed64_cmp (*(const fixed64_t *) a, *(const fixed64_t *) b) > 0;
}
static int fx_ge (const void *a, const void *b) {
  return fixed64_cmp (*(const fixed64_t *) a, *(const fixed64_t *) b) >= 0;
}
static int fx_scan (FILE *f, void *out) {
  char buf[128];
  if (fgets (buf, sizeof (buf), f) == NULL) return 0;
  char *end;
  *(fixed64_t *) out = fixed64_from_string (buf, &end);
  return end != buf;
}
static void fx_print (FILE *f, const void *val) {
  char buf[128];
  fixed64_to_string (*(const fixed64_t *) val, buf, sizeof (buf));
  fputs (buf, f);
}
static const basic_num_ops fx_ops = {
  sizeof (fixed64_t),
  _Alignof (fixed64_t),
  fx_from_int,
  fx_to_int,
  fx_add,
  fx_sub,
  fx_mul,
  fx_div,
  fx_neg,
  fx_eq,
  fx_ne,
  fx_lt,
  fx_le,
  fx_gt,
  fx_ge,
  fx_scan,
  fx_print,
};

/* ---- MBF 5-byte backend ---- */
static double mbf_to_double (const unsigned char *m) {
  if (m[0] == 0) return 0.0;
  int exp = m[0] - 129;
  uint32_t mant
    = ((uint32_t) (m[1] & 0x7F) << 24) | ((uint32_t) m[2] << 16) | ((uint32_t) m[3] << 8) | m[4];
  double frac = 1.0 + mant / 2147483648.0; /* 2^31 */
  double val = ldexp (frac, exp - 31);
  if (m[1] & 0x80) val = -val;
  return val;
}

static void double_to_mbf (double val, unsigned char *m) {
  if (val == 0.0) {
    memset (m, 0, 5);
    return;
  }
  int sign = val < 0;
  double a = fabs (val);
  int exp;
  double frac = frexp (a, &exp); /* a = frac * 2^exp, frac in [0.5,1) */
  uint32_t mant = (uint32_t) ((frac * 2 - 1.0) * 2147483648.0 + 0.5);
  m[0] = (unsigned char) (exp + 128);
  m[1] = (unsigned char) ((sign ? 0x80 : 0) | ((mant >> 24) & 0x7F));
  m[2] = (unsigned char) (mant >> 16);
  m[3] = (unsigned char) (mant >> 8);
  m[4] = (unsigned char) mant;
}

static void mbf_from_int (long x, void *out) { double_to_mbf ((double) x, (unsigned char *) out); }
static long mbf_to_int (const void *a) { return (long) mbf_to_double ((const unsigned char *) a); }
static void mbf_binary (void *r, const void *a, const void *b, double (*op) (double, double)) {
  double ra = mbf_to_double ((const unsigned char *) a);
  double rb = mbf_to_double ((const unsigned char *) b);
  double_to_mbf (op (ra, rb), (unsigned char *) r);
}
static double add_d (double a, double b) { return a + b; }
static double sub_d (double a, double b) { return a - b; }
static double mul_d (double a, double b) { return a * b; }
static double div_d (double a, double b) { return a / b; }
static void mbf_add (void *r, const void *a, const void *b) { mbf_binary (r, a, b, add_d); }
static void mbf_sub (void *r, const void *a, const void *b) { mbf_binary (r, a, b, sub_d); }
static void mbf_mul (void *r, const void *a, const void *b) { mbf_binary (r, a, b, mul_d); }
static void mbf_div (void *r, const void *a, const void *b) { mbf_binary (r, a, b, div_d); }
static void mbf_neg (void *r, const void *a) {
  double_to_mbf (-mbf_to_double ((const unsigned char *) a), (unsigned char *) r);
}
static int mbf_eq (const void *a, const void *b) {
  return mbf_to_double ((const unsigned char *) a) == mbf_to_double ((const unsigned char *) b);
}
static int mbf_ne (const void *a, const void *b) {
  return mbf_to_double ((const unsigned char *) a) != mbf_to_double ((const unsigned char *) b);
}
static int mbf_lt (const void *a, const void *b) {
  return mbf_to_double ((const unsigned char *) a) < mbf_to_double ((const unsigned char *) b);
}
static int mbf_le (const void *a, const void *b) {
  return mbf_to_double ((const unsigned char *) a) <= mbf_to_double ((const unsigned char *) b);
}
static int mbf_gt (const void *a, const void *b) {
  return mbf_to_double ((const unsigned char *) a) > mbf_to_double ((const unsigned char *) b);
}
static int mbf_ge (const void *a, const void *b) {
  return mbf_to_double ((const unsigned char *) a) >= mbf_to_double ((const unsigned char *) b);
}
static int mbf_scan (FILE *f, void *out) {
  double d;
  if (fscanf (f, "%lf", &d) != 1) return 0;
  double_to_mbf (d, (unsigned char *) out);
  return 1;
}
static void mbf_print (FILE *f, const void *val) { fprintf (f, "%.15g", mbf_to_double (val)); }
static const basic_num_ops mbf_ops = {
  5,      1,      mbf_from_int, mbf_to_int, mbf_add, mbf_sub, mbf_mul,  mbf_div,   mbf_neg,
  mbf_eq, mbf_ne, mbf_lt,       mbf_le,     mbf_gt,  mbf_ge,  mbf_scan, mbf_print,
};

/* ---- global ---- */
const basic_num_ops *basic_curr_num_ops = &dbl_ops;

void basic_set_num_mode (basic_num_mode mode) {
  switch (mode) {
  case BASIC_NUM_MODE_LONG_DOUBLE: basic_curr_num_ops = &ldb_ops; break;
  case BASIC_NUM_MODE_FIXED64: basic_curr_num_ops = &fx_ops; break;
  case BASIC_NUM_MODE_MBF: basic_curr_num_ops = &mbf_ops; break;
  case BASIC_NUM_MODE_DOUBLE:
  default: basic_curr_num_ops = &dbl_ops; break;
  }
}

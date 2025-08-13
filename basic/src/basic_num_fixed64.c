#include <stdlib.h>
#include <string.h>

#include "basic_num.h"
#include "fixed64/fixed64.h"

static basic_num_t from_int (long x) {
  basic_num_t r;
  r.f64 = fixed64_from_int (x);
  return r;
}
static basic_num_t from_string (const char *s, char **end) {
  basic_num_t r;
  r.f64 = fixed64_from_string (s, end);
  return r;
}
static long to_int (basic_num_t x) { return fixed64_to_int (x.f64); }
static basic_num_t add (basic_num_t a, basic_num_t b) {
  basic_num_t r;
  r.f64 = fixed64_add (a.f64, b.f64);
  return r;
}
static basic_num_t sub (basic_num_t a, basic_num_t b) {
  basic_num_t r;
  r.f64 = fixed64_sub (a.f64, b.f64);
  return r;
}
static basic_num_t mul (basic_num_t a, basic_num_t b) {
  basic_num_t r;
  r.f64 = fixed64_mul (a.f64, b.f64);
  return r;
}
static basic_num_t divi (basic_num_t a, basic_num_t b) {
  basic_num_t r;
  r.f64 = fixed64_div (a.f64, b.f64);
  return r;
}
static basic_num_t neg (basic_num_t a) {
  basic_num_t r;
  r.f64 = fixed64_neg (a.f64);
  return r;
}
static int eq (basic_num_t a, basic_num_t b) { return fixed64_cmp (a.f64, b.f64) == 0; }
static int ne (basic_num_t a, basic_num_t b) { return fixed64_cmp (a.f64, b.f64) != 0; }
static int lt (basic_num_t a, basic_num_t b) { return fixed64_cmp (a.f64, b.f64) < 0; }
static int le (basic_num_t a, basic_num_t b) { return fixed64_cmp (a.f64, b.f64) <= 0; }
static int gt (basic_num_t a, basic_num_t b) { return fixed64_cmp (a.f64, b.f64) > 0; }
static int ge (basic_num_t a, basic_num_t b) { return fixed64_cmp (a.f64, b.f64) >= 0; }
static int to_chars (basic_num_t x, char *buf, size_t size) {
  return fixed64_to_string (x.f64, buf, size);
}
static int scan (FILE *f, basic_num_t *out) {
  char buf[128], *end;
  if (fgets (buf, sizeof (buf), f) == NULL) return 0;
  out->f64 = fixed64_from_string (buf, &end);
  if (end == buf) return 0;
  return 1;
}
static void print (FILE *f, basic_num_t x) {
  char buf[128];
  fixed64_to_string (x.f64, buf, sizeof (buf));
  fputs (buf, f);
}
static basic_num_t fabs_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_abs (x.f64);
  return r;
}
static basic_num_t sqrt_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_sqrt (x.f64);
  return r;
}
static basic_num_t sin_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_sin (x.f64);
  return r;
}
static basic_num_t cos_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_cos (x.f64);
  return r;
}
static basic_num_t tan_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_tan (x.f64);
  return r;
}
static basic_num_t sinh_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_sinh (x.f64);
  return r;
}
static basic_num_t cosh_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_cosh (x.f64);
  return r;
}
static basic_num_t tanh_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_tanh (x.f64);
  return r;
}
static basic_num_t asinh_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_asinh (x.f64);
  return r;
}
static basic_num_t acosh_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_acosh (x.f64);
  return r;
}
static basic_num_t atanh_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_atanh (x.f64);
  return r;
}
static basic_num_t asin_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_asin (x.f64);
  return r;
}
static basic_num_t acos_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_acos (x.f64);
  return r;
}
static basic_num_t atan_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_atan (x.f64);
  return r;
}
static basic_num_t log_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_log (x.f64);
  return r;
}
static basic_num_t log2_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_log2 (x.f64);
  return r;
}
static basic_num_t log10_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_log10 (x.f64);
  return r;
}
static basic_num_t exp_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_exp (x.f64);
  return r;
}
static basic_num_t pow_ (basic_num_t x, basic_num_t y) {
  basic_num_t r;
  r.f64 = fixed64_pow (x.f64, y.f64);
  return r;
}
static basic_num_t floor_ (basic_num_t x) {
  basic_num_t r;
  r.f64 = fixed64_floor (x.f64);
  return r;
}

void basic_num_init_fixed64 (void) {
  basic_num.from_int = from_int;
  basic_num.from_string = from_string;
  basic_num.to_int = to_int;
  basic_num.add = add;
  basic_num.sub = sub;
  basic_num.mul = mul;
  basic_num.div = divi;
  basic_num.neg = neg;
  basic_num.eq = eq;
  basic_num.ne = ne;
  basic_num.lt = lt;
  basic_num.le = le;
  basic_num.gt = gt;
  basic_num.ge = ge;
  basic_num.to_chars = to_chars;
  basic_num.scan = scan;
  basic_num.print = print;
  basic_num.fabs = fabs_;
  basic_num.sqrt = sqrt_;
  basic_num.sin = sin_;
  basic_num.cos = cos_;
  basic_num.tan = tan_;
  basic_num.sinh = sinh_;
  basic_num.cosh = cosh_;
  basic_num.tanh = tanh_;
  basic_num.asinh = asinh_;
  basic_num.acosh = acosh_;
  basic_num.atanh = atanh_;
  basic_num.asin = asin_;
  basic_num.acos = acos_;
  basic_num.atan = atan_;
  basic_num.log = log_;
  basic_num.log2 = log2_;
  basic_num.log10 = log10_;
  basic_num.exp = exp_;
  basic_num.pow = pow_;
  basic_num.floor = floor_;
}

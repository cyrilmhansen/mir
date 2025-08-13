#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "basic_num.h"
#include "ryu/ld2s.h"

static basic_num_t from_int (long x) {
  basic_num_t r;
  r.ld = (long double) x;
  return r;
}
static basic_num_t from_string (const char *s, char **end) {
  basic_num_t r;
  r.ld = strtold (s, end);
  return r;
}
static long to_int (basic_num_t x) { return (long) x.ld; }
static basic_num_t add (basic_num_t a, basic_num_t b) {
  basic_num_t r;
  r.ld = a.ld + b.ld;
  return r;
}
static basic_num_t sub (basic_num_t a, basic_num_t b) {
  basic_num_t r;
  r.ld = a.ld - b.ld;
  return r;
}
static basic_num_t mul (basic_num_t a, basic_num_t b) {
  basic_num_t r;
  r.ld = a.ld * b.ld;
  return r;
}
static basic_num_t divi (basic_num_t a, basic_num_t b) {
  basic_num_t r;
  r.ld = a.ld / b.ld;
  return r;
}
static basic_num_t neg (basic_num_t a) {
  basic_num_t r;
  r.ld = -a.ld;
  return r;
}
static int eq (basic_num_t a, basic_num_t b) { return a.ld == b.ld; }
static int ne (basic_num_t a, basic_num_t b) { return a.ld != b.ld; }
static int lt (basic_num_t a, basic_num_t b) { return a.ld < b.ld; }
static int le (basic_num_t a, basic_num_t b) { return a.ld <= b.ld; }
static int gt (basic_num_t a, basic_num_t b) { return a.ld > b.ld; }
static int ge (basic_num_t a, basic_num_t b) { return a.ld >= b.ld; }
static int to_chars (basic_num_t x, char *buf, size_t size) {
  (void) size;
  ld2s_buffered (x.ld, buf);
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
static int scan (FILE *f, basic_num_t *out) {
  char buf[128], *end;
  if (fgets (buf, sizeof (buf), f) == NULL) return 0;
  out->ld = strtold (buf, &end);
  if (end == buf) return 0;
  return 1;
}
static void print (FILE *f, basic_num_t x) { fprintf (f, "%.21Lg", x.ld); }
static basic_num_t fabs_ (basic_num_t x) {
  basic_num_t r;
  r.ld = fabsl (x.ld);
  return r;
}
static basic_num_t sqrt_ (basic_num_t x) {
  basic_num_t r;
  r.ld = sqrtl (x.ld);
  return r;
}
static basic_num_t sin_ (basic_num_t x) {
  basic_num_t r;
  r.ld = sinl (x.ld);
  return r;
}
static basic_num_t cos_ (basic_num_t x) {
  basic_num_t r;
  r.ld = cosl (x.ld);
  return r;
}
static basic_num_t tan_ (basic_num_t x) {
  basic_num_t r;
  r.ld = tanl (x.ld);
  return r;
}
static basic_num_t sinh_ (basic_num_t x) {
  basic_num_t r;
  r.ld = sinhl (x.ld);
  return r;
}
static basic_num_t cosh_ (basic_num_t x) {
  basic_num_t r;
  r.ld = coshl (x.ld);
  return r;
}
static basic_num_t tanh_ (basic_num_t x) {
  basic_num_t r;
  r.ld = tanhl (x.ld);
  return r;
}
static basic_num_t asinh_ (basic_num_t x) {
  basic_num_t r;
  r.ld = asinhl (x.ld);
  return r;
}
static basic_num_t acosh_ (basic_num_t x) {
  basic_num_t r;
  r.ld = acoshl (x.ld);
  return r;
}
static basic_num_t atanh_ (basic_num_t x) {
  basic_num_t r;
  r.ld = atanhl (x.ld);
  return r;
}
static basic_num_t asin_ (basic_num_t x) {
  basic_num_t r;
  r.ld = asinl (x.ld);
  return r;
}
static basic_num_t acos_ (basic_num_t x) {
  basic_num_t r;
  r.ld = acosl (x.ld);
  return r;
}
static basic_num_t atan_ (basic_num_t x) {
  basic_num_t r;
  r.ld = atanl (x.ld);
  return r;
}
static basic_num_t log_ (basic_num_t x) {
  basic_num_t r;
  r.ld = logl (x.ld);
  return r;
}
static basic_num_t log2_ (basic_num_t x) {
  basic_num_t r;
  r.ld = log2l (x.ld);
  return r;
}
static basic_num_t log10_ (basic_num_t x) {
  basic_num_t r;
  r.ld = log10l (x.ld);
  return r;
}
static basic_num_t exp_ (basic_num_t x) {
  basic_num_t r;
  r.ld = expl (x.ld);
  return r;
}
static basic_num_t pow_ (basic_num_t x, basic_num_t y) {
  basic_num_t r;
  r.ld = powl (x.ld, y.ld);
  return r;
}
static basic_num_t floor_ (basic_num_t x) {
  basic_num_t r;
  r.ld = floorl (x.ld);
  return r;
}

void basic_num_init_long_double (void) {
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

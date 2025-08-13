#include "fixed64.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void mul64 (uint64_t a, uint64_t b, uint64_t *hi, uint64_t *lo) {
  uint64_t a0 = (uint32_t) a, a1 = a >> 32;
  uint64_t b0 = (uint32_t) b, b1 = b >> 32;
  uint64_t p0 = a0 * b0;
  uint64_t p1 = a0 * b1;
  uint64_t p2 = a1 * b0;
  uint64_t p3 = a1 * b1;
  uint64_t mid = (p0 >> 32) + (uint32_t) p1 + (uint32_t) p2;
  uint64_t mid_lo = mid & 0xffffffffu;
  uint64_t mid_hi = mid >> 32;
  *lo = (p0 & 0xffffffffu) | (mid_lo << 32);
  *hi = p3 + (p1 >> 32) + (p2 >> 32) + mid_hi;
}

static void add128 (uint64_t *hi, uint64_t *lo, uint64_t ah, uint64_t al) {
  uint64_t l = *lo + al;
  *hi = *hi + ah + (l < *lo);
  *lo = l;
}

static void sub128 (uint64_t *hi, uint64_t *lo, uint64_t bh, uint64_t bl) {
  uint64_t l = *lo - bl;
  *hi = *hi - bh - (*lo < bl);
  *lo = l;
}

static int ge128 (uint64_t ah, uint64_t al, uint64_t bh, uint64_t bl) {
  return ah > bh || (ah == bh && al >= bl);
}

fixed64_t fixed64_mul (fixed64_t a, fixed64_t b) {
  int neg = (a.hi < 0) ^ (b.hi < 0);
  fixed64_t aa = fixed64_abs (a);
  fixed64_t bb = fixed64_abs (b);
  uint64_t alo = aa.lo, ahi = (uint64_t) aa.hi;
  uint64_t blo = bb.lo, bhi = (uint64_t) bb.hi;
  uint64_t p0_hi, p0_lo, p1_hi, p1_lo, p2_hi, p2_lo, p3_hi, p3_lo;
  mul64 (alo, blo, &p0_hi, &p0_lo);
  mul64 (ahi, blo, &p1_hi, &p1_lo);
  mul64 (alo, bhi, &p2_hi, &p2_lo);
  mul64 (ahi, bhi, &p3_hi, &p3_lo);
  uint64_t res_hi = p3_lo, res_lo = 0;
  add128 (&res_hi, &res_lo, p1_hi, p1_lo);
  add128 (&res_hi, &res_lo, p2_hi, p2_lo);
  add128 (&res_hi, &res_lo, 0, p0_hi);
  fixed64_t r;
  r.hi = (int64_t) res_hi;
  r.lo = res_lo;
  return neg ? fixed64_neg (r) : r;
}

fixed64_t fixed64_div (fixed64_t a, fixed64_t b) {
  if (b.hi == 0 && b.lo == 0) return fixed64_from_int (0);
  int neg = (a.hi < 0) ^ (b.hi < 0);
  fixed64_t aa = fixed64_abs (a);
  fixed64_t bb = fixed64_abs (b);

  uint64_t n2 = (uint64_t) aa.hi; /* highest */
  uint64_t n1 = aa.lo;            /* middle */
  uint64_t n0 = 0;                /* lowest */
  uint64_t d1 = (uint64_t) bb.hi;
  uint64_t d0 = bb.lo;

  uint64_t q1 = 0, q0 = 0; /* quotient */
  uint64_t r1 = 0, r0 = 0; /* remainder */

  for (int i = 0; i < 192; i++) {
    uint64_t bit = n2 >> 63;
    n2 = (n2 << 1) | (n1 >> 63);
    n1 = (n1 << 1) | (n0 >> 63);
    n0 <<= 1;

    uint64_t carry = r0 >> 63;
    r0 = (r0 << 1) | bit;
    r1 = (r1 << 1) | carry;

    carry = q0 >> 63;
    q0 <<= 1;
    q1 = (q1 << 1) | carry;

    if (ge128 (r1, r0, d1, d0)) {
      sub128 (&r1, &r0, d1, d0);
      q0 |= 1;
    }
  }

  fixed64_t r;
  r.hi = (int64_t) q1;
  r.lo = q0;
  return neg ? fixed64_neg (r) : r;
}

#define FIX64_FRAC 18446744073709551616.0 /* 2^64 */

fixed64_t fixed64_from_double (double d) {
  int neg = d < 0;
  if (neg) d = -d;
  double intpart;
  double frac = modf (d, &intpart);
  fixed64_t r;
  r.hi = (int64_t) intpart;
  r.lo = (uint64_t) (frac * FIX64_FRAC);
  if (neg) r = fixed64_neg (r);
  return r;
}

double fixed64_to_double (fixed64_t x) {
  if (x.hi < 0) {
    fixed64_t t = fixed64_neg (x);
    return -((double) t.hi + (double) t.lo / FIX64_FRAC);
  }
  return (double) x.hi + (double) x.lo / FIX64_FRAC;
}

fixed64_t fixed64_from_string (const char *s, char **endptr) {
  double d = strtod (s, endptr);
  return fixed64_from_double (d);
}

int fixed64_to_string (fixed64_t x, char *buf, size_t size) {
  double d = fixed64_to_double (x);
  return snprintf (buf, size, "%.17g", d);
}

fixed64_t fixed64_abs (fixed64_t x) { return x.hi < 0 ? fixed64_neg (x) : x; }

fixed64_t fixed64_stub_unary (fixed64_t a) {
  (void) a;
  return fixed64_from_int (0);
}

fixed64_t fixed64_stub_binary (fixed64_t a, fixed64_t b) {
  (void) a;
  (void) b;
  return fixed64_from_int (0);
}

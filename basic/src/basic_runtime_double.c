#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "basic_runtime.h"
#include "basic_num.h"
#include "basic_runtime_shared.h"

extern int basic_pos_val;
#define BASIC_MAX_FILES 16
extern FILE *basic_files[];

static int seeded = 0;
static uint64_t rng_state = 0;

static uint64_t basic_next_u64 (void) {
  uint64_t x = rng_state;
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  rng_state = x;
  return x * UINT64_C (2685821657736338717);
}

void basic_randomize (basic_num_t n, basic_num_t has_seed) {
  if (basic_num_ne (has_seed, BASIC_ZERO)) {
    rng_state = (uint64_t) basic_num_to_int (n);
  } else {
    rng_state = (((uint64_t) time (NULL)) << 32) ^ (uint64_t) clock ();
  }
  seeded = 1;
}

basic_num_t basic_rnd (basic_num_t n) {
  if (!seeded) {
    basic_randomize (BASIC_ZERO, BASIC_ZERO);
  }
  uint64_t r = basic_next_u64 ();
  basic_num_t hi = basic_num_from_int ((long) (r >> 32));
  basic_num_t lo = basic_num_from_int ((long) (r & UINT32_MAX));
  basic_num_t base = basic_num_from_int ((long) (1ULL << 32));
  basic_num_t frac = basic_num_add (hi, basic_num_div (lo, base));
  frac = basic_num_div (frac, base);
  return basic_num_mul (frac, n);
}

basic_num_t basic_input (void) {
  basic_num_t x = BASIC_ZERO;
  if (!basic_num_scan (stdin, &x)) return BASIC_ZERO;
  return x;
}

basic_num_t basic_pos (void) { return basic_num_from_int (basic_pos_val); }

basic_num_t basic_input_hash (int64_t n) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) return BASIC_ZERO;
  basic_num_t x = BASIC_ZERO;
  if (!basic_num_scan (basic_files[idx], &x)) return BASIC_ZERO;
  return x;
}

basic_num_t basic_read (void) {
  if (basic_data_pos >= basic_data_len || basic_data_items[basic_data_pos].is_str)
    return BASIC_ZERO;
  return basic_data_items[basic_data_pos++].num;
}

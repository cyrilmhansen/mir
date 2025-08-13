#ifndef BASIC_NUM_OPS_H
#define BASIC_NUM_OPS_H

#include <stddef.h>
#include <stdalign.h>
#include <stdio.h>

#include "fixed64/fixed64.h"

#ifdef __cplusplus
extern "C" {
#endif

/* numeric storage */
typedef struct {
  alignas (16) unsigned char v[16];
} basic_num_t;

#define BASIC_ZERO ((basic_num_t) {0})

typedef struct basic_num_ops {
  size_t size;
  size_t align;
  void (*from_int) (long, void *out);
  long (*to_int) (const void *);
  void (*add) (void *res, const void *a, const void *b);
  void (*sub) (void *res, const void *a, const void *b);
  void (*mul) (void *res, const void *a, const void *b);
  void (*div) (void *res, const void *a, const void *b);
  void (*neg) (void *res, const void *a);
  int (*eq) (const void *a, const void *b);
  int (*ne) (const void *a, const void *b);
  int (*lt) (const void *a, const void *b);
  int (*le) (const void *a, const void *b);
  int (*gt) (const void *a, const void *b);
  int (*ge) (const void *a, const void *b);
  int (*scan) (FILE *f, void *out);
  void (*print) (FILE *f, const void *val);
} basic_num_ops;

typedef enum {
  BASIC_NUM_MODE_DOUBLE,
  BASIC_NUM_MODE_LONG_DOUBLE,
  BASIC_NUM_MODE_FIXED64,
  BASIC_NUM_MODE_MBF,
} basic_num_mode;

extern const basic_num_ops *basic_curr_num_ops;

void basic_set_num_mode (basic_num_mode mode);

static inline size_t basic_num_size (void) { return basic_curr_num_ops->size; }
static inline size_t basic_num_align (void) { return basic_curr_num_ops->align; }

static inline basic_num_t basic_num_from_int (long x) {
  basic_num_t r;
  basic_curr_num_ops->from_int (x, &r);
  return r;
}

static inline long basic_num_to_int (basic_num_t a) { return basic_curr_num_ops->to_int (&a); }

static inline basic_num_t basic_num_add (basic_num_t a, basic_num_t b) {
  basic_num_t r;
  basic_curr_num_ops->add (&r, &a, &b);
  return r;
}

static inline basic_num_t basic_num_sub (basic_num_t a, basic_num_t b) {
  basic_num_t r;
  basic_curr_num_ops->sub (&r, &a, &b);
  return r;
}

static inline basic_num_t basic_num_mul (basic_num_t a, basic_num_t b) {
  basic_num_t r;
  basic_curr_num_ops->mul (&r, &a, &b);
  return r;
}

static inline basic_num_t basic_num_div (basic_num_t a, basic_num_t b) {
  basic_num_t r;
  basic_curr_num_ops->div (&r, &a, &b);
  return r;
}

static inline basic_num_t basic_num_neg (basic_num_t a) {
  basic_num_t r;
  basic_curr_num_ops->neg (&r, &a);
  return r;
}

static inline int basic_num_eq (basic_num_t a, basic_num_t b) {
  return basic_curr_num_ops->eq (&a, &b);
}

static inline int basic_num_ne (basic_num_t a, basic_num_t b) {
  return basic_curr_num_ops->ne (&a, &b);
}

static inline int basic_num_lt (basic_num_t a, basic_num_t b) {
  return basic_curr_num_ops->lt (&a, &b);
}

static inline int basic_num_le (basic_num_t a, basic_num_t b) {
  return basic_curr_num_ops->le (&a, &b);
}

static inline int basic_num_gt (basic_num_t a, basic_num_t b) {
  return basic_curr_num_ops->gt (&a, &b);
}

static inline int basic_num_ge (basic_num_t a, basic_num_t b) {
  return basic_curr_num_ops->ge (&a, &b);
}

static inline int basic_num_scan (FILE *f, basic_num_t *out) {
  return basic_curr_num_ops->scan (f, out);
}

static inline void basic_num_print (FILE *f, basic_num_t a) { basic_curr_num_ops->print (f, &a); }

#ifdef __cplusplus
}
#endif

#endif

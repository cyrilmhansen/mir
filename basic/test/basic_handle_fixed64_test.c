#include <assert.h>
#include <stdio.h>

#include "basic_num.h"
#include "basic_runtime.h"

int main (void) {
  basic_num_t ctx = basic_mir_ctx ();
  assert (basic_num_ne (ctx, BASIC_ZERO));
  basic_num_t ctx2 = basic_mir_ctx ();
  assert (basic_num_eq (ctx, ctx2));

  basic_num_t mod = basic_mir_mod (ctx, "tmod");
  assert (basic_num_ne (mod, BASIC_ZERO));
  basic_num_t func = basic_mir_func (mod, "tfunc", BASIC_ZERO);
  assert (basic_num_ne (func, BASIC_ZERO));
  basic_num_t r1 = basic_mir_reg (func);
  basic_num_t r2 = basic_mir_reg (func);
  basic_num_t r3 = basic_mir_reg (func);
  basic_num_t lbl = basic_mir_label (func);
  assert (basic_num_ne (r1, BASIC_ZERO));
  assert (basic_num_ne (r2, BASIC_ZERO));
  assert (basic_num_ne (r3, BASIC_ZERO));
  assert (basic_num_ne (lbl, BASIC_ZERO));
  basic_mir_emitlbl (func, lbl);
  basic_mir_emit (func, "DMOV", r1, basic_num_from_int (2), BASIC_ZERO);
  basic_mir_emit (func, "DMOV", r2, basic_num_from_int (3), BASIC_ZERO);
  basic_mir_emit (func, "MOV", r3, r1, BASIC_ZERO);
  basic_mir_emit (func, "DADD", r3, r3, r2);
  basic_mir_emit (func, "MOV", r1, r3, BASIC_ZERO);
  basic_mir_emit (func, "DNEG", r1, r1, BASIC_ZERO);
  basic_mir_ret (func, r1);
  basic_mir_finish (mod);
  basic_num_t res = basic_mir_run (func, BASIC_ZERO, BASIC_ZERO, BASIC_ZERO, BASIC_ZERO);
  assert (basic_num_eq (res, basic_num_neg (basic_num_from_int (5))));
  printf ("basic_handle_fixed64_test OK\n");
  return 0;
}

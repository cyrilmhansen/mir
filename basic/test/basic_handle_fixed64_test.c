#include <assert.h>
#include <stdio.h>

#include "basic_num.h"
#include "basic_runtime.h"

int main (void) {
  basic_num_t ctx, ctx2, mod, func, r1, r2, r3, lbl, dummy, res;
  basic_mir_ctx (&ctx);
  assert (basic_num_ne (ctx, BASIC_ZERO));
  basic_mir_ctx (&ctx2);
  assert (basic_num_eq (ctx, ctx2));

  basic_mir_mod (&mod, ctx, "tmod");
  assert (basic_num_ne (mod, BASIC_ZERO));
  basic_mir_func (&func, mod, "tfunc", BASIC_ZERO);
  assert (basic_num_ne (func, BASIC_ZERO));
  basic_mir_reg (&r1, func);
  basic_mir_reg (&r2, func);
  basic_mir_reg (&r3, func);
  basic_mir_label (&lbl, func);
  assert (basic_num_ne (r1, BASIC_ZERO));
  assert (basic_num_ne (r2, BASIC_ZERO));
  assert (basic_num_ne (r3, BASIC_ZERO));
  assert (basic_num_ne (lbl, BASIC_ZERO));
  basic_mir_emitlbl (&dummy, func, lbl);
  basic_mir_emit (&dummy, func, "DMOV", r1, basic_num_from_int (2), BASIC_ZERO);
  basic_mir_emit (&dummy, func, "DMOV", r2, basic_num_from_int (3), BASIC_ZERO);
  basic_mir_emit (&dummy, func, "MOV", r3, r1, BASIC_ZERO);
  basic_mir_emit (&dummy, func, "DADD", r3, r3, r2);
  basic_mir_emit (&dummy, func, "MOV", r1, r3, BASIC_ZERO);
  basic_mir_emit (&dummy, func, "DNEG", r1, r1, BASIC_ZERO);
  basic_mir_ret (&dummy, func, r1);
  basic_mir_finish (&dummy, mod);
  basic_mir_run (&res, func, BASIC_ZERO, BASIC_ZERO, BASIC_ZERO, BASIC_ZERO);
  assert (basic_num_eq (res, basic_num_neg (basic_num_from_int (5))));
  printf ("basic_handle_fixed64_test OK\n");
  return 0;
}

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
  basic_num_t reg = basic_mir_reg (func);
  basic_num_t lbl = basic_mir_label (func);
  assert (basic_num_ne (reg, BASIC_ZERO));
  assert (basic_num_ne (lbl, BASIC_ZERO));
  basic_mir_emitlbl (func, lbl);
  basic_mir_ret (func, reg);
  basic_mir_finish (mod);
  printf ("basic_handle_fixed64_test OK\n");
  return 0;
}

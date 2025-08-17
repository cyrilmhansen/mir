#include <assert.h>

#include "basic_runtime.h"
#include "basic_num.h"

int main (void) {
  basic_num_t ctx, mod, func, r1, r2, tmp;

  basic_mir_ctx (&ctx);
  basic_mir_mod (&mod, ctx, "test");
  basic_mir_func (&func, mod, "add", basic_num_from_int (0));
  basic_mir_reg (&r1, func);
  basic_mir_reg (&r2, func);

  basic_mir_emit (&tmp, func, "MOV", r1, basic_num_from_int (5), BASIC_ZERO);
  basic_mir_emit (&tmp, func, "MOV", r2, basic_num_from_int (3), BASIC_ZERO);
  basic_mir_emit (&tmp, func, "DADD", r1, r1, r2);
  basic_mir_ret (&tmp, func, r1);
  basic_mir_finish (&tmp, mod);
  basic_mir_run (&tmp, func, BASIC_ZERO, BASIC_ZERO, BASIC_ZERO, BASIC_ZERO);

  assert (basic_num_to_int (tmp) == 8);
  return 0;
}

#include "../mir.h"
#include "../mir-gen.h"
#include <assert.h>

int main (void) {
  MIR_context_t ctx = MIR_init ();
  MIR_module_t m = MIR_new_module (ctx, "m");
  MIR_type_t res_t = MIR_T_F;
  MIR_item_t func = MIR_new_func (ctx, "conv", 1, &res_t, 1, MIR_T_U64, "a");
  MIR_reg_t a = MIR_reg (ctx, "a", func->u.func);
  MIR_reg_t r = MIR_new_func_reg (ctx, func->u.func, MIR_T_F, "r");
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_UI2F, MIR_new_reg_op (ctx, r), MIR_new_reg_op (ctx, a)));
  MIR_append_insn (ctx, func, MIR_new_ret_insn (ctx, 1, MIR_new_reg_op (ctx, r)));
  MIR_finish_func (ctx);
  MIR_finish_module (ctx);
  MIR_load_module (ctx, m);
  MIR_gen_init (ctx);
  MIR_link (ctx, MIR_set_gen_interface, NULL);
  MIR_gen (ctx, func);
  assert (MIR_helper_used_p (ctx, "mir.ui2f"));
  assert (!MIR_helper_used_p (ctx, "mir.ui2d"));
  MIR_gen_finish (ctx);
  MIR_finish (ctx);
  return 0;
}

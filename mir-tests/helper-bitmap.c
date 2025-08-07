#include "mir.h"
#include "mir-gen.h"
#include "mir-bitmap.h"
#include <assert.h>

int main (void) {
  MIR_context_t ctx = MIR_init ();
  MIR_module_t m = MIR_new_module (ctx, "m");
  MIR_type_t res_type = MIR_T_F;
  MIR_var_t arg = {MIR_T_U64, "u"};
  MIR_func_t func = MIR_new_func_arr (ctx, "f", 1, &res_type, 1, &arg);
  MIR_reg_t res = MIR_new_func_reg (ctx, func, MIR_T_F, "res");
  MIR_reg_t u = MIR_reg ("u", func);
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_UI2F, MIR_new_reg_op (ctx, res),
                                 MIR_new_reg_op (ctx, u)));
  MIR_append_insn (ctx, func, MIR_new_ret_insn (ctx, 1, MIR_new_reg_op (ctx, res)));
  MIR_finish_func (ctx);
  MIR_finish_module (ctx);
  MIR_load_module (ctx, m);
  MIR_gen_init (ctx);
  MIR_link (ctx, MIR_set_gen_interface, NULL);
  MIR_gen (ctx, DLIST_HEAD (MIR_item_t, m->items));
  bitmap_t bm = _MIR_get_helpers_bitset (ctx);
  int ui2f_idx = _MIR_helper_num (ctx, "mir.ui2f");
  int va_arg_idx = _MIR_helper_num (ctx, "mir.va_arg");
  assert (ui2f_idx >= 0 && va_arg_idx >= 0);
  assert (bitmap_bit_p (bm, ui2f_idx));
  assert (!bitmap_bit_p (bm, va_arg_idx));
  MIR_gen_finish (ctx);
  MIR_finish (ctx);
  return 0;
}

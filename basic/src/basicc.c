#include "basic_common.h"
#include "basic_num_hooks.h"

static void basic_mir_binop_default (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code,
                                     MIR_op_t dst, MIR_op_t src1, MIR_op_t src2) {
  MIR_append_insn (ctx, func, MIR_new_insn (ctx, code, dst, src1, src2));
}

static void basic_mir_unop_default (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code,
                                    MIR_op_t dst, MIR_op_t src) {
  MIR_append_insn (ctx, func, MIR_new_insn (ctx, code, dst, src));
}

static void basic_mir_bcmp_default (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code,
                                    MIR_op_t label, MIR_op_t src1, MIR_op_t src2) {
  MIR_append_insn (ctx, func, MIR_new_insn (ctx, code, label, src1, src2));
}

static void basic_mir_i2n_default (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src) {
#if defined(BASIC_USE_LONG_DOUBLE)
  MIR_append_insn (ctx, func, MIR_new_insn (ctx, MIR_I2LD, dst, src));
#else
  MIR_append_insn (ctx, func, MIR_new_insn (ctx, MIR_I2D, dst, src));
#endif
}

static void basic_mir_n2i_default (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src) {
#if defined(BASIC_USE_LONG_DOUBLE)
  MIR_append_insn (ctx, func, MIR_new_insn (ctx, MIR_LD2I, dst, src));
#else
  MIR_append_insn (ctx, func, MIR_new_insn (ctx, MIR_D2I, dst, src));
#endif
}

static void basic_mir_pass_arg_default (MIR_context_t ctx, MIR_item_t func, MIR_op_t *op) {
  (void) ctx;
  (void) func;
  (void) op;
}

basic_num_hooks_t basic_num_hooks = {
  .mir_binop = basic_mir_binop_default,
  .mir_unop = basic_mir_unop_default,
  .mir_bcmp = basic_mir_bcmp_default,
  .mir_i2n = basic_mir_i2n_default,
  .mir_n2i = basic_mir_n2i_default,
  .mir_pass_arg = basic_mir_pass_arg_default,
};

void basic_num_init (MIR_context_t ctx) { (void) ctx; }

#include "basicc_core.c"

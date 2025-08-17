#ifndef BASIC_NUM_HOOKS_H
#define BASIC_NUM_HOOKS_H

#include "basic_common.h"

typedef struct {
  void (*mir_binop) (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code, MIR_op_t dst,
                     MIR_op_t src1, MIR_op_t src2);
  void (*mir_unop) (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code, MIR_op_t dst,
                    MIR_op_t src);
  void (*mir_bcmp) (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code, MIR_op_t label,
                    MIR_op_t src1, MIR_op_t src2);
  void (*mir_i2n) (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src);
  void (*mir_n2i) (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src);
  MIR_type_t (*get_reg_type) (void);
  void (*mir_pass_arg) (MIR_context_t ctx, MIR_item_t func, MIR_op_t *op);
} basic_num_hooks_t;

extern basic_num_hooks_t basic_num_hooks;

void basic_num_init (MIR_context_t ctx);

#endif /* BASIC_NUM_HOOKS_H */

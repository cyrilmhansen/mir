#ifndef BASIC_FIXED64_HOOKS_H
#define BASIC_FIXED64_HOOKS_H

#include "basic_common.h"

MIR_op_t basic_mem (MIR_context_t ctx, MIR_item_t func, MIR_op_t op, MIR_type_t t);
void basic_mir_binop (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code, MIR_op_t dst,
                      MIR_op_t src1, MIR_op_t src2);
void basic_mir_unop (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code, MIR_op_t dst,
                     MIR_op_t src);
void basic_mir_bcmp (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code, MIR_op_t label,
                     MIR_op_t src1, MIR_op_t src2);
void basic_mir_i2n (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src);
void basic_mir_n2i (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src);
void basic_mir_u2n (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src);

extern MIR_item_t fixed64_add_proto, fixed64_add_import, fixed64_sub_proto, fixed64_sub_import,
  fixed64_mul_proto, fixed64_mul_import, fixed64_div_proto, fixed64_div_import, fixed64_eq_proto,
  fixed64_eq_import, fixed64_ne_proto, fixed64_ne_import, fixed64_lt_proto, fixed64_lt_import,
  fixed64_le_proto, fixed64_le_import, fixed64_gt_proto, fixed64_gt_import, fixed64_ge_proto,
  fixed64_ge_import, fixed64_from_int_proto, fixed64_from_int_import, fixed64_to_int_proto,
  fixed64_to_int_import, fixed64_from_uint_proto, fixed64_from_uint_import, fixed64_neg_proto,
  fixed64_neg_import;

#endif /* BASIC_FIXED64_HOOKS_H */

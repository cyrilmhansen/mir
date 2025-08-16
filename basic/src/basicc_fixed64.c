#include "basic_common.h"

#define BASIC_MIR_MOV MIR_MOV

static MIR_item_t fixed64_add_proto, fixed64_add_import, fixed64_sub_proto, fixed64_sub_import,
  fixed64_mul_proto, fixed64_mul_import, fixed64_div_proto, fixed64_div_import, fixed64_eq_proto,
  fixed64_eq_import, fixed64_ne_proto, fixed64_ne_import, fixed64_lt_proto, fixed64_lt_import,
  fixed64_le_proto, fixed64_le_import, fixed64_gt_proto, fixed64_gt_import, fixed64_ge_proto,
  fixed64_ge_import, fixed64_from_int_proto, fixed64_from_int_import, fixed64_from_string_proto,
  fixed64_from_string_import, fixed64_to_int_proto, fixed64_to_int_import, fixed64_neg_proto,
  fixed64_neg_import;

static MIR_op_t fixed64_emit_num_const (MIR_context_t ctx, basic_num_t v) {
  basic_num_t *p = basic_pool_alloc (sizeof (basic_num_t));
  *p = v;
  MIR_item_t data_item = MIR_new_data (ctx, NULL, MIR_T_U8, sizeof (basic_num_t), p);
  return MIR_new_ref_op (ctx, data_item);
}
#define BASIC_EMIT_NUM_CONST fixed64_emit_num_const

static MIR_op_t basic_mem (MIR_context_t ctx, MIR_item_t func, MIR_op_t op, MIR_type_t t) {
  MIR_reg_t r;
  if (op.mode == MIR_OP_REG) {
    r = op.u.reg;
  } else {
    char buf[32];
    static int addr_id = 0;
    safe_snprintf (buf, sizeof (buf), "$ba%d", addr_id++);
    r = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
    MIR_append_insn (ctx, func, MIR_new_insn (ctx, MIR_MOV, MIR_new_reg_op (ctx, r), op));
  }
  return _MIR_new_var_mem_op (ctx, t, sizeof (basic_num_t), r, MIR_NON_VAR, 1);
}

static void basic_mir_binop (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code, MIR_op_t dst,
                             MIR_op_t src1, MIR_op_t src2) {
  MIR_item_t proto = NULL, import = NULL;
  switch (code) {
  case MIR_DADD:
    proto = fixed64_add_proto;
    import = fixed64_add_import;
    break;
  case MIR_DSUB:
    proto = fixed64_sub_proto;
    import = fixed64_sub_import;
    break;
  case MIR_DMUL:
    proto = fixed64_mul_proto;
    import = fixed64_mul_import;
    break;
  case MIR_DDIV:
    proto = fixed64_div_proto;
    import = fixed64_div_import;
    break;
  case MIR_DEQ:
    proto = fixed64_eq_proto;
    import = fixed64_eq_import;
    break;
  case MIR_DNE:
    proto = fixed64_ne_proto;
    import = fixed64_ne_import;
    break;
  case MIR_DLT:
    proto = fixed64_lt_proto;
    import = fixed64_lt_import;
    break;
  case MIR_DLE:
    proto = fixed64_le_proto;
    import = fixed64_le_import;
    break;
  case MIR_DGT:
    proto = fixed64_gt_proto;
    import = fixed64_gt_import;
    break;
  case MIR_DGE:
    proto = fixed64_ge_proto;
    import = fixed64_ge_import;
    break;
  default: MIR_append_insn (ctx, func, MIR_new_insn (ctx, code, dst, src1, src2)); return;
  }
  MIR_op_t src1_mem = basic_mem (ctx, func, src1, MIR_T_BLK);
  MIR_op_t src2_mem = basic_mem (ctx, func, src2, MIR_T_BLK);
  char buf[32];
  static int tmp_id = 0;
  safe_snprintf (buf, sizeof (buf), "$t%d", tmp_id++);
  MIR_reg_t lo = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
  safe_snprintf (buf, sizeof (buf), "$t%d", tmp_id++);
  MIR_reg_t hi = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
  MIR_append_insn (ctx, func,
                   MIR_new_call_insn (ctx, 6, MIR_new_ref_op (ctx, proto),
                                      MIR_new_ref_op (ctx, import), MIR_new_reg_op (ctx, lo),
                                      MIR_new_reg_op (ctx, hi), src1_mem, src2_mem));
  MIR_op_t dst_mem = basic_mem (ctx, func, dst, MIR_T_BLK);
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_MOV,
                                 MIR_new_mem_op (ctx, MIR_T_I64, 0, dst_mem.u.mem.base, 0, 1),
                                 MIR_new_reg_op (ctx, lo)));
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_MOV,
                                 MIR_new_mem_op (ctx, MIR_T_I64, 8, dst_mem.u.mem.base, 0, 1),
                                 MIR_new_reg_op (ctx, hi)));
}
#define BASIC_MIR_BINOP basic_mir_binop

static void basic_mir_unop (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code, MIR_op_t dst,
                            MIR_op_t src) {
  if (code == MIR_DNEG) {
    MIR_op_t src_mem = basic_mem (ctx, func, src, MIR_T_BLK);
    char buf[32];
    static int tmp_id = 0;
    safe_snprintf (buf, sizeof (buf), "$t%d", tmp_id++);
    MIR_reg_t res_lo = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
    safe_snprintf (buf, sizeof (buf), "$t%d", tmp_id++);
    MIR_reg_t res_hi = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
    MIR_append_insn (ctx, func,
                     MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, fixed64_neg_proto),
                                        MIR_new_ref_op (ctx, fixed64_neg_import),
                                        MIR_new_reg_op (ctx, res_lo), MIR_new_reg_op (ctx, res_hi),
                                        src_mem));
    MIR_op_t dst_mem = basic_mem (ctx, func, dst, MIR_T_BLK);
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_MOV,
                                   MIR_new_mem_op (ctx, MIR_T_I64, 0, dst_mem.u.mem.base, 0, 1),
                                   MIR_new_reg_op (ctx, res_lo)));
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_MOV,
                                   MIR_new_mem_op (ctx, MIR_T_I64, 8, dst_mem.u.mem.base, 0, 1),
                                   MIR_new_reg_op (ctx, res_hi)));
    return;
  }
  MIR_append_insn (ctx, func, MIR_new_insn (ctx, code, dst, src));
}
#define BASIC_MIR_UNOP basic_mir_unop

static void basic_mir_bcmp (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code,
                            MIR_op_t label, MIR_op_t src1, MIR_op_t src2) {
  MIR_item_t proto = NULL, import = NULL;
  switch (code) {
  case MIR_DBEQ:
    proto = fixed64_eq_proto;
    import = fixed64_eq_import;
    break;
  case MIR_DBNE:
    proto = fixed64_ne_proto;
    import = fixed64_ne_import;
    break;
  case MIR_DBLT:
    proto = fixed64_lt_proto;
    import = fixed64_lt_import;
    break;
  case MIR_DBLE:
    proto = fixed64_le_proto;
    import = fixed64_le_import;
    break;
  case MIR_DBGT:
    proto = fixed64_gt_proto;
    import = fixed64_gt_import;
    break;
  case MIR_DBGE:
    proto = fixed64_ge_proto;
    import = fixed64_ge_import;
    break;
  default: MIR_append_insn (ctx, func, MIR_new_insn (ctx, code, label, src1, src2)); return;
  }
  char buf[32];
  static int btmp_id = 0;
  safe_snprintf (buf, sizeof (buf), "$bcmp%d", btmp_id++);
  MIR_reg_t tmp = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
  MIR_op_t src1_mem = basic_mem (ctx, func, src1, MIR_T_BLK);
  MIR_op_t src2_mem = basic_mem (ctx, func, src2, MIR_T_BLK);
  MIR_append_insn (ctx, func,
                   MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, proto),
                                      MIR_new_ref_op (ctx, import), MIR_new_reg_op (ctx, tmp),
                                      src1_mem, src2_mem));
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_BNE, label, MIR_new_reg_op (ctx, tmp),
                                 MIR_new_int_op (ctx, 0)));
}
#define BASIC_MIR_BCMP basic_mir_bcmp

static void basic_mir_i2n (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src) {
  char buf[32];
  static int tmp_id = 0;
  safe_snprintf (buf, sizeof (buf), "$t%d", tmp_id++);
  MIR_reg_t lo = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
  safe_snprintf (buf, sizeof (buf), "$t%d", tmp_id++);
  MIR_reg_t hi = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
  MIR_append_insn (ctx, func,
                   MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, fixed64_from_int_proto),
                                      MIR_new_ref_op (ctx, fixed64_from_int_import),
                                      MIR_new_reg_op (ctx, lo), MIR_new_reg_op (ctx, hi), src));
  MIR_op_t dst_mem = basic_mem (ctx, func, dst, MIR_T_BLK);
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_MOV,
                                 MIR_new_mem_op (ctx, MIR_T_I64, 0, dst_mem.u.mem.base, 0, 1),
                                 MIR_new_reg_op (ctx, lo)));
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_MOV,
                                 MIR_new_mem_op (ctx, MIR_T_I64, 8, dst_mem.u.mem.base, 0, 1),
                                 MIR_new_reg_op (ctx, hi)));
}
#define BASIC_MIR_I2N basic_mir_i2n

static void basic_mir_n2i (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src) {
  MIR_op_t src_mem = basic_mem (ctx, func, src, MIR_T_BLK);
  MIR_append_insn (ctx, func,
                   MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, fixed64_to_int_proto),
                                      MIR_new_ref_op (ctx, fixed64_to_int_import), dst, src_mem));
}
#define BASIC_MIR_N2I basic_mir_n2i

static void basic_fixed64_init (MIR_context_t ctx) {
  MIR_type_t i64_pair[2] = {MIR_T_I64, MIR_T_I64};
  MIR_type_t i64_ = MIR_T_I64;
  MIR_var_t bin_vars[2];
  bin_vars[0].name = "a";
  bin_vars[0].type = MIR_T_BLK;
  bin_vars[0].size = sizeof (basic_num_t);
  bin_vars[1].name = "b";
  bin_vars[1].type = MIR_T_BLK;
  bin_vars[1].size = sizeof (basic_num_t);
  fixed64_add_proto = MIR_new_proto_arr (ctx, "fixed64_add_p", 2, i64_pair, 2, bin_vars);
  fixed64_add_import = MIR_new_import (ctx, "fixed64_add");
  fixed64_sub_proto = MIR_new_proto_arr (ctx, "fixed64_sub_p", 2, i64_pair, 2, bin_vars);
  fixed64_sub_import = MIR_new_import (ctx, "fixed64_sub");
  fixed64_mul_proto = MIR_new_proto_arr (ctx, "fixed64_mul_p", 2, i64_pair, 2, bin_vars);
  fixed64_mul_import = MIR_new_import (ctx, "fixed64_mul");
  fixed64_div_proto = MIR_new_proto_arr (ctx, "fixed64_div_p", 2, i64_pair, 2, bin_vars);
  fixed64_div_import = MIR_new_import (ctx, "fixed64_div");

  MIR_var_t un_vars[1];
  un_vars[0].name = "a";
  un_vars[0].type = MIR_T_BLK;
  un_vars[0].size = sizeof (basic_num_t);
  fixed64_neg_proto = MIR_new_proto_arr (ctx, "fixed64_neg_p", 2, i64_pair, 1, un_vars);
  fixed64_neg_import = MIR_new_import (ctx, "fixed64_neg");

  MIR_var_t cmp_vars[2];
  cmp_vars[0].name = "a";
  cmp_vars[0].type = MIR_T_BLK;
  cmp_vars[0].size = sizeof (basic_num_t);
  cmp_vars[1].name = "b";
  cmp_vars[1].type = MIR_T_BLK;
  cmp_vars[1].size = sizeof (basic_num_t);
  fixed64_eq_proto = MIR_new_proto_arr (ctx, "fixed64_eq_p", 1, &i64_, 2, cmp_vars);
  fixed64_eq_import = MIR_new_import (ctx, "fixed64_eq");
  fixed64_ne_proto = MIR_new_proto_arr (ctx, "fixed64_ne_p", 1, &i64_, 2, cmp_vars);
  fixed64_ne_import = MIR_new_import (ctx, "fixed64_ne");
  fixed64_lt_proto = MIR_new_proto_arr (ctx, "fixed64_lt_p", 1, &i64_, 2, cmp_vars);
  fixed64_lt_import = MIR_new_import (ctx, "fixed64_lt");
  fixed64_le_proto = MIR_new_proto_arr (ctx, "fixed64_le_p", 1, &i64_, 2, cmp_vars);
  fixed64_le_import = MIR_new_import (ctx, "fixed64_le");
  fixed64_gt_proto = MIR_new_proto_arr (ctx, "fixed64_gt_p", 1, &i64_, 2, cmp_vars);
  fixed64_gt_import = MIR_new_import (ctx, "fixed64_gt");
  fixed64_ge_proto = MIR_new_proto_arr (ctx, "fixed64_ge_p", 1, &i64_, 2, cmp_vars);
  fixed64_ge_import = MIR_new_import (ctx, "fixed64_ge");

  MIR_var_t int_vars[1];
  int_vars[0].name = "i";
  int_vars[0].type = MIR_T_I64;
  fixed64_from_int_proto = MIR_new_proto_arr (ctx, "fixed64_from_int_p", 2, i64_pair, 1, int_vars);
  fixed64_from_int_import = MIR_new_import (ctx, "fixed64_from_int");

  MIR_var_t str_vars[2];
  str_vars[0].name = "s";
  str_vars[0].type = MIR_T_P;
  str_vars[1].name = "end";
  str_vars[1].type = MIR_T_P;
  fixed64_from_string_proto
    = MIR_new_proto_arr (ctx, "fixed64_from_string_p", 2, i64_pair, 2, str_vars);
  fixed64_from_string_import = MIR_new_import (ctx, "fixed64_from_string");

  MIR_var_t to_int_vars[1];
  to_int_vars[0].name = "a";
  to_int_vars[0].type = MIR_T_BLK;
  to_int_vars[0].size = sizeof (basic_num_t);
  fixed64_to_int_proto = MIR_new_proto_arr (ctx, "fixed64_to_int_p", 1, &i64_, 1, to_int_vars);
  fixed64_to_int_import = MIR_new_import (ctx, "fixed64_to_int");
}
#define BASIC_NUM_INIT basic_fixed64_init

#include "basicc.c"

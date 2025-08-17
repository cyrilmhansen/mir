#include "basic_fixed64_hooks.h"

static int safe_snprintf (char *buf, size_t size, const char *fmt, ...) {
  va_list ap;
  int res;
  va_start (ap, fmt);
  res = vsnprintf (buf, size, fmt, ap);
  va_end (ap);
  if (res < 0 || (size_t) res >= size) {
    fprintf (stderr, "safe_snprintf: truncated output\n");
    abort ();
  }
  return res;
}

MIR_op_t basic_mem (MIR_context_t ctx, MIR_item_t func, MIR_op_t op, MIR_type_t t) {
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
  return MIR_new_mem_op (ctx, t, 0, r, 0, 1);
}

void basic_mir_binop (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code, MIR_op_t dst,
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
  MIR_op_t src1_mem = basic_mem (ctx, func, src1, MIR_T_BLK + 1);
  MIR_op_t src2_mem = basic_mem (ctx, func, src2, MIR_T_BLK + 1);
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
  MIR_op_t dst_mem = basic_mem (ctx, func, dst, MIR_T_BLK + 1);
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_MOV,
                                 MIR_new_mem_op (ctx, MIR_T_I64, 0, dst_mem.u.mem.base, 0, 1),
                                 MIR_new_reg_op (ctx, lo)));
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_MOV,
                                 MIR_new_mem_op (ctx, MIR_T_I64, 8, dst_mem.u.mem.base, 0, 1),
                                 MIR_new_reg_op (ctx, hi)));
}

void basic_mir_unop (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code, MIR_op_t dst,
                     MIR_op_t src) {
  if (code == MIR_DNEG) {
    MIR_op_t src_mem = basic_mem (ctx, func, src, MIR_T_BLK + 1);
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
    MIR_op_t dst_mem = basic_mem (ctx, func, dst, MIR_T_BLK + 1);
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

void basic_mir_bcmp (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code, MIR_op_t label,
                     MIR_op_t src1, MIR_op_t src2) {
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
  MIR_op_t src1_mem = basic_mem (ctx, func, src1, MIR_T_BLK + 1);
  MIR_op_t src2_mem = basic_mem (ctx, func, src2, MIR_T_BLK + 1);
  MIR_append_insn (ctx, func,
                   MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, proto),
                                      MIR_new_ref_op (ctx, import), MIR_new_reg_op (ctx, tmp),
                                      src1_mem, src2_mem));
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_BNE, label, MIR_new_reg_op (ctx, tmp),
                                 MIR_new_int_op (ctx, 0)));
}

void basic_mir_i2n (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src) {
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
  MIR_op_t dst_mem = basic_mem (ctx, func, dst, MIR_T_BLK + 1);
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_MOV,
                                 MIR_new_mem_op (ctx, MIR_T_I64, 0, dst_mem.u.mem.base, 0, 1),
                                 MIR_new_reg_op (ctx, lo)));
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_MOV,
                                 MIR_new_mem_op (ctx, MIR_T_I64, 8, dst_mem.u.mem.base, 0, 1),
                                 MIR_new_reg_op (ctx, hi)));
}

void basic_mir_u2n (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src) {
  char buf[32];
  static int tmp_id = 0;
  safe_snprintf (buf, sizeof (buf), "$t%d", tmp_id++);
  MIR_reg_t lo = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
  safe_snprintf (buf, sizeof (buf), "$t%d", tmp_id++);
  MIR_reg_t hi = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
  MIR_append_insn (ctx, func,
                   MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, fixed64_from_uint_proto),
                                      MIR_new_ref_op (ctx, fixed64_from_uint_import),
                                      MIR_new_reg_op (ctx, lo), MIR_new_reg_op (ctx, hi), src));
  MIR_op_t dst_mem = basic_mem (ctx, func, dst, MIR_T_BLK + 1);
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_MOV,
                                 MIR_new_mem_op (ctx, MIR_T_I64, 0, dst_mem.u.mem.base, 0, 1),
                                 MIR_new_reg_op (ctx, lo)));
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_MOV,
                                 MIR_new_mem_op (ctx, MIR_T_I64, 8, dst_mem.u.mem.base, 0, 1),
                                 MIR_new_reg_op (ctx, hi)));
}

void basic_mir_n2i (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src) {
  MIR_op_t src_mem = basic_mem (ctx, func, src, MIR_T_BLK + 1);
  MIR_append_insn (ctx, func,
                   MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, fixed64_to_int_proto),
                                      MIR_new_ref_op (ctx, fixed64_to_int_import), dst, src_mem));
}

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "basic_runtime_fixed64.h"
#include "basic_runtime.h"
#include "basic_num.h"
#define BASIC_PRNG128 1

static int seeded = 0;
#if defined(BASIC_PRNG128)
static uint64_t s[4] = {0, 0, 0, 0};

static uint64_t rotl (const uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }

static uint64_t xoshiro256ss_next (void) {
  const uint64_t result = rotl (s[1] * 5, 7) * 9;
  const uint64_t t = s[1] << 17;
  s[2] ^= s[0];
  s[3] ^= s[1];
  s[1] ^= s[2];
  s[0] ^= s[3];
  s[2] ^= t;
  s[3] = rotl (s[3], 45);
  return result;
}

static __uint128_t basic_next_u128 (void) {
  __uint128_t hi = (__uint128_t) xoshiro256ss_next () << 64;
  return hi | xoshiro256ss_next ();
}

static uint64_t splitmix64 (uint64_t *x) {
  uint64_t z = (*x += UINT64_C (0x9E3779B97F4A7C15));
  z = (z ^ (z >> 30)) * UINT64_C (0xBF58476D1CE4E5B9);
  z = (z ^ (z >> 27)) * UINT64_C (0x94D049BB133111EB);
  return z ^ (z >> 31);
}

void basic_randomize (basic_num_t n, basic_num_t has_seed) {
  uint64_t seed;
  if (basic_num_ne (has_seed, BASIC_ZERO)) {
    seed = (uint64_t) basic_num_to_int (n);
  } else {
    seed = (((uint64_t) time (NULL)) << 32) ^ (uint64_t) clock ();
  }
  uint64_t sm = seed;
  for (int i = 0; i < 4; i++) s[i] = splitmix64 (&sm);
  seeded = 1;
}
#else
static uint64_t rng_state = 0;

static uint64_t basic_next_u64 (void) {
  uint64_t x = rng_state;
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  rng_state = x;
  return x * UINT64_C (2685821657736338717);
}

void basic_randomize (basic_num_t n, basic_num_t has_seed) {
  if (basic_num_ne (has_seed, BASIC_ZERO)) {
    rng_state = (uint64_t) basic_num_to_int (n);
  } else {
    rng_state = (((uint64_t) time (NULL)) << 32) ^ (uint64_t) clock ();
  }
  seeded = 1;
}
#endif

extern int basic_pos_val;
#define BASIC_MAX_FILES 16
extern FILE *basic_files[];
typedef struct BasicData {
  int is_str;
  basic_num_t num;
  char *str;
} BasicData;
extern BasicData *basic_data_items;
extern size_t basic_data_len;
extern size_t basic_data_pos;

static MIR_item_t fixed64_add_proto, fixed64_add_import, fixed64_sub_proto, fixed64_sub_import,
  fixed64_mul_proto, fixed64_mul_import, fixed64_div_proto, fixed64_div_import, fixed64_eq_proto,
  fixed64_eq_import, fixed64_ne_proto, fixed64_ne_import, fixed64_lt_proto, fixed64_lt_import,
  fixed64_le_proto, fixed64_le_import, fixed64_gt_proto, fixed64_gt_import, fixed64_ge_proto,
  fixed64_ge_import, fixed64_from_int_proto, fixed64_from_int_import, fixed64_to_int_proto,
  fixed64_to_int_import, fixed64_neg_proto, fixed64_neg_import;

static MIR_op_t basic_mem (MIR_context_t ctx, MIR_item_t func, MIR_op_t op, MIR_type_t t) {
  MIR_reg_t r;
  if (op.mode == MIR_OP_REG) {
    r = op.u.reg;
  } else {
    char buf[32];
    static int addr_id = 0;
    snprintf (buf, sizeof (buf), "$ba%d", addr_id++);
    r = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
    MIR_append_insn (ctx, func, MIR_new_insn (ctx, MIR_MOV, MIR_new_reg_op (ctx, r), op));
  }
  return MIR_new_mem_op (ctx, t, sizeof (basic_num_t), r, 0, 1);
}

static MIR_insn_t basic_mir_binop (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code,
                                   MIR_op_t dst, MIR_op_t src1, MIR_op_t src2) {
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
  default: return MIR_new_insn (ctx, code, dst, src1, src2);
  }
  MIR_op_t dst_mem = basic_mem (ctx, func, dst, MIR_T_RBLK);
  MIR_op_t src1_mem = basic_mem (ctx, func, src1, MIR_T_BLK);
  MIR_op_t src2_mem = basic_mem (ctx, func, src2, MIR_T_BLK);
  return MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, proto), MIR_new_ref_op (ctx, import),
                            dst_mem, src1_mem, src2_mem);
}

static MIR_insn_t basic_mir_unop (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code,
                                  MIR_op_t dst, MIR_op_t src) {
  if (code == MIR_DNEG) {
    MIR_op_t dst_mem = basic_mem (ctx, func, dst, MIR_T_RBLK);
    MIR_op_t src_mem = basic_mem (ctx, func, src, MIR_T_BLK);
    return MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, fixed64_neg_proto),
                              MIR_new_ref_op (ctx, fixed64_neg_import), dst_mem, src_mem);
  }
  return MIR_new_insn (ctx, code, dst, src);
}

static MIR_insn_t basic_mir_i2n (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src) {
  MIR_op_t dst_mem = basic_mem (ctx, func, dst, MIR_T_RBLK);
  return MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, fixed64_from_int_proto),
                            MIR_new_ref_op (ctx, fixed64_from_int_import), dst_mem, src);
}

static MIR_insn_t basic_mir_n2i (MIR_context_t ctx, MIR_item_t func, MIR_op_t dst, MIR_op_t src) {
  MIR_op_t src_mem = basic_mem (ctx, func, src, MIR_T_BLK);
  return MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, fixed64_to_int_proto),
                            MIR_new_ref_op (ctx, fixed64_to_int_import), dst, src_mem);
}

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
  snprintf (buf, sizeof (buf), "$bcmp%d", btmp_id++);
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

void basic_runtime_fixed64_init (MIR_context_t ctx) {
  MIR_type_t blk = MIR_T_BLK;
  MIR_type_t i64 = MIR_T_I64;
  MIR_var_t res_arg;
  res_arg.name = "res";
  res_arg.type = MIR_T_RBLK;
  res_arg.size = sizeof (basic_num_t);
  MIR_var_t bin_vars[3];
  bin_vars[0] = res_arg;
  bin_vars[1].name = "a";
  bin_vars[1].type = MIR_T_BLK;
  bin_vars[1].size = sizeof (basic_num_t);
  bin_vars[2].name = "b";
  bin_vars[2].type = MIR_T_BLK;
  bin_vars[2].size = sizeof (basic_num_t);
  fixed64_add_proto = MIR_new_proto_arr (ctx, "fixed64_add_p", 0, NULL, 3, bin_vars);
  fixed64_add_import = MIR_new_import (ctx, "fixed64_add");
  fixed64_sub_proto = MIR_new_proto_arr (ctx, "fixed64_sub_p", 0, NULL, 3, bin_vars);
  fixed64_sub_import = MIR_new_import (ctx, "fixed64_sub");
  fixed64_mul_proto = MIR_new_proto_arr (ctx, "fixed64_mul_p", 0, NULL, 3, bin_vars);
  fixed64_mul_import = MIR_new_import (ctx, "fixed64_mul");
  fixed64_div_proto = MIR_new_proto_arr (ctx, "fixed64_div_p", 0, NULL, 3, bin_vars);
  fixed64_div_import = MIR_new_import (ctx, "fixed64_div");
  MIR_var_t un_vars[2];
  un_vars[0] = res_arg;
  un_vars[1].name = "a";
  un_vars[1].type = MIR_T_BLK;
  un_vars[1].size = sizeof (basic_num_t);
  fixed64_neg_proto = MIR_new_proto_arr (ctx, "fixed64_neg_p", 0, NULL, 2, un_vars);
  fixed64_neg_import = MIR_new_import (ctx, "fixed64_neg");
  MIR_var_t cmp_vars[2];
  cmp_vars[0].name = "a";
  cmp_vars[0].type = MIR_T_BLK;
  cmp_vars[0].size = sizeof (basic_num_t);
  cmp_vars[1].name = "b";
  cmp_vars[1].type = MIR_T_BLK;
  cmp_vars[1].size = sizeof (basic_num_t);
  fixed64_eq_proto = MIR_new_proto_arr (ctx, "fixed64_eq_p", 1, &i64, 2, cmp_vars);
  fixed64_eq_import = MIR_new_import (ctx, "fixed64_eq");
  fixed64_ne_proto = MIR_new_proto_arr (ctx, "fixed64_ne_p", 1, &i64, 2, cmp_vars);
  fixed64_ne_import = MIR_new_import (ctx, "fixed64_ne");
  fixed64_lt_proto = MIR_new_proto_arr (ctx, "fixed64_lt_p", 1, &i64, 2, cmp_vars);
  fixed64_lt_import = MIR_new_import (ctx, "fixed64_lt");
  fixed64_le_proto = MIR_new_proto_arr (ctx, "fixed64_le_p", 1, &i64, 2, cmp_vars);
  fixed64_le_import = MIR_new_import (ctx, "fixed64_le");
  fixed64_gt_proto = MIR_new_proto_arr (ctx, "fixed64_gt_p", 1, &i64, 2, cmp_vars);
  fixed64_gt_import = MIR_new_import (ctx, "fixed64_gt");
  fixed64_ge_proto = MIR_new_proto_arr (ctx, "fixed64_ge_p", 1, &i64, 2, cmp_vars);
  fixed64_ge_import = MIR_new_import (ctx, "fixed64_ge");
  MIR_var_t int_vars[2];
  int_vars[0] = res_arg;
  int_vars[1].name = "i";
  int_vars[1].type = MIR_T_I64;
  fixed64_from_int_proto = MIR_new_proto_arr (ctx, "fixed64_from_int_p", 0, NULL, 2, int_vars);
  fixed64_from_int_import = MIR_new_import (ctx, "fixed64_from_int");
  MIR_var_t to_int_vars[1];
  to_int_vars[0].name = "a";
  to_int_vars[0].type = MIR_T_BLK;
  to_int_vars[0].size = sizeof (basic_num_t);
  fixed64_to_int_proto = MIR_new_proto_arr (ctx, "fixed64_to_int_p", 1, &i64, 1, to_int_vars);
  fixed64_to_int_import = MIR_new_import (ctx, "fixed64_to_int");
}

int basic_mir_emit_fixed64 (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code, MIR_op_t *ops,
                            size_t nops) {
  MIR_insn_t insn;
  switch (code) {
  case MIR_DADD:
  case MIR_DSUB:
  case MIR_DMUL:
  case MIR_DDIV:
  case MIR_DEQ:
  case MIR_DNE:
  case MIR_DLT:
  case MIR_DLE:
  case MIR_DGT:
  case MIR_DGE: insn = basic_mir_binop (ctx, func, code, ops[0], ops[1], ops[2]); break;
  case MIR_DNEG: insn = basic_mir_unop (ctx, func, code, ops[0], ops[1]); break;
  case MIR_I2D: insn = basic_mir_i2n (ctx, func, ops[0], ops[1]); break;
  case MIR_D2I: insn = basic_mir_n2i (ctx, func, ops[0], ops[1]); break;
  case MIR_DBEQ:
  case MIR_DBNE:
  case MIR_DBLT:
  case MIR_DBLE:
  case MIR_DBGT:
  case MIR_DBGE: basic_mir_bcmp (ctx, func, code, ops[0], ops[1], ops[2]); return 1;
  default: return 0;
  }
  MIR_append_insn (ctx, func, insn);
  return 1;
}

void basic_input (basic_num_t *res) {
  if (!basic_num_scan (stdin, res)) *res = BASIC_ZERO;
}

void basic_pos (basic_num_t *res) { *res = basic_num_from_int (basic_pos_val); }

void basic_input_hash (basic_num_t *res, basic_num_t n) {
  int idx = basic_num_to_int (n);
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) {
    *res = BASIC_ZERO;
    return;
  }
  if (!basic_num_scan (basic_files[idx], res)) *res = BASIC_ZERO;
}

void basic_read (basic_num_t *res) {
  if (basic_data_pos >= basic_data_len || basic_data_items[basic_data_pos].is_str) {
    *res = BASIC_ZERO;
    return;
  }
  *res = basic_data_items[basic_data_pos++].num;
}

#if defined(BASIC_PRNG128)
void basic_rnd (basic_num_t *res, basic_num_t n) {
  if (!seeded) {
    basic_randomize (BASIC_ZERO, BASIC_ZERO);
  }
  __uint128_t r = basic_next_u128 ();
  uint64_t hi = (uint64_t) (r >> 64);
  uint64_t lo = (uint64_t) r;
  basic_num_t base = basic_num_from_int ((long) (1ULL << 32));
  basic_num_t base64 = basic_num_mul (base, base);
  basic_num_t hi_num = basic_num_add (basic_num_mul (basic_num_from_int ((long) (hi >> 32)), base),
                                      basic_num_from_int ((long) (hi & UINT32_MAX)));
  basic_num_t lo_num = basic_num_add (basic_num_mul (basic_num_from_int ((long) (lo >> 32)), base),
                                      basic_num_from_int ((long) (lo & UINT32_MAX)));
  basic_num_t frac = basic_num_div (hi_num, base64);
  basic_num_t base128 = basic_num_mul (base64, base64);
  frac = basic_num_add (frac, basic_num_div (lo_num, base128));
  *res = basic_num_mul (frac, n);
}
#else
void basic_rnd (basic_num_t *res, basic_num_t n) {
  if (!seeded) {
    basic_randomize (BASIC_ZERO, BASIC_ZERO);
  }
  uint64_t r = basic_next_u64 ();
  basic_num_t hi = basic_num_from_int ((long) (r >> 32));
  basic_num_t lo = basic_num_from_int ((long) (r & UINT32_MAX));
  basic_num_t base = basic_num_from_int ((long) (1ULL << 32));
  basic_num_t frac = basic_num_add (hi, basic_num_div (lo, base));
  frac = basic_num_div (frac, base);
  *res = basic_num_mul (frac, n);
}
#endif

#include "basic_common.h"
#include "basic_num_hooks.h"
#include "basic_fixed64_hooks.h"

#define BASIC_MIR_MOV MIR_MOV

MIR_item_t fixed64_add_proto, fixed64_add_import, fixed64_sub_proto, fixed64_sub_import,
  fixed64_mul_proto, fixed64_mul_import, fixed64_div_proto, fixed64_div_import, fixed64_eq_proto,
  fixed64_eq_import, fixed64_ne_proto, fixed64_ne_import, fixed64_lt_proto, fixed64_lt_import,
  fixed64_le_proto, fixed64_le_import, fixed64_gt_proto, fixed64_gt_import, fixed64_ge_proto,
  fixed64_ge_import, fixed64_from_int_proto, fixed64_from_int_import, fixed64_to_int_proto,
  fixed64_to_int_import, fixed64_neg_proto, fixed64_neg_import;

/*
  Numeric constants must be emitted via the helpers `emit_num_const` or
  `BASIC_MIR_new_num_op` so that values are materialized in memory and
  referenced from MIR instructions.  Avoid creating constants with
  `MIR_new_*_op` directly, which embeds immediates and breaks the fixed64
  representation.
*/
static MIR_op_t fixed64_emit_num_const (MIR_context_t ctx, basic_num_t v) {
  basic_num_t *p = basic_pool_alloc (sizeof (basic_num_t));
  *p = v;
  MIR_item_t data_item = MIR_new_data (ctx, NULL, MIR_T_U8, sizeof (basic_num_t), p);
  return MIR_new_ref_op (ctx, data_item);
}
#define BASIC_EMIT_NUM_CONST fixed64_emit_num_const

static void basic_fixed64_init (MIR_context_t ctx) {
  MIR_type_t i64_pair[2] = {MIR_T_I64, MIR_T_I64};
  MIR_type_t i64_ = MIR_T_I64;
  MIR_var_t bin_vars[2];
  bin_vars[0].name = "a";
  bin_vars[0].type = MIR_T_BLK + 1;
  bin_vars[0].size = sizeof (basic_num_t);
  bin_vars[1].name = "b";
  bin_vars[1].type = MIR_T_BLK + 1;
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
  un_vars[0].type = MIR_T_BLK + 1;
  un_vars[0].size = sizeof (basic_num_t);
  fixed64_neg_proto = MIR_new_proto_arr (ctx, "fixed64_neg_p", 2, i64_pair, 1, un_vars);
  fixed64_neg_import = MIR_new_import (ctx, "fixed64_neg");

  MIR_var_t cmp_vars[2];
  cmp_vars[0].name = "a";
  cmp_vars[0].type = MIR_T_BLK + 1;
  cmp_vars[0].size = sizeof (basic_num_t);
  cmp_vars[1].name = "b";
  cmp_vars[1].type = MIR_T_BLK + 1;
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

  MIR_var_t to_int_vars[1];
  to_int_vars[0].name = "a";
  to_int_vars[0].type = MIR_T_BLK + 1;
  to_int_vars[0].size = sizeof (basic_num_t);
  fixed64_to_int_proto = MIR_new_proto_arr (ctx, "fixed64_to_int_p", 1, &i64_, 1, to_int_vars);
  fixed64_to_int_import = MIR_new_import (ctx, "fixed64_to_int");
}

basic_num_hooks_t basic_num_hooks = {
  .mir_binop = basic_mir_binop,
  .mir_unop = basic_mir_unop,
  .mir_bcmp = basic_mir_bcmp,
  .mir_i2n = basic_mir_i2n,
  .mir_n2i = basic_mir_n2i,
};

void basic_num_init (MIR_context_t ctx) { basic_fixed64_init (ctx); }

static inline MIR_item_t basic_proto_num (MIR_context_t ctx, const char *name, size_t nargs,
                                          MIR_var_t *vars) {
  MIR_var_t all_vars[nargs + 1];
  all_vars[0].type = MIR_T_P;
  all_vars[0].name = "res";
  for (size_t i = 0; i < nargs; ++i) {
    all_vars[i + 1] = vars[i];
    if (all_vars[i + 1].type == BASIC_MIR_NUM_T) {
      all_vars[i + 1].type = MIR_T_BLK;
      all_vars[i + 1].size = sizeof (basic_num_t);
    }
  }
  return MIR_new_proto_arr (ctx, name, 0, NULL, nargs + 1, all_vars);
}

#define BASIC_PROTO_NUM(ctx, name, nargs, ...) \
  basic_proto_num (ctx, name, nargs, (MIR_var_t[]) {__VA_ARGS__})

#include "basicc_core.c"

#ifndef BASIC_RUNTIME_FIXED64_H
#define BASIC_RUNTIME_FIXED64_H

#include "basic_num.h"
#include "mir.h"
#include "basic_pool.h"

#ifdef BASIC_USE_FIXED64

#define BASIC_MIR_NUM_T MIR_T_BLK
static inline MIR_op_t BASIC_MIR_new_num_op (MIR_context_t ctx, basic_num_t v) {
  basic_num_t *p = basic_pool_alloc (sizeof (basic_num_t));
  *p = v;
  MIR_item_t data_item = MIR_new_data (ctx, NULL, MIR_T_U8, sizeof (basic_num_t), p);
  return MIR_new_ref_op (ctx, data_item);
}

void basic_runtime_fixed64_init (MIR_context_t ctx);
int basic_mir_emit_fixed64 (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code, MIR_op_t *ops,
                            size_t nops);

#else

static inline void basic_runtime_fixed64_init (MIR_context_t ctx) { (void) ctx; }
static inline int basic_mir_emit_fixed64 (MIR_context_t ctx, MIR_item_t func, MIR_insn_code_t code,
                                          MIR_op_t *ops, size_t nops) {
  (void) ctx;
  (void) func;
  (void) code;
  (void) ops;
  (void) nops;
  return 0;
}

#endif

#endif /* BASIC_RUNTIME_FIXED64_H */

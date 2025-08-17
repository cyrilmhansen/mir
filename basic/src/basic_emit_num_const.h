#ifndef BASIC_EMIT_NUM_CONST_H
#define BASIC_EMIT_NUM_CONST_H

#include "basic_num.h"
#include "mir.h"
#include "basic_pool.h"

#define MIR_NEW_OP(name) MIR_new_##name##_op

/*
  Numeric constants must be emitted via this helper so that values are
  materialized in memory and referenced from MIR instructions.  Avoid creating
  constants with MIR_new_*_op directly, which embeds immediates and breaks the
  fixed64 representation.
*/
static inline MIR_op_t fixed64_emit_num_const (MIR_context_t ctx, basic_num_t v) {
#if defined(BASIC_USE_FIXED64)
  basic_num_t *p = basic_pool_alloc (sizeof (basic_num_t));
  *p = v;
  MIR_item_t data_item = MIR_new_data (ctx, NULL, MIR_T_U8, sizeof (basic_num_t), p);
  return MIR_new_ref_op (ctx, data_item);
#elif defined(BASIC_USE_LONG_DOUBLE)
  return MIR_NEW_OP (ldouble) (ctx, v);
#else
  return MIR_NEW_OP (double) (ctx, v);
#endif
}

#endif /* BASIC_EMIT_NUM_CONST_H */

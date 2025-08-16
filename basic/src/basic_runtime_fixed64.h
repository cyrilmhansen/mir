#ifndef BASIC_RUNTIME_FIXED64_H
#define BASIC_RUNTIME_FIXED64_H

#include "basic_num.h"
#include "mir.h"
#include "basic_pool.h"
#include <stdint.h>

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

void basic_input (basic_num_t *);
void basic_read (basic_num_t *);
void basic_pos (basic_num_t *);
void basic_rnd (basic_num_t *, basic_num_t);
void basic_input_hash (basic_num_t *, int64_t);

void basic_abs_wrap (basic_num_t *, basic_num_t);
void basic_sgn_wrap (basic_num_t *, basic_num_t);
void basic_sqr_wrap (basic_num_t *, basic_num_t);
void basic_sin_wrap (basic_num_t *, basic_num_t);
void basic_cos_wrap (basic_num_t *, basic_num_t);
void basic_tan_wrap (basic_num_t *, basic_num_t);
void basic_atn_wrap (basic_num_t *, basic_num_t);
void basic_sinh_wrap (basic_num_t *, basic_num_t);
void basic_cosh_wrap (basic_num_t *, basic_num_t);
void basic_tanh_wrap (basic_num_t *, basic_num_t);
void basic_asinh_wrap (basic_num_t *, basic_num_t);
void basic_acosh_wrap (basic_num_t *, basic_num_t);
void basic_atanh_wrap (basic_num_t *, basic_num_t);
void basic_asin_wrap (basic_num_t *, basic_num_t);
void basic_acos_wrap (basic_num_t *, basic_num_t);
void basic_log_wrap (basic_num_t *, basic_num_t);
void basic_log2_wrap (basic_num_t *, basic_num_t);
void basic_log10_wrap (basic_num_t *, basic_num_t);
void basic_exp_wrap (basic_num_t *, basic_num_t);
void basic_fact_wrap (basic_num_t *, basic_num_t);
void basic_pow_wrap (basic_num_t *, basic_num_t, basic_num_t);
void basic_pi_wrap (basic_num_t *);
void basic_val_wrap (basic_num_t *, const char *);
void basic_int_wrap (basic_num_t *, basic_num_t);
void basic_timer_wrap (basic_num_t *);
void basic_time_wrap (basic_num_t *);
void basic_date_wrap (basic_num_t *);
void basic_peek_wrap (basic_num_t *, basic_num_t);
void basic_eof_wrap (basic_num_t *, int64_t);
void basic_system_wrap (basic_num_t *, const char *);
void basic_get_error_handler_wrap (basic_num_t *);
void basic_get_line_wrap (basic_num_t *);

char *basic_chr_wrap (int64_t);
char *basic_unichar_wrap (int64_t);
char *basic_string_wrap (int64_t, const char *);
char *basic_left_wrap (const char *, int64_t);
char *basic_right_wrap (const char *, int64_t);
char *basic_mid_wrap (const char *, int64_t, int64_t);
char *basic_input_chr_wrap (int64_t);

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

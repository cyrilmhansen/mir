#include "basic_runtime.h"
#include "basic_runtime_shared.h"
#include "basic_pool.h"
#ifdef BASIC_USE_FIXED64
#include "basic_runtime_fixed64.h"
#define basic_abs basic_abs_wrap
#define basic_sgn basic_sgn_wrap
#define basic_sqr basic_sqr_wrap
#define basic_sin basic_sin_wrap
#define basic_cos basic_cos_wrap
#define basic_tan basic_tan_wrap
#define basic_atn basic_atn_wrap
#define basic_sinh basic_sinh_wrap
#define basic_cosh basic_cosh_wrap
#define basic_tanh basic_tanh_wrap
#define basic_asinh basic_asinh_wrap
#define basic_acosh basic_acosh_wrap
#define basic_atanh basic_atanh_wrap
#define basic_asin basic_asin_wrap
#define basic_acos basic_acos_wrap
#define basic_log basic_log_wrap
#define basic_log2 basic_log2_wrap
#define basic_log10 basic_log10_wrap
#define basic_exp basic_exp_wrap
#define basic_pow basic_pow_wrap
#define basic_pi basic_pi_wrap
#define basic_val basic_val_wrap
#define basic_int basic_int_wrap
#define basic_timer basic_timer_wrap
#define basic_time basic_time_wrap
#define basic_date basic_date_wrap
#define basic_peek basic_peek_wrap
#define basic_eof basic_eof_wrap
#define basic_system basic_system_wrap
#define basic_get_error_handler basic_get_error_handler_wrap
#define basic_get_line basic_get_line_wrap
#define basic_fact basic_fact_wrap
#endif
#include "mir-hash.h"

#include <dlfcn.h>
#include <string.h>

void basic_eval (const char *);

#define RUNTIME_HASH_SIZE 256

static BasicRuntimeSymbol runtime_symbols[] = {
  {"basic_print", basic_print},
  {"basic_print_str", basic_print_str},
  {"basic_input", basic_input},
  {"basic_input_str", basic_input_str},
  {"basic_get", basic_get},
  {"basic_inkey", basic_inkey},
  {"basic_put", basic_put},
  {"basic_strcmp", basic_strcmp},
  {"basic_open", basic_open},
  {"basic_close", basic_close},
  {"basic_print_hash", basic_print_hash},
  {"basic_print_hash_str", basic_print_hash_str},
  {"basic_input_hash", basic_input_hash},
  {"basic_input_hash_str", basic_input_hash_str},
  {"basic_get_hash", basic_get_hash},
  {"basic_put_hash", basic_put_hash},
  {"basic_eval", basic_eval},
  {"basic_eof", basic_eof},
  {"basic_read", basic_read},
  {"basic_read_str", basic_read_str},
  {"basic_restore", basic_restore},
  {"basic_clear_array", basic_clear_array},
  {"basic_dim_alloc", basic_dim_alloc},
  {"basic_home", basic_home},
  {"basic_vtab", basic_vtab},
  {"basic_randomize", basic_randomize},
  {"basic_rnd", basic_rnd},
  {"basic_abs", basic_abs},
  {"basic_sgn", basic_sgn},
  {"basic_iabs", basic_iabs},
  {"basic_isgn", basic_isgn},
  {"basic_sqr", basic_sqr},
  {"basic_sin", basic_sin},
  {"basic_cos", basic_cos},
  {"basic_tan", basic_tan},
  {"basic_atn", basic_atn},
  {"basic_sinh", basic_sinh},
  {"basic_cosh", basic_cosh},
  {"basic_tanh", basic_tanh},
  {"basic_asinh", basic_asinh},
  {"basic_acosh", basic_acosh},
  {"basic_atanh", basic_atanh},
  {"basic_asin", basic_asin},
  {"basic_acos", basic_acos},
  {"basic_log", basic_log},
  {"basic_log2", basic_log2},
  {"basic_log10", basic_log10},
  {"basic_exp", basic_exp},
  {"basic_pow", basic_pow},
  {"basic_pi", basic_pi},
  {"basic_instr", basic_instr},
  {"basic_screen", basic_screen},
  {"basic_cls", basic_cls},
  {"basic_color", basic_color},
  {"basic_key_off", basic_key_off},
  {"basic_locate", basic_locate},
  {"basic_tab", basic_tab},
  {"basic_htab", basic_htab},
  {"basic_pos", basic_pos},
  {"basic_text", basic_text},
  {"basic_inverse", basic_inverse},
  {"basic_normal", basic_normal},
  {"basic_hgr2", basic_hgr2},
  {"basic_hcolor", basic_hcolor},
  {"basic_hplot", basic_hplot},
  {"basic_hplot_to", basic_hplot_to},
  {"basic_hplot_to_current", basic_hplot_to_current},
  {"basic_move", basic_move},
  {"basic_draw", basic_draw},
  {"basic_draw_line", basic_draw_line},
  {"basic_circle", basic_circle},
  {"basic_rect", basic_rect},
  {"basic_fill", basic_fill},
  {"basic_mode", basic_mode},
  {"basic_profile_line", basic_profile_line},
  {"basic_profile_func_enter", basic_profile_func_enter},
  {"basic_profile_func_exit", basic_profile_func_exit},
  {"basic_chr", basic_chr},
  {"basic_unichar", basic_unichar},
  {"basic_string", basic_string},
  {"basic_concat", basic_concat},
  {"basic_left", basic_left},
  {"basic_right", basic_right},
  {"basic_mid", basic_mid},
  {"basic_mirror", basic_mirror},
  {"basic_upper", basic_upper},
  {"basic_lower", basic_lower},
  {"basic_len", basic_len},
  {"basic_val", basic_val},
  {"basic_str", basic_str},
  {"basic_asc", basic_asc},
  {"basic_int", basic_int},
  {"basic_timer", basic_timer},
  {"basic_time", basic_time},
  {"basic_time_str", basic_time_str},
  {"basic_date", basic_date},
  {"basic_date_str", basic_date_str},
  {"basic_input_chr", basic_input_chr},
  {"basic_peek", basic_peek},
  {"basic_poke", basic_poke},
  {"basic_stop", basic_stop},
  {"basic_return_error", basic_return_error},
  {"basic_set_error_handler", basic_set_error_handler},
  {"basic_get_error_handler", basic_get_error_handler},
  {"basic_set_line", basic_set_line},
  {"basic_get_line", basic_get_line},
  {"basic_enable_line_tracking", basic_enable_line_tracking},
  {"basic_delay", basic_delay},
  {"basic_beep", basic_beep},
  {"basic_sound", basic_sound},
  {"basic_sound_off", basic_sound_off},
  {"basic_system", basic_system},
  {"basic_system_out", basic_system_out},
  {"basic_strdup", basic_strdup},
  {"basic_free", basic_free},
  {"basic_calloc", basic_calloc},
  {"basic_pool_reset", basic_pool_reset},
  {"memset", memset},
  {"basic_mir_ctx", basic_mir_ctx},
  {"basic_mir_mod", basic_mir_mod},
  {"basic_mir_func", basic_mir_func},
  {"basic_mir_reg", basic_mir_reg},
  {"basic_mir_label", basic_mir_label},
  {"basic_mir_emit", basic_mir_emit},
  {"basic_mir_emitlbl", basic_mir_emitlbl},
  {"basic_mir_ret", basic_mir_ret},
  {"basic_mir_finish", basic_mir_finish},
  {"basic_mir_run", basic_mir_run},
  {"basic_mir_dump", basic_mir_dump},
  {"basic_fact", basic_fact},
};

#define RUNTIME_SYMBOLS_CNT (sizeof (runtime_symbols) / sizeof (runtime_symbols[0]))

static BasicRuntimeSymbol *runtime_symbol_hash[RUNTIME_HASH_SIZE];
static int runtime_symbol_hash_inited = 0;

static size_t runtime_symbol_hash_func (const char *s) {
  return (size_t) mir_hash (s, strlen (s), 0);
}

static void runtime_symbol_hash_insert (BasicRuntimeSymbol *sym) {
  size_t h = runtime_symbol_hash_func (sym->name) & (RUNTIME_HASH_SIZE - 1);
  while (runtime_symbol_hash[h] != NULL) h = (h + 1) & (RUNTIME_HASH_SIZE - 1);
  runtime_symbol_hash[h] = sym;
}

static void runtime_symbol_hash_init (void) {
  for (size_t i = 0; i < RUNTIME_SYMBOLS_CNT; ++i) runtime_symbol_hash_insert (&runtime_symbols[i]);
  runtime_symbol_hash_inited = 1;
}

void basic_runtime_add_symbols (BasicRuntimeSymbol *syms, size_t cnt) {
  if (!runtime_symbol_hash_inited) runtime_symbol_hash_init ();
  for (size_t i = 0; i < cnt; ++i) runtime_symbol_hash_insert (&syms[i]);
}

void *basic_runtime_resolve (const char *name) {
  if (!runtime_symbol_hash_inited) runtime_symbol_hash_init ();
  size_t h = runtime_symbol_hash_func (name) & (RUNTIME_HASH_SIZE - 1);
  for (;;) {
    BasicRuntimeSymbol *sym = runtime_symbol_hash[h];
    if (sym == NULL) break;
    if (strcmp (name, sym->name) == 0) return sym->fn;
    h = (h + 1) & (RUNTIME_HASH_SIZE - 1);
  }
  return dlsym (NULL, name);
}

size_t basic_runtime_symbols (BasicRuntimeSymbol **syms) {
  if (syms != NULL) *syms = runtime_symbols;
  return RUNTIME_SYMBOLS_CNT;
}

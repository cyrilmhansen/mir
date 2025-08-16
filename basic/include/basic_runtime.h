#ifndef BASIC_RUNTIME_H
#define BASIC_RUNTIME_H
#include "basic_num.h"
#include "mir.h"
#include "mir-gen.h"
#include <stddef.h>

typedef struct BasicRuntimeSymbol {
  const char *name;
  void *fn;
} BasicRuntimeSymbol;

size_t basic_runtime_symbols (BasicRuntimeSymbol **syms);

long basic_len (const char *s);
long basic_instr (const char *s, const char *sub);
long basic_asc (const char *s);

basic_num_t basic_mir_ctx (void);
basic_num_t basic_mir_mod (basic_num_t ctx, const char *name);
basic_num_t basic_mir_func (basic_num_t mod, const char *name, basic_num_t nargs);
basic_num_t basic_mir_reg (basic_num_t func);
basic_num_t basic_mir_label (basic_num_t func);
basic_num_t basic_mir_emit (basic_num_t func, const char *op, basic_num_t a, basic_num_t b,
                            basic_num_t c);
basic_num_t basic_mir_emitlbl (basic_num_t func, basic_num_t label);
basic_num_t basic_mir_ret (basic_num_t func, basic_num_t reg);
basic_num_t basic_mir_finish (basic_num_t mod);
basic_num_t basic_mir_run (basic_num_t func, basic_num_t a1, basic_num_t a2, basic_num_t a3,
                           basic_num_t a4);
basic_num_t basic_mir_dump (basic_num_t func);

void *basic_dim_alloc (void *base, basic_num_t len, basic_num_t is_str);
void basic_clear_array (void *base, basic_num_t len, basic_num_t is_str);
void basic_runtime_fini (void);

#endif /* BASIC_RUNTIME_H */

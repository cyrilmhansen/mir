#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASIC_USE_FIXED64
#include "basic_num.h"
#include "mir.h"
#include "mir-gen.h"

typedef enum { H_CTX, H_MOD, H_FUNC } HandleKind;

typedef struct {
  HandleKind kind;
  MIR_context_t ctx;
  void *ptr;
} Handle;

static Handle *handle_tab = NULL;
static size_t handle_len = 0;

static basic_num_t new_handle (HandleKind kind, MIR_context_t ctx, void *ptr) {
  handle_tab = realloc (handle_tab, (handle_len + 1) * sizeof (Handle));
  handle_tab[handle_len] = (Handle) {kind, ctx, ptr};
  return basic_num_from_int (++handle_len);
}

static Handle *get_handle (basic_num_t h) {
  size_t idx = (size_t) basic_num_to_int (h);
  if (idx == 0 || !basic_num_eq (basic_num_from_int ((long) idx), h) || idx > handle_len)
    return NULL;
  return &handle_tab[idx - 1];
}

static basic_num_t basic_mir_ctx_impl (void) {
  static basic_num_t ctx_handle = BASIC_ZERO;
  if (basic_num_ne (ctx_handle, BASIC_ZERO)) return ctx_handle;
  MIR_context_t ctx = MIR_init ();
  ctx_handle = new_handle (H_CTX, ctx, ctx);
  return ctx_handle;
}

basic_num_t basic_mir_ctx (void) { return basic_mir_ctx_impl (); }

basic_num_t basic_mir_mod (basic_num_t ctx_h, const char *name) {
  Handle *h = get_handle (ctx_h);
  if (h == NULL || h->kind != H_CTX) return BASIC_ZERO;
  MIR_context_t ctx = h->ctx;
  MIR_module_t mod = MIR_new_module (ctx, name);
  return new_handle (H_MOD, ctx, mod);
}

basic_num_t basic_mir_func (basic_num_t mod_h, const char *name, basic_num_t nargs_d) {
  Handle *h = get_handle (mod_h);
  if (h == NULL || h->kind != H_MOD) return BASIC_ZERO;
  MIR_context_t ctx = h->ctx;
  size_t nargs = (size_t) basic_num_to_int (nargs_d);
  MIR_type_t res = MIR_T_D;
  MIR_var_t *vars = nargs ? malloc (nargs * sizeof (MIR_var_t)) : NULL;
  for (size_t i = 0; i < nargs; i++) {
    size_t len = snprintf (NULL, 0, "a%zu", i);
    char *arg_name = malloc (len + 1);
    snprintf (arg_name, len + 1, "a%zu", i);
    vars[i].type = MIR_T_D;
    vars[i].name = arg_name;
  }
  MIR_item_t func = MIR_new_func_arr (ctx, name, 1, &res, nargs, vars);
  return new_handle (H_FUNC, ctx, func);
}

int main (void) {
  basic_num_t ctx = basic_mir_ctx ();
  assert (basic_num_ne (ctx, BASIC_ZERO));
  basic_num_t ctx2 = basic_mir_ctx ();
  assert (basic_num_eq (ctx, ctx2));
  basic_num_t mod = basic_mir_mod (ctx, "m");
  assert (basic_num_ne (mod, BASIC_ZERO));
  basic_num_t func = basic_mir_func (mod, "f", BASIC_ZERO);
  assert (basic_num_ne (func, BASIC_ZERO));
  Handle *hc = get_handle (ctx);
  MIR_finish (hc->ctx);
  printf ("basic_handles_fixed64_test OK\n");
  free (handle_tab);
  return 0;
}

#ifndef BASIC_RUNTIME_H
#define BASIC_RUNTIME_H
#include "mir.h"
#include "mir-gen.h"

double basic_mir_ctx (void);
double basic_mir_mod (double ctx, const char *name);
double basic_mir_func (double mod, const char *name, double nargs);
double basic_mir_reg (double func);
double basic_mir_label (double func);
double basic_mir_emit (double func, const char *op, double a, double b, double c);
double basic_mir_emitlbl (double func, double label);
double basic_mir_ret (double func, double reg);
double basic_mir_finish (double mod);
double basic_mir_run (double func, double a1, double a2, double a3, double a4);
double basic_mir_dump (double func);

#endif /* BASIC_RUNTIME_H */

#ifndef BASIC_NUM_H
#define BASIC_NUM_H

#if defined(BASIC_USE_LONG_DOUBLE)
typedef long double basic_num_t;
#define BASIC_NUM_SCANF "%Lf"
#define BASIC_NUM_PRINTF "%.21Lg"
#define BASIC_SNPRINTF snprintf
#define BASIC_STRTOF strtold
#define BASIC_FABS fabsl
#define BASIC_SQRT sqrtl
#define BASIC_SIN sinl
#define BASIC_COS cosl
#define BASIC_TAN tanl
#define BASIC_ATAN atanl
#define BASIC_LOG logl
#define BASIC_EXP expl
#define BASIC_FLOOR floorl
#else
typedef double basic_num_t;
#define BASIC_NUM_SCANF "%lf"
#define BASIC_NUM_PRINTF "%.15g"
#define BASIC_SNPRINTF snprintf
#define BASIC_STRTOF strtod
#define BASIC_FABS fabs
#define BASIC_SQRT sqrt
#define BASIC_SIN sin
#define BASIC_COS cos
#define BASIC_TAN tan
#define BASIC_ATAN atan
#define BASIC_LOG log
#define BASIC_EXP exp
#define BASIC_FLOOR floor
#endif

#endif /* BASIC_NUM_H */

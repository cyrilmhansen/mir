#ifndef BASIC_COMMON_H
#define BASIC_COMMON_H

#include "basic_num.h"
#include "mir.h"
#include "mir-gen.h"
#include "basic_runtime.h"
#include "arena.h"
#include "basic_pool.h"
#ifdef BASIC_USE_FIXED64
#include "basic_runtime_fixed64.h"
#endif
#include "basic_runtime_shared.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <math.h>

#ifndef BASIC_SRC_DIR
#define BASIC_SRC_DIR "."
#endif

static int safe_snprintf (char *buf, size_t size, const char *fmt, ...);

#endif /* BASIC_COMMON_H */

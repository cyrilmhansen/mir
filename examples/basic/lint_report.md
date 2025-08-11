# Lint Report for basicc.c

## cppcheck
- Reported multiple `memleakOnRealloc` errors and possible null pointer dereferences in parameter handling and line parsing.

## clang-tidy
- Warned about potential memory leak in `parse_file` and numerous uses of insecure C library calls such as `snprintf`, `sprintf`, and `fprintf`.

## GCC (-Wall -Wextra)
- Compilation produced an error for missing `<stdarg.h>` and several warnings including implicit `strdup` declarations and unhandled enum values in `gen_stmt`.


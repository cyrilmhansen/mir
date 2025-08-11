# basicc.c linter results

## cpplint
- Reported 2875 issues, mainly whitespace and line-length warnings.

## clang-tidy
- Warned about insecure `fprintf` usage around lines 5048, 5054, and 5065.

## gcc -Wall -Wextra -fsyntax-only
- Highlighted numerous unhandled enumeration values in `gen_stmt` (switch at line 3579).
- Suggested adding braces around an empty `else` body in `repl` (line 4987).

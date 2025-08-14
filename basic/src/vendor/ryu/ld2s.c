// Copyright 2018 Ulf Adams
//
// The contents of this file may be used under the terms of the Apache License,
// Version 2.0.
//
//    (See accompanying file LICENSE-Apache2 or copy at
//     http://www.apache.org/licenses/LICENSE-2.0)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.

#include "ryu/ld2s.h"
#include "ryu/ryu_generic_128.h"

#include <stdlib.h>

int ld2s_buffered_n (long double d, char *result) {
  const struct floating_decimal_128 v = long_double_to_fd128 (d);
  return generic_to_chars (v, result);
}

void ld2s_buffered (long double d, char *result) {
  const int index = ld2s_buffered_n (d, result);
  result[index] = '\0';
}

char *ld2s (long double d) {
  char *const result = (char *) malloc (64);
  ld2s_buffered (d, result);
  return result;
}

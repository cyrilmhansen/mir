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
#include "ryu/ryu.h"

#include <stdlib.h>

int ld2s_buffered_n (long double d, char *result) { return d2s_buffered_n ((double) d, result); }

void ld2s_buffered (long double d, char *result) { d2s_buffered ((double) d, result); }

char *ld2s (long double d) { return d2s ((double) d); }

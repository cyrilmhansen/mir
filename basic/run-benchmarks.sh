#!/usr/bin/env bash
# Run BASIC benchmarks in JIT and compiled modes.
set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BASICC="$ROOT/basic/basicc"

# Build basicc if necessary
if [ ! -x "$BASICC" ]; then
  (cd "$ROOT" && make basic/basicc)
fi

run_bench() {
  local name="$1"
  local file="$ROOT/basic/samples/${name}.bas"
  echo "== ${name} =="
  echo "-- JIT --"
  (cd "$ROOT" && { TIMEFORMAT='real %R'; time "$BASICC" -j "$file" >/dev/null; } 2>&1)
  echo "-- Compiled --"
  (cd "$ROOT" && { TIMEFORMAT='real %R'; time "$BASICC" -b -o "$ROOT/basic/${name}-bench" "$file" >/dev/null; } 2>&1)
  { TIMEFORMAT='real %R'; time "$ROOT/basic/${name}-bench" >/dev/null; } 2>&1
}

run_bench sieve
run_bench fib
run_bench ahl_benchmark

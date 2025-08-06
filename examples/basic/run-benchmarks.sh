#!/usr/bin/env bash
# Run BASIC benchmarks in JIT and compiled modes.
set -eu

ROOT="$(cd "$(dirname "$0")/../../" && pwd)"
BASICC="$ROOT/basic/basicc"

# Build basicc if necessary
if [ ! -x "$BASICC" ]; then
  (cd "$ROOT" && make basic/basicc)
fi

run_bench() {
  local name="$1"
  local file="$ROOT/examples/basic/${name}.bas"
  echo "== ${name} =="
  echo "-- JIT --"
  (cd "$ROOT" && time -p "$BASICC" -j "$file" >/dev/null)
  echo "-- Compiled --"
  (cd "$ROOT" && time -p "$BASICC" -b -o "$ROOT/basic/${name}-bench" "$file" >/dev/null)
  time -p "$ROOT/basic/${name}-bench" >/dev/null
}

run_bench sieve
run_bench fib

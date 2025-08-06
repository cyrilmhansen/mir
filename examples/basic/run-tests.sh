#!/usr/bin/env bash
# Run BASIC compiler example tests.
set -eu
ROOT="$(cd "$(dirname "$0")/../../" && pwd)"
BASICC="$ROOT/basic/basicc"
# Build basicc if missing
if [ ! -x "$BASICC" ]; then
	(cd "$ROOT" && make basic/basicc)
fi

run_test() {
	local name="$1"
	local in_file="$ROOT/examples/basic/$name.in"
	local src="$ROOT/examples/basic/$name.bas"
	local exp="$ROOT/examples/basic/$name.out"
	local out="$ROOT/basic/$name.out"
	if [ -f "$in_file" ]; then
		"$BASICC" "$src" < "$in_file" > "$out"
	else
		"$BASICC" "$src" > "$out"
	fi
	diff "$exp" "$out"
}

for t in hello relop adder guess string strfuncs instr gosub; do
        echo "Running $t"
        run_test "$t"
        echo "$t OK"
done

echo "Running fleuves (no explain)"
"$BASICC" "$ROOT/examples/basic/fleuves.bas" < "$ROOT/examples/basic/fleuves.in" >/dev/null || true
echo "fleuves (no explain) done"

echo "Running fleuves (with explain)"
"$BASICC" "$ROOT/examples/basic/fleuves.bas" < "$ROOT/examples/basic/fleuves_explain.in" >/dev/null || true
echo "fleuves (with explain) done"


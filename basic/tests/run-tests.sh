#!/usr/bin/env bash
# Run BASIC compiler example tests.
set -eu
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

# Determine BASIC compiler binaries to test. If none are supplied as arguments,
# test the double, long double, and fixed-point variants built by the makefile.
if [ "$#" -gt 0 ]; then
        BASICCS=("$@")
else
        BASICCS=(
                "$ROOT/basic/basicc"
                "$ROOT/basic/basicc-ld"
                "$ROOT/basic/basicc-fix"
        )
fi

run_tests() {
        BASICC="$1"

        exp_dir="$ROOT/basic/tests/expected"
        case "$(basename "$BASICC")" in
                *-ld)  exp_dir="$exp_dir/ld" ;;
                *-fix) exp_dir="$exp_dir/fixed64" ;;
        esac

        # Build compiler if missing
        if [ ! -x "$BASICC" ]; then
                (cd "$ROOT" && make "basic/$(basename "$BASICC")")
        fi

       echo "Running usage tests"
       for opt in -h --usage -?; do
               out="$(mktemp)"
               "$BASICC" "$opt" > "$out" 2>&1
               grep -q "Usage:" "$out"
               rm -f "$out"
       done
       echo "Usage tests OK"

        echo "Running REPL smoke test"
        printf '10 PRINT 1\nRUN\nQUIT\n' | "$BASICC" > /dev/null
        echo "REPL smoke test OK"

run_test() {
        local name="$1"; shift
        local opts=("$@")
        local src="$ROOT/basic/tests/programs/$name.bas"
        local in_file="$ROOT/basic/tests/inputs/$name.in"
        local exp="$exp_dir/$name.out"
        local out="$ROOT/basic/$name.out"
        local err="$ROOT/basic/$name.err"
        mkdir -p "$(dirname "$out")"
       if [ "$name" = "vtab" ]; then
                       "$BASICC" "${opts[@]}" "$src" > "$out" 2> "$err"
               elif [ "$name" = "ahl_benchmark" ]; then
                       local time_file="$ROOT/basic/${name}.time"
                       local start end
                       start=$(date +%s%N)
                       "$BASICC" "${opts[@]}" "$src" > "$out" 2> "$err"
                       end=$(date +%s%N)
                       python3 - "$start" "$end" <<'PY' > "$time_file"
import sys
start=int(sys.argv[1])
end=int(sys.argv[2])
print((end-start)/1e9)
PY
               else
                       if [ -f "$in_file" ]; then
                               "$BASICC" "${opts[@]}" "$src" < "$in_file" > "$out" 2> "$err"
                       elif [ "$name" = "mandelbrot" ]; then
                               timeout 0.6 "$BASICC" "${opts[@]}" "$src" 2> "$err" | head -n 200 > "$out" || true
                       else
                               "$BASICC" "${opts[@]}" "$src" > "$out" 2> "$err"
                       fi
               fi
                # Strip terminal alternate screen sequences to keep diffs stable
                perl -0 -i -pe 's/\x1b\[\?1049[hl]//g' "$out"
                if [ "$name" = "circle" ] || [ "$name" = "box" ]; then
                        grep -ao "$(echo "$name" | tr a-z A-Z)" "$out" > "$out.filtered" || true
                        mv "$out.filtered" "$out"
                fi
                if grep -q "Unsupported statement" "$err"; then
                        echo "Unsupported statement in $name" >&2
                        exit 1
                fi
                if [ -s "$err" ]; then
                        echo "Unexpected stderr for $name"
                        cat "$err"
                        exit 1
                fi
                rm -f "$err"

                if [ "$name" = "datediff" ]; then
                        local y m d doy total
                        y=$(sed -n '1p' "$in_file")
                        m=$(sed -n '2p' "$in_file")
                        d=$(sed -n '3p' "$in_file")
                        set -- $(python3 - "$y" "$m" "$d" <<'PY'
import sys, datetime
y, m, d = map(int, sys.argv[1:])
dt = datetime.date(y, m, d)
print(dt.timetuple().tm_yday, datetime.date(y, 12, 31).timetuple().tm_yday)
PY
)
                        doy=$1
                        total=$2
                        exp="$out.expected"
                        printf "Year: \nMonth: \nDay: \nDays since Jan 1: %d\nDays until Dec 31: %d\n" $((doy-1)) $((total-doy)) > "$exp"
                        diff "$exp" "$out"
                        rm -f "$exp"
                elif [ "$name" = "pi" ]; then
                        grep -q '^PI=3.14159265358979' "$out"
                        grep -q '^TIME=' "$out" >/dev/null
                elif [ "$name" = "pi_builtin" ]; then
                        grep -q '^3.14159265358979' "$out"
                else
                        diff "$exp" "$out"
                fi
               if [ "$name" = "ahl_benchmark" ]; then
                       echo "ahl_benchmark time: $(cat "$time_file") s"
                       rm -f "$time_file"
               fi
        }

       if [[ "$BASICC" == *-fix ]]; then
                for name in arith cmp loop dim_expr io align convert; do
                        echo "Running $name (interp)"
                        run_test "$name"
                        echo "$name interp OK"
                        echo "Running $name (JIT)"
                        run_test "$name" -j
                        echo "$name JIT OK"
                done
                return
       fi

        echo "Running hcolor_test"
        "$ROOT/basic/hcolor_test" > "$ROOT/basic/hcolor_test.out" 2> "$ROOT/basic/hcolor_test.err"
        if [ -s "$ROOT/basic/hcolor_test.err" ]; then
                echo "Unexpected stderr for hcolor_test"
                cat "$ROOT/basic/hcolor_test.err"
                exit 1
        fi
        rm -f "$ROOT/basic/hcolor_test.err"
        diff "$ROOT/basic/test/hcolor_test.out" "$ROOT/basic/hcolor_test.out"
        echo "hcolor_test OK"

        echo "Building basic_pool_test"
        if [[ "$BASICC" == *-ld ]]; then
                cc -Wall -Wextra -DBASIC_USE_LONG_DOUBLE -I"$ROOT/basic/include" -I"$ROOT/basic/src" -I"$ROOT/basic/src/vendor" \
                        "$ROOT/basic/src/basic_pool.c" \
                        "$ROOT/basic/test/basic_pool_test.c" \
                        -o "$ROOT/basic/basic_pool_test"
        else
                cc -Wall -Wextra -I"$ROOT/basic/include" -I"$ROOT/basic/src" -I"$ROOT/basic/src/vendor" \
                        "$ROOT/basic/src/basic_pool.c" \
                        "$ROOT/basic/test/basic_pool_test.c" \
                        -o "$ROOT/basic/basic_pool_test"
        fi
        echo "Running basic_pool_test"
        "$ROOT/basic/basic_pool_test" > "$ROOT/basic/basic_pool_test.out" 2> "$ROOT/basic/basic_pool_test.err"
        if [ -s "$ROOT/basic/basic_pool_test.err" ]; then
                echo "Unexpected stderr for basic_pool_test"
                cat "$ROOT/basic/basic_pool_test.err"
                exit 1
        fi
        rm -f "$ROOT/basic/basic_pool_test.err"
        diff "$ROOT/basic/test/basic_pool_test.out" "$ROOT/basic/basic_pool_test.out"
        echo "basic_pool_test OK"

        echo "Running basic_pool_test"
        "$ROOT/basic/basic_pool_test" > "$ROOT/basic/basic_pool_test.out" 2> "$ROOT/basic/basic_pool_test.err"
        if [ -s "$ROOT/basic/basic_pool_test.err" ]; then
                echo "Unexpected stderr for basic_pool_test"
                cat "$ROOT/basic/basic_pool_test.err"
                exit 1
        fi
        rm -f "$ROOT/basic/basic_pool_test.err"
        diff "$ROOT/basic/test/basic_pool_test.out" "$ROOT/basic/basic_pool_test.out"
        echo "basic_pool_test OK"

        echo "Building basic_num_array_align_test"
        if [[ "$BASICC" == *-ld ]]; then
                cc -Wall -Wextra -DBASIC_USE_LONG_DOUBLE -I"$ROOT/basic/include" -I"$ROOT/basic/src" \
                        "$ROOT/basic/src/basic_pool.c" \
                        "$ROOT/basic/test/basic_num_array_align_test.c" \
                        -lm -o "$ROOT/basic/basic_num_array_align_test"
        else
                cc -Wall -Wextra -I"$ROOT/basic/include" -I"$ROOT/basic/src" \
                        "$ROOT/basic/src/basic_pool.c" \
                        "$ROOT/basic/test/basic_num_array_align_test.c" \
                        -lm -o "$ROOT/basic/basic_num_array_align_test"
        fi
        echo "Running basic_num_array_align_test"
        "$ROOT/basic/basic_num_array_align_test" > "$ROOT/basic/basic_num_array_align_test.out" 2> "$ROOT/basic/basic_num_array_align_test.err"
        if [ -s "$ROOT/basic/basic_num_array_align_test.err" ]; then
                echo "Unexpected stderr for basic_num_array_align_test"
                cat "$ROOT/basic/basic_num_array_align_test.err"
                exit 1
        fi
        rm -f "$ROOT/basic/basic_num_array_align_test.err"
        diff "$ROOT/basic/test/basic_num_array_align_test.out" "$ROOT/basic/basic_num_array_align_test.out"
        echo "basic_num_array_align_test OK"

        echo "Building basic_runtime_lowmem_test"
        cc -Wall -Wextra -I"$ROOT/basic/include" -I"$ROOT/basic/src" -I"$ROOT/basic/src/vendor" -I"$ROOT" -ffunction-sections \
                "$ROOT/basic/src/basic_pool.c" \
                "$ROOT/basic/src/basic_runtime.c" \
                "$ROOT/basic/test/basic_runtime_lowmem_test.c" \
                -Wl,--gc-sections -lm -o "$ROOT/basic/basic_runtime_lowmem_test"
        echo "Running basic_runtime_lowmem_test"
        "$ROOT/basic/basic_runtime_lowmem_test" > "$ROOT/basic/basic_runtime_lowmem_test.out" 2> "$ROOT/basic/basic_runtime_lowmem_test.err"
        if [ -s "$ROOT/basic/basic_runtime_lowmem_test.err" ]; then
                echo "Unexpected stderr for basic_runtime_lowmem_test"
                cat "$ROOT/basic/basic_runtime_lowmem_test.err"
                exit 1
        fi
        rm -f "$ROOT/basic/basic_runtime_lowmem_test.err"
        diff "$ROOT/basic/test/basic_runtime_lowmem_test.out" "$ROOT/basic/basic_runtime_lowmem_test.out"
        echo "basic_runtime_lowmem_test OK"

        echo "Building extlib"
        if [[ "$BASICC" == *-ld ]]; then
                cc -shared -fPIC -Wall -Wextra -DBASIC_USE_LONG_DOUBLE -I"$ROOT/basic/include" -I"$ROOT/basic/src" -I"$ROOT/basic/src/vendor" \
                        "$ROOT/basic/test/extlib.c" -o "$ROOT/basic/libextlib.so"
        else
                cc -shared -fPIC -Wall -Wextra -I"$ROOT/basic/include" -I"$ROOT/basic/src" -I"$ROOT/basic/src/vendor" \
                        "$ROOT/basic/test/extlib.c" -o "$ROOT/basic/libextlib.so"
        fi
        echo "Running extern"
        LD_PRELOAD="$ROOT/basic/libextlib.so" "$BASICC" "$ROOT/basic/tests/programs/extern.bas" \
                > "$ROOT/basic/extern.out" 2> "$ROOT/basic/extern.err"
        if [ -s "$ROOT/basic/extern.err" ]; then
                echo "Unexpected stderr for extern"
                cat "$ROOT/basic/extern.err"
                exit 1
        fi
        rm -f "$ROOT/basic/extern.err"
        diff "$exp_dir/extern.out" "$ROOT/basic/extern.out"
        echo "extern OK"

        echo "Running prec53"
        run_test prec53
        echo "prec53 OK"

        for src in "$ROOT/basic/tests/programs/"*.bas; do
                name=$(basename "$src" .bas)
               case "$name" in
                       base0_cli|base1_cli|extern|resume|dim_expr_error|line_number_float|print_expr_error|input_multi|incdec|prec53)
                               continue
                               ;;
                       ahl_benchmark)
                               if [[ "$BASICC" == *-ld ]]; then
                                       continue
                               fi
                               ;;
               esac
                echo "Running $name"
                run_test "$name"
                echo "$name OK"
        done
        echo "Running base0_cli"
        "$BASICC" --option-base 0 "$ROOT/basic/tests/programs/base0_cli.bas" > "$ROOT/basic/base0_cli.out" 2> "$ROOT/basic/base0_cli.err"
        if [ -s "$ROOT/basic/base0_cli.err" ]; then
                echo "Unexpected stderr for base0_cli"
                cat "$ROOT/basic/base0_cli.err"
                exit 1
        fi
        rm -f "$ROOT/basic/base0_cli.err"
        diff "$exp_dir/base0_cli.out" "$ROOT/basic/base0_cli.out"
        echo "base0_cli OK"

        echo "Running base1_cli"
        "$BASICC" --option-base 1 "$ROOT/basic/tests/programs/base1_cli.bas" > "$ROOT/basic/base1_cli.out" 2> "$ROOT/basic/base1_cli.err"
        if [ -s "$ROOT/basic/base1_cli.err" ]; then
                echo "Unexpected stderr for base1_cli"
                cat "$ROOT/basic/base1_cli.err"
                exit 1
        fi
        rm -f "$ROOT/basic/base1_cli.err"
        diff "$exp_dir/base1_cli.out" "$ROOT/basic/base1_cli.out"
        echo "base1_cli OK"

        echo "Running hello (no line tracking)"
        "$BASICC" --no-line-tracking "$ROOT/basic/tests/programs/hello.bas" > "$ROOT/basic/hello-no-line.out"
        diff "$exp_dir/hello.out" "$ROOT/basic/hello-no-line.out"
        echo "hello (no line tracking) OK"

        echo "Running resume (expect error)"
        if "$BASICC" --no-line-tracking "$ROOT/basic/tests/programs/resume.bas" >/dev/null 2> "$ROOT/basic/resume.err"; then
                echo "resume should have failed"
                exit 1
        fi
        grep -q "line tracking" "$ROOT/basic/resume.err"
        echo "resume error OK"

echo "Running dim expression (expect error)"
if "$BASICC" "$ROOT/basic/tests/programs/dim_expr_error.bas" >/dev/null 2> "$ROOT/basic/dim_expr_error.err"; then
echo "dim expression should have failed"
exit 1
fi
grep -q "expected integer" "$ROOT/basic/dim_expr_error.err"
echo "dim expression error OK"

echo "Running line number float (expect error)"
if "$BASICC" "$ROOT/basic/tests/programs/line_number_float.bas" >/dev/null 2> "$ROOT/basic/line_number_float.err"; then
echo "line number float should have failed"
exit 1
fi
grep -q "expected integer" "$ROOT/basic/line_number_float.err"
echo "line number float error OK"

echo "Running print expression (expect error)"
"$BASICC" "$ROOT/basic/tests/programs/print_expr_error.bas" >/dev/null \
        2> "$ROOT/basic/print_expr_error.err" || true
if ! grep -q "parse error at line 10" "$ROOT/basic/print_expr_error.err"; then
        echo "print expression should have failed"
        exit 1
fi
echo "print expression error OK"


        echo "Running repl LOAD"
        printf 'LOAD %s\nRUN\nQUIT\n' "$ROOT/basic/tests/programs/hello.bas" | "$BASICC" > "$ROOT/basic/repl-load.out"
        diff "$exp_dir/repl-load.out" "$ROOT/basic/repl-load.out"
        echo "repl LOAD done"

        echo "Running repl LIST"
        printf 'LOAD %s\nLIST\nQUIT\n' "$ROOT/basic/tests/programs/funcproc.bas" | "$BASICC" > "$ROOT/basic/repl-list.out"
        diff "$exp_dir/repl-list.out" "$ROOT/basic/repl-list.out"
        echo "repl LIST done"

        echo "Running repl CODE"
        printf '10 PRINT "HI"\nCOMPILE CODE repl-code.bin\nQUIT\n' | "$BASICC" > "$ROOT/basic/repl-code.out"
        diff "$exp_dir/repl-code.out" "$ROOT/basic/repl-code.out"
        test -s repl-code.bin
        rm -f repl-code.bin
        echo "repl CODE done"

        echo "Running repl PROFILING"
        printf '10 DEF FNA(X)=X+1\n20 PRINT FNA(1)\n30 PRINT FNA(2)\n40 END\nRUN PROFILING\nQUIT\n' | "$BASICC" > "$ROOT/basic/repl-prof.out"
        grep -q 'line 20: count 1' "$ROOT/basic/repl-prof.out"
        grep -q 'func FNA: count 2' "$ROOT/basic/repl-prof.out"
        echo "repl PROFILING done"

        echo "Running repl PROFILING funcproc"
        printf 'LOAD %s\nRUN PROFILING\nQUIT\n' "$ROOT/basic/tests/programs/funcproc.bas" | "$BASICC" > "$ROOT/basic/repl-prof-funcproc.out"
        grep -q 'func ADD: count 1' "$ROOT/basic/repl-prof-funcproc.out"
        grep -q 'func HELLO: count 2' "$ROOT/basic/repl-prof-funcproc.out"
        echo "repl PROFILING funcproc done"

echo "Running bullfight sample"
set +e
printf 'NO\n' | timeout 1 "$BASICC" "$ROOT/basic/samples/bcg/bullfight.bas" \
  > "$ROOT/basic/bullfight.out" 2> "$ROOT/basic/bullfight.err"
rc=$?
set -e
if [ $rc -eq 139 ]; then
        echo "bullfight sample segfault"
        exit 1
fi
if [ -s "$ROOT/basic/bullfight.err" ]; then
        echo "Unexpected stderr for bullfight"
        cat "$ROOT/basic/bullfight.err"
        exit 1
fi
grep -q BULL "$ROOT/basic/bullfight.out" || {
        echo "bullfight output missing"; exit 1; }
rm -f "$ROOT/basic/bullfight.err" "$ROOT/basic/bullfight.out"
echo "bullfight sample OK"

echo "Running hexapawn sample"
set +e
timeout 1 "$BASICC" "$ROOT/basic/samples/bcg/hexapawn.bas" > /dev/null 2> "$ROOT/basic/hexapawn.err"
rc=$?
set -e
if [ $rc -eq 139 ]; then
echo "hexapawn sample segfault"
exit 1
fi
rm -f "$ROOT/basic/hexapawn.err"
echo "hexapawn sample OK"
}

for cc in "${BASICCS[@]}"; do
        echo "=== Testing with $(basename "$cc") ==="
        run_tests "$cc"
done


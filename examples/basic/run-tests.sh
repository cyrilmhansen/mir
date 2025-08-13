#!/usr/bin/env bash
# Run BASIC compiler example tests.
set -eu
ROOT="$(cd "$(dirname "$0")/../../" && pwd)"

# Determine BASIC compiler binaries to test. If none are supplied as arguments,
# test the double and long double variants built by the makefile.
if [ "$#" -gt 0 ]; then
        BASICCS=("$@")
else
        BASICCS=(
                "$ROOT/basic/basicc"
                "$ROOT/basic/basicc-ld"
        )
fi

run_tests() {
        BASICC="$1"

        # Build compiler if missing
        if [ ! -x "$BASICC" ]; then
                (cd "$ROOT" && make "basic/$(basename "$BASICC")")
        fi

        echo "Running REPL smoke test"
        printf '10 PRINT 1\nRUN\nQUIT\n' | "$BASICC" > /dev/null
        echo "REPL smoke test OK"

        run_test() {
                local name="$1"
                local in_file="$ROOT/examples/basic/$name.in"
                local src="$ROOT/examples/basic/$name.bas"
                local exp="$ROOT/examples/basic/$name.out"
                local out="$ROOT/basic/$name.out"
                local err="$ROOT/basic/$name.err"
                if [ "$name" = "vtab" ]; then
                        "$BASICC" "$src" > "$out" 2> "$err"
                else
                        if [ -f "$in_file" ]; then
                                "$BASICC" "$src" < "$in_file" > "$out" 2> "$err"
                        elif [ "$name" = "mandelbrot" ]; then
                                timeout 0.6 "$BASICC" "$src" 2> "$err" | head -n 200 > "$out" || true
                        else
                                "$BASICC" "$src" > "$out" 2> "$err"
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
                else
                        diff "$exp" "$out"
                fi
        }

        echo "Running hcolor_test"
        "$ROOT/basic/hcolor_test" > "$ROOT/basic/hcolor_test.out" 2> "$ROOT/basic/hcolor_test.err"
        if [ -s "$ROOT/basic/hcolor_test.err" ]; then
                echo "Unexpected stderr for hcolor_test"
                cat "$ROOT/basic/hcolor_test.err"
                exit 1
        fi
        rm -f "$ROOT/basic/hcolor_test.err"
        diff "$ROOT/examples/basic/hcolor_test.out" "$ROOT/basic/hcolor_test.out"
        echo "hcolor_test OK"

        echo "Building basic_pool_test"
        if [[ "$BASICC" == *-ld ]]; then
                cc -Wall -Wextra -DBASIC_USE_LONG_DOUBLE -I"$ROOT/examples/basic" \
                        "$ROOT/examples/basic/basic_pool.c" \
                        "$ROOT/examples/basic/basic_pool_test.c" \
                        -o "$ROOT/basic/basic_pool_test"
        else
                cc -Wall -Wextra -I"$ROOT/examples/basic" \
                        "$ROOT/examples/basic/basic_pool.c" \
                        "$ROOT/examples/basic/basic_pool_test.c" \
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
        diff "$ROOT/examples/basic/basic_pool_test.out" "$ROOT/basic/basic_pool_test.out"
        echo "basic_pool_test OK"

        echo "Running basic_pool_test"
        "$ROOT/basic/basic_pool_test" > "$ROOT/basic/basic_pool_test.out" 2> "$ROOT/basic/basic_pool_test.err"
        if [ -s "$ROOT/basic/basic_pool_test.err" ]; then
                echo "Unexpected stderr for basic_pool_test"
                cat "$ROOT/basic/basic_pool_test.err"
                exit 1
        fi
        rm -f "$ROOT/basic/basic_pool_test.err"
        diff "$ROOT/examples/basic/basic_pool_test.out" "$ROOT/basic/basic_pool_test.out"
        echo "basic_pool_test OK"

        echo "Building basic_runtime_lowmem_test"
        cc -Wall -Wextra -I"$ROOT/examples/basic" -I"$ROOT" -ffunction-sections \
                "$ROOT/examples/basic/basic_pool.c" \
                "$ROOT/examples/basic/basic_runtime.c" \
                "$ROOT/examples/basic/basic_runtime_lowmem_test.c" \
                -Wl,--gc-sections -lm -o "$ROOT/basic/basic_runtime_lowmem_test"
        echo "Running basic_runtime_lowmem_test"
        "$ROOT/basic/basic_runtime_lowmem_test" > "$ROOT/basic/basic_runtime_lowmem_test.out" 2> "$ROOT/basic/basic_runtime_lowmem_test.err"
        if [ -s "$ROOT/basic/basic_runtime_lowmem_test.err" ]; then
                echo "Unexpected stderr for basic_runtime_lowmem_test"
                cat "$ROOT/basic/basic_runtime_lowmem_test.err"
                exit 1
        fi
        rm -f "$ROOT/basic/basic_runtime_lowmem_test.err"
        diff "$ROOT/examples/basic/basic_runtime_lowmem_test.out" "$ROOT/basic/basic_runtime_lowmem_test.out"
        echo "basic_runtime_lowmem_test OK"

        echo "Building extlib"
        if [[ "$BASICC" == *-ld ]]; then
                cc -shared -fPIC -Wall -Wextra -DBASIC_USE_LONG_DOUBLE -I"$ROOT/examples/basic" \
                        "$ROOT/examples/basic/extlib.c" -o "$ROOT/basic/libextlib.so"
        else
                cc -shared -fPIC -Wall -Wextra -I"$ROOT/examples/basic" \
                        "$ROOT/examples/basic/extlib.c" -o "$ROOT/basic/libextlib.so"
        fi
        echo "Running extern"
        LD_PRELOAD="$ROOT/basic/libextlib.so" "$BASICC" "$ROOT/examples/basic/extern.bas" \
                > "$ROOT/basic/extern.out" 2> "$ROOT/basic/extern.err"
        if [ -s "$ROOT/basic/extern.err" ]; then
                echo "Unexpected stderr for extern"
                cat "$ROOT/basic/extern.err"
                exit 1
        fi
        rm -f "$ROOT/basic/extern.err"
        diff "$ROOT/examples/basic/extern.out" "$ROOT/basic/extern.out"
        echo "extern OK"


#for t in hello relop adder string strfuncs instr gosub on funcproc graphics normal vtab screen hplot_bounds readhplot restore data_read data_multi clear hgr2reset circle box sudoku array_oob_read array_oob_write dim_expr pi baseconv mir_demo datediff date random rnd_noarg hexoct def_fn; do
for t in hello relop delay adder string strfuncs instr gosub on funcproc vtab restore data_read data_multi clear circle box sudoku array_oob_read array_oob_write dim_expr pi baseconv mir_demo datediff date random rnd_noarg hexoct def_fn let chain mat log ifendif; do
                echo "Running $t"
                run_test "$t"
                echo "$t OK"
        done


echo "Running P019"
"$BASICC" "$ROOT/examples/basic/test/mbasic-nbs/P019.BAS" > "$ROOT/basic/P019.out" 2> "$ROOT/basic/P019.err"
if [ -s "$ROOT/basic/P019.err" ]; then
echo "Unexpected stderr for P019"
cat "$ROOT/basic/P019.err"
exit 1
fi
rm -f "$ROOT/basic/P019.err"
diff "$ROOT/examples/basic/test/mbasic-nbs/P019.out" "$ROOT/basic/P019.out"
echo "P019 OK"
        echo "Running base0_cli"
        "$BASICC" --option-base 0 "$ROOT/examples/basic/base0_cli.bas" > "$ROOT/basic/base0_cli.out" 2> "$ROOT/basic/base0_cli.err"
        if [ -s "$ROOT/basic/base0_cli.err" ]; then
                echo "Unexpected stderr for base0_cli"
                cat "$ROOT/basic/base0_cli.err"
                exit 1
        fi
        rm -f "$ROOT/basic/base0_cli.err"
        diff "$ROOT/examples/basic/base0_cli.out" "$ROOT/basic/base0_cli.out"
        echo "base0_cli OK"

        echo "Running base1_cli"
        "$BASICC" --option-base 1 "$ROOT/examples/basic/base1_cli.bas" > "$ROOT/basic/base1_cli.out" 2> "$ROOT/basic/base1_cli.err"
        if [ -s "$ROOT/basic/base1_cli.err" ]; then
                echo "Unexpected stderr for base1_cli"
                cat "$ROOT/basic/base1_cli.err"
                exit 1
        fi
        rm -f "$ROOT/basic/base1_cli.err"
        diff "$ROOT/examples/basic/base1_cli.out" "$ROOT/basic/base1_cli.out"
        echo "base1_cli OK"

        echo "Running hello (no line tracking)"
        "$BASICC" --no-line-tracking "$ROOT/examples/basic/hello.bas" > "$ROOT/basic/hello-no-line.out"
        diff "$ROOT/examples/basic/hello.out" "$ROOT/basic/hello-no-line.out"
        echo "hello (no line tracking) OK"

        echo "Running resume (expect error)"
        if "$BASICC" --no-line-tracking "$ROOT/examples/basic/resume.bas" >/dev/null 2> "$ROOT/basic/resume.err"; then
                echo "resume should have failed"
                exit 1
        fi
        grep -q "line tracking" "$ROOT/basic/resume.err"
        echo "resume error OK"

echo "Running dim expression (expect error)"
if "$BASICC" "$ROOT/examples/basic/dim_expr_error.bas" >/dev/null 2> "$ROOT/basic/dim_expr_error.err"; then
echo "dim expression should have failed"
exit 1
fi
grep -q "expected integer" "$ROOT/basic/dim_expr_error.err"
echo "dim expression error OK"

echo "Running line number float (expect error)"
if "$BASICC" "$ROOT/examples/basic/line_number_float.bas" >/dev/null 2> "$ROOT/basic/line_number_float.err"; then
echo "line number float should have failed"
exit 1
fi
grep -q "expected integer" "$ROOT/basic/line_number_float.err"
echo "line number float error OK"

echo "Running print expression (expect error)"
"$BASICC" "$ROOT/examples/basic/print_expr_error.bas" >/dev/null \
        2> "$ROOT/basic/print_expr_error.err" || true
if ! grep -q "parse error at line 10" "$ROOT/basic/print_expr_error.err"; then
        echo "print expression should have failed"
        exit 1
fi
echo "print expression error OK"

        echo "Running repl LOAD"
        printf 'LOAD %s\nRUN\nQUIT\n' "$ROOT/examples/basic/hello.bas" | "$BASICC" > "$ROOT/basic/repl-load.out"
        diff "$ROOT/examples/basic/repl-load.out" "$ROOT/basic/repl-load.out"
        echo "repl LOAD done"

        echo "Running repl LIST"
        printf 'LOAD %s\nLIST\nQUIT\n' "$ROOT/examples/basic/funcproc.bas" | "$BASICC" > "$ROOT/basic/repl-list.out"
        diff "$ROOT/examples/basic/repl-list.out" "$ROOT/basic/repl-list.out"
        echo "repl LIST done"

        echo "Running repl CODE"
        printf '10 PRINT "HI"\nCOMPILE CODE repl-code.bin\nQUIT\n' | "$BASICC" > "$ROOT/basic/repl-code.out"
        diff "$ROOT/examples/basic/repl-code.out" "$ROOT/basic/repl-code.out"
        test -s repl-code.bin
        rm -f repl-code.bin
        echo "repl CODE done"

        echo "Running repl PROFILING"
        printf '10 DEF FNA(X)=X+1\n20 PRINT FNA(1)\n30 PRINT FNA(2)\n40 END\nRUN PROFILING\nQUIT\n' | "$BASICC" > "$ROOT/basic/repl-prof.out"
        grep -q 'line 20: count 1' "$ROOT/basic/repl-prof.out"
        grep -q 'func FNA: count 2' "$ROOT/basic/repl-prof.out"
        echo "repl PROFILING done"

        echo "Running repl PROFILING funcproc"
        printf 'LOAD %s\nRUN PROFILING\nQUIT\n' "$ROOT/examples/basic/funcproc.bas" | "$BASICC" > "$ROOT/basic/repl-prof-funcproc.out"
        grep -q 'func ADD: count 1' "$ROOT/basic/repl-prof-funcproc.out"
        grep -q 'func HELLO: count 2' "$ROOT/basic/repl-prof-funcproc.out"
        echo "repl PROFILING funcproc done"
}

for cc in "${BASICCS[@]}"; do
        echo "=== Testing with $(basename "$cc") ==="
        run_tests "$cc"
done


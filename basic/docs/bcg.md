# BASIC Computer Games Archive

This repository now includes 106 classic programs from the public domain *BASIC Computer Games* collection (`bcg.zip`).

## Compilation experiment

Each program was compiled with the MIR BASIC compiler using:

```
./basic/basicc -S program.bas -o /tmp/tmp
```

The `-S` option emits MIR text and binary without executing the program, allowing a quick compile check without interactive input.

* 35 programs compiled successfully.
* 71 programs failed to compile or crashed the compiler (status 139).

### Programs that compiled successfully

```
23matches.bas
3dplot.bas
battle.bas
bombardment.bas
bowling.bas
boxing.bas
buzzword.bas
calendar.bas
change.bas
chemist.bas
chief.bas
craps.bas
cube.bas
dice.bas
digits.bas
evenwins.bas
flipflop.bas
hi-lo.bas
king.bas
lem.bas
letter.bas
life.bas
litquiz.bas
mathdice.bas
name.bas
nicomachus.bas
orbit.bas
poetry.bas
russianroulette.bas
splat.bas
synonym.bas
target.bas
test1.bas
train.bas
war.bas
```

Programs not listed above either triggered parser errors or caused a segmentation fault during compilation. Many of the failures likely require features not yet supported by the MIR BASIC compiler.

## Running tests

Repository tests continue to pass:

```
make basic-test
```

See `basic/samples/bcg` for the imported sources.

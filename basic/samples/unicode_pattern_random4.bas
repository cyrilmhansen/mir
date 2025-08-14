' === unicode_pattern_short_bg_palettes.bas ===
' 3 sections, 400 glyphs each, with always-on background gradients
' Works with mirbasic/basicc style: single-line IFs, simple GOSUBs

5  ESC$=CHR$(27) : PRINT ESC$;"[2J";ESC$;"[H";
6  DIM S$(6)        ' tile buffer (max needed)
7  DIM USED(6)      ' to avoid repeating a set
8  DIM P(16)        ' palette color slots (max 16)
9  WRAP=100         ' columns per line before newline

10 FOR SEC=1 TO 3
20   ' --- choose a distinct tile set (1..6) ---
30   SETID=1+INT(RND*6) : IF USED(SETID)=1 THEN 30
40   USED(SETID)=1

50   ' --- load tiles & choose a palette for this section ---
60   GOSUB 6000          ' -> S$(), SZ, SPECIAL
70   GOSUB 6500          ' -> P(), PL (palette & length)

80   ' --- print exactly 400 glyphs with gradient background ---
90   CNT=400 : COL=0 : PRINT ESC$;"[0m";
100  IF SPECIAL=1 THEN 2000 ELSE 3000
110  PRINT ESC$;"[0m"; : PRINT : PRINT
120 NEXT SEC
130 END

' ---------- Load tile set ----------
6000 SPECIAL=0 : SZ=0
6010 IF SETID=1 THEN SZ=2 : S$(1)=UNICHAR$(&H2571) : S$(2)=UNICHAR$(&H2572) : RETURN        ' ╱ ╲
6020 IF SETID=2 THEN SZ=4 : S$(1)=UNICHAR$(&H2596) : S$(2)=UNICHAR$(&H2597) : S$(3)=UNICHAR$(&H2598) : S$(4)=UNICHAR$(&H259D) : RETURN  ' ▖ ▗ ▘ ▝
6030 IF SETID=3 THEN SZ=6 : S$(1)=UNICHAR$(&H250C) : S$(2)=UNICHAR$(&H2510) : S$(3)=UNICHAR$(&H2514) : S$(4)=UNICHAR$(&H2518) : S$(5)=UNICHAR$(&H2502) : S$(6)=UNICHAR$(&H2500) : RETURN ' corners/lines
6040 IF SETID=4 THEN SZ=4 : S$(1)=UNICHAR$(&H2591) : S$(2)=UNICHAR$(&H2592) : S$(3)=UNICHAR$(&H2593) : S$(4)=UNICHAR$(&H2588) : RETURN  ' ░ ▒ ▓ █
6050 IF SETID=5 THEN SPECIAL=1 : RETURN                                                        ' Braille random
6060 IF SETID=6 THEN SZ=4 : S$(1)=UNICHAR$(&H1F7E5) : S$(2)=UNICHAR$(&H1F7E6) : S$(3)=UNICHAR$(&H1F7E9) : S$(4)=UNICHAR$(&H1F7E8) : RETURN ' 🟥 🟦 🟩 🟨
6070 RETURN

' ---------- Pick a background palette (hand-picked gradients) ----------
' Palettes are 256-color background codes (xterm): we fill P(1..PL)
6500 PAL=1+INT(RND*6)
6510 IF PAL=1 THEN PL=7 : P(1)=196 : P(2)=202 : P(3)=208 : P(4)=214 : P(5)=220 : P(6)=226 : P(7)=228 : RETURN      ' Sunset
6520 IF PAL=2 THEN PL=6 : P(1)=17  : P(2)=18  : P(3)=19  : P(4)=20  : P(5)=21  : P(6)=45  : RETURN                 ' Ocean
6530 IF PAL=3 THEN PL=6 : P(1)=22  : P(2)=28  : P(3)=34  : P(4)=40  : P(5)=46  : P(6)=82  : RETURN                 ' Forest
6540 IF PAL=4 THEN PL=6 : P(1)=232 : P(2)=235 : P(3)=238 : P(4)=241 : P(5)=244 : P(6)=247 : RETURN                 ' Grayscale
6550 IF PAL=5 THEN PL=6 : P(1)=201 : P(2)=207 : P(3)=213 : P(4)=219 : P(5)=225 : P(6)=231 : RETURN                 ' Candy
6560 IF PAL=6 THEN PL=7 : P(1)=88  : P(2)=124 : P(3)=160 : P(4)=196 : P(5)=202 : P(6)=208 : P(7)=214 : RETURN      ' Fire
6570 RETURN

' ---------- Apply background color for current column ----------
' Maps COL (0..WRAP-1) to a palette index 1..PL
6900 IDX = 1 + INT((COL * PL) / WRAP) : IF IDX<1 THEN IDX=1 : IF IDX>PL THEN IDX=PL
6910 PRINT ESC$;"[48;5;";P(IDX);"m";
6920 RETURN

' ---------- Braille section (SPECIAL=1) ----------
2000 IF CNT<=0 THEN RETURN
2010 GOSUB 6900
2020 GLY$=UNICHAR$(&H2800+INT(RND*256))
2030 PRINT GLY$;
2040 COL=COL+1 : IF COL>=WRAP THEN PRINT : COL=0
2050 CNT=CNT-1 : IF CNT>0 THEN 2010 ELSE RETURN

' ---------- Standard set section ----------
3000 IF CNT<=0 THEN RETURN
3010 GOSUB 6900
3020 IDX=1+INT(RND*SZ) : GLY$=S$(IDX)
3030 PRINT GLY$;
3040 COL=COL+1 : IF COL>=WRAP THEN PRINT : COL=0
3050 CNT=CNT-1 : IF CNT>0 THEN 3000 ELSE RETURN


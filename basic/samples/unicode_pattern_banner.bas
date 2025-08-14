10 N = 0
20 IF RND < 0.5 THEN C = &H1FB7A ELSE C = &H1FB71
30 PRINT UNICHAR$(C);
40 N = N + 1
50 IF N < 800 THEN GOTO 20
110 N = 0
120 IF RND < 0.5 THEN C = &H2571 ELSE C = &H2572
130 PRINT UNICHAR$(C);
140 N = N + 1
150 IF N < 800 THEN GOTO 120
200 REM Random colored squares (🟥 🟦 🟩 🟨)
210 N = 0
211 RED$  = UNICHAR$(&H1F7E5)  ' 🟥  Red large square
212 BLUE$ = UNICHAR$(&H1F7E6)  ' 🟦  Blue large square
213 GREEN$= UNICHAR$(&H1F7E9)  ' 🟩  Green large square
214 YELL$ = UNICHAR$(&H1F7E8)  ' 🟨  Yellow large square
215 DIM T$(4)
220 T$(1)=RED$ : T$(2)=BLUE$ : T$(3)=GREEN$ : T$(4)=YELL$
230 R = INT(RND * 4) + 1
240 PRINT T$(R);
250 N = N + 1
260 IF N < 200 THEN GOTO 230
600 END

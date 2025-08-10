10 REM Mandelbrot fractal with color and half-block graphics
20 REM Uses ANSI escape codes for 256 colors; best in xterm-256color or xterm-kitty
30 REM Each text cell renders two points via foreground/background colors
40 W=80:H=48:MAXI=50:ESC$=CHR$(27)
50 FOR PY=0 TO H-1 STEP 2
60  Y1=PY/H*3.0-1.5
70  Y2=(PY+1)/H*3.0-1.5
80  FOR PX=0 TO W-1
90   X=PX/W*3.5-2.5
100   A=0:B=0:I=0
110   WHILE A*A+B*B<=4 AND I<MAXI
120    T=A*A-B*B+X
130    B=2*A*B+Y1
140    A=T:I=I+1
150   WEND
160   C1=16+INT(I*239/MAXI)
170   A=0:B=0:I=0
180   WHILE A*A+B*B<=4 AND I<MAXI
190    T=A*A-B*B+X
200    B=2*A*B+Y2
210    A=T:I=I+1
220   WEND
230   C2=16+INT(I*239/MAXI)
240   PRINT ESC$;"[38;5;";STR$(C1);";48;5;";STR$(C2);"m▀";
250  NEXT PX
260  PRINT ESC$;"[0m"
270 NEXT PY
280 END

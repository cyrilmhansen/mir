10 PRINT "Base converter: convert numbers between bases 2-36."
20 PRINT "Enter source base, target base, and number."
30 DIGITS$="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
40 SB=10: B=16
50 WHILE 1
60 PRINT "Source base (2-36) [";SB;"]: ";: INPUT SB$
70 IF SB$<>"" THEN SB=VAL(SB$)
80 PRINT "Target base (2-36) [";B;"]: ";: INPUT B$
90 IF B$<>"" THEN B=VAL(B$)
100 PRINT "Number to convert: ";: INPUT N$
110 N=0
120 FOR I=1 TO LEN(N$)
130  V=ASC(MID$(N$,I,1))
140  IF V>=97 AND V<=122 THEN V=V-32
150  IF V>=48 AND V<=57 THEN V=V-48
160  IF V>=65 AND V<=90 THEN V=V-55
170  N=N*SB+V
180 NEXT I
190 IF N=0 THEN PRINT "0": GOTO 230
200 R$=""
210 WHILE N>0
220  R=N MOD B
230  N=N\B
240  R$=MID$(DIGITS$,R+1,1)+R$
250 WEND
260 PRINT R$
270 PRINT "Another conversion (Y/N)? ";: INPUT Q$
280 IF Q$<>"Y" AND Q$<>"y" THEN END
290 WEND
300 END

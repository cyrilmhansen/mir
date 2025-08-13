10 PRINT "Base converter: convert numbers between bases 2-36."
20 PRINT "Enter source base, target base, and number."
30 DIM D$(35),A$(63)
40 FOR I=0 TO 35: READ D$(I): NEXT I
50 SB=10: B=16
60 WHILE 1
70 PRINT "Source base (2-36) [";SB;"]: ";: INPUT SB$
80 IF SB$<>"" THEN SB=VAL(SB$)
90 PRINT "Target base (2-36) [";B;"]: ";: INPUT B$
100 IF B$<>"" THEN B=VAL(B$)
110 PRINT "Number to convert: ";: INPUT N$
120 N=0
130 FOR I=1 TO LEN(N$)
140  V=ASC(MID$(N$,I,1))
150  IF V>=97 AND V<=122 THEN V=V-32
160  IF V>=48 AND V<=57 THEN V=V-48
170  IF V>=65 AND V<=90 THEN V=V-55
180  N=N*SB+V
190 NEXT I
200 IF N=0 THEN PRINT "0": GOTO 270
210 I=0
220 WHILE N>0
230  R=N MOD B
240  N=N\B
250  A$(I)=D$(R)
260  I=I+1
270 WEND
280 FOR J=I-1 TO 0 STEP -1: PRINT A$(J);: NEXT J
290 PRINT
300 PRINT "Another conversion (Y/N)? ";: INPUT Q$
310 IF Q$<>"Y" AND Q$<>"y" THEN END
320 WEND
330 END
340 DATA "0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"

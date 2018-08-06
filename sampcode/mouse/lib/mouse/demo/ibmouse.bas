1000  ' REFERENCE test of mouse/interpretive BASIC interface	IN ibmouse.bas
1010  '
1020  ' This program is intended to test the parameter passing from interpretive
1030  ' BASIC to the mouse driver.
1040  '
1050  ' Note:  This program is written for interpretive BASIC.  Do not attempt
1060  ' attempt to run it using compiled BASIC.
1070  '
1080 DEF SEG = 0
1090 MSEG = 256*PEEK(51*4+3)+PEEK(51*4+2)
1100 MOUSE = 256*PEEK(51*4+1)+PEEK(51*4)+2
1110 IF MSEG OR (MOUSE-2) THEN 1130
1120 PRINT "Mouse Driver not found" : END
1130 DEF SEG=MSEG
1140 IF PEEK (MOUSE-2) = 207, THEN 1120 	        '207 is iret
1150 '
1160 CLS
1170 '
1180 M1% = 0                                            ' Reset the mouse
1190 CALL MOUSE(M1%, M2%, M3%, M4%)
1200 IF NOT (M1%) THEN PRINT "Mouse not installed." : END
1210 '
1220 M1% = 1                                            ' Show the mouse cursor
1230 CALL MOUSE(M1%, M2%, M3%, M4%)
1240 PRINT "Mouse cursor should now be visible..."
1250 '
1260 M1% = 4
1270 FOR M4% = 0 TO 200 STEP 8
1280     FOR M3% = 0 TO 640 STEP 2
1290         CALL MOUSE(M1%, M2%, M3%, M4%)
1300     NEXT
1310 NEXT
1320 '
1330 M1%=4                                              ' Move the mouse
1340 M3%=320                                            '  cursor to the 
1350 M4%=100                                            '  center of the
1360 CALL MOUSE (M1%, M2%, M3%, M4%)                    '  screen
1370 '
1380 M1% = 0                                            ' Reset the mouse
1390 CALL MOUSE(M1%, M2%, M3%, M4%)
1400 END

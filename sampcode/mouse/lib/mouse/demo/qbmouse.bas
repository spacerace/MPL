	' REFERENCE test of mouse() in mouse.lib        IN qbmouse.bas
	'
	' This program is intended to test the parameter passing from
	' QucikBASIC through the mouse library function mouse(), to
	' the mouse driver.
	'
	' To create qbmouse.exe:
	'
	'               QB QBMOUSE;
	'               LINK QBMOUSE+SUBS,,,BCOM20+MOUSE.LIB
	'
	' Note:  This program is written in QuickBASIC (compiled).  Do not
	' attempt to run it under interepted BASIC, since the mouse() function
	' is not available.
	'

	DIM BOUNDS%(4)

	CALL CHKDRV				' Make sure mouse driver exists
	SCREEN 2				' 640x200 display (CGA mode 6)
	CLS					' Clear the screen

	M1% = 0 				' Reset the mouse
	CALL MOUSE(M1%, M2%, M3%, M4%)

	M1% = 1 				' Show the mouse cursor
	CALL MOUSE(M1%, M2%, M3%, M4%)

	PRINT "Mouse cursor should now be sweeping the screen."
	M1% = 4
	FOR M3% = 0 TO 640 STEP 8
	    FOR M4% = 0 TO 200 STEP 4
		CALL MOUSE(M1%, M2%, M3%, M4%)	' Set mouse cursor position
	    NEXT
	NEXT

	M1% = 2 				' Hide the mouse cursor
	CALL MOUSE(M1%, M2%, M3%, M4%)
	CLS

	PRINT "ษออออออออออออออออออออออออออออออออออออออออออออออออป"
	PRINT "บ  Function 16 test.  Cursor should disappear    บ"
	PRINT "บ  when moved over double line.                  บ"
	PRINT "บ                                                บ"
	PRINT "บ  Type any key to make cursor reappear.         บ"
	PRINT "บ                                                บ"
	PRINT "บ  Type ESCape to exit.                          บ"
	PRINT "ศออออออออออออออออออออออออออออออออออออออออออออออออผ"

100	M1%= 4					' Position mouse cursor
	M3% = 320				'  in center of screen
	M4% = 100
	CALL MOUSE(M1%, M2%, M3%, M4%)

	M1% = 1 				' Show mouse cursor
	CALL MOUSE(M1%, M2%, M3%, M4%)
'
	M1% = 16
	BOUNDS%(0) = 0				' Exclude mouse cursor from
	BOUNDS%(1) = 0				'  the rectangle with corners
	BOUNDS%(2) = 392			'  at (0,0), (0,56), (392,0),
	BOUNDS%(3) = 56 			'  and (392,56)
	M4% = VARPTR(BOUNDS%(0))		' M4% = array address
	CALL MOUSE(M1%, M2%, M3%, M4%)

200	A$ = INKEY$
	IF A$ = "" THEN 200
	IF A$ <> "" THEN 100

	M1% = 2 				' Make sure the mouse cursor
	CALL MOUSE(M1%, M2%, M3%, M4%)		'  is hidden
	CLS

	PRINT "Move the mouse cursor to the UPPER LEFT corner of the screen."
	PRINT "RELEASE both mouse buttons, and then press ESCape."
	M1% = 1 				' Make sure the mouse cursor
	CALL MOUSE(M1%, M2%, M3%, M4%)		'  is displayed
	CALL MOUSE(M1%, M2%, M3%, M4%)
300	A$ = INKEY$
	IF A$ <> "" THEN 300
	M1% = 3 				' Get mouse position and
	CALL MOUSE(M1%, M2%, M3%, M4%)		'  button status
	M1% = 2 				' Make sure the mouse cursor
	CALL MOUSE(M1%, M2%, M3%, M4%)		'  is hidden
	IF (M2% <> 0) THEN PRINT "Mouse button information error"
	IF (M3% <> 0) THEN PRINT "Mouse x coordinate position error ";M3%
	IF (M4% <> 0) THEN PRINT "Mouse y coordinate position error ";M4%
	M1% = 1 				' Make sure the mouse cursor
	CALL MOUSE(M1%, M2%, M3%, M4%)		'  is displayed

	M1% = 2 				' Make sure the mouse cursor
	CALL MOUSE(M1%, M2%, M3%, M4%)		'  is hidden
	PRINT "Move the mouse cursor to the LOWER RIGHT corner of the screen."
	PRINT "HOLD DOWN both mouse buttons, and then press ESCape."
	M1% = 1 				' Make sure the mouse cursor
	CALL MOUSE(M1%, M2%, M3%, M4%)		'  is displayed
400	A$ = INKEY$
	IF A$ <> "" THEN 400
	M1% = 3 				' Get mouse position and
	CALL MOUSE(M1%, M2%, M3%, M4%)		'  button status
	M1% = 2 				' Make sure the mouse cursor
	CALL MOUSE(M1%, M2%, M3%, M4%)		'  is hidden
	IF (M2% <> 3) THEN PRINT "Mouse button information error"
	IF (M3% <> 639) THEN PRINT "Mouse x coordinate position error ";M3%
	IF (M4% <> 199) THEN PRINT "Mouse y coordinate position error ";M4%
	M1% = 1 				' Make sure the mouse cursor
	CALL MOUSE(M1%, M2%, M3%, M4%)		'  is displayed

	PRINT
	PRINT "Press any key to terminate."
500	A$ = INKEY$
	IF A$ ="" THEN 500
	M1% = 0 				' Reset the mouse
	CALL MOUSE(M1%, M2%, M3%, M4%)

	END

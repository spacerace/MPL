C----------------------------------------------------------------------
C
C This program tests the 4 parameter mouse driver interface function
C
C	       To Run :
C			FOR1 FORMOUSE;
C			PAS1;
C			LINK FORMOUSE+SUBS,,,MOUSE
C			      OR
C			MAKE FORMOUSE.MAK
C
C----------------------------------------------------------------------
	INTEGER*2     M1, M2, M3, M4
	INTEGER*2     PTRADD(2), BOUND(4)
	INTEGER*4     PTR
	EXTERNAL      CHKDRV, GRAF
	EQUIVALENCE   (PTRADD(1), PTR)

	CALL CHKDRV

	M1 = 0
	CALL MOUSES(M1, M2, M3, M4)
	IF (M1 .NE. 0) GOTO 100
	WRITE (*, 800) ' Mouse Driver not installed'
	GOTO 99999

 100	CALL GRAF

	M1 = 1
	CALL MOUSES(M1, M2, M3, M4)

	WRITE (*, 800) ' mouse cursor should be sweeping the screen'

	M1 = 4
	DO 210 M3 = 0, 640, 16
	DO 200 M4 = 0, 200, 2
	CALL MOUSES(M1, M2, M3, M4)
 200	CONTINUE
 210	CONTINUE

	CALL GRAF

	WRITE(*,800)'++-----------------------------------------------+'
	WRITE(*,800)' | Function 16 test.  Cursor should disappear    |'
	WRITE(*,800)' | when moved over DOTTED line.                  |'
	WRITE(*,800)' | Cursor should appear in center of screen      |'
	WRITE(*,800)' |                                               |'
	WRITE(*,800)' | Type ENTER to make cursor reappear.           |'
	WRITE(*,800)' | Type C (Upper Case) and ENTER to continue.    |'
	WRITE(*,800)' +-----------------------------------------------+'

 300	M1 = 4
	M3 = 320
	M4 = 100
	CALL MOUSES(M1, M2, M3, M4)

	M1 = 1
	CALL MOUSES(M1, M2, M3, M4)

	M1 = 16
	BOUND(1) = 0
	BOUND(2) = 0
	BOUND(3) = 384
	BOUND(4) = 56
	PTR = LOCFAR(BOUND)
	CALL MOUSES(M1, M2, M3, PTRADD(1))

 400	READ (*,800) CH
	IF (CH .NE. 'C') GOTO 300

C	Test buttons and mouse position

	CALL GRAF

	WRITE (*,800) ' Move mouse to UPPER LEFT corner.'
	WRITE (*,800) ' PRESS both buttons, press C and ENTER'
	M1 = 1
	CALL MOUSES(M1, M2, M3, M4)
 500	READ (*,800) CH
	IF (CH .NE. 'C') GOTO 500
	M1 = 3
	M2 = 9999
	M3 = 9999
	M4 = 9999
	CALL MOUSES(M1, M2, M3, M4)
	M1 = 2
	CALL MOUSES(M1, M2, M3, M4)
	IF (M2 .NE. 3) WRITE(*, 700) ' BUTTON INFO ERROR : ',M2
	IF (M3 .NE. 0) WRITE(*, 700) ' X COORD ERROR : ',M3
	IF (M4 .NE. 0) WRITE(*, 700) ' Y COORD ERROR : ',M4
	M1 = 1
	CALL MOUSES(M1, M2, M3, M4)


	M1 = 2
	CALL MOUSE(M1, M2, M3, M4)
	WRITE (*,800) ' Move mouse to LOWER RIGHT corner.'
	WRITE (*,800) ' RELEASE both buttons, then press C then ENTER'
	M1 = 1
	CALL MOUSES(M1, M2, M3, M4)
 600	READ (*,800) CH
	IF (CH .NE. 'C') GOTO 600
	M1 = 3
	M3 = 9999
	M4 = 9999
	CALL MOUSES(M1, M2, M3, M4)
	M1 = 2
	CALL MOUSES(M1, M2, M3, M4)
	IF (M2 .NE. 0) WRITE(*, 700) ' BUTTON INFO ERROR : ',M2
	IF (M3 .NE. 639) WRITE(*, 700) ' X COORD ERROR : ',M3
	IF (M4 .NE. 199) WRITE(*, 700) ' Y COORD ERROR : ',M4
	M1 = 1
	CALL MOUSES(M1, M2, M3, M4)


 700	FORMAT (A, I4)

 800	FORMAT(A)

	STOP
99999	END

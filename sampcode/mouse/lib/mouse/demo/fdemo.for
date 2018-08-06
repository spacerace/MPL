       PROGRAM MTEST

C
C      -- Mouse Library function calls demo in MS FORTRAN V3.31
C

C      -- mouse parameters --
       INTEGER*2   M1, M2, M3, M4, M5
       INTEGER*2   MCURSOR(32), BOUND(4)
       INTEGER*2   ARRLOC(2)
       INTEGER*4   ARRADDS
       EXTERNAL    CHKDRV, GRAF
       EQUIVALENCE (ARRLOC(1),ARRADDS)

C      -- initialize data for function call 9 --
C	  Mouse call #9 needs 2 byte integer input, in V3.31
C	  use INT2 function.

       DO 50 I = 1, 16
  50   MCURSOR (I) = INT2(#ffff)

       MCURSOR(17) = INT2(#0780)
       MCURSOR(18) = INT2(#b8b8)
       MCURSOR(19) = INT2(#3060)
       MCURSOR(20) = INT2(#6038)
       MCURSOR(21) = INT2(#f07e)
       MCURSOR(22) = INT2(#8841)
       MCURSOR(23) = INT2(#f031)
       MCURSOR(24) = INT2(#7020)
       MCURSOR(25) = INT2(#401c)
       MCURSOR(26) = INT2(#6006)
       MCURSOR(27) = INT2(#3c06)
       MCURSOR(28) = INT2(#03fe)
       MCURSOR(29) = INT2(#0001)
       MCURSOR(30) = INT2(#f001)
       MCURSOR(31) = INT2(#0301)
       MCURSOR(32) = INT2(#007c)

C      -- Chech for mouse and driver installation  --
       CALL CHKDRV

       M1 = 0
       CALL MOUSES(M1, M2, M3, M4)

       IF ( M1 .EQ. 0 ) THEN
	  WRITE(*,*)' Microsoft mouse NOT found'
	  STOP
       ENDIF


       WRITE(*,*) ' Enter "c" to continue'
  100  READ (*,200) CH
  200  FORMAT(A)
       IF ( CH .NE. 'c' ) GOTO 100

C      -- Change to graphics mode --
       CALL GRAF

C      -- Function 9 Graphics Cursor --
       M1 = 9
       M2 = 1
       M3 = 1
       ARRADDS = LOCFAR(MCURSOR)
       CALL MOUSES(M1, M2, M3, ARRLOC(1))

       M1 = 1
       CALL MOUSES(M1, M2, M3, M4)

C      -- Function 16 Conditional Off --
       M1 = 16
       BOUND(1) = 0
       BOUND(2) = 0
       BOUND(3) = 50
       BOUND(4) = 300
       ARRADDS = LOCFAR(BOUND)
       CALL MOUSES(M1, M2, M3, ARRLOC(1))


       WRITE(*,*) ' ENTER "q" TO CONTINUE'
  300  READ (*,200) CH
       IF ( CH .NE. 'q' ) GOTO 300

       STOP
       END


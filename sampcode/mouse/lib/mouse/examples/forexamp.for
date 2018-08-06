C	    Mouse Function Calls and Microsoft FORTRAN
C
C	    To Run :	FOR1 FOREXAMP;
C			PAS2
C			LINK FOREXAMP+SUBS,,,MOUSE;
C			     or
C			MAKE FOREXAMP.MAK
C

       PROGRAM MTEST

C
C      -- Mouse Library calls test in MS FORTRAN V3.31 --
C

       INTEGER*2   M1, M2, M3, M4
       EXTERNAL    GRAF, CHKDRV


C      --  Call driver checking routine  --
       CALL CHKDRV()

C      -- Mouse init call --
       M1 = 0
       CALL MOUSES(M1, M2, M3, M4)
       IF ( M1 .EQ. 0 ) THEN
	     WRITE(*,*)' Microsoft Mouse NOT found'
	     STOP
       ENDIF

C      -- Place Cursor in the center of the screen --
       M1 = 4
       M3 = 200
       M4 = 100
       CALL MOUSES(M1, M2, M3, M4)

C      -- Set minimum and maximum horizontal position --
       M1 = 7
       M3 = 150
       M4 = 450
       CALL MOUSES(M1, M2, M3, M4)

C      -- Set minimum and maximum vertical position --
       M1 = 8
       M3 = 50
       M4 = 150
       CALL MOUSES(M1, M2, M3, M4)

       CALL GRAF()

       WRITE(*,*) ' Graphics cursor limited to the screen center.'
       WRITE(*,*) ' Press the left mouse button to EXIT.'

       M1 = 1
       CALL MOUSES(M1, M2, M3, M4)


C      -- Loop for left moue button pressed  --
       M2 = 9999
 100	  M1 = 3
	  CALL MOUSES(M1, M2, M3, M4)
       IF ( M2 .NE. 1 ) GOTO 100

       STOP
       END

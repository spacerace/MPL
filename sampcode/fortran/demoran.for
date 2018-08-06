	PROGRAM DEMORAN
c
c  This program demonstrates a uniform pseudo-random number
c  generator. 
c
c  Portions of this program are based on ideas presented in the
c  the book "Numerical Recipes - The Art of Scientific Computing"
c  by William H. Press, Brian P. Flannery, Saul A. Teukolsky, and 
c  William T. Vetterling, Cambridge University Press 1986
c
c  If 1000 numbers and 10 bins are requested, each bin should
c  (ideally) be filled with 100 numbers. The percentage error
c  is printed for each bin.
c
c  The following routines are provided:
c
c  REAL FUNCTION RAND () 
c    returns a real number in the range 0. to 1.0
c 
c  INTEGER FUNCTION RANDLIM (ILO,IHI)
c    returns a random integer in the range ILO to IHI
c
c  REAL FUNCTION SRAND (SEED)
c    initializes either generator (seed = 0. to 259199.)
c
c  SUBROUTINE SECOND (TX)
c    returns the number of seconds and hundreths of seconds elapsed
c    since midnight
c
c
c  NOTE -- Both generators should produce identical results
c
	INTEGER BINS(0:999), RANDLIM
	ERR(I) = (I-FLOAT(NREP/NBINS))/(NREP/NBINS)*100.
	WRITE (*,*) 'You will be asked to provide the following:'
	WRITE (*,*) 'how many random numbers to generate'
	WRITE (*,*) 'how many bins to use (1-1000)'
	WRITE (*,*) 'which generator to use (1 or 2)'
	WRITE (*,*)
10	CONTINUE
	WRITE (*,*) 'Input three numbers separated by blanks or commas'
	WRITE (*,*) 'or CTRL-Z to end'
	READ (*,*,END=999) NREP,NBINS,IGEN
	SEED = SRAND(1.0)
	CALL SECOND (T1)
	DO 100 I=1,NREP
	  IF (IGEN .EQ. 1) THEN
            IX = RANDLIM(0,NBINS-1)
	  ELSE
	    IX = NBINS*RAND()
	  ENDIF
	  BINS(IX) = BINS(IX)+1
100	CONTINUE
	CALL SECOND (T2)
	WRITE (*,*) 'Time elapsed=',t2-t1
	WRITE (*,*) 'Numbers generated per second=',nrep/(t2-t1)
	WRITE (*,*)
	WRITE (*,*) 'Bin    Count  % Error'
	WRITE (*,*) '----  ------- -------'
	DO 200 I=0,NBINS-1
	  WRITE (*,'(1x,i4,i9,f7.1,''%'')') i+1,bins(i),err(bins(i))
          BINS(I) = 0
200	CONTINUE
	GO TO 10
999	CONTINUE
	END
	FUNCTION RANDOM ()
c
c  If called, RANDOM just returns 0.0
c
	INTEGER RANDLIM
	PARAMETER (IA=7141, IC=54773, IM=259200)
	RANDOM = 0.0
	RETURN
c
c  REAL FUNCTION SRAND (SEED)
c    initializes either generator (seed = 0. to 259199.)
c
	ENTRY SRAND (X)
	SRAND = X
    	SEED = X
	RETURN
c 
c  INTEGER FUNCTION RANDLIM (ILO,IHI)
c    returns a random integer in the range ILO to IHI
c
	ENTRY RANDLIM (ILO,IHI)
	SEED = MOD (INT(SEED)*IA+IC,IM)
	RANDLIM = ILO+(IHI-ILO+1)*SEED/IM
	RETURN
c
c  REAL FUNCTION RAND () 
c    returns a real number in the range 0. to 1.0
c 
	ENTRY RAND ()
	SEED = MOD (INT(SEED)*IA+IC,IM)
	RAND = SEED/IM
	END
	SUBROUTINE SECOND (TX)
c
c  SUBROUTINE SECOND (TX)
c    returns the number of seconds and hundredths of seconds elapsed
c    since midnight
c
	INTEGER*2 IH,IM,IS,IHU
	CALL GETTIM (IH,IM,IS,IHU)
	TX = IH*3600.+IM*60+IS+IHU/100.
	END

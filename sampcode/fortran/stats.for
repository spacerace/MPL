C**********************************************************************
C
C  STATS.FOR
C
C        Calculates simple statistics (minimum, maximum, mean, median,
C        variance, and standard deviation) of up to 50 values.
C
C        Reads one value at a time from unit 5.  Echoes values and
C        writes results to unit 6.
C
C        All calculations are done in single precision.
C
C
C***********************************************************************



      DIMENSION DAT(50)
      OPEN(5,FILE=' ')

      N=0
      DO 10 I=1,50
      READ(5,99999,END=20) DAT(I)
      N=I
 10   CONTINUE

C Too many values.  Write error message and die.

      WRITE(6,99998) N
      STOP
  
C Test to see if there's more than one value.  We don't want to divide 
C by zero.

20    IF(N.LE.1) THEN 

C Too few values. Print message and die.

         WRITE(6,99997) 

      ELSE

C Echo input values to output.

         WRITE(6,99996)
         WRITE(6,99995) (DAT(I),I=1,N)

C Calculate mean, standard deviation, and median.

         CALL MEAN (DAT,N,DMEAN)
         CALL STDEV (DAT,N,DMEAN,DSTDEV,DSTVAR)
         CALL MEDIAN (DAT,N,DMEDN,DMIN,DMAX)

         WRITE(6,99994) N,DMEAN,DMIN,DMAX,DMEDN,DSTVAR,DSTDEV

      ENDIF

      STOP

99999 FORMAT(E14.6)
99998 FORMAT('0 ********STAT: TOO MANY VALUES-- ',I5)
99997 FORMAT('0 ********STAT: TOO FEW VALUES (1 OR LESS) ')
99996 FORMAT(//,10X,
     +' ******************SAMPLE DATA VALUES*****************'//)
99995 FORMAT(5(1X,1PE14.6))
99994 FORMAT(///,10X,
     +' ******************SAMPLE STATISTICS******************',//,
     +15X,'          Sample size = ',I5,/,
     +15X,'          Mean        = ',1PE14.6,/,
     +15X,'          Minimum     = ',E14.6,/,
     +15X,'          Maximum     = ',E14.6,/
     +15X,'          Median      = ',E14.6,/
     +15X,'          Variance    = ',E14.6,/
     +15X,'          St deviation= ',E14.6////)

      END

C Calculate the mean (XMEAN) of the N values in array X.

      SUBROUTINE  MEAN (X,N,XMEAN)
      DIMENSION X(N)

      SUM=0.0
      DO 10 I=1,N
         SUM=SUM+X(I)
   10 CONTINUE

      XMEAN=SUM/FLOAT(N)

      RETURN
      END

C Calculate the standard deviation (XSTDEV) and variance (XVAR)
C of the N values in X using the mean (XMEAN).
C This divides by zero when N = 1.

      SUBROUTINE STDEV (X,N,XMEAN,XSTDEV,XVAR)
      DIMENSION X(N)

      SUMSQ=0.0
      DO 10 I=1,N
         XDIFF=X(I)-XMEAN
         SUMSQ=SUMSQ+XDIFF*XDIFF
   10 CONTINUE

      XVAR=SUMSQ/FLOAT(N-1)
      XSTDEV=SQRT(XVAR)

      RETURN
      END


C Calculate the median (XMEDN), minimum (XMIN), and maximum (XMAX) of
C the N values in X.
C MEDIAN sorts the array and then calculates the median value.

      SUBROUTINE MEDIAN (X,N,XMEDN,XMIN,XMAX)
      DIMENSION X(N)

      CALL SORT (X,N)

      IF(MOD(N,2).EQ.0) THEN
         K=N/2
         XMEDN=(X(K)+X(K+1))/2.0
      ELSE
         K=(N+1)/2
         XMEDN=X(K)
      ENDIF

      XMIN=X(1)
      XMAX=X(N)

      END

C Sort the N values in array X.  SORT uses a bubble sort
C that quits when no values were exchanged on the last pass.
C Each pass goes from the first element to where the last
C exchange occurred on the previous pass.

      SUBROUTINE SORT (X,N)
      DIMENSION X(N)

      IBND=N
  20  IXCH=0
     
      DO 100 J=1,IBND-1 
           IF(X(J).GT.X(J+1))THEN
              TEMP=X(J)
              X(J)=X(J+1)
              X(J+1)=TEMP
              IXCH=J
           ENDIF
 100  CONTINUE

      IF (IXCH.EQ.0) RETURN
      IBND=IXCH
      GO TO 20
   
      END


CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
C       "WHETSTONE INSTRUCTIONS PER SECONDS" MEASURE OF FORTRAN
C       AND CPU PERFORMANCE.
C
C         References on Whetstones:    Computer Journal Feb 76
C                                      pg 43-49 vol 19 no 1.
C                                      Curnow and Wichman.
C
C                                      and Timing Studies using a
C                                      synthetic Whetstone Benchmark
C                                      S. Harbaugh & J. Forakris
C
C         References on FORTRAN Benchmarks:
C
C                                   -  Computer Languages, Jan 1986
C                                   -  EDN, Oct 3, 1985, Array Processors
C                                           for PCs
C                                   -  Byte, Feb 1984.
C                                                         
C       03/03/87
C          Seeing that Microsoft distributed this without
C          shipping the commented version, the timing loop 
C          was reworked to eliminate all do loops and to put
C          in code to print the variation in the measurment to
C          make it more cook-book.  The 3 loop method described
C          below was eliminated because it caused confusion.
C          The printout was grouped and placed at the end of the test
C          so that the outputs could be checked.  
C          although it is ugly code, it checks with the Ada version
C          and original article.          
C          Because the Whetstones are printed as a reciprical,
C          you can not average Whetstones to reduce time errors,
C          you must run it multiple times and accumulate time.
C          (AKT)
C          
C
C       01/01/87
C          fixed second subroutine to return seconds, not centi
C          seconds and used double precision variables. (AKT)
C   
C       12/15/86
C          Modified by Microsoft, removed reading in loop 
C          option, added timer routine, removed meta-commands
C          on large model.  Changed default looping from 100 to 10.
C
C       9/24/84
C 
C          ADDED CODE TO THESE SO THAT IT HAS VARIABLE LOOPING
C
C          from DEC but DONE BY OUTSIDE CONTRACTOR, OLD STYLE CODING
C          not representative of DEC coding 
C   
C          A. TETEWSKY, c/o 
C          555 TECH SQ MS 92 
C          CAMBRIDGE MASS 02139           617/258-1287
C 
C          benchmarking notes:   1)    insure that timer has
C                                      sufficient resolution and
C                                      uses elapsed CPU time
C
C                                2)    to be useful for mainframe
C                                      comparisons, measure
C                                      INTEGER*4 time and large
C                                      memory model or quote
C                                      both large and small model
C                                      times.  It may be necessary
C                                      to make the arrays in this
C                                      program large enough to span
C                                      a 64K byte boundary because
C                                      many micro compilers will
C                                      generate small model code
C                                      for small arrays even with 
C                                      large models.
C
C                                 3)   Make sure that it loops
C                                      long enough to gain
C                                      stability, i.e. third-second
C                                      loop = first loop time.
C
C         research notes,
C         structure and definition:
C         I received this code as a black box and based on some
C         study, discovered the following background.
C
C            n1-n10 are loop counters for 10 tests, and tests
C            n1,n5, and n10 are skipped.
C            computed goto's are used to skip over tests that
C            are not wanted.
C     
C
C            n1-n10 scale with I.   When I is set to 10,
C            kilo whets per second = 1000/ (time for doing n1-n10),
C            the definition found in the literature.
C
C            If I were 100, the scale factor would be 10,000.
C            which explains the 10,000 discovered in this code because
C            it was shipped with IMUCH wired to 100.
C
C            the original DEC version uses a do-loop, 
C                  imuch=100
C                  do 200 loop=1,3
C                       i = loop*imuch
C                       n1-n10 scales by I
C                       ... whetstones here ...
C              200 continue
C 
C            and it took me a while to figure out why it worked.
C
C            This code loops three times 
C                 TIMES(1) is time for 1*I  whets
C                 TIMES(2) is time for 2*I
C                 TIMES(3) is time for 3*I
C            and TIMES(3)-TIMES(2) =  time for 1*I. 
C            As long as TIMES(3)-TIMES(2) =  TIMES(1) to
C            4 digits, then the cycle counter is sufficiently
C            large enough for a given clock resolution.
C
C            By scaling whets * (IMUCH/10), you can alter IMUCH.
C            The default definition is IMUCH = 10, hence the factor
C            is unity.  IMUCH should be a factor of 10.
C
C
C            Problems I have found:
C            -  the SECONDS function is a single precision number
C               and as CPUs get faster, you need to loop longer
C               so that significant digits are not dropped.
C
C
C       WHETS.FOR       09/27/77     TDR
C       ...WHICH IS AN IMPROVED VERSION OF:
C       WHET2A.FTN      01/22/75     RBG
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
        REAL      X1,X2,X3,X4,X,Y,Z,T,T1,T2,E1
        INTEGER   J,K,L,I, N1,N2,N3,N4,N5,N6,N7,N8,N9,N10,N11,ISAVE
        COMMON    T,T1,T2,E1(4),J,K,L
C
C
        REAL*8         BEGTIM, ENDTIM, DIFTIM
        REAL*8         DT,  WHETS, WS, ERR, WERR, PERR
        INTEGER*4      IO1, IO1L, KREP, MKREP
C
        REAL*4         SECNDS
        EXTERNAL       SECNDS
C
C****************************************************************
C                         
C                                            
        MKREP  =    2
        KREP   =    0
        WRITE(*,*) ' Suggest inner > 10, outer > 1 '
        WRITE(*,*) ' ENTER the number of inner/outer loops  ' 
        READ(*,*)  I  ,IO1 
 7020   CONTINUE
        WRITE(*,*) ' Starting ',IO1,' loops, inner loop = ',I  
C       ***** BEGININNING OF TIMED INTERVAL *****
        IO1L   = 0
        BEGTIM = DBLE(SECNDS(0.0E+00) )
 7010   CONTINUE
C
C       ... the Whetstone code here ...
C
        T=0.499975E00
        T1=0.50025E00
        T2=2.0E00
C
        ISAVE=I
        N1=0
        N2=12*I
        N3=14*I
        N4=345*I
        N5=0
        N6=210*I
        N7=32*I
        N8=899*I
        N9=616*I
        N10=0
        N11=93*I
        N12=0
        X1=1.0E0
        X2=-1.0E0
        X3=-1.0E0
        X4=-1.0E0
        IF(N1)19,19,11
 11     DO 18 I=1,N1,1
        X1=(X1+X2+X3-X4)*T
        X2=(X1+X2-X3+X4)*T
        X4=(-X1+X2+X3+X4)*T
        X3=(X1-X2+X3+X4)*T
 18     CONTINUE
 19     CONTINUE
        E1(1)=1.0E0
        E1(2)=-1.0E0
        E1(3)=-1.0E0
        E1(4)=-1.0E0
        IF(N2)29,29,21
 21     DO 28 I=1,N2,1
        E1(1)=(E1(1)+E1(2)+E1(3)-E1(4))*T
        E1(2)=(E1(1)+E1(2)-E1(3)+E1(4))*T
        E1(3)=(E1(1)-E1(2)+E1(3)+E1(4))*T
        E1(4)=(-E1(1)+E1(2)+E1(3)+E1(4))*T
 28     CONTINUE
 29     CONTINUE
        IF(N3)39,39,31
 31     DO 38 I=1,N3,1
 38     CALL PA(E1)
 39     CONTINUE
        J=1
        IF(N4)49,49,41
 41     DO 48 I=1,N4,1
        IF(J-1)43,42,43
 42     J=2
        GOTO44
 43     J=3
 44     IF(J-2)46,46,45
 45     J=0
        GOTO47
 46     J=1
 47     IF(J-1)411,412,412
 411    J=1
        GOTO48
 412    J=0
 48     CONTINUE
 49     CONTINUE
        J=1
        K=2
        L=3
        IF(N6)69,69,61
 61     DO 68 I=1,N6,1
        J=J*(K-J)*(L-K)
        K=L*K-(L-J)*K
        L=(L-K)*(K+J)
        E1(L-1)=J+K+L
        E1(K-1)=J*K*L
 68     CONTINUE
 69     CONTINUE
        X=0.5E0
        Y=0.5E0
        IF(N7)79,79,71
 71     DO 78 I=1,N7,1
        X=T*ATAN(T2*SIN(X)*COS(X)/(COS(X+Y)+COS(X-Y)-1.0E0))
        Y=T*ATAN(T2*SIN(Y)*COS(Y)/(COS(X+Y)+COS(X-Y)-1.0E0))
 78     CONTINUE
 79     CONTINUE
        X=1.0E0
        Y=1.0E0
        Z=1.0E0
        IF(N8)89,89,81
 81     DO 88 I=1,N8,1
 88     CALL P3(X,Y,Z)
 89     CONTINUE
        J=1
        K=2
        L=3
        E1(1)=1.0E0
        E1(2)=2.0E0
        E1(3)=3.0E0
        IF(N9)99,99,91
 91     DO 98 I=1,N9,1
 98     CALL P0
 99     CONTINUE
        J=2
        K=3
        IF(N10)109,109,101
 101    DO 108 I=1,N10,1
        J=J+K
        K=J+K
        J=J-K
        K=K-J-J
 108    CONTINUE
 109    CONTINUE
        X=0.75E0
        IF(N11)119,119,111
 111    DO 118 I=1,N11,1
 118    X=SQRT(EXP(ALOG(X)/T1))
 119    CONTINUE
        I = ISAVE 
C
C       ... the whetstone ends here
C
C         ... loop counter instead of do loop ...
          IO1L = IO1L + 1
          IF( IO1L .LT. IO1) GOTO 7010
C       ******* END of TIME INTERVALED ***********
C
        ENDTIM = DBLE(SECNDS(0.0E+00))
        DIFTIM = ENDTIM - BEGTIM
C       whets  = 1000/(TIME FOR 10 inner ITERATIONS OF PROGRAM LOOP)
C       or 100 for every 1 inner count
        WHETS = (100.0D+00* DBLE( FLOAT(IO1*I  ))/DIFTIM)
        WRITE(*,*) ' START TIME = ',BEGTIM
        WRITE(*,*) ' END   TIME = ',ENDTIM
        WRITE(*,*) ' DIF   TIME = ',DIFTIM
C
        WRITE (*,201) WHETS
  201   FORMAT(' SPEED IS: ',1PE10.3,' THOUSAND WHETSTONE',
     2     ' SINGLE PRECISION INSTRUCTIONS PER SECOND')
        CALL POUT(N1,N1,N1,X1,X2,X3,X4)
        CALL POUT(N2,N3,N2,E1(1),E1(2),E1(3),E1(4))
        CALL POUT(N3,N2,N2,E1(1),E1(2),E1(3),E1(4))
        CALL POUT(N4,J,J,X1,X2,X3,X4)
        CALL POUT(N6,J,K,E1(1),E1(2),E1(3),E1(4))
        CALL POUT(N7,J,K,X,X,Y,Y)
        CALL POUT(N8,J,K,X,Y,Z,Z)
        CALL POUT(N9,J,K,E1(1),E1(2),E1(3),E1(4))
        CALL POUT(N10,J,K,X1,X2,X3,X4)
        CALL POUT(N11,J,K,X,X,X,X)
C
C
C       ... repeat but double (MULTIPLY UP) inner count ...
        KREP = KREP + 1
        IF( KREP .LT. MKREP) THEN
            DT     = DIFTIM
            WT     = WHETS
            I=I*MKREP
            GOTO 7020
        ENDIF
C
C       ... compute sensitivity  
C
        ERR =  DIFTIM - (DT*DBLE(FLOAT(MKREP)))    
        WERR=  WT-WHETS          
        PERR=  WERR*100.0D+00/WHETS
        WRITE(*,*) ' Time ERR = ',ERR, ' seconds '
        WRITE(*,*) ' Whet ERR = ',WERR,' kwhets/sec '
        WRITE(*,*) ' %    ERR = ',PERR,' % whet error '
        IF( DIFTIM .LT. 10.0D+00) THEN
         WRITE(*,*)
     1   ' TIME is less than 10 seconds, suggest larger inner loop '
        ENDIF
C
        STOP
        END
C
        SUBROUTINE PA(E)
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
        COMMON T,T1,T2
        DIMENSION E(4)
        J=0
 1      E(1)=(E(1)+E(2)+E(3)-E(4))*T
        E(2)=(E(1)+E(2)-E(3)+E(4))*T
        E(3)=(E(1)-E(2)+E(3)+E(4))*T
        E(4)=(-E(1)+E(2)+E(3)+E(4))/T2
        J=J+1
        IF(J-6)1,2,2
 2      CONTINUE
        RETURN
        END
        SUBROUTINE P0
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
        COMMON T,T1,T2,E1(4),J,K,L
        E1(J)=E1(K)
        E1(K)=E1(L)
        E1(L)=E1(J)
        RETURN
        END
        SUBROUTINE P3(X,Y,Z)
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
        COMMON T,T1,T2
        X1=X
        Y1=Y
        X1=T*(X1+Y1)
        Y1=T*(X1+Y1)
        Z=(X1+Y1)/T2
        RETURN
        END
        SUBROUTINE POUT(N,J,K,X1,X2,X3,X4)
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
        WRITE(*,1)N,J,K,X1,X2,X3,X4
 1      FORMAT(1H,3(I7,1X),4(1PE12.4,1X))
        RETURN
        END

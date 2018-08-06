'  EXP.BAS -- COMPUTE EXPONENTIAL WITH INFINITE SERIES
'
' ***********************************************************************
' *                                                                     *
' *  EXP                                                                *
' *                                                                     *
' *  This routine computes EXP(x) using the following infinite series:  *
' *                                                                     *
' *                       x    x^2   x^3   x^4   x^5                    *
' *         EXP(x) = 1 + --- + --- + --- + --- + --- + ...              *
' *                       1!    2!    3!    4!    5!                    *
' *                                                                     *
' *                                                                     *
' *  The program requests a value for x and a value for the convergence *
' *  criterion, C.  The program will continue evaluating the terms of   *
' *  the series until the difference between two terms is less than C.  *
' *                                                                     *
' *  The result of the calculation and the number of terms required to  *
' *  converge are printed.  The program will repeat until an x of 0 is  *
' *  entered.                                                           *
' *                                                                     *
' ***********************************************************************

'
'  Initialize program variables
'
INITIALIZE:
        TERMS = 1
        FACT = 1
        LAST = 1.E35
        DELTA = 1.E34
        EX = 1
'
'  Input user data
'
        INPUT "Enter number:  "; X
        IF X = 0 THEN END
        INPUT "Enter convergence criterion (.0001 for 4 places):  "; C

'
'  Compute exponential until difference of last 2 terms is < C
'
        WHILE ABS(LAST - DELTA) >= C
            LAST = DELTA
            FACT = FACT * TERMS
            DELTA = X^TERMS / FACT
            EX = EX + DELTA
            TERMS = TERMS + 1
        WEND

'
'  Display answer and number of terms required to converge
'
        PRINT EX
        PRINT TERMS; "elements required to converge"
        PRINT

        GOTO INITIALIZE

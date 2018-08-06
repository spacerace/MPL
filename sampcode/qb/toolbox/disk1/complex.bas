  ' ************************************************
  ' **  Name:          COMPLEX                    **
  ' **  Type:          Toolbox                    **
  ' **  Module:        COMPLEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Demonstrates a set of complex number functions and
  ' subprograms.
  '
  ' USAGE:         No command line parameters
  ' .MAK FILE:     COMPLEX.BAS
  '                CARTESIA.BAS
  ' PARAMETERS:    (none)
  ' VARIABLES:     a          Variable of type Complex
  '                b          Variable of type Complex
  '                c          Variable of type Complex
  '                x$         String representation of a complex number
  '                y$         String representation of a complex number
  '                z$         String representation of a complex number
  
    TYPE Complex
        r AS SINGLE
        i AS SINGLE
    END TYPE
  
  ' Subprograms
    DECLARE SUB ComplexSub (a AS Complex, b AS Complex, c AS Complex)
    DECLARE SUB ComplexSqr (a AS Complex, c AS Complex)
    DECLARE SUB ComplexRoot (a AS Complex, b AS Complex, c AS Complex)
    DECLARE SUB ComplexReciprocal (a AS Complex, c AS Complex)
    DECLARE SUB ComplexAdd (a AS Complex, b AS Complex, c AS Complex)
    DECLARE SUB ComplexLog (a AS Complex, c AS Complex)
    DECLARE SUB ComplexPower (a AS Complex, b AS Complex, c AS Complex)
    DECLARE SUB Complex2String (a AS Complex, x$)
    DECLARE SUB String2Complex (x$, a AS Complex)
    DECLARE SUB ComplexDiv (a AS Complex, b AS Complex, c AS Complex)
    DECLARE SUB ComplexExp (a AS Complex, c AS Complex)
    DECLARE SUB ComplexMul (a AS Complex, b AS Complex, c AS Complex)
    DECLARE SUB Rec2pol (x!, y!, r!, theta!)
  
    DIM a AS Complex, b AS Complex, c AS Complex
  
    CLS
    INPUT "Enter first complex number  "; x$
    String2Complex x$, a
    Complex2String a, x$
    PRINT x$
    PRINT
  
    ComplexExp a, c
    Complex2String c, z$
    PRINT "ComplexExp", , z$
  
    ComplexLog a, c
    Complex2String c, z$
    PRINT "ComplexLog", , z$
  
    ComplexReciprocal a, c
    Complex2String c, z$
    PRINT "ComplexReciprocal", z$
  
    ComplexSqr a, c
    Complex2String c, z$
    PRINT "ComplexSqr", , z$
  
    PRINT
    INPUT "Enter second complex number "; y$
    String2Complex y$, b
    Complex2String b, y$
    PRINT y$
    PRINT
  
    ComplexAdd a, b, c
    Complex2String c, z$
    PRINT "ComplexAdd", , z$
  
    ComplexSub a, b, c
    Complex2String c, z$
    PRINT "ComplexSub", , z$
  
    ComplexMul a, b, c
    Complex2String c, z$
    PRINT "ComplexMul", , z$
  
    ComplexDiv a, b, c
    Complex2String c, z$
    PRINT "ComplexDiv", , z$
  
    ComplexPower a, b, c
    Complex2String c, z$
    PRINT "ComplexPower", , z$
  
    ComplexRoot a, b, c
    Complex2String c, z$
    PRINT "ComplexRoot", , z$
  

  ' ************************************************
  ' **  Name:          Complex2String             **
  ' **  Type:          Subprogram                 **
  ' **  Module:        COMPLEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Makes a string representation of a complex number.
  '
  ' EXAMPLE OF USE:  Complex2String a, x$
  ' PARAMETERS:      a          Complex number variable (type Complex)
  '                  x$         String representation of the complex number
  ' VARIABLES:       r$         Working string, real part
  '                  i$         Working string, imaginary part
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Complex
  '                     r AS SINGLE
  '                     i AS SINGLE
  '                  END TYPE
  '
  '                  DECLARE SUB Complex2String (a AS Complex, x$)
  '
    SUB Complex2String (a AS Complex, x$) STATIC
      
      ' Form the left part of the string
        IF a.r < 0 THEN
            r$ = "(" + STR$(a.r)
        ELSE
            r$ = "(" + MID$(STR$(a.r), 2)
        END IF
      
      ' Form the right part of the string
        IF a.i < 0 THEN
            i$ = STR$(a.i)
        ELSE
            i$ = "+" + MID$(STR$(a.i), 2)
        END IF
      
      ' The whole is more complex than the sum of the parts
        x$ = r$ + i$ + "i)"
      
    END SUB

  ' ************************************************
  ' **  Name:          ComplexAdd                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        COMPLEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Adds two complex numbers.
  '
  ' EXAMPLE OF USE:  ComplexAdd a, b, c
  ' PARAMETERS:      a          First complex number for the addition
  '                  b          Second complex number for the addition
  '                  c          Result of the complex number addition
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Complex
  '                     r AS SINGLE
  '                     i AS SINGLE
  '                  END TYPE
  '
  '         DECLARE SUB ComplexAdd (a AS Complex, b AS Complex, c AS Complex)
  '
    SUB ComplexAdd (a AS Complex, b AS Complex, c AS Complex) STATIC
        c.r = a.r + b.r
        c.i = a.i + b.i
    END SUB

  ' ************************************************
  ' **  Name:          ComplexDiv                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        COMPLEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Divides two complex numbers.
  '
  ' EXAMPLE OF USE:  ComplexDiv a, b, c
  ' PARAMETERS:      a          First complex number for the division
  '                  b          Second complex number for the division
  '                  c          Result of the complex number division a/b
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Complex
  '                     r AS SINGLE
  '                     i AS SINGLE
  '                  END TYPE
  '
  '        DECLARE SUB ComplexDiv (a AS Complex, b AS Complex, c AS Complex)
  '
    SUB ComplexDiv (a AS Complex, b AS Complex, c AS Complex) STATIC
        t! = b.r * b.r + b.i * b.i
        c.r = (a.r * b.r + a.i * b.i) / t!
        c.i = (a.i * b.r - a.r * b.i) / t!
    END SUB

  ' ************************************************
  ' **  Name:          ComplexExp                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        COMPLEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Calculates the exponential function of a complex number.
  '
  ' EXAMPLE OF USE:  ComplexExp a, c
  ' PARAMETERS:      a          Complex number argument
  '                  c          Complex result of the calculations
  ' VARIABLES:       t!         Temporary working value
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Complex
  '                     r AS SINGLE
  '                     i AS SINGLE
  '                  END TYPE
  '
  '                  DECLARE SUB ComplexExp (a AS Complex, c AS Complex)
  '
    SUB ComplexExp (a AS Complex, c AS Complex) STATIC
        t! = EXP(a.r)
        c.r = t! * COS(a.i)
        c.i = t! * SIN(a.i)
    END SUB

  ' ************************************************
  ' **  Name:          ComplexLog                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        COMPLEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Calculates the log of a complex number.
  '
  ' EXAMPLE OF USE:  ComplexLog a, c
  ' PARAMETERS:      a          Complex number argument
  '                  c          Complex result of the calculations
  ' VARIABLES:       r!         Magnitude of complex number a
  '                  theta!     Angle of complex number a
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Complex
  '                     r AS SINGLE
  '                     i AS SINGLE
  '                  END TYPE
  '
  '                  DECLARE SUB ComplexLog (a AS Complex, c AS Complex)
  '                  DECLARE SUB Rec2pol (x!, y!, r!, theta!)
  '
    SUB ComplexLog (a AS Complex, c AS Complex) STATIC
        CALL Rec2pol(a.r, a.i, r!, theta!)
        IF r! <> 0! THEN
            c.r = LOG(r!)
            c.i = theta!
        ELSE
            ERROR 5
        END IF
    END SUB

  ' ************************************************
  ' **  Name:          ComplexMul                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        COMPLEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Multiplies two complex numbers.
  '
  ' EXAMPLE OF USE:  ComplexMul a, b, c
  ' PARAMETERS:      a          First complex number for the multiplication
  '                  b          Second complex number for the multiplication
  '                  c          Result of the complex number multiplication
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Complex
  '                     r AS SINGLE
  '                     i AS SINGLE
  '                  END TYPE
  '
  '         DECLARE SUB ComplexMul (a AS Complex, b AS Complex, c AS Complex)
  '
    SUB ComplexMul (a AS Complex, b AS Complex, c AS Complex) STATIC
        c.r = a.r * b.r - a.i * b.i
        c.i = a.r * b.i + a.i * b.r
    END SUB

  ' ************************************************
  ' **  Name:          ComplexPower               **
  ' **  Type:          Subprogram                 **
  ' **  Module:        COMPLEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Calculates a complex number raised to a complex number.
  '
  ' EXAMPLE OF USE:  ComplexPower a, b, c
  ' PARAMETERS:      a          Complex number to be raised to a power
  '                  b          Complex number to raise a to
  '                  c          Result of a raised to the power of b
  ' VARIABLES:       t1         Structure of type Complex
  '                  t2         Structure of type Complex
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Complex
  '                     r AS SINGLE
  '                     i AS SINGLE
  '                  END TYPE
  '
  '      DECLARE SUB ComplexPower (a AS Complex, b AS Complex, c AS Complex)
  '      DECLARE SUB ComplexExp (a AS Complex, c AS Complex)
  '      DECLARE SUB ComplexLog (a AS Complex, c AS Complex)
  '      DECLARE SUB ComplexMul (a AS Complex, b AS Complex, c AS Complex)
  '
    SUB ComplexPower (a AS Complex, b AS Complex, c AS Complex) STATIC
        DIM t1 AS Complex, t2 AS Complex
        IF a.r <> 0! OR a.i <> 0! THEN
            CALL ComplexLog(a, t1)
            CALL ComplexMul(t1, b, t2)
            CALL ComplexExp(t2, c)
        ELSE
            ERROR 5
        END IF
    END SUB

  ' ************************************************
  ' **  Name:          ComplexReciprocal          **
  ' **  Type:          Subprogram                 **
  ' **  Module:        COMPLEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Calculates the reciprocal of a complex number.
  '
  ' EXAMPLE OF USE:  ComplexReciprocal a, c
  ' PARAMETERS:      a          Complex number to be processed
  '                  c          Result of calculating 1/a
  ' VARIABLES:       t          Structure of type Complex
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Complex
  '                     r AS SINGLE
  '                     i AS SINGLE
  '                  END TYPE
  '
  '         DECLARE SUB ComplexReciprocal (a AS Complex, c AS Complex)
  '         DECLARE SUB ComplexDiv (a AS Complex, b AS Complex, c AS Complex)
  '
    SUB ComplexReciprocal (a AS Complex, c AS Complex) STATIC
        DIM t AS Complex
        t.r = 1!
        t.i = 0
        ComplexDiv t, a, c
    END SUB

  ' ************************************************
  ' **  Name:          ComplexRoot                **
  ' **  Type:          Subprogram                 **
  ' **  Module:        COMPLEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Calculates the complex root of a complex number.
  '
  ' EXAMPLE OF USE:  ComplexRoot a, b, c
  ' PARAMETERS:      a          First complex number
  '                  b          Complex number root
  '                  c          Result of finding the bth root of a
  ' VARIABLES:       t          Structure of type Complex
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Complex
  '                     r AS SINGLE
  '                     i AS SINGLE
  '                  END TYPE
  '
  '       DECLARE SUB ComplexRoot (a AS Complex, b AS Complex, c AS Complex)
  '       DECLARE SUB ComplexReciprocal (a AS Complex, c AS Complex)
  '       DECLARE SUB ComplexPower (a AS Complex, b AS Complex, c AS Complex)
  '
    SUB ComplexRoot (a AS Complex, b AS Complex, c AS Complex) STATIC
        DIM t AS Complex
        IF b.r <> 0! OR b.i <> 0! THEN
            CALL ComplexReciprocal(b, t)
            CALL ComplexPower(a, t, c)
        ELSE
            ERROR 5
        END IF
    END SUB

  ' ************************************************
  ' **  Name:          ComplexSqr                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        COMPLEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Calculates the square root of a complex number.
  '
  'EXAMPLE OF USE:   ComplexSqr a, c
  'PARAMETERS:       a          Complex number argument
  '                  c          Result of finding the square root of a
  'VARIABLES:        r!         Magnitude of complex number a
  '                  theta!     Angle of complex number a
  '                  rs!        Square root of r!
  '                  h!         One half of theta!
  '
  'MODULE LEVEL
  ' DECLARATIONS:    TYPE Complex
  '                     r AS SINGLE
  '                     i AS SINGLE
  '                  END TYPE
  '
  '                  DECLARE SUB ComplexSqr (a AS Complex, c AS Complex)
  '
    SUB ComplexSqr (a AS Complex, c AS Complex) STATIC
        CALL Rec2pol(a.r, a.i, r!, theta!)
        rs! = SQR(r!)
        h! = theta! / 2!
        c.r = rs! * COS(h!)
        c.i = rs! * SIN(h!)
    END SUB

  ' ************************************************
  ' **  Name:          ComplexSub                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        COMPLEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Subtracts two complex numbers.
  '
  ' EXAMPLE OF USE:  ComplexSub a, b, c
  ' PARAMETERS:      a          First complex number
  '                  b          Second Complex number
  '                  c          Result of subtracting b from a
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Complex
  '                     r AS SINGLE
  '                     i AS SINGLE
  '                  END TYPE
  '
  '         DECLARE SUB ComplexSub (a AS Complex, b AS Complex, c AS Complex)
  '
    SUB ComplexSub (a AS Complex, b AS Complex, c AS Complex) STATIC
        c.r = a.r - b.r
        c.i = a.i - b.i
    END SUB

  ' ************************************************
  ' **  Name:          String2Complex             **
  ' **  Type:          Subprogram                 **
  ' **  Module:        COMPLEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Converts a string representation of a complex
  ' number to a type Complex variable.
  '
  ' EXAMPLE OF USE:  String2Complex x$, a
  ' PARAMETERS:      x$         String representation of a complex number
  '                  a          Complex number structure of type Complex
  ' VARIABLES:       j%         Index to first numerical character
  '                  i%         Pointer to the "i" or "j" character
  '                  k%         Pointer to start of imaginary part
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Complex
  '                     r AS SINGLE
  '                     i AS SINGLE
  '                  END TYPE
  '
  '                  DECLARE SUB Complex2String (a AS Complex, x$)
  '
    SUB String2Complex (x$, a AS Complex) STATIC
      
      ' Real part starts just after left parenthesis
        j% = INSTR(x$, "(") + 1
      
      ' Step forward to find start of number
        DO UNTIL INSTR("+-0123456789", MID$(x$, j%, 1)) OR j% > LEN(x$)
            j% = j% + 1
        LOOP
      
      ' Imaginary part ends at the "i" or "j"
        i% = INSTR(LCASE$(x$), "i")
        IF INSTR(LCASE$(x$), "j") > i% THEN
            i% = INSTR(LCASE$(x$), "j")
        END IF
      
      ' Step back to find start of imaginary part
        FOR k% = i% TO 1 STEP -1
            IF INSTR("+-", MID$(x$, k%, 1)) THEN
                EXIT FOR
            END IF
        NEXT k%
      
      ' Error if pointers don't make sense
        IF j% = 0 OR j% > LEN(x$) THEN
            PRINT "Error: String2Complex - unrecognizable string format"
            SYSTEM
        END IF
      
      ' Grab the real part
        a.r = VAL(MID$(x$, j%))
      
      ' Grab the imaginary part
        IF k% > j% THEN
            a.i = VAL(MID$(x$, k%))
        ELSEIF k% = j% THEN
            a.r = 0
            a.i = VAL(MID$(x$, j%))
        ELSE
            a.i = 0
        END IF
      
    END SUB


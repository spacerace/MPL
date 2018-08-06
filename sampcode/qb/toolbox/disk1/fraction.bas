  ' ************************************************
  ' **  Name:          FRACTION                   **
  ' **  Type:          Toolbox                    **
  ' **  Module:        FRACTION.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Demonstrates a collection of functions and subprograms
  ' for working with fractions.
  '
  ' USAGE:           No command line parameters
  ' .MAK FILE:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       a          Structure of type Fraction
  '                  b          Structure of type Fraction
  '                  c          Structure of type Fraction
  '                  f$         Input string for fraction problems
  '                  fa$        First fraction in string format
  '                  fb$        Second fraction in string format
  '                  operator$  Function indicator
  '                  fc$        Resultant fraction in string output form
  
  ' Data structure definitions
    TYPE Fraction
        Num AS LONG
        Den AS LONG
    END TYPE
  
  ' Subprograms
    DECLARE SUB FractionReduce (a AS Fraction)
    DECLARE SUB String2Fraction (f$, a AS Fraction)
    DECLARE SUB FractionAdd (a AS Fraction, b AS Fraction, c AS Fraction)
    DECLARE SUB FractionDiv (a AS Fraction, b AS Fraction, c AS Fraction)
    DECLARE SUB FractionMul (a AS Fraction, b AS Fraction, c AS Fraction)
    DECLARE SUB FractionSub (a AS Fraction, b AS Fraction, c AS Fraction)
    DECLARE SUB SplitFractions (f$, fa$, operator$, fb$)
  
  ' Functions
    DECLARE FUNCTION Fraction2String$ (a AS Fraction)
    DECLARE FUNCTION GreatestComDiv& (n1&, n2&)
    DECLARE FUNCTION LeastComMul& (n1&, n2&)
  
  ' Data structures
    DIM a AS Fraction
    DIM b AS Fraction
    DIM c AS Fraction
  
  ' Demonstrate the LeastComMul& function
    CLS
    PRINT "LeastComMul&(21&, 49&)    =", LeastComMul&(21&, 49&)
    PRINT
  
  ' Demonstrate the GreatestComDiv& function
    PRINT "GreatestComDiv&(21&, 49&) =", GreatestComDiv&(21&, 49&)
    PRINT
  
  ' Demonstrate the fraction routines
    DO
        PRINT
        PRINT "Enter a fraction problem, or simply press Enter"
        PRINT "Example: 2/3 + 4/5"
        PRINT
        LINE INPUT f$
        IF INSTR(f$, "/") = 0 THEN
            EXIT DO
        END IF
        SplitFractions f$, fa$, operator$, fb$
        String2Fraction fa$, a
        String2Fraction fb$, b
        SELECT CASE operator$
        CASE "+"
            FractionAdd a, b, c
        CASE "-"
            FractionSub a, b, c
        CASE "*"
            FractionMul a, b, c
        CASE "/"
            FractionDiv a, b, c
        CASE ELSE
            BEEP
        END SELECT
        fc$ = Fraction2String$(c)
        PRINT "Result (reduced to lowest terms) is "; fc$
    LOOP

  ' ************************************************
  ' **  Name:          Fraction2String$           **
  ' **  Type:          Function                   **
  ' **  Module:        FRACTION.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Converts a type Fraction variable to a string.
  '
  ' EXAMPLE OF USE:  fa$ = Fraction2String$(a)
  ' PARAMETERS:      a          Structure of type Fraction
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Fraction
  '                     Num AS LONG
  '                     Den AS LONG
  '                  END TYPE
  '
  '                  DECLARE FUNCTION Fraction2String$ (a AS Fraction)
  '
    FUNCTION Fraction2String$ (a AS Fraction) STATIC
        Fraction2String$ = STR$(a.Num) + "/" + STR$(a.Den)
    END FUNCTION

  ' ************************************************
  ' **  Name:          FractionAdd                **
  ' **  Type:          Subprogram                 **
  ' **  Module:        FRACTION.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Adds two fractions and reduces the result to lowest terms.
  '
  ' EXAMPLE OF USE:  FractionAdd a, b, c
  ' PARAMETERS:      a          First fraction to add
  '                  b          Second fraction to add
  '                  c          Resulting fraction
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Fraction
  '                     Num AS LONG
  '                     Den AS LONG
  '                  END TYPE
  '
  '     DECLARE SUB FractionReduce (a AS Fraction)
  '     DECLARE SUB FractionAdd (a AS Fraction, b AS Fraction, c AS Fraction)
  '     DECLARE FUNCTION GreatestComDiv& (n1&, n2&)
  '
    SUB FractionAdd (a AS Fraction, b AS Fraction, c AS Fraction)
        c.Num = a.Num * b.Den + a.Den * b.Num
        c.Den = a.Den * b.Den
        FractionReduce c
    END SUB

  ' ************************************************
  ' **  Name:          FractionDiv                **
  ' **  Type:          Subprogram                 **
  ' **  Module:        FRACTION.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Divides two fractions and reduces the result to
  ' lowest terms.
  '
  ' EXAMPLE OF USE:  FractionDiv a, b, c
  ' PARAMETERS:      a          First fraction
  '                  b          Fraction to divide into first
  '                  c          Resulting fraction
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Fraction
  '                     Num AS LONG
  '                     Den AS LONG
  '                  END TYPE
  '
  '     DECLARE SUB FractionReduce (a AS Fraction)
  '     DECLARE SUB FractionDiv (a AS Fraction, b AS Fraction, c AS Fraction)
  '     DECLARE FUNCTION GreatestComDiv& (n1&, n2&)
  '
    SUB FractionDiv (a AS Fraction, b AS Fraction, c AS Fraction)
        c.Num = a.Num * b.Den
        c.Den = a.Den * b.Num
        FractionReduce c
    END SUB

  ' ************************************************
  ' **  Name:          FractionMul                **
  ' **  Type:          Subprogram                 **
  ' **  Module:        FRACTION.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Multiplies two fractions and reduces the result to
  ' lowest terms.
  '
  ' EXAMPLE OF USE:  FractionMul a, b, c
  ' PARAMETERS:      a          First fraction to multiply
  '                  b          Second fraction to multiply
  '                  c          Resulting fraction
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Fraction
  '                     Num AS LONG
  '                     Den AS LONG
  '                  END TYPE
  '
  '     DECLARE SUB FractionReduce (a AS Fraction)
  '     DECLARE SUB FractionMul (a AS Fraction, b AS Fraction, c AS Fraction)
  '     DECLARE FUNCTION GreatestComDiv& (n1&, n2&)
  '
    SUB FractionMul (a AS Fraction, b AS Fraction, c AS Fraction)
        c.Num = a.Num * b.Num
        c.Den = a.Den * b.Den
        FractionReduce c
    END SUB

  ' ************************************************
  ' **  Name:          FractionReduce             **
  ' **  Type:          Subprogram                 **
  ' **  Module:        FRACTION.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Reduces a fraction to its lowest terms.
  '
  ' EXAMPLE OF USE:  FractionReduce a
  ' PARAMETERS:      a          Fraction to reduce
  ' VARIABLES:       d&         Greatest common divisor of the numerator and
  '                             denominator
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Fraction
  '                     Num AS LONG
  '                     Den AS LONG
  '                  END TYPE
  '
  '                  DECLARE SUB FractionReduce (a AS Fraction)
  '                  DECLARE FUNCTION GreatestComDiv& (n1&, n2&)
  '
    SUB FractionReduce (a AS Fraction)
        d& = GreatestComDiv&(a.Num, a.Den)
        a.Num = a.Num / d&
        a.Den = a.Den / d&
    END SUB

  ' ************************************************
  ' **  Name:          FractionSub                **
  ' **  Type:          Subprogram                 **
  ' **  Module:        FRACTION.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Subtracts two fractions and reduces the result to
  ' lowest terms.
  '
  ' EXAMPLE OF USE:  FractionSub a, b, c
  ' PARAMETERS:      a          First fraction
  '                  b          Fraction to subtract from the first
  '                  c          Resulting fraction
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE Fraction
  '                     Num AS LONG
  '                     Den AS LONG
  '                  END TYPE
  '
  '     DECLARE SUB FractionReduce (a AS Fraction)
  '     DECLARE SUB FractionSub (a AS Fraction, b AS Fraction, c AS Fraction)
  '     DECLARE FUNCTION GreatestComDiv& (n1&, n2&)
  '
    SUB FractionSub (a AS Fraction, b AS Fraction, c AS Fraction)
        c.Num = a.Num * b.Den - a.Den * b.Num
        c.Den = a.Den * b.Den
        FractionReduce c
    END SUB

  ' ************************************************
  ' **  Name:          GreatestComDiv&            **
  ' **  Type:          Function                   **
  ' **  Module:        FRACTION.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the greatest common divisor of two long integers.
  '
  ' EXAMPLE OF USE:  gcd& = GreatestComDiv& (n1&, n2&)
  ' PARAMETERS:      n1&        First long integer
  '                  n2&        Second long integer
  ' VARIABLES:       ta&        Working copy of n1&
  '                  tb&        Working copy of n2&
  '                  tc&        Working variable
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION GreatestComDiv& (n1&, n2&)
  '
    FUNCTION GreatestComDiv& (n1&, n2&)
        ta& = n1&
        tb& = n2&
        DO
            tc& = ta& MOD tb&
            ta& = tb&
            tb& = tc&
        LOOP WHILE tc&
        GreatestComDiv& = ta&
    END FUNCTION

  ' ************************************************
  ' **  Name:          LeastComMul&               **
  ' **  Type:          Function                   **
  ' **  Module:        FRACTION.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the least common multiple of two long integers.
  '
  ' EXAMPLE OF USE:  lcm& = LeastComMul& (n1&, n2&)
  ' PARAMETERS:      n1&         First long integer
  '                  n2&         Second long integer
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION LeastComMul& (n1&, n2&)
  '
    FUNCTION LeastComMul& (n1&, n2&)
        LeastComMul& = ABS(n1& * n2& / GreatestComDiv&(n1&, n2&))
    END FUNCTION

  ' ************************************************
  ' **  Name:          SplitFractions             **
  ' **  Type:          Subprogram                 **
  ' **  Module:        FRACTION.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Splits an input fraction problem string into
  ' three strings representing each of the two
  ' fractions and a one-character string of the
  ' operation given.
  '
  ' EXAMPLE OF USE: SplitFractions f$, fa$, operator$, fb$
  ' PARAMETERS:     f$         Input string from the FRACTIONS demonstration
  '                            program
  '                 fa$        First fraction, extracted from f$
  '                 operator$  Mathematical operation symbol, from f$
  '                 fb$        Second fraction, extracted from f$
  ' VARIABLES:      i%         Looping index
  '                 ndx%       Index to mathematical operation symbol
  ' MODULE LEVEL
  '   DECLARATIONS: DECLARE SUB SplitFractions (f$, fa$, operator$, fb$)
  '
    SUB SplitFractions (f$, fa$, operator$, fb$)
        fa$ = ""
        fb$ = ""
        operator$ = ""
        FOR i% = 1 TO 4
            ndx% = INSTR(f$, MID$("+-*/", i%, 1))
            IF ndx% THEN
                IF i% = 4 THEN
                    ndx% = INSTR(ndx% + 1, f$, "/")
                END IF
                fa$ = LEFT$(f$, ndx% - 1)
                fb$ = MID$(f$, ndx% + 1)
                operator$ = MID$(f$, ndx%, 1)
                EXIT FOR
            END IF
        NEXT i%
    END SUB

  ' ************************************************
  ' **  Name:          String2Fraction            **
  ' **  Type:          Subprogram                 **
  ' **  Module:        FRACTION.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Converts a string to a type Fraction variable.
  '
  ' EXAMPLE OF USE: String2Fraction f$, a
  ' PARAMETERS:     f$         String representation of a fraction
  '                 a          Structure of type Fraction
  ' VARIABLES:      (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB String2Fraction (f$, a AS Fraction)
  '
    SUB String2Fraction (f$, a AS Fraction)
        a.Num = VAL(f$)
        a.Den = VAL(MID$(f$, INSTR(f$, "/") + 1))
    END SUB


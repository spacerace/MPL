  ' ************************************************
  ' **  Name:          PROBSTAT                   **
  ' **  Type:          Toolbox                    **
  ' **  Module:        PROBSTAT.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Demonstrates several probability and statistics-
  ' related mathematical functions.
  '
  ' USAGE:          No command line parameters
  ' .MAK FILE:      (none)
  ' PARAMETERS:     (none)
  ' VARIABLES:      a#()       Array of numbers to be processed
  '                 i%         Index into array
  '                 n&         Number of items for combinations and permutations
  '                 r&         Quantity for combinations and permutations
  
  
    DECLARE FUNCTION Combinations# (n&, r&)
    DECLARE FUNCTION Factorial# (n&)
    DECLARE FUNCTION Permutations# (n&, r&)
    DECLARE FUNCTION GeometricMean# (a#())
    DECLARE FUNCTION HarmonicMean# (a#())
    DECLARE FUNCTION ArithmeticMean# (a#())
    DECLARE FUNCTION QuadraticMean# (a#())
  
  ' Demonstrations
    CLS
    PRINT "PROBSTAT"
    PRINT
    PRINT "Array of numbers..."
    DIM a#(-3 TO 6)
    FOR i% = -3 TO 6
        READ a#(i%)
        PRINT a#(i%),
    NEXT i%
    PRINT
    DATA  1.2,3.4,5.6,7.8,9.1,2.3,4.5,6.7,8.9,1.2
  
    PRINT
    PRINT "Arithmetic mean = "; ArithmeticMean#(a#())
    PRINT "Geometric mean  = "; GeometricMean#(a#())
    PRINT "Harmonic mean   = "; HarmonicMean#(a#())
    PRINT "Quadratic mean  = "; QuadraticMean#(a#())
    PRINT
  
    n& = 17
    r& = 5
    PRINT "Combinations of"; n&; "objects taken";
    PRINT r&; "at a time = "; Combinations#(n&, r&)
  
    PRINT "Permutations of"; n&; "objects taken";
    PRINT r&; "at a time = "; Permutations#(n&, r&)
  
    PRINT
    PRINT "Factorial of 17 = "; Factorial#(17&)
  
  ' All done
    END
  

  ' ************************************************
  ' **  Name:          ArithmeticMean#            **
  ' **  Type:          Function                   **
  ' **  Module:        PROBSTAT.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the arithmetic mean of an array of numbers.
  '
  ' EXAMPLE OF USE:  ArithmeticMean# a#()
  ' PARAMETERS:      a#()       Array of double-precision numbers to be
  '                             processed
  ' VARIABLES:       n%         Count of array entries
  '                  sum#       Sum of the array entries
  '                  i%         Looping index
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION ArithmeticMean# (a#())
  '
    FUNCTION ArithmeticMean# (a#()) STATIC
        n% = 0
        sum# = 0
        FOR i% = LBOUND(a#) TO UBOUND(a#)
            n% = n% + 1
            sum# = sum# + a#(i%)
        NEXT i%
        ArithmeticMean# = sum# / n%
    END FUNCTION

  ' ************************************************
  ' **  Name:          Combinations#              **
  ' **  Type:          Function                   **
  ' **  Module:        PROBSTAT.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the number of combinations of n& items
  ' taken r& at a time.
  '
  ' EXAMPLE OF USE:  c# = Combinations#(n&, r&)
  ' PARAMETERS:      n&         Number of items
  '                  r&         Taken r& at a time
  ' VARIABLES:       result#    Working result variable
  '                  j&         Working copy of r&
  '                  k&         Difference between n& and r&
  '                  h&         Values from r& through n&
  '                  i&         Values from 1 through j&
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Combinations# (n&, r&)
  '
    FUNCTION Combinations# (n&, r&) STATIC
        result# = 1
        j& = r&
        k& = n& - r&
        h& = n&
        IF j& > k& THEN
            SWAP j&, k&
        END IF
        FOR i& = 1 TO j&
            result# = (result# * h&) / i&
            h& = h& - 1
        NEXT i&
        Combinations# = result#
    END FUNCTION

  ' ************************************************
  ' **  Name:          Factorial#                 **
  ' **  Type:          Function                   **
  ' **  Module:        PROBSTAT.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the factorial of n& (recursive).
  '
  ' EXAMPLE OF USE:  f# = Factorial#(n&)
  ' PARAMETERS:      n&         Number to be evaluated
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Factorial# (n&)
  '
    FUNCTION Factorial# (n&)
        IF n& > 1 THEN
            Factorial# = n& * Factorial#(n& - 1)
        ELSE
            Factorial# = 1
        END IF
    END FUNCTION

  ' ************************************************
  ' **  Name:          GeometricMean#             **
  ' **  Type:          Function                   **
  ' **  Module:        PROBSTAT.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the geometric mean of an array of numbers.
  '
  ' EXAMPLE OF USE:  gm# = GeometricMean#(a#())
  ' PARAMETERS:      a#()       Array of numbers to be processed
  ' VARIABLES:       n%         Count of numbers
  '                  product#   Product of all the numbers
  '                  i%         Index to array entries
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION GeometricMean# (a#())
  '
    FUNCTION GeometricMean# (a#()) STATIC
        n% = 0
        product# = 1
        FOR i% = LBOUND(a#) TO UBOUND(a#)
            n% = n% + 1
            product# = product# * a#(i%)
        NEXT i%
        GeometricMean# = product# ^ (1 / n%)
    END FUNCTION

  ' ************************************************
  ' **  Name:          HarmonicMean#              **
  ' **  Type:          Function                   **
  ' **  Module:        PROBSTAT.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the harmonic mean of an array of numbers.
  '
  ' EXAMPLE OF USE:  hm# = HarmonicMean#(a#())
  ' PARAMETERS:      a#()       Array of numbers to be processed
  ' VARIABLES:       n%         Number of array entries
  '                  sum#       Sum of the reciprocal of each number
  '                  i%         Index to each array entry
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION HarmonicMean# (a#())
  '
    FUNCTION HarmonicMean# (a#()) STATIC
        n% = 0
        sum# = 0
        FOR i% = LBOUND(a#) TO UBOUND(a#)
            n% = n% + 1
            sum# = sum# + 1# / a#(i%)
        NEXT i%
        HarmonicMean# = n% / sum#
    END FUNCTION

  ' ************************************************
  ' **  Name:          Permutations#              **
  ' **  Type:          Function                   **
  ' **  Module:        PROBSTAT.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the permutations of n& items taken r& at a time.
  '
  ' EXAMPLE OF USE:  perm# = Permutations#(n&, r&)
  ' PARAMETERS:      n&         Number of items
  '                  r&         Taken r& at a time
  ' VARIABLES:       p#         Working variable for permutations
  '                  i&         Loop index
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Permutations# (n&, r&)
  '
    FUNCTION Permutations# (n&, r&) STATIC
        p# = 1
        FOR i& = n& - r& + 1 TO n&
            p# = p# * i&
        NEXT i&
        Permutations# = p#
    END FUNCTION

  ' ************************************************
  ' **  Name:          QuadraticMean#             **
  ' **  Type:          Function                   **
  ' **  Module:        PROBSTAT.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the quadratic mean of an array of numbers.
  '
  ' EXAMPLE OF USE:  qm# = QuadraticMean#(a#())
  ' PARAMETERS:      a#()       Array of numbers to be processed
  ' VARIABLES:       n%         Count of array entries
  '                  sum#       Sum of the square of each number
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION QuadraticMean# (a#())
  '
    FUNCTION QuadraticMean# (a#()) STATIC
        n% = 0
        sum# = 0
        FOR i% = LBOUND(a#) TO UBOUND(a#)
            n% = n% + 1
            sum# = sum# + a#(i%) ^ 2
        NEXT i%
        QuadraticMean# = SQR(sum# / n%)
    END FUNCTION


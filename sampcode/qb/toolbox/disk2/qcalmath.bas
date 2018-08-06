  ' ************************************************
  ' **  Name:          QCALMATH                   **
  ' **  Type:          Toolbox                    **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Collection of math functions and subprograms for
  ' the QCAL program.
  '
  ' USAGE:         (loaded by the QCAL program)
  '.MAK FILE:      (none)
  ' PARAMETERS:    (none)
  ' VARIABLES:     (none)
  ' Constants
    CONST PI = 3.141592653589793#
    CONST L10 = 2.302585092994046#
  
  ' Functions
    DECLARE FUNCTION AbsoluteX# (x#)
    DECLARE FUNCTION Add# (y#, x#)
    DECLARE FUNCTION ArcCosine# (x#)
    DECLARE FUNCTION ArcHypCosine# (x#)
    DECLARE FUNCTION ArcHypSine# (x#)
    DECLARE FUNCTION ArcHypTangent# (x#)
    DECLARE FUNCTION ArcSine# (x#)
    DECLARE FUNCTION ArcTangent# (x#)
    DECLARE FUNCTION Ceil# (x#)
    DECLARE FUNCTION ChangeSign# (x#)
    DECLARE FUNCTION Cosine# (x#)
    DECLARE FUNCTION Divide# (y#, x#)
    DECLARE FUNCTION Exponential# (x#)
    DECLARE FUNCTION FractionalPart# (x#)
    DECLARE FUNCTION HypCosine# (x#)
    DECLARE FUNCTION HypSine# (x#)
    DECLARE FUNCTION HypTangent# (x#)
    DECLARE FUNCTION IntegerPart# (x#)
    DECLARE FUNCTION LogBase10# (x#)
    DECLARE FUNCTION LogBaseN# (y#, x#)
    DECLARE FUNCTION LogE# (x#)
    DECLARE FUNCTION Modulus# (y#, x#)
    DECLARE FUNCTION Multiply# (y#, x#)
    DECLARE FUNCTION OneOverX# (x#)
    DECLARE FUNCTION Sign# (x#)
    DECLARE FUNCTION Sine# (x#)
    DECLARE FUNCTION SquareRoot# (x#)
    DECLARE FUNCTION Subtract# (y#, x#)
    DECLARE FUNCTION Tangent# (x#)
    DECLARE FUNCTION Xsquared# (x#)
    DECLARE FUNCTION YRaisedToX# (y#, x#)
  
  ' ************************************************
  ' **  Name:          AbsoluteX#                 **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = AbsoluteX#(x#)
  ' PARAMETERS:      x#         Double-precision value to be evaluated
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION AbsoluteX# (x#)
  '
    FUNCTION AbsoluteX# (x#) STATIC
        AbsoluteX# = ABS(x#)
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Add#                       **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  z# = Add#(y#, x#)
  ' PARAMETERS:      y#         First number
  '                  x#         Second number
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Add# (y#, x#)
  '
    FUNCTION Add# (y#, x#) STATIC
        Add# = y# + x#
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          ArcCosine#                 **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = ArcCosine#(x#)
  ' PARAMETERS:      x#         Number to be evaluated
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION ArcCosine# (x#)
  '
    FUNCTION ArcCosine# (x#) STATIC
        x2# = x# * x#
        IF x2# < 1# THEN
            ArcCosine# = PI / 2# - ATN(x# / SQR(1# - x# * x#))
        ELSE
            PRINT "Error: ACS(x#) where x# < 1"
            SYSTEM
        END IF
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          ArcHypCosine#              **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = ArcHypCosine#(x#)
  ' PARAMETERS:      x#         Number to be evaluated
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION ArcHypCosine# (x#)
  '
    FUNCTION ArcHypCosine# (x#) STATIC
        IF ABS(x#) > 1# THEN
            ArcHypCosine# = LOG(x# + SQR(x# * x# - 1#))
        ELSE
            PRINT "Error: AHS(x#) where -1 <= x# <= +1"
            SYSTEM
        END IF
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          ArcHypSine#                **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = ArcHypSine#(x#)
  ' PARAMETERS:      x#    Number to be evaluated
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION AryHypSine# (x#)
  '
    FUNCTION ArcHypSine# (x#) STATIC
        ArcHypSine# = LOG(x# + SQR(1# + x# * x#))
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          ArcHypTangent#             **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = ArcHypTangent#(x#)
  ' PARAMETERS:      x#         Number to be evaluated
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION ArcHypTangent# (x#)
  '
    FUNCTION ArcHypTangent# (x#) STATIC
        IF ABS(x#) < 1 THEN
            ArcHypTangent# = LOG((1# + x#) / (1# - x#)) / 2#
        ELSE
            PRINT "Error: AHT(x#) where x# <= -1 or x# >= +1"
            SYSTEM
        END IF
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          ArcSine#                   **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = ArcSine#(x#)
  ' PARAMETERS:      x#         Number to be evaluated
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION ArcSine# (x#)
  '
    FUNCTION ArcSine# (x#) STATIC
        x2# = x# * x#
        IF x2# < 1# THEN
            ArcSine# = ATN(x# / SQR(1# - x# * x#))
        ELSE
            PRINT "Error: ASN(x#) where x# >= 1"
            SYSTEM
        END IF
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          ArcTangent#                **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = ArcTangent#(x#)
  ' PARAMETERS:      x#         Number to be evaluated
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION ArcTangent# (x#)
  '
    FUNCTION ArcTangent# (x#) STATIC
        ArcTangent# = ATN(x#)
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Ceil#                      **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = Ceil#(x#)
  ' PARAMETERS:      x#         Number to be evaluated
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Ceil# (x#)
  '
    FUNCTION Ceil# (x#) STATIC
        Ceil# = -INT(-x#)
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          ChangeSign#                **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = ChangeSign#(x#)
  ' PARAMETERS:      x#         Number to be evaluated
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION ChangeSign# (x#)
  '
    FUNCTION ChangeSign# (x#) STATIC
        ChangeSign# = -x#
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Cosine#                    **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = Cosine#(x#)
  ' PARAMETERS:      x#         Angle to be evaluated
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Cosine# (x#)
  '
    FUNCTION Cosine# (x#) STATIC
        Cosine# = COS(x#)
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Divide#                    **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = Divide#(y#, x#)
  ' PARAMETERS:      y#         Number to be processed
  '                  x#         Number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Divide# (y#, x#)
  '
    FUNCTION Divide# (y#, x#) STATIC
        IF x# <> 0 THEN
            Divide# = y# / x#
        ELSE
            PRINT "Error: Division by zero"
            SYSTEM
        END IF
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Dup                        **
  ' **  Type:          Subprogram                 **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  Dup stack#(), ptr%
  ' PARAMETERS:      stack#()   Numeric stack
  '                  ptr%       Index to last entry on stack
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Dup (Stack#(), ptr%)
  '
    SUB Dup (stack#(), ptr%) STATIC
        IF ptr% THEN
            ptr% = ptr% + 1
            stack#(ptr%) = stack#(ptr% - 1)
        END IF
    END SUB
  
  ' ************************************************
  ' **  Name:          Exponential#               **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = Exponential#(x#)
  ' PARAMETERS:      x#         Number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Exponential# (x#)
  '
    FUNCTION Exponential# (x#) STATIC
        Exponential# = EXP(x#)
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          FractionalPart#            **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = FractionalPart#(x#)
  ' PARAMETERS:      x#         Number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION FractionalPart# (x#)
  '
    FUNCTION FractionalPart# (x#) STATIC
        IF x# >= 0 THEN
            FractionalPart# = x# - INT(x#)
        ELSE
            FractionalPart# = x# - INT(x#) - 1#
        END IF
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          HypCosine#                 **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = HypCosine#(x#)
  ' PARAMETERS:      x#         Number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION HypCosine# (x#)
  '
    FUNCTION HypCosine# (x#) STATIC
        HypCosine# = (EXP(x#) + EXP(-x#)) / 2#
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          HypSine#                   **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = HypSine#(x#)
  ' PARAMETERS:      x#         Number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION HypSine# (x#)
  '
    FUNCTION HypSine# (x#) STATIC
        HypSine# = (EXP(x#) - EXP(-x#)) / 2#
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          HypTangent#                **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = HypTangent#(x#)
  ' PARAMETERS:      x#         Number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION HypTangent# (x#)
  '
    FUNCTION HypTangent# (x#) STATIC
        HypTangent# = (EXP(x#) - EXP(-x#)) / (EXP(x#) + EXP(-x#))
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          IntegerPart#               **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = IntegerPart#(x#)
  ' PARAMETERS:      x#         Number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION IntegerPart# (x#)
  '
    FUNCTION IntegerPart# (x#) STATIC
        IntegerPart# = INT(x#)
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          LogBase10#                 **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = Log10#(x#)
  ' PARAMETERS:      x#         Number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION LogBase10# (x#)
  '
    FUNCTION LogBase10# (x#) STATIC
        IF x# > 0 THEN
            LogBase10# = LOG(x#) / L10
        ELSE
            PRINT "Error: LOG10(x#) where x# <= 0"
            SYSTEM
        END IF
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          LogBaseN#                  **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = LogBaseN#(y#, x#)
  ' PARAMETERS:      y#         Number to be processed
  '                  x#         The base for finding the logarithm
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION LogBaseN# (y#, x#)
  '
    FUNCTION LogBaseN# (y#, x#) STATIC
        IF x# <= 0 THEN
            PRINT "Error: LOGN(y#, x#) where x# <= 0"
            SYSTEM
        ELSEIF x# = 1# THEN
            PRINT "Error: LOGN(y#, x#) where x# = 1"
            SYSTEM
        ELSEIF y# <= 0 THEN
            PRINT "Error: LOGN(y#, x#) where y# is <= 0"
            SYSTEM
        ELSE
            LogBaseN# = LOG(y#) / LOG(x#)
        END IF
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          LogE#                      **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = LogE#(x#)
  ' PARAMETERS:      x#         Number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION LogE# (x#)
  '
    FUNCTION LogE# (x#) STATIC
        IF x# > 0 THEN
            LogE# = LOG(x#)
        ELSE
            PRINT "Error: LOGE(x#) where x# <= 0"
            SYSTEM
        END IF
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Modulus#                   **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = Modulus#(y#, x#)
  ' PARAMETERS:      y#         Number to be divided
  '                  x#         Number for dividing by
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Modulus# (y#, x#)
  '
    FUNCTION Modulus# (y#, x#) STATIC
        IF x# <> 0 THEN
            Modulus# = y# - INT(y# / x#) * x#
        ELSE
            PRINT "Error: MOD(y#, x#) where x# = 0"
            SYSTEM
        END IF
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Multiply#                  **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = Multiply#(y#, x#)
  ' PARAMETERS:      y#         First number to be processed
  '                  x#         Second number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Multiply# (y#, x#)
  '
    FUNCTION Multiply# (y#, x#) STATIC
        Multiply# = y# * x#
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          OneOverX#                  **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = OneOverX#(x#)
  ' PARAMETERS:      x#         Number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION OneOverX# (x#)
  '
    FUNCTION OneOverX# (x#) STATIC
        IF x# <> 0 THEN
            OneOverX# = 1# / x#
        ELSE
            PRINT "Error: 1/x where x = 0"
            SYSTEM
        END IF
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Sign#                      **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = Sign#(x#)
  ' PARAMETERS:      x#         Number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Sign# (x#)
  '
    FUNCTION Sign# (x#) STATIC
        Sign# = SGN(x#)
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Sine#                      **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = Sine#(x#)
  ' PARAMETERS:      x#         Angle, expressed in radians
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Sine# (x#)
  '
    FUNCTION Sine# (x#) STATIC
        Sine# = SIN(x#)
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          SquareRoot#                **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = SquareRoot#(x#)
  ' PARAMETERS:      x#         Number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION SquareRoot# (x#)
  '
    FUNCTION SquareRoot# (x#) STATIC
        IF x# >= 0 THEN
            SquareRoot# = SQR(x#)
        ELSE
            PRINT "Error: SQR(x#) where x# < 0"
            SYSTEM
        END IF
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Subtract#                  **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:   y# = Subtract#(y#, x#)
  ' PARAMETERS:       y#         Number to be processed
  '                   x#         Number to be processed
  ' VARIABLES:        (none)
  ' MODULE LEVEL
  '   DECLARATIONS:   DECLARE FUNCTION Subtract# (y#, x#)
  '
    FUNCTION Subtract# (y#, x#) STATIC
        Subtract# = y# - x#
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          SwapXY                     **
  ' **  Type:          Subprogram                 **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  SwapXY stack#(), ptr%
  ' PARAMETERS:      stack#()   Numeric stack
  '                  ptr%       Pointer to top of stack
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB SwapXY (stack#(), ptr%
  '
    SUB SwapXY (stack#(), ptr%) STATIC
        IF ptr% > 1 THEN
            SWAP stack#(ptr%), stack#(ptr% - 1)
        END IF
    END SUB
  
  ' ************************************************
  ' **  Name:          Tangent#                   **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = Tangent#(x#)
  ' PARAMETERS:      x#         Angle, expressed in radians
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Tangent# (x#)
  '
    FUNCTION Tangent# (x#) STATIC
        Tangent# = TAN(x#)
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Xsquared#                  **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  y# = Xsquared#(x#)
  ' PARAMETERS:      x#         Number to be processed
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Xsquared# (x#)
  '
    FUNCTION Xsquared# (x#) STATIC
        Xsquared# = x# * x#
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          YRaisedToX#                **
  ' **  Type:          Function                   **
  ' **  Module:        QCALMATH.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' EXAMPLE OF USE:  z# = YRaisedToX#(y#, x#)
  ' PARAMETERS:      y#         Number to be raised to a power
  '                  x#         Power to raise the other number to
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION YRaisedToX# (y#, x#)
  '
    FUNCTION YRaisedToX# (y#, x#) STATIC
        YRaisedToX# = y# ^ x#
    END FUNCTION
  

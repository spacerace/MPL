  ' ************************************************
  ' **  Name:          DOLLARS                    **
  ' **  Type:          Toolbox                    **
  ' **  Module:        DOLLARS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' USAGE:         No command line parameters
  ' .MAK FILE:     (none)
  ' PARAMETERS:    (none)
  ' VARIABLES:     n#         Number for demonstration of the functions
  
    DECLARE FUNCTION Comma$ (n#)
    DECLARE FUNCTION DollarString$ (amount#, length%)
    DECLARE FUNCTION Round# (n#, place%)
  
    CLS
    n# = 1234567.76543#
    PRINT "Number n#:", , n#
    PRINT "Comma$(n#)", , Comma$(n#)
    PRINT "Comma$(Round#(n#, -2))", Comma$(Round#(n#, -2))
    PRINT
    PRINT "DollarString$(n#, 20)", ":"; DollarString$(n#, 20); ":"
    PRINT , , " 12345678901234567890"
    PRINT
  
    PRINT "Round#(n#, -3)", Round#(n#, -3)
    PRINT "Round#(n#, -2)", Round#(n#, -2)
    PRINT "Round#(n#, -1)", Round#(n#, -1)
    PRINT "Round#(n#, 0)", , Round#(n#, 0)
    PRINT "Round#(n#, 1)", , Round#(n#, 1)
    PRINT "Round#(n#, 2)", , Round#(n#, 2)
  
  ' ************************************************
  ' **  Name:          Comma$                     **
  ' **  Type:          Function                   **
  ' **  Module:        DOLLARS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Creates a string representing a double-precision
  ' number, with commas inserted every three digits.
  '
  ' EXAMPLE OF USE:    n$  =  Comma$(n#)
  ' PARAMETERS:        n#     Number to be formatted
  ' VARIABLES:         tn$    Temporary string of the number
  '                    dp%    Position of the decimal point
  '                    i%     Index into tn$
  ' MODULE LEVEL
  '   DECLARATIONS:           DECLARE FUNCTION Comma$ (n#)
  '
    FUNCTION Comma$ (n#) STATIC
        tn$ = STR$(n#)
        dp% = INSTR(tn$, ".")
        IF dp% = 0 THEN
            dp% = LEN(tn$) + 1
        END IF
        IF dp% > 4 THEN
            FOR i% = dp% - 3 TO 3 STEP -3
                tn$ = LEFT$(tn$, i% - 1) + "," + MID$(tn$, i%)
            NEXT i%
        END IF
        Comma$ = LTRIM$(tn$)
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          DollarString$              **
  ' **  Type:          Function                   **
  ' **  Module:        DOLLARS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a string representation of a dollar amount,
  ' rounded to the nearest cent, with commas separating
  ' groups of three digits, and with a preceding dollar sign.
  '
  ' EXAMPLE OF USE:    d$ = DollarString$(dollars#)
  ' PARAMETERS:        dollars#   Amount of money
  ' VARIABLES:         tmp$       Temporary working string
  ' MODULE LEVEL
  '   DECLARATIONS:    DECLARE FUNCTION Comma$ (n#)
  '                    DECLARE FUNCTION DollarString$ (amount#, length%)
  '                    DECLARE FUNCTION Round# (n#, place%)
  '
    FUNCTION DollarString$ (amount#, length%) STATIC
        tmp$ = SPACE$(length%) + "$" + Comma$(Round#(amount#, -2))
        DollarString$ = RIGHT$(tmp$, length%)
        tmp$ = ""
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Round#                     **
  ' **  Type:          Function                   **
  ' **  Module:        DOLLARS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Rounds a number at the power of 10 decimal place.
  '
  ' EXAMPLE OF USE:  x# = Round#(n#, place%)
  ' EXAMPLES:        Round#(12.3456#, -2) = 12.35#
  '                  Round#(12.3456#, -1) = 12.3#
  '                  Round#(12.3456#, 0)  = 12#
  '                  Round#(12.3456#, 1)  = 10#
  ' PARAMETERS:      n#         Number to be rounded
  '                  place%     Power of 10 for rounding the number
  ' VARIABLES:       pTen#      10 raised to the indicated power of 10
  ' MODULE LEVEL
  '   DECLARATIONS:             DECLARE FUNCTION Round# (n#, place%)
  '
    FUNCTION Round# (n#, powerOfTen%) STATIC
        pTen# = 10# ^ powerOfTen%
        Round# = INT(n# / pTen# + .5#) * pTen#
    END FUNCTION
  

  ' ************************************************
  ' **  Name:          QCAL                       **
  ' **  Type:          Program                    **
  ' **  Module:        QCAL.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' USAGE:           QCAL [number] [function] [...]
  ' .MAK FILE:       QCAL.BAS
  '                  QCALMATH.BAS
  ' PARAMETERS:      [number]      Numbers to be placed on the stack
  '                  [function]    Operations to be performed on the stack
  '                                contents
  ' VARIABLES:       cmd$          Working copy of COMMAND$
  '                  stack#()      Array representing the numeric stack
  '                  ptr%          Index into the stack
  '                  parm$         Each number of command extracted from cmd$
  
  ' Constants
    CONST PI = 3.141592653589793#
  
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
    DECLARE FUNCTION NextParameter$ (cmd$)
    DECLARE FUNCTION OneOverX# (x#)
    DECLARE FUNCTION Sign# (x#)
    DECLARE FUNCTION Sine# (x#)
    DECLARE FUNCTION SquareRoot# (x#)
    DECLARE FUNCTION Subtract# (y#, x#)
    DECLARE FUNCTION Tangent# (x#)
    DECLARE FUNCTION Xsquared# (x#)
    DECLARE FUNCTION YRaisedToX# (y#, x#)
  
  ' Subprograms
    DECLARE SUB QcalHelp ()
    DECLARE SUB Process (parm$, stack#(), ptr%)
    DECLARE SUB DisplayStack (stack#(), ptr%)
    DECLARE SUB SwapXY (stack#(), ptr%)
  
  ' Get the command line
    cmd$ = COMMAND$
  
  ' First check if user is asking for help
    IF cmd$ = "" OR cmd$ = "HELP" OR cmd$ = "?" THEN
        QcalHelp
        SYSTEM
    END IF
  
  ' Create a pseudo stack
    DIM stack#(1 TO 20)
    ptr% = 0
  
  ' Process each part of the command line
    DO UNTIL cmd$ = ""
        parm$ = NextParameter$(cmd$)
        Process parm$, stack#(), ptr%
        IF ptr% < 1 THEN
            PRINT "Not enough stack values"
            SYSTEM
        END IF
    LOOP
  
  ' Display results
    DisplayStack stack#(), ptr%
  
  ' All done
    END

  ' ************************************************
  ' **  Name:          DisplayStack               **
  ' **  Type:          Subprogram                 **
  ' **  Module:        QCAL.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Displays the value(s) left on the stack when QCAL
  ' is finished processing the command line.
  '
  ' EXAMPLE OF USE:  DisplayStack stack#(), ptr%
  ' PARAMETERS:      stack#()   Array of numbers representing the stack
  '                  ptr%       Index into the stack
  ' VARIABLES:       i%         Looping index
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB DisplayStack (stack#(), ptr%)
  '
    SUB DisplayStack (stack#(), ptr%) STATIC
        PRINT
        IF ptr% > 1 THEN
            PRINT "Stack ... ",
        ELSE
            PRINT "Result... ",
        END IF
        FOR i% = 1 TO ptr%
            PRINT stack#(i%),
        NEXT i%
        PRINT
    END SUB

  ' ************************************************
  ' **  Name:          NextParameter$             **
  ' **  Type:          Function                   **
  ' **  Module:        QCAL.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Extracts parameters from the front of the
  ' command line.  Parameters are groups of any
  ' characters separated by spaces.
  '
  ' EXAMPLE OF USE:  parm$ = NextParameter$(cmd$)
  ' PARAMETERS:      cmd$       The working copy of COMMAND$
  ' VARIABLES:       parm$      Each number or command from cmd$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION NextParameter$ (cmd$)
  '
    FUNCTION NextParameter$ (cmd$) STATIC
        parm$ = ""
        DO WHILE LEFT$(cmd$, 1) <> " " AND cmd$ <> ""
            parm$ = parm$ + LEFT$(cmd$, 1)
            cmd$ = MID$(cmd$, 2)
        LOOP
        DO WHILE LEFT$(cmd$, 1) = " " AND cmd$ <> ""
            cmd$ = MID$(cmd$, 2)
        LOOP
        NextParameter$ = parm$
    END FUNCTION

  ' ************************************************
  ' **  Name:          Process                    **
  ' **  Type:          Subprogram                 **
  ' **  Module:        QCAL.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Processes each command parameter for the QCAL
  ' program.
  '
  ' EXAMPLE OF USE:  Process parm$, stack#(), ptr%
  ' PARAMETERS:      parm$      The command line parameter to be processed
  '                  stack#()   Array of numbers representing the stack
  '                  ptr%       Index pointing to last stack entry
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Process (parm$, stack#(), ptr%)
  '
    SUB Process (parm$, stack#(), ptr%) STATIC
        SELECT CASE parm$
        CASE "+"
            ptr% = ptr% - 1
            IF ptr% > 0 THEN
                stack#(ptr%) = Add#(stack#(ptr%), stack#(ptr% + 1))
            END IF
        CASE "-"
            ptr% = ptr% - 1
            IF ptr% > 0 THEN
                stack#(ptr%) = Subtract#(stack#(ptr%), stack#(ptr% + 1))
            END IF
        CASE "*"
            ptr% = ptr% - 1
            IF ptr% > 0 THEN
                stack#(ptr%) = Multiply#(stack#(ptr%), stack#(ptr% + 1))
            END IF
        CASE "/"
            ptr% = ptr% - 1
            IF ptr% > 0 THEN
                stack#(ptr%) = Divide#(stack#(ptr%), stack#(ptr% + 1))
            END IF
        CASE "CHS"
            IF ptr% > 0 THEN
                stack#(ptr%) = ChangeSign#(stack#(ptr%))
            END IF
        CASE "ABS"
            IF ptr% > 0 THEN
                stack#(ptr%) = AbsoluteX#(stack#(ptr%))
            END IF
        CASE "SGN"
            IF ptr% > 0 THEN
                stack#(ptr%) = Sign#(stack#(ptr%))
            END IF
        CASE "INT"
            IF ptr% > 0 THEN
                stack#(ptr%) = IntegerPart#(stack#(ptr%))
            END IF
        CASE "MOD"
            ptr% = ptr% - 1
            IF ptr% > 0 THEN
                stack#(ptr%) = Modulus#(stack#(ptr%), stack#(ptr% + 1))
            END IF
        CASE "FRC"
            IF ptr% > 0 THEN
                stack#(ptr%) = FractionalPart#(stack#(ptr%))
            END IF
        CASE "1/X"
            IF ptr% > 0 THEN
                stack#(ptr%) = OneOverX#(stack#(ptr%))
            END IF
        CASE "SQR"
            IF ptr% > 0 THEN
                stack#(ptr%) = SquareRoot#(stack#(ptr%))
            END IF
        CASE "X2"
            IF ptr% > 0 THEN
                stack#(ptr%) = Xsquared#(stack#(ptr%))
            END IF
        CASE "SIN"
            IF ptr% > 0 THEN
                stack#(ptr%) = Sine#(stack#(ptr%))
            END IF
        CASE "COS"
            IF ptr% > 0 THEN
                stack#(ptr%) = Cosine#(stack#(ptr%))
            END IF
        CASE "TAN"
            IF ptr% > 0 THEN
                stack#(ptr%) = Tangent#(stack#(ptr%))
            END IF
        CASE "ASN"
            IF ptr% > 0 THEN
                stack#(ptr%) = ArcSine#(stack#(ptr%))
            END IF
        CASE "ACS"
            IF ptr% > 0 THEN
                stack#(ptr%) = ArcCosine#(stack#(ptr%))
            END IF
        CASE "ATN"
            IF ptr% > 0 THEN
                stack#(ptr%) = ArcTangent#(stack#(ptr%))
            END IF
        CASE "HSN"
            IF ptr% > 0 THEN
                stack#(ptr%) = HypSine#(stack#(ptr%))
            END IF
        CASE "HCS"
            IF ptr% > 0 THEN
                stack#(ptr%) = HypCosine#(stack#(ptr%))
            END IF
        CASE "HTN"
            IF ptr% > 0 THEN
                stack#(ptr%) = HypTangent#(stack#(ptr%))
            END IF
        CASE "AHS"
            IF ptr% > 0 THEN
                stack#(ptr%) = ArcHypSine#(stack#(ptr%))
            END IF
        CASE "AHC"
            IF ptr% > 0 THEN
                stack#(ptr%) = ArcHypCosine#(stack#(ptr%))
            END IF
        CASE "AHT"
            IF ptr% > 0 THEN
                stack#(ptr%) = ArcHypTangent#(stack#(ptr%))
            END IF
        CASE "LOG"
            IF ptr% > 0 THEN
                stack#(ptr%) = LogE#(stack#(ptr%))
            END IF
        CASE "LOG10"
            IF ptr% > 0 THEN
                stack#(ptr%) = LogBase10#(stack#(ptr%))
            END IF
        CASE "LOGN"
            ptr% = ptr% - 1
            IF ptr% > 0 THEN
                stack#(ptr%) = LogBaseN#(stack#(ptr%), stack#(ptr% + 1))
            END IF
        CASE "EXP"
            IF ptr% > 0 THEN
                stack#(ptr%) = Exponential#(stack#(ptr%))
            END IF
        CASE "CEIL"
            IF ptr% > 0 THEN
                stack#(ptr%) = Ceil#(stack#(ptr%))
            END IF
        CASE "Y^X"
            ptr% = ptr% - 1
            IF ptr% > 0 THEN
                stack#(ptr%) = YRaisedToX#(stack#(ptr%), stack#(ptr% + 1))
            END IF
        CASE "PI"
            ptr% = ptr% + 1
            stack#(ptr%) = PI
        CASE "SWAP"
            SwapXY stack#(), ptr%
        CASE "DUP"
            IF ptr% > 0 THEN
                stack#(ptr% + 1) = stack#(ptr%)
                ptr% = ptr% + 1
            END IF
        CASE ELSE
            ptr% = ptr% + 1
            stack#(ptr%) = VAL(parm$)
        END SELECT
    END SUB

  ' ************************************************
  ' **  Name:          QcalHelp                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        QCAL.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Displays a help screen when QCAL is run with no
  ' parameters or with a parameter of ? or HELP.
  '
  ' EXAMPLE OF USE:  QcalHelp
  ' PARAMETERS:      (none)
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB QcalHelp ()
  '
    SUB QcalHelp STATIC
        PRINT
        PRINT "Usage:  QCAL [number] [function] [...] <Enter>"
        PRINT
        PRINT "Numbers are placed on an RPN stack, and functions operate"
        PRINT "on the stacked quantities.  When the program is finished,"
        PRINT "whatever is left on the stack is displayed."
        PRINT
        PRINT "List of available functions..."
        PRINT
        PRINT "Two numbers:     +  -  *  /"
        PRINT "One number:      CHS ABS SGN INT MOD FRC CHS 1/X SQR X2 CEIL"
        PRINT "Trigonometric:   SIN COS TAN ASN ACS ATN"
        PRINT "Hyperbolic:      HSN HCS HTN AHS AHC AHT"
        PRINT "Logarithmic:     LOG LOG10 LOGN EXP Y^X"
        PRINT "Constants:       PI"
        PRINT "Stack:           SWAP DUP"
    END SUB


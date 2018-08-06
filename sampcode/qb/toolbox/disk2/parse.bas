  ' ************************************************
  ' **  Name:          PARSE                      **
  ' **  Type:          Toolbox                    **
  ' **  Module:        PARSE.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' USAGE:           No command line parameters
  ' .MAK FILE:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       a$()       Array of words parsed from x$
  '                  x$         String to be parsed
  '                  sep$       Characters defining word separation
  '                  word$      Each word from the string
  '                  n%         Index to each word in array
  
    DECLARE SUB ParseLine (x$, sep$, a$())
    DECLARE SUB ParseWord (a$, sep$, word$)
  
  ' Initialization
    CLS
    DIM a$(1 TO 99)
  
  ' Demonstrate ParseWord
    x$ = "This is a test line. A,B,C, etc."
    sep$ = " ,"
    PRINT "x$:", x$
    PRINT "sep$:", CHR$(34); sep$; CHR$(34)
    ParseWord x$, sep$, word$
    PRINT "ParseWord x$, sep$, word$"
    PRINT "x$:", x$
    PRINT "word$:", word$
  
  ' Demonstrate ParseLine
    PRINT
    x$ = "This is a test line. A,B,C, etc."
    sep$ = " ,"
    PRINT "x$:", x$
    PRINT "sep$:", CHR$(34); sep$; CHR$(34)
    ParseLine x$, sep$, a$()
    PRINT "ParseLine x$, sep$, a$()"
    PRINT "a$()..."
    DO
        n% = n% + 1
        PRINT n%, a$(n%)
    LOOP UNTIL a$(n% + 1) = ""
  
  ' All done
    END
  

  ' ************************************************
  ' **  Name:          ParseLine                  **
  ' **  Type:          Subprogram                 **
  ' **  Module:        PARSE.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Breaks a string into an array of words, as defined
  ' by any characters listed in sep$.
  '
  ' EXAMPLE OF USE:  ParseLine x$, sep$, a$()
  ' PARAMETERS:      x$      String to be parsed
  '                  sep$    List of characters defined as word separators
  '                  a$()    Returned array of words
  ' VARIABLES:       t$      Temporary work string
  '                  i%      Index to array entries
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB ParseLine (x$, sep$, a$())
  '
    SUB ParseLine (x$, sep$, a$()) STATIC
        t$ = x$
        FOR i% = LBOUND(a$) TO UBOUND(a$)
            ParseWord t$, sep$, a$(i%)
            IF a$(i%) = "" THEN
                EXIT FOR
            END IF
        NEXT i%
        t$ = ""
    END SUB

  ' ************************************************
  ' **  Name:          ParseWord                  **
  ' **  Type:          Subprogram                 **
  ' **  Module:        PARSE.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Breaks off the first word in a$, as delimited by
  ' any characters listed in sep$.
  '
  ' EXAMPLE OF USE:  ParseWord a$, sep$, word$
  ' PARAMETERS:      a$         String to be parsed
  '                  sep$       List of characters defined as word separators
  '                  word$      Returned first word parsed from a$
  ' VARIABLES:       lena%      Length of a$
  '                  i%         Looping index
  '                  j%         Looping index
  '                  k%         Looping index
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB ParseWord (a$, sep$, word$)
  '
    SUB ParseWord (a$, sep$, word$) STATIC
        word$ = ""
        lena% = LEN(a$)
        IF a$ = "" THEN
            EXIT SUB
        END IF
        FOR i% = 1 TO lena%
            IF INSTR(sep$, MID$(a$, i%, 1)) = 0 THEN
                EXIT FOR
            END IF
        NEXT i%
        FOR j% = i% TO lena%
            IF INSTR(sep$, MID$(a$, j%, 1)) THEN
                EXIT FOR
            END IF
        NEXT j%
        FOR k% = j% TO lena%
            IF INSTR(sep$, MID$(a$, k%, 1)) = 0 THEN
                EXIT FOR
            END IF
        NEXT k%
        IF i% > lena% THEN
            a$ = ""
            EXIT SUB
        END IF
        IF j% > lena% THEN
            word$ = MID$(a$, i%)
            a$ = ""
            EXIT SUB
        END IF
        word$ = MID$(a$, i%, j% - i%)
        IF k% > lena% THEN
            a$ = ""
        ELSE
            a$ = MID$(a$, k%)
        END IF
    END SUB


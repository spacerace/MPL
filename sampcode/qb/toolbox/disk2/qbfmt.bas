  ' ************************************************
  ' **  Name:          QBFMT                      **
  ' **  Type:          Program                    **
  ' **  Module:        QBFMT.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Reformats a QuickBASIC program by indenting
  ' lines according to the structure of the statements.  The
  ' default amount is 4 spaces if no indention parameter
  ' is given on the command line.
  '
  ' USAGE:  QBFMT filename [indention]
  '         Command$ = filename [indention]
  ' .MAK FILE:   QBFMT.BAS
  '              PARSE.BAS
  '              STRINGS.BAS
  ' PARAMETERS:  filename(.BAS)   Name of QuickBASIC module to be formatted;
  '                               the module must be saved in "Text" format
  ' VARIABLES:   md$              Working copy of COMMAND$ contents
  '              fileName$        Name of QuickBASIC module to be formatted
  '              dpoint%          Position of the decimal point character
  '                               in cmd$
  '              ndent$           Part of cmd$ dealing with optional
  '                               indention amount
  '              indention%       Number of character columns per
  '                               indention level
  '              progline$        Each line of the file being processed
  '              indentLevel%     Keeps track of current indention amount
  '              nest$            Message placed in file if faulty structure
  '                               detected
  
    DECLARE FUNCTION LtrimSet$ (a$, set$)
    DECLARE FUNCTION RtrimSet$ (a$, set$)
    DECLARE SUB Indent (a$, indention%, indentLevel%)
    DECLARE SUB ParseWord (a$, sep$, word$)
    DECLARE SUB SetCode (a$, keyWord$, code%)
    DECLARE SUB SplitUp (a$, comment$, keyWord$)
  
  ' Decipher the user command line
    cmd$ = COMMAND$
    IF cmd$ = "" THEN
        PRINT
        PRINT "Usage:  QBFMT filename(.BAS) [indention]"
        SYSTEM
    ELSE
        ParseWord cmd$, " ,", fileName$
        dpoint% = INSTR(fileName$, ".")
        IF dpoint% THEN
            fileName$ = LEFT$(fileName$, dpoint% - 1)
        END IF
        ParseWord cmd$, " ,", ndent$
        indention% = VAL(ndent$)
        IF indention% < 1 THEN
            indention% = 4
        END IF
    END IF
  
  ' Try to open the indicated files
    PRINT
    ON ERROR GOTO ErrorTrapOne
    OPEN fileName$ + ".BAS" FOR INPUT AS #1
    OPEN fileName$ + ".@$@" FOR OUTPUT AS #2
    ON ERROR GOTO 0
  
  ' Process each line of the file
    DO
        LINE INPUT #1, progLine$
        Indent progLine$, indention%, indentLevel%
        PRINT progLine$
        PRINT #2, progLine$
        IF indentLevel% < 0 OR (EOF(1) AND indentLevel% <> 0) THEN
            SOUND 555, 5
            SOUND 333, 9
            nest$ = "'<<<<<<<<<<<<<<<<<<<<< Nesting error detected!"
            PRINT nest$
            PRINT #2, nest$
            indentLevel% = 0
        END IF
    LOOP UNTIL EOF(1)
  
  ' Close all files
    CLOSE
  
  ' Delete any old .BAK file
    ON ERROR GOTO ErrorTrapTwo
    KILL fileName$ + ".BAK"
    ON ERROR GOTO 0
  
  ' Rename the files
    NAME fileName$ + ".BAS" AS fileName$ + ".BAK"
    NAME fileName$ + ".@$@" AS fileName$ + ".BAS"
  
  ' We're done
    END
  
  '----------- Error trapping routines
  
ErrorTrapOne:
    PRINT "Error while opening files"
    SYSTEM
  
ErrorTrapTwo:
    RESUME NEXT
  
  ' ************************************************
  ' **  Name:          Indent                     **
  ' **  Type:          Subprogram                 **
  ' **  Module:        QBFMT.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Determines the indention for each line.
  '
  ' EXAMPLE OF USE:  Indent a$, indention%, indentLevel%
  ' PARAMETERS:      a$             Program line to be indented
  '                  indention%     Spaces to add for each indention level
  '                  indentLevel%   Level of indention
  ' VARIABLES:       comment$       Part of program line that represents a
  '                                 REMARK
  '                  keyWord$       First word of the program line
  '                  code%          Indention control code determined by
  '                                 keyWord$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Indent (a$, indention%, indentLevel%)
  '
    SUB Indent (a$, indention%, indentLevel%) STATIC
      
      ' Break line into manageable parts
        SplitUp a$, comment$, keyWord$
      
        IF keyWord$ <> "" THEN
          
          ' Set indention code according to type of keyword
            SetCode a$, keyWord$, code%
          
          ' Build a string of spaces for the indicated indention
            SELECT CASE code%
            CASE -2
                a$ = SPACE$(indention% * indentLevel%) + a$
            CASE -1
                a$ = SPACE$(indention% * indentLevel%) + a$
                indentLevel% = indentLevel% - 1
            CASE 0
                a$ = SPACE$(indention% * (indentLevel% + 1)) + a$
            CASE 1
                indentLevel% = indentLevel% + 1
                a$ = SPACE$(indention% * indentLevel%) + a$
            CASE ELSE
            END SELECT
        ELSE
            a$ = SPACE$(indention% * indentLevel% + 2)
        END IF
      
      ' Round out the position of trailing comments
        IF comment$ <> "" THEN
            IF a$ <> SPACE$(LEN(a$)) AND a$ <> "" THEN
                a$ = a$ + SPACE$(16 - (LEN(a$) MOD 16))
            END IF
        END IF
      
      ' Tack the comment back onto the end of the line
        a$ = a$ + comment$
      
    END SUB
  
  ' ************************************************
  ' **  Name:          SetCode                    **
  ' **  Type:          Subprogram                 **
  ' **  Module:        QBFMT.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Determines a code number for the type of indention
  ' implied by the various types of keywords that begin
  ' each line of QuickBASIC programs.
  '
  ' EXAMPLE OF USE:   SetCode a$, keyWord$, code%
  ' PARAMETERS:       a$         Program line to indent
  '                   keyWord$   First word of the program line
  '                   code%      Returned code indicating the action to be taken
  ' VARIABLES:        (none)
  ' MODULE LEVEL
  '   DECLARATIONS:   DECLARE SUB SetCode (a$, keyWord$, code%)
  '
    SUB SetCode (a$, keyWord$, code%) STATIC
        SELECT CASE keyWord$
        CASE "DEF"
            IF INSTR(a$, "=") THEN
                code% = 0
            ELSE
                IF INSTR(a$, " SEG") = 0 THEN
                    code% = 1
                END IF
            END IF
        CASE "ELSE"
            code% = -2
        CASE "ELSEIF"
            code% = -2
        CASE "CASE"
            code% = -2
        CASE "END"
            IF a$ <> "END" THEN
                code% = -1
            ELSE
                code% = 0
            END IF
        CASE "FOR"
            code% = 1
        CASE "DO"
            code% = 1
        CASE "SELECT"
            code% = 1
        CASE "IF"
            IF RIGHT$(a$, 4) = "THEN" THEN
                code% = 1
            ELSE
                code% = 0
            END IF
        CASE "NEXT"
            code% = -1
        CASE "LOOP"
            code% = -1
        CASE "SUB"
            code% = 1
        CASE "FUNCTION"
            code% = 1
        CASE "TYPE"
            code% = 1
        CASE "WHILE"
            code% = 1
        CASE "WEND"
            code% = -1
        CASE ELSE
            code% = 0
        END SELECT
    END SUB
  
  ' ************************************************
  ' **  Name:          SplitUp                    **
  ' **  Type:          Subprogram                 **
  ' **  Module:        QBFMT.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Splits the line into statement, comment, and keyword.
  '
  ' EXAMPLE OF USE:  SplitUp a$, comment$, keyWord$
  ' PARAMETERS:      a$         Program line to be split up
  '                  comment$   Part of line following "REM" or "'"
  '                  keyWord$   First word of program line
  ' VARIABLES:       set$       Characters to be trimmed, space and tab
  '                  strFlag%   Indication of a quoted string
  '                  k%         Index to start of REMARK
  '                  i%         Looping index
  '                  m%         Pointer to REMARK
  '                  sptr%      Pointer to first space following the
  '                             first word in a$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB SplitUp (a$, comment$, keyWord$)
  '
    SUB SplitUp (a$, comment$, keyWord$) STATIC
        set$ = " " + CHR$(9)
        strFlag% = 0
        k% = 0
        FOR i% = LEN(a$) TO 1 STEP -1
            IF MID$(a$, i%, 1) = CHR$(34) THEN
                IF strFlag% = 0 THEN
                    strFlag% = 1
                ELSE
                    strFlag% = 0
                END IF
            END IF
            IF MID$(a$, i%, 1) = "'" OR MID$(a$, i%, 3) = "REM" THEN
                IF strFlag% = 0 THEN
                    k% = i%
                END IF
            END IF
        NEXT i%
        IF k% > 0 THEN
            m% = 0
            FOR j% = k% - 1 TO 1 STEP -1
                IF INSTR(set$, MID$(a$, j%, 1)) = 0 THEN
                    IF m% = 0 THEN m% = j%
                END IF
            NEXT j%
            IF m% THEN
                comment$ = MID$(a$, m% + 1)
                a$ = LEFT$(a$, m%)
            ELSE
                comment$ = a$
                a$ = ""
            END IF
        ELSE
            comment$ = ""
        END IF
        a$ = LtrimSet$(a$, set$)
        a$ = RtrimSet$(a$, set$)
        comment$ = LtrimSet$(comment$, set$)
        comment$ = RtrimSet$(comment$, set$)
        sptr% = INSTR(a$, " ")
        IF sptr% THEN
            keyWord$ = UCASE$(LEFT$(a$, sptr% - 1))
        ELSE
            keyWord$ = UCASE$(a$)
        END IF
    END SUB
  

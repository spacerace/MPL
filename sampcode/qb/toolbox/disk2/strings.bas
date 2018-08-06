  ' ************************************************
  ' **  Name:          STRINGS                    **
  ' **  Type:          Toolbox                    **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  ' USAGE:           No command line parameters
  ' .MAK FILE:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       a$       Working string for demonstrations
  '                  b$       Working string for demonstrations
  '                  c$       Working string for demonstrations
  '                  x$       Working string for demonstrations
  '                  y$       Working string for demonstrations
  '                  set$     Set of characters that define word separations
  
    DECLARE FUNCTION Ascii2Ebcdic$ (a$)
    DECLARE FUNCTION BestMatch$ (a$, x$, y$)
    DECLARE FUNCTION Center$ (a$, n%)
    DECLARE FUNCTION Detab$ (a$, tabs%)
    DECLARE FUNCTION Ebcdic2Ascii$ (e$)
    DECLARE FUNCTION Entab$ (a$, tabs%)
    DECLARE FUNCTION FilterIn$ (a$, set$)
    DECLARE FUNCTION FilterOut$ (a$, set$)
    DECLARE FUNCTION Lpad$ (a$, n%)
    DECLARE FUNCTION LtrimSet$ (a$, set$)
    DECLARE FUNCTION Ord% (a$)
    DECLARE FUNCTION Repeat$ (a$, n%)
    DECLARE FUNCTION Replace$ (a$, find$, substitute$)
    DECLARE FUNCTION Reverse$ (a$)
    DECLARE FUNCTION ReverseCase$ (a$)
    DECLARE FUNCTION Rpad$ (a$, n%)
    DECLARE FUNCTION RtrimSet$ (a$, set$)
    DECLARE FUNCTION Translate$ (a$, f$, t$)
  
  ' Subprograms
    DECLARE SUB BuildAEStrings ()
  
  ' Quick demonstrations
    CLS
    a$ = "This is a test"
    PRINT "a$", , a$
    PRINT "ReverseCase$(a$)", ReverseCase$(a$)
    PRINT "Reverse$(a$)", , Reverse$(a$)
    PRINT "Repeat$(a$, 3)", Repeat$(a$, 3)
    PRINT
  
    set$ = "T this"
    PRINT "set$", , set$
    PRINT "LtrimSet$(a$, set$)", LtrimSet$(a$, set$)
    PRINT "RtrimSet$(a$, set$)", RtrimSet$(a$, set$)
    PRINT "FilterOut$(a$, set$)", FilterOut$(a$, set$)
    PRINT "FilterIn$(a$, set$)", FilterIn$(a$, set$)
    PRINT
  
    a$ = "elephant"
    x$ = "alpha"
    y$ = "omega"
    PRINT "a$", , a$
    PRINT "x$", , x$
    PRINT "y$", , y$
    PRINT "BestMatch$(a$, x$, y$)", BestMatch$(a$, x$, y$)
    PRINT
  
    PRINT "Press any key to continue"
    DO
    LOOP UNTIL INKEY$ <> ""
  
    CLS
    a$ = "BEL"
    PRINT "a$", , a$
    PRINT "Ord%(a$)", , Ord%(a$)
    PRINT
  
    a$ = "This is a test"
    find$ = "s"
    substitute$ = "<s>"
    PRINT "a$", , , a$
    PRINT "find$", , , find$
    PRINT "substitute$", , , substitute$
    PRINT "Replace$(a$, find$, substitute$)", Replace$(a$, find$, substitute$)
    PRINT
  
    PRINT "a$", , a$
    PRINT "Lpad$(a$, 40)", , ":"; Lpad$(a$, 40); ":"
    PRINT "Rpad$(a$, 40)", , ":"; Rpad$(a$, 40); ":"
    PRINT "Center$(a$, 40)", ":"; Center$(a$, 40); ":"
    PRINT
  
    a$ = "a$ character" + STRING$(2, 9) + "count" + CHR$(9) + "is"
    PRINT a$; LEN(a$)
    PRINT "a$ = Detab$(a$, 8)"
    a$ = Detab$(a$, 8)
    PRINT a$; LEN(a$)
    PRINT "a$ = Entab$(a$, 8)"
    a$ = Entab$(a$, 8)
    PRINT a$; LEN(a$)
    PRINT
  
    PRINT "Press any key to continue"
    DO
    LOOP UNTIL INKEY$ <> ""
  
    CLS
    a$ = "You know this test string has vowels."
    x$ = "aeiou"
    y$ = "eioua"
    PRINT "a$", , a$
    PRINT "x$", , x$
    PRINT "y$", , y$
    PRINT "Translate$(a$, x$, y$)", Translate$(a$, x$, y$)
    PRINT
  
    a$ = "This is a test."
    b$ = Ascii2Ebcdic$(a$)
    c$ = Ebcdic2Ascii$(b$)
    PRINT "a$", , a$
    PRINT "b$ = Ascii2Ebcdic$(a$)", b$
    PRINT "c$ = Ebcdic2Ascii$(b$)", c$
    PRINT
  
    END
  

  ' ************************************************
  ' **  Name:          Ascii2Ebcdic$              **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a$ with each character translated from ASCII to EBCDIC.
  '
  ' EXAMPLE OF USE:  e$ = Ascii2Ebcdic$(a$)
  ' PARAMETERS:      a$         String of ASCII characters to be
  '                             converted
  ' VARIABLES:       ebcdic$    Table of translation characters
  '                  ascii$     Table of translation characters
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Ascii2Ebcdic$ (a$)
  '
    FUNCTION Ascii2Ebcdic$ (a$) STATIC
        SHARED ebcdic$, ascii$
        IF ebcdic$ = "" THEN
            BuildAEStrings
        END IF
        Ascii2Ebcdic$ = Translate$(a$, ascii$, ebcdic$)
    END FUNCTION

  ' ************************************************
  ' **  Name:          BestMatch$                 **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns either x$ or y$, whichever is a best match to a$.
  '
  ' EXAMPLE OF USE:  b$ = BestMatch$(a$, x$, y$)
  ' PARAMETERS:      a$          The string to be matched
  '                  x$          The first string to compare with a$
  '                  y$          The second string to compare with a$
  ' VARIABLES:       ua$         Uppercase working copy of a$
  '                  ux$         Uppercase working copy of x$
  '                  uy$         Uppercase working copy of y$
  '                  lena%       Length of a$
  '                  i%          Length of substrings of ua$
  '                  j%          Index into ua$
  '                  t$          Substrings of ua$
  '                  xscore%     Accumulated score for substring matches
  '                              found in ux$
  '                  yscore%     Accumulated score for substring matches
  '                              found in uy$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION BestMatch$ (a$, x$, y$)
  '
    FUNCTION BestMatch$ (a$, x$, y$) STATIC
        ua$ = UCASE$(a$)
        ux$ = UCASE$(x$)
        uy$ = UCASE$(y$)
        lena% = LEN(ua$)
        FOR i% = 1 TO lena%
            FOR j% = 1 TO lena% - i% + 1
                t$ = MID$(ua$, j%, i%)
                IF INSTR(ux$, t$) THEN
                    xscore% = xscore% + i% + i%
                END IF
                IF INSTR(uy$, t$) THEN
                    yscore% = yscore% + i% + i%
                END IF
            NEXT j%
        NEXT i%
        IF xscore% > yscore% THEN
            BestMatch$ = x$
        ELSE
            BestMatch$ = y$
        END IF
    END FUNCTION

  ' ************************************************
  ' **  Name:          BuildAEStrings             **
  ' **  Type:          Subprogram                 **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Called by the Ascii2Ebcdic$ and Ebcdic2Ascii$
  ' functions to build the translation strings.
  ' This subprogram is called only once.
  '
  ' EXAMPLE OF USE:  Called automatically by either the Ascii2Ebcdic$ or
  '                  Ebcdic2Ascii$ function
  ' PARAMETERS:      ascii$     Shared by Ascii2Ebcdic$, Ebcdic2Ascii$, and
  '                             BuildAEStrings
  '                  ebcdic$    Shared by Ascii2Ebcdic$, Ebcdic2Ascii$, and
  '                             BuildAEStrings
  ' VARIABLES:       i%         Index into strings
  '                  byte%      Binary value of character byte
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB BuildAEStrings ()
  '
    SUB BuildAEStrings STATIC
        SHARED ebcdic$, ascii$
        ascii$ = SPACE$(128)
        ebcdic$ = ebcdic$ + "00010203372D2E2F1605250B0C0D0E0F"
        ebcdic$ = ebcdic$ + "101112133C3D322618193F271C1D1E1F"
        ebcdic$ = ebcdic$ + "404F7F7B5B6C507D4D5D5C4E6B604B61"
        ebcdic$ = ebcdic$ + "F0F1F2F3F4F5F6F7F8F97A5E4C7E6E6F"
        ebcdic$ = ebcdic$ + "7CC1C2C3C4C5C6C7C8C9D1D2D3D4D5D6"
        ebcdic$ = ebcdic$ + "D7D8D9E2E3E4E5E6E7E8E94AE05A5F6D"
        ebcdic$ = ebcdic$ + "79818283848586878889919293949596"
        ebcdic$ = ebcdic$ + "979899A2A3A4A5A6A7A8A9C06AD0A107"
        FOR i% = 0 TO 127
            MID$(ascii$, i% + 1, 1) = CHR$(i%)
            byte% = VAL("&H" + MID$(ebcdic$, i% + i% + 1, 2))
            MID$(ebcdic$, i% + 1, 1) = CHR$(byte%)
        NEXT i%
        ebcdic$ = LEFT$(ebcdic$, 128)
    END SUB

  ' ************************************************
  ' **  Name:          Center$                    **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Pads a$ with spaces on both ends until text is
  ' centered and the string length is n%.
  '
  ' EXAMPLE OF USE:  b$ = Center$(a$, n%)
  ' PARAMETERS:      a$         String of characters to be padded with spaces
  '                  n%         Desired length of resulting string
  ' VARIABLES:       pad%       Number of spaces to pad at ends of string
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Center$ (a$, n%)
  '
    FUNCTION Center$ (a$, n%) STATIC
        a$ = LTRIM$(RTRIM$(a$))
        pad% = n% - LEN(a$)
        IF pad% > 0 THEN
            Center$ = SPACE$(pad% \ 2) + a$ + SPACE$(pad% - pad% \ 2)
        ELSE
            Center$ = a$
        END IF
    END FUNCTION

  ' ************************************************
  ' **  Name           Detab$                     **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Replaces all tab characters with spaces, using
  ' tabs% to determine proper alignment.
  '
  ' EXAMPLE OF USE:  b$ = Detab$(a$, tabs%)
  ' PARAMETERS:      a$           String with possible tab characters
  '                  tabs%        Tab spacing
  ' VARIABLES:       t$           Working copy of a$
  '                  tb$          Tab character
  '                  tp%          Pointer to position in t$ of a tab character
  '                  sp$          Spaces to replace a given tab character
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Detab$ (a$, tabs%)
  '
    FUNCTION Detab$ (a$, tabs%) STATIC
        t$ = a$
        tb$ = CHR$(9)
        DO
            tp% = INSTR(t$, tb$)
            IF tp% THEN
                Sp$ = SPACE$(tabs% - ((tp% - 1) MOD tabs%))
                t$ = LEFT$(t$, tp% - 1) + Sp$ + MID$(t$, tp% + 1)
            END IF
        LOOP UNTIL tp% = 0
        Detab$ = t$
        t$ = ""
    END FUNCTION

  ' ************************************************
  ' **  Name:          Ebcdic2Ascii$              **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a$ with each character translated from
  ' EBCDIC to ASCII.
  '
  ' EXAMPLE OF USE:  b$ = Ascii2Ebcdic$(a$)
  ' PARAMETERS:      a$          String of EBCDIC characters to be converted
  ' VARIABLES:       ebcdic$     Table of translation characters
  '                  ascii$      Table of translation characters
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Ebcdic2Ascii$ (e$)
  '
    FUNCTION Ebcdic2Ascii$ (e$) STATIC
        SHARED ebcdic$, ascii$
        IF ebcdic$ = "" THEN
            BuildAEStrings
        END IF
        Ebcdic2Ascii$ = Translate$(e$, ebcdic$, ascii$)
    END FUNCTION

  ' ************************************************
  ' **  Name:          Entab$                     **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Replaces groups of spaces, where possible, with
  ' tab characters, keeping the alignment indicated
  ' by the value of tabs%.
  '
  ' EXAMPLE OF USE:  b$ = Entab$(a$, tabs%)
  ' PARAMETERS:      a$            String with possible tab characters
  '                  tabs%         Tab spacing
  ' VARIABLES:       t$            Working copy of a$
  '                  tb$           Tab character
  '                  i%            Index into t$
  '                  k%            Count of spaces being replaced
  '                  j%            Index into t$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Entab$ (a$, tabs%)
  '
    FUNCTION Entab$ (a$, tabs%) STATIC
        t$ = a$
        tb$ = CHR$(9)
        FOR i% = (LEN(t$) \ tabs%) * tabs% + 1 TO tabs% STEP -tabs%
            IF MID$(t$, i% - 1, 1) = " " THEN
                k% = 0
                FOR j% = 1 TO tabs%
                    IF MID$(t$, i% - j%, 1) <> " " THEN
                        k% = i% - j%
                        EXIT FOR
                    END IF
                NEXT j%
                IF k% = 0 THEN
                    k% = i% - tabs% - 1
                END IF
                t$ = LEFT$(t$, k%) + tb$ + MID$(t$, i%)
            END IF
        NEXT i%
        Entab$ = t$
        t$ = ""
    END FUNCTION

  ' ************************************************
  ' **  Name:          FilterIn$                  **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a$ with all occurrences of any characters
  ' that are not in set$ deleted.
  '
  ' EXAMPLE OF USE:  b$ = FilterIn$(a$, set$)
  ' PARAMETERS:      a$          String to be processed
  '                  set$        Set of characters to be retained
  ' VARIABLES:       i%          Index into a$
  '                  j%          Count of characters retained
  '                  lena%       Length of a$
  '                  t$          Working string space
  '                  c$          Each character of a$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION FilterIn$ (a$, set$)
  '
    FUNCTION FilterIn$ (a$, set$) STATIC
        i% = 1
        j% = 0
        lena% = LEN(a$)
        t$ = a$
        DO UNTIL i% > lena%
            c$ = MID$(a$, i%, 1)
            IF INSTR(set$, c$) THEN
                j% = j% + 1
                MID$(t$, j%, 1) = c$
            END IF
            i% = i% + 1
        LOOP
        FilterIn$ = LEFT$(t$, j%)
        t$ = ""
    END FUNCTION

  ' ************************************************
  ' **  Name:          FilterOut$                 **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a$ with all occurrences of any characters
  ' from set$ deleted.
  '
  ' EXAMPLE OF USE:  b$ = FilterOut$(a$, set$)
  ' PARAMETERS:      a$           String to be processed
  '                  set$         Set of characters to be retained
  ' VARIABLES:       i%           Index into a$
  '                  j%           Count of characters retained
  '                  lena%        Length of a$
  '                  t$           Working string space
  '                  c$           Each character of a$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION FilterOut$ (a$, set$)
  '
    FUNCTION FilterOut$ (a$, set$) STATIC
        i% = 1
        j% = 0
        lena% = LEN(a$)
        t$ = a$
        DO UNTIL i% > lena%
            c$ = MID$(a$, i%, 1)
            IF INSTR(set$, c$) = 0 THEN
                j% = j% + 1
                MID$(t$, j%, 1) = c$
            END IF
            i% = i% + 1
        LOOP
        FilterOut$ = LEFT$(t$, j%)
        t$ = ""
    END FUNCTION

  ' ************************************************
  ' **  Name:          Lpad$                      **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a string of length n%, with a$ left justified
  ' and padded on the right with spaces.
  '
  ' EXAMPLE OF USE:  b$ = Lpad$(a$, n%)
  ' PARAMETERS:      a$          String to be left justified and padded
  '                  n%          Length of string result
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Lpad$ (a$, n%)
  '
    FUNCTION Lpad$ (a$, n%) STATIC
        Lpad$ = LEFT$(LTRIM$(a$) + SPACE$(n%), n%)
    END FUNCTION

  ' ************************************************
  ' **  Name:          LtrimSet$                  **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Trims occurrences of any characters in set$
  ' from the left of a$.
  '
  ' EXAMPLE OF USE:  b$ = LtrimSet$(a$, set$)
  ' PARAMETERS:      a$           String to be trimmed
  '                  set$         Set of characters to be trimmed
  ' VARIABLES:       i%           Index into a$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION LtrimSet$ (a$, set$)
  '
    FUNCTION LtrimSet$ (a$, set$) STATIC
        IF a$ <> "" THEN
            FOR i% = 1 TO LEN(a$)
                IF INSTR(set$, MID$(a$, i%, 1)) = 0 THEN
                    LtrimSet$ = MID$(a$, i%)
                    EXIT FUNCTION
                END IF
            NEXT i%
        END IF
        LtrimSet$ = ""
    END FUNCTION

  ' ************************************************
  ' **  Name:          Ord%                       **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Similar to ASC() function; returns
  ' numeric byte values for the ANSI standard
  ' mnemonics for control characters.
  '
  ' EXAMPLE OF USE:  byte% = Ord%(a$)
  ' PARAMETERS:      a$          ANSI standard character mnemonic string
  ' VARIABLES:      (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Ord% (a$)
  '
    FUNCTION Ord% (a$) STATIC
        SELECT CASE UCASE$(a$)
        CASE "NUL"              'Null
            Ord% = 0
        CASE "SOH"              'Start of heading
            Ord% = 1
        CASE "STX"              'Start of text
            Ord% = 2
        CASE "ETX"              'End of text
            Ord% = 3
        CASE "EOT"              'End of transmission
            Ord% = 4
        CASE "ENQ"              'Enquiry
            Ord% = 5
        CASE "ACK"              'Acknowledge
            Ord% = 6
        CASE "BEL"              'Bell
            Ord% = 7
        CASE "BS"               'Backspace
            Ord% = 8
        CASE "HT"               'Horizontal tab
            Ord% = 9
        CASE "LF"               'Line feed
            Ord% = 10
        CASE "VT"               'Vertical tab
            Ord% = 11
        CASE "FF"               'Form feed
            Ord% = 12
        CASE "CR"               'Carriage return
            Ord% = 13
        CASE "SO"               'Shift out
            Ord% = 14
        CASE "SI"               'Shift in
            Ord% = 15
        CASE "DLE"              'Data link escape
            Ord% = 16
        CASE "DC1"              'Device control 1
            Ord% = 17
        CASE "DC2"              'Device control 2
            Ord% = 18
        CASE "DC3"              'Device control 3
            Ord% = 19
        CASE "DC4"              'Device control 4
            Ord% = 20
        CASE "NAK"              'Negative acknowledge
            Ord% = 21
        CASE "SYN"              'Synchronous idle
            Ord% = 22
        CASE "ETB"              'End of transmission block
            Ord% = 23
        CASE "CAN"              'Cancel
            Ord% = 24
        CASE "EM"               'End of medium
            Ord% = 25
        CASE "SUB"              'Substitute
            Ord% = 26
        CASE "ESC"              'Escape
            Ord% = 27
        CASE "FS"               'File separator
            Ord% = 28
        CASE "GS"               'Group separator
            Ord% = 29
        CASE "RS"               'Record separator
            Ord% = 30
        CASE "US"               'Unit separator
            Ord% = 31
        CASE "SP"               'Space
            Ord% = 32
        CASE "UND"              'Underline
            Ord% = 95
        CASE "GRA"              'Grave accent
            Ord% = 96
        CASE "LCA"              'Lowercase a
            Ord% = 97
        CASE "LCB"              'Lowercase b
            Ord% = 98
        CASE "LCC"              'Lowercase c
            Ord% = 99
        CASE "LCD"              'Lowercase d
            Ord% = 100
        CASE "LCE"              'Lowercase e
            Ord% = 101
        CASE "LCF"              'Lowercase f
            Ord% = 102
        CASE "LCG"              'Lowercase g
            Ord% = 103
        CASE "LCH"              'Lowercase h
            Ord% = 104
        CASE "LCI"              'Lowercase i
            Ord% = 105
        CASE "LCJ"              'Lowercase j
            Ord% = 106
        CASE "LCK"              'Lowercase k
            Ord% = 107
        CASE "LCL"              'Lowercase l
            Ord% = 108
        CASE "LCM"              'Lowercase m
            Ord% = 109
        CASE "LCN"              'Lowercase n
            Ord% = 110
        CASE "LCO"              'Lowercase o
            Ord% = 111
        CASE "LCP"              'Lowercase p
            Ord% = 112
        CASE "LCQ"              'Lowercase q
            Ord% = 113
        CASE "LCR"              'Lowercase r
            Ord% = 114
        CASE "LCS"              'Lowercase s
            Ord% = 115
        CASE "LCT"              'Lowercase t
            Ord% = 116
        CASE "LCU"              'Lowercase u
            Ord% = 117
        CASE "LCV"              'Lowercase v
            Ord% = 118
        CASE "LCW"              'Lowercase w
            Ord% = 119
        CASE "LCX"              'Lowercase x
            Ord% = 120
        CASE "LCY"              'Lowercase y
            Ord% = 121
        CASE "LCZ"              'Lowercase z
            Ord% = 122
        CASE "LBR"              'Left brace
            Ord% = 123
        CASE "VLN"              'Vertical line
            Ord% = 124
        CASE "RBR"              'Right brace
            Ord% = 125
        CASE "TIL"              'Tilde
            Ord% = 126
        CASE "DEL"              'Delete
            Ord% = 127
        CASE ELSE               'Not ANSI Standard ORD mnemonic
            Ord% = -1
        END SELECT
    END FUNCTION

  ' ************************************************
  ' **  Name:          Repeat$                    **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a string formed by concatenating n%
  ' copies of a$ together.
  '
  ' EXAMPLE OF USE:  b$ = Repeat$(a$, n%)
  ' PARAMETERS:      a$           String to be repeated
  '                  n%           Number of copies of a$ to concatenate
  ' VARIABLES:       lena%        Length of a$
  '                  lent&        Length of result
  '                  t$           Work space for building result
  '                  ndx%         Index into t$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Repeat$ (a$, n%)
  '
    FUNCTION Repeat$ (a$, n%) STATIC
        lena% = LEN(a$)
        lent& = n% * lena%
        IF lent& < 0 OR lent& > 32767 THEN
            PRINT "ERROR: Repeat$ - Negative repetition, or result too long"
            SYSTEM
        ELSEIF lent& = 0 THEN
            Repeat$ = ""
        ELSE
            t$ = SPACE$(lent&)
            ndx% = 1
            DO
                MID$(t$, ndx%, lena%) = a$
                ndx% = ndx% + lena%
            LOOP UNTIL ndx% > lent&
            Repeat$ = t$
            t$ = ""
        END IF
    END FUNCTION

  ' ************************************************
  ' **  Name:          Replace$                   **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Replaces all occurrences of find$ in a$ with substitute$.
  '
  ' EXAMPLE OF USE:  b$ = Replace$(a$, find$, substitute$)
  ' PARAMETERS:      a$            String to make substring replacements in
  '                  find$         Substring to be searched for
  '                  substitutes$  String for replacing the found
  '                                substrings
  ' VARIABLES:       t$            Working copy of a$
  '                  lenf%         Length of find$
  '                  lens%         Length of substitute$
  '                  i%            Index into a$, pointing at substrings
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Replace$ (a$, find$, substitute$)
  '
    FUNCTION Replace$ (a$, find$, substitute$) STATIC
        t$ = a$
        lenf% = LEN(find$)
        lens% = LEN(substitute$)
        i% = 1
        DO
            i% = INSTR(i%, t$, find$)
            IF i% = 0 THEN
                EXIT DO
            END IF
            t$ = LEFT$(t$, i% - 1) + substitute$ + MID$(t$, i% + lenf%)
            i% = i% + lens%
        LOOP
        Replace$ = t$
        t$ = ""
    END FUNCTION

  ' ************************************************
  ' **  Name:          Reverse$                   **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Reverses the order of all characters in a$.
  '
  ' EXAMPLE OF USE:  b$ = Reverse$(a$)
  ' PARAMETERS:      a$         String to be processed
  ' VARIABLES:       n%         Length of the string
  '                  r$         Working string space
  '                  i%         Index into the string
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Reverse$ (a$)
  '
    FUNCTION Reverse$ (a$) STATIC
        n% = LEN(a$)
        r$ = a$
        FOR i% = 1 TO n%
            MID$(r$, i%, 1) = MID$(a$, n% - i% + 1, 1)
        NEXT i%
        Reverse$ = r$
        r$ = ""
    END FUNCTION

  ' ************************************************
  ' **  Name:          ReverseCase$               **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Changes all lowercase characters to uppercase
  ' and all uppercase characters to lowercase.
  '
  ' EXAMPLE OF USE:  b$ = ReverseCase$(a$)
  ' PARAMETERS:      a$         String to be processed
  ' VARIABLES:       r$         Working copy of a$
  '                  i%         Index into r$
  '                  t$         Character from middle of a$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION ReverseCase$ (a$)
  '
    FUNCTION ReverseCase$ (a$) STATIC
        r$ = a$
        FOR i% = 1 TO LEN(a$)
            t$ = MID$(a$, i%, 1)
            IF LCASE$(t$) <> t$ THEN
                MID$(r$, i%, 1) = LCASE$(t$)
            ELSE
                MID$(r$, i%, 1) = UCASE$(t$)
            END IF
        NEXT i%
        ReverseCase$ = r$
        r$ = ""
    END FUNCTION

  ' ************************************************
  ' **  Name:          Rpad$                      **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns string of length n%, with a$ right justified
  ' and padded on the left with spaces.
  '
  ' EXAMPLE OF USE:  b$ = Rpad$(a$, n%)
  ' PARAMETERS:      a$           String to be right justified and padded
  '                  n%           Length of string result
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Rpad$ (a$, n%)
  '
    FUNCTION Rpad$ (a$, n%) STATIC
        Rpad$ = RIGHT$(SPACE$(n%) + RTRIM$(a$), n%)
    END FUNCTION

  ' ************************************************
  ' **  Name:          RtrimSet$                  **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Trims occurrences of any characters in set$
  ' from the right of a$.
  '
  ' EXAMPLE OF USE:  b$ = LtrimSet$(a$, set$)
  ' PARAMETERS:      a$           String to be trimmed
  '                  set$         Set of characters to be trimmed
  ' VARIABLES:       i%           Index into a$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION RtrimSet$ (a$, set$)
  '
    FUNCTION RtrimSet$ (a$, set$) STATIC
        IF a$ <> "" THEN
            FOR i% = LEN(a$) TO 1 STEP -1
                IF INSTR(set$, MID$(a$, i%, 1)) = 0 THEN
                    RtrimSet$ = LEFT$(a$, i%)
                    EXIT FUNCTION
                END IF
            NEXT i%
        END IF
        RtrimSet$ = ""
    END FUNCTION

  ' ************************************************
  ' **  Name:          Translate$                 **
  ' **  Type:          Function                   **
  ' **  Module:        STRINGS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a$ with each character translated from
  ' f$ to t$.  If a character from a$ is found in f$,
  ' it is replaced with the character located
  ' in the same position in t$.
  '
  ' EXAMPLE OF USE:  b$ = Translate$ (a$, f$, t$)
  ' PARAMETERS:      a$         String to be translated
  '                  f$         Table of lookup characters
  '                  t$         Table of replacement characters
  ' VARIABLES:       ta$        Working copy of a$
  '                  lena%      Length of a$
  '                  lenf%      Length of f$
  '                  lent%      Length of t$
  '                  i%         Index into ta$
  '                  ptr%       Pointer into f$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Translate$ (a$, f$, t$)
  '
    FUNCTION Translate$ (a$, f$, t$) STATIC
        ta$ = a$
        lena% = LEN(ta$)
        lenf% = LEN(f$)
        lent% = LEN(t$)
        IF lena% > 0 AND lenf% > 0 AND lent% > 0 THEN
            FOR i% = 1 TO lena%
                ptr% = INSTR(f$, MID$(ta$, i%, 1))
                IF ptr% THEN
                    MID$(ta$, i%, 1) = MID$(t$, ptr%, 1)
                END IF
            NEXT i%
        END IF
        Translate$ = ta$
        ta$ = ""
    END FUNCTION


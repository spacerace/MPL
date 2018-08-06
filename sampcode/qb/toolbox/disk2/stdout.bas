  ' ************************************************
  ' **  Name:          STDOUT                     **
  ' **  Type:          Toolbox                    **
  ' **  Module:        STDOUT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' USAGE:            No command line parameters
  ' REQUIREMENTS:     MIXED.QLB/.LIB
  '                   ANSI.SYS
  ' .MAK FILE:        (none)
  ' PARAMETERS:       (none)
  ' VARIABLES:        t0         Timer variable
  '                   bell$      ASCII character 7 (bell)
  
  ' Attribute definitions
    CONST NORMAL = 0
    CONST BRIGHT = 1
    CONST UNDERSCORE = 4
    CONST BLINK = 5
    CONST REVERSE = 7
    CONST INVISIBLE = 8
    CONST BLACKFOREGROUND = 30
    CONST REDFOREGROUND = 31
    CONST GREENFOREGROUND = 32
    CONST YELLOWFOREGROUND = 33
    CONST BLUEFOREGROUND = 34
    CONST MAGENTAFOREGROUND = 35
    CONST CYANFOREGROUND = 36
    CONST WHITEFOREGROUND = 37
    CONST BLACKBACKGROUND = 40
    CONST REDBACKGROUND = 41
    CONST GREENBACKGROUND = 42
    CONST YELLOWBACKGROUND = 43
    CONST BLUEBACKGROUND = 44
    CONST MAGENTABACKGROUND = 45
    CONST CYANBACKGROUND = 46
    CONST WHITEBACKGROUND = 47
  
    TYPE RegTypeX
        ax    AS INTEGER
        bx    AS INTEGER
        cx    AS INTEGER
        dx    AS INTEGER
        Bp    AS INTEGER
        si    AS INTEGER
        di    AS INTEGER
        flags AS INTEGER
        ds    AS INTEGER
        es    AS INTEGER
    END TYPE
  
  ' Subprograms
    DECLARE SUB InterruptX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
    DECLARE SUB ClearLine ()
    DECLARE SUB ClearScreen ()
    DECLARE SUB StdOut (a$)
    DECLARE SUB CrLf ()
    DECLARE SUB CursorPosition (row%, col%)
    DECLARE SUB CursorDown (n%)
    DECLARE SUB CursorLeft (n%)
    DECLARE SUB CursorRight (n%)
    DECLARE SUB CursorUp (n%)
    DECLARE SUB AssignKey (keyCode%, assign$)
    DECLARE SUB Attribute (attr%)
  
  ' Demonstrate the ClearLine and ClearScreen routines
    CLS
    PRINT "This will be erased quickly, in two steps..."
    t0 = TIMER
    DO
    LOOP UNTIL TIMER - t0 > 2
    LOCATE 1, 27
    ClearLine
    t0 = TIMER
    DO
    LOOP UNTIL TIMER - t0 > 2
    LOCATE 15, 1
    ClearScreen
  
  ' Demonstrate the StdOut routine
    bell$ = CHR$(7)
    StdOut "Sending a 'Bell' to StdOut" + bell$
    CrLf
  
  ' Set cursor position
    CursorPosition 3, 20
    StdOut "* CursorPosition 3, 20"
    CrLf
  
  ' Move the cursor around the screen
    StdOut "Cursor movements..."
    CrLf
    CursorDown 1
    StdOut "Down 1"
    CursorRight 12
    StdOut "Right 12"
    CursorDown 2
    StdOut "Down 2"
    CursorLeft 99
    StdOut "Left 99"
    CrLf
  
  ' Character attributes
    CrLf
    Attribute YELLOWFOREGROUND
    Attribute BRIGHT
    Attribute BLUEBACKGROUND
    StdOut "Bright yellow on blue"
    CrLf
    Attribute NORMAL
    StdOut "Back to normal attributes"
    CrLf
  
  ' Key reassignment
    AssignKey 97, "REM The 'a' and 'b' keys have been redefined" + CHR$(13)
    AssignKey 98, "EXIT" + CHR$(13)
    CursorDown 1
    Attribute BRIGHT
    Attribute YELLOWFOREGROUND
    StdOut "NOTE:"
    CrLf
    StdOut "Press the 'a' key and then the 'b' key ... "
    CrLf
    StdOut "The program will then continue ........ "
    Attribute NORMAL
    CrLf
    SHELL
    AssignKey 97, ""
    AssignKey 98, ""
  
  ' ************************************************
  ' **  Name:          AssignKey                  **
  ' **  Type:          Subprogram                 **
  ' **  Module:        STDOUT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Assigns a string to any key using ANSI.SYS driver.
  '
  ' EXAMPLE OF USE:  AssignKey keyCode%, assign$
  ' PARAMETERS:      keyCode%   ASCII number for key to be reassigned
  '                  assign$    String to assign to key
  ' VARIABLES:       k$         Command string for ANSI.SYS driver
  '                  i%         Index to each character of assign$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB AssignKey (keyCode%, assign$)
  '
    SUB AssignKey (keyCode%, assign$) STATIC
        IF keyCode% <= 0 THEN
            k$ = "[0;"
        ELSE
            k$ = "["
        END IF
        k$ = k$ + MID$(STR$(keyCode%), 2)
        IF assign$ <> "" THEN
            FOR i% = 1 TO LEN(assign$)
                k$ = k$ + ";" + MID$(STR$(ASC(MID$(assign$, i%))), 2)
            NEXT i%
        END IF
        StdOut CHR$(27) + k$ + "p"
    END SUB
  
  ' ************************************************
  ' **  Name:          Attribute                  **
  ' **  Type:          Subprogram                 **
  ' **  Module:        STDOUT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Sets the foreground, background, and other color
  ' attributes.
  '
  ' EXAMPLE OF USE:  Attribute attr%
  ' PARAMETERS:      attr%      Number for attribute to be set
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB StdOut (a$)
  '                  DECLARE SUB Attribute (attr%)
  '
    SUB Attribute (attr%) STATIC
        StdOut CHR$(27) + "[" + MID$(STR$(attr%), 2) + "m"
    END SUB
  
  ' ************************************************
  ' **  Name:          ClearLine                  **
  ' **  Type:          Subprogram                 **
  ' **  Module:        STDOUT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Clears the display line from the current cursor
  ' position to the end of the line.
  '
  ' EXAMPLE OF USE:  ClearLine
  ' PARAMETERS:      (none)
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB ClearLine ()
  '                  DECLARE SUB StdOut (a$)
  '
    SUB ClearLine STATIC
        StdOut CHR$(27) + "[K"
    END SUB
  
  ' ************************************************
  ' **  Name:          ClearScreen                **
  ' **  Type:          Subprogram                 **
  ' **  Module:        STDOUT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Clears the screen and moves the cursor to the
  ' home position.
  '
  ' EXAMPLE OF USE:  ClearScreen
  ' PARAMETERS:      (none)
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB ClearScreen ()
  '                  DECLARE SUB StdOut (a$)
  '
    SUB ClearScreen STATIC
        StdOut CHR$(27) + "[2J"
    END SUB
  
  ' ************************************************
  ' **  Name:          CrLf                       **
  ' **  Type:          Subprogram                 **
  ' **  Module:        STDOUT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Sends line feed and carriage return characters
  ' to standard output.
  '
  ' EXAMPLE OF USE:  CrLf
  ' PARAMETERS:      (none)
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB StdOut (a$)
  '                  DECLARE SUB CrLf ()
  '
    SUB CrLf STATIC
        StdOut CHR$(13) + CHR$(10)
    END SUB
  
  ' ************************************************
  ' **  Name:          CursorDown                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        STDOUT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Moves the cursor n% lines down the screen.
  '
  ' EXAMPLE OF USE:   CursorDown n%
  ' PARAMETERS:       n%         Number of lines to move the cursor down
  ' VARIABLES:        (none)
  ' MODULE LEVEL
  '   DECLARATIONS:   DECLARE SUB StdOut (a$)
  '                   DECLARE SUB CursorDown (n%)
  '
    SUB CursorDown (n%) STATIC
        StdOut CHR$(27) + "[" + MID$(STR$(n%), 2) + "B"
    END SUB
  
  ' ************************************************
  ' **  Name:          CursorHome                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        STDOUT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Moves the cursor to the top left of the
  ' screen.
  '
  ' EXAMPLE OF USE:  CursorHome
  ' PARAMETERS:      (none)
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB CursorHome
  '
    SUB CursorHome STATIC
        StdOut CHR$(27) + "[H"
    END SUB
  
  ' ************************************************
  ' **  Name:          CursorLeft                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        STDOUT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Moves the cursor n% columns left on the screen.
  '
  ' EXAMPLE OF USE:  CursorLeft n%
  ' PARAMETERS:      n%      Number of columns to move the cursor left
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB CursorLeft (n%)
  '
    SUB CursorLeft (n%) STATIC
        StdOut CHR$(27) + "[" + MID$(STR$(n%), 2) + "D"
    END SUB
  
  ' ************************************************
  ' **  Name:          CursorPosition             **
  ' **  Type:          Subprogram                 **
  ' **  Module:        STDOUT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Moves the cursor to the indicated row and column.
  '
  ' EXAMPLE OF USE:  CursorPosition row%, col%
  ' PARAMETERS:      row%       Row to move the cursor to
  '                  col%       Column to move the cursor to
  ' VARIABLES:       row$       String representation of row%
  '                  col$       String representation of col%
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB CursorPosition (row%, col%)
  '
    SUB CursorPosition (row%, col%) STATIC
        row$ = MID$(STR$(row%), 2)
        col$ = MID$(STR$(col%), 2)
        StdOut CHR$(27) + "[" + row$ + ";" + col$ + "H"
    END SUB
  
  ' ************************************************
  ' **  Name:          CursorRight                **
  ' **  Type:          Subprogram                 **
  ' **  Module:        STDOUT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Moves the cursor n% columns right on the screen.
  '
  ' EXAMPLE OF USE:  CursorRight n%
  ' PARAMETERS:      n%     Number of columns to move the cursor right
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB CursorRight (n%)
  '
    SUB CursorRight (n%) STATIC
        StdOut CHR$(27) + "[" + MID$(STR$(n%), 2) + "C"
    END SUB
  
  ' ************************************************
  ' **  Name:          CursorUp                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        STDOUT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Moves the cursor n% lines up the screen.
  '
  ' EXAMPLE OF USE:  CursorUp n%
  ' PARAMETERS:      n%         Number of lines to move the cursor up
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB CursorUp (n%)
  '
    SUB CursorUp (n%) STATIC
        StdOut CHR$(27) + "[" + MID$(STR$(n%), 2) + "A"
    END SUB
  
  ' ************************************************
  ' **  Name:          StdOut                     **
  ' **  Type:          Subprogram                 **
  ' **  Module:        STDOUT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Writes string to the MS-DOS standard output.
  '
  ' EXAMPLE OF USE:  StdOut a$
  ' PARAMETERS:      a$         String to be output
  ' VARIABLES:       regX       Structure of type RegTypeX
  ' MODULE LEVEL
  '   DECLARATIONS:    DECLARE SUB InterruptX (intnum%, inreg AS RegTypeX,
  '                                          outreg AS RegTypeX)
  '                  DECLARE SUB StdOut (a$)
  '
    SUB StdOut (a$) STATIC
        DIM regX AS RegTypeX
        regX.ax = &H4000
        regX.cx = LEN(a$)
        regX.bx = 1
        regX.ds = VARSEG(a$)
        regX.dx = SADD(a$)
        InterruptX &H21, regX, regX
        IF regX.flags AND 1 THEN
            PRINT "Error while calling StdOut:"; regX.ax
            SYSTEM
        END IF
    END SUB
  

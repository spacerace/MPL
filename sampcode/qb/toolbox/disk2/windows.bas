  ' ************************************************
  ' **  Name:          WINDOWS                    **
  ' **  Type:          Toolbox                    **
  ' **  Module:        WINDOWS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  ' USAGE:           No command line parameters
  ' REQUIREMENTS:    MIXED.QLB/.LIB
  '                  Mouse (optional)
  ' .MAK FILES:      WINDOWS.BAS
  '                  BITS.BAS
  '                  BIOSCALL.BAS
  '                  MOUSSUBS.BAS
  '                  KEYS.BAS
  ' PARAMETERS:      (none)
  ' VARIABLES:       w1         Structure of type WindowsType
  '                  w2         Structure of type WindowsType
  '                  w3         Structure of type WindowsType
  '                  w1Text$()  Strings to display in first window
  '                  w2Text$()  Strings to display in second window
  '                  w3Text$()  Strings to display in third window
  '                  w1Title$   Title string for first window
  '                  w1Prompt$  Prompt string for first window
  '                  w2Title$   Title string for second window
  '                  w2Prompt$  Prompt string for second window
  '                  w3Title$   Title string for third window
  '                  arrow$     String showing up and down arrows
  '                  entSymbol$ String showing the Enter key symbol
  '                  w3Prompt$  Prompt string for third window
  '                  i%         Looping index
  '                  t0         Timer value
  
  
  ' Define color constants
    CONST BLACK = 0
    CONST BLUE = 1
    CONST GREEN = 2
    CONST CYAN = 3
    CONST RED = 4
    CONST MAGENTA = 5
    CONST BROWN = 6
    CONST WHITE = 7
    CONST BRIGHT = 8
    CONST BLINK = 16
    CONST YELLOW = BROWN + BRIGHT
  
    TYPE WindowsType
        action       AS INTEGER
        edgeLine     AS INTEGER
        row          AS INTEGER
        col          AS INTEGER
        fgdEdge      AS INTEGER
        bgdEdge      AS INTEGER
        fgdBody      AS INTEGER
        bgdBody      AS INTEGER
        fgdHighlight AS INTEGER
        bgdHighlight AS INTEGER
        fgdTitle     AS INTEGER
        bgdTitle     AS INTEGER
        fgdPrompt    AS INTEGER
        bgdPrompt    AS INTEGER
        returnCode   AS INTEGER
    END TYPE
  
  ' Functions
    DECLARE FUNCTION InKeyCode% ()
  
  ' Subprograms
    DECLARE SUB Windows (w AS WindowsType, wText$(), wTitle$, wPrompt$)
    DECLARE SUB WindowsPop ()
    DECLARE SUB VideoState (mode%, columns%, page%)
    DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
    DECLARE SUB MouseMickey (horizontal%, vertical%)
    DECLARE SUB MouseNow (leftButton%, rightButton%, xMouse%, yMouse%)
  
  ' Data structures
    DIM w1 AS WindowsType
    DIM w2 AS WindowsType
    DIM w3 AS WindowsType
  
  ' Arrays
    DIM w1Text$(1 TO 5)
    DIM w2Text$(1 TO 3)
    DIM w3Text$(1 TO 9)
  
  ' Define first window
    w1.action = 0
    w1.edgeLine = 1
    w1.row = 2
    w1.col = 3
    w1.fgdEdge = YELLOW
    w1.bgdEdge = BLUE
    w1.fgdBody = BRIGHT + WHITE
    w1.bgdBody = BLUE
    w1.fgdHighlight = 0
    w1.bgdHighlight = 0
    w1.fgdTitle = YELLOW
    w1.bgdTitle = BLUE
    w1.fgdPrompt = YELLOW
    w1.bgdPrompt = BLUE
    w1Title$ = " First Window "
    w1Text$(1) = "This window demonstrates how information"
    w1Text$(2) = "can be displayed without requesting any"
    w1Text$(3) = "response from the user.  The action code"
    w1Text$(4) = "is 0, causing an immediate return to the"
    w1Text$(5) = "program after the window is displayed."
    w1Prompt$ = ""
  
  ' Define second window
    w2.action = 1
    w2.edgeLine = 2
    w2.row = 10
    w2.col = 12
    w2.fgdEdge = CYAN + BRIGHT
    w2.bgdEdge = BLACK
    w2.fgdBody = YELLOW
    w2.bgdBody = BLACK
    w2.fgdHighlight = 0
    w2.bgdHighlight = 0
    w2.fgdTitle = CYAN + BRIGHT
    w2.bgdTitle = BLUE
    w2.fgdPrompt = CYAN + BRIGHT
    w2.bgdPrompt = BLUE
    w2Title$ = " Second window, action code is 1 "
    w2Text$(1) = "This window waits for the user to press"
    w2Text$(2) = "any key before continuing.  The key code"
    w2Text$(3) = "is passed back to the calling program."
    w2Prompt$ = " Press any key to continue. "
  
  ' Define third window
    w3.action = 2
    w3.edgeLine = 2
    w3.row = 7
    w3.col = 15
    w3.fgdEdge = YELLOW
    w3.bgdEdge = WHITE
    w3.fgdBody = BLACK
    w3.bgdBody = WHITE
    w3.fgdHighlight = WHITE + BRIGHT
    w3.bgdHighlight = BLACK
    w3.fgdTitle = YELLOW
    w3.bgdTitle = WHITE
    w3.fgdPrompt = YELLOW
    w3.bgdPrompt = WHITE
    w3Title$ = " Third window, action is 2 (menu selection) "
    arrows$ = CHR$(24) + " " + CHR$(25) + " "
    entSymbol$ = CHR$(17) + CHR$(196) + CHR$(217)
    w3Prompt$ = " <Character> " + arrows$ + entSymbol$ + " or use mouse "
    w3Text$(1) = "1. This is the first line in the window."
    w3Text$(2) = "2. This is the second."
    w3Text$(3) = "3. This is the third line."
    w3Text$(4) = "4. The fourth."
    w3Text$(5) = "5. The fifth."
    w3Text$(6) = "A. You can press <A> or <a> to select this line."
    w3Text$(7) = "B. You can press <1> to <5> for one of the first 5 lines."
    w3Text$(8) = "C. Try moving the cursor up or down and pressing Enter."
    w3Text$(9) = "D. Also, try the mouse. Click with left button."
  
  ' Initialize the display
    SCREEN 0, , 0, 0
    WIDTH 80
    CLS
    FOR i% = 1 TO 20
        PRINT STRING$(80, 178)
    NEXT i%
    LOCATE 6, 24
    PRINT " * Windows toolbox demonstration * "
  
  ' Wait for any key to be pressed
    LOCATE 22, 1
    PRINT "Press any key to continue"
    DO
    LOOP UNTIL INKEY$ <> ""
  
  ' Clear the "press any key" prompt
    LOCATE 22, 1
    PRINT SPACE$(25)
  
  ' Create the three windows
    Windows w1, w1Text$(), w1Title$, w1Prompt$
    Windows w2, w2Text$(), w2Title$, w2Prompt$
    Windows w3, w3Text$(), w3Title$, w3Prompt$
  
  ' Display the result codes, and erase each window
    FOR i% = 1 TO 4
        LOCATE 21, 1
        COLOR WHITE, BLACK
        PRINT "The three return codes...";
        PRINT w1.returnCode; w2.returnCode; w3.returnCode
        COLOR YELLOW
        PRINT "Every five seconds another window will disappear..."
        COLOR WHITE, BLACK
        t0 = TIMER
        DO
        LOOP UNTIL TIMER - t0 > 5
        WindowsPop
    NEXT i%
  
  ' All done
    CLS
    END
  
  ' ************************************************
  ' **  Name:          Windows                    **
  ' **  Type:          Subprogram                 **
  ' **  Module:        WINDOWS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Displays a rectangular window for information display
  ' or menu selection.
  '
  ' EXAMPLE OF USE:  Windows w1, wText$(), wTitle$, wPrompt$
  ' PARAMETERS:      w1            Structure of type WindowsType
  '                  wTest$()      Array of strings to be displayed
  '                  wTitle$       Title string
  '                  wPrompt$      Prompt string
  ' VARIABLES:       mode%         Current video mode
  '                  columns%      Current number of character columns
  '                  page%         Current video page
  '                  cursorRow%    Saved cursor row position
  '                  cursorCol%    Saved cursor column position
  '                  newpage%      Next video page
  '                  lbText%       Lower boundary of array of text lines
  '                  ubText%       Upper boundary of array of text lines
  '                  i%            Looping index
  '                  maxlen%       Length of longest string to display
  '                  length%       Length of each array string
  '                  row2%         Row number at bottom right corner of window
  '                  col2%         Column number at bottom right corner of
  '                                window
  '                  ul%           Upper left corner border character code
  '                  ur%           Upper right corner border character code
  '                  ll%           Lower left corner border character code
  '                  lr%           Lower right corner border character code
  '                  vl%           Vertical border character code
  '                  hl%           Horizontal border character code
  '                  r%            Index to each line of text
  '                  ptr%          Highlighted line pointer
  '                  lastPtr%      Last highlighted line
  '                  horizontal%   Horizontal mouse mickies
  '                  vertical%     Vertical mouse mickies
  '                  mickies       Accumulated vertical mickies
  '                  choice$       Set of unique characters for each menu line
  '                  tmp$          Work string
  '                  kee%          Key code returned by InKeyCode% function
  '                  leftButton%   Mouse left button state
  '                  rightButton%  Mouse right button state
  '                  xMouse%       Mouse X position
  '                  yMouse%       Mouse Y position
  ' MODULE LEVEL
  '   DECLARATIONS:  SUB Windows (w AS WindowsType, wText$(), wTitle$,
  '                               wPrompt$) STATIC
  '
    SUB Windows (w AS WindowsType, wText$(), wTitle$, wPrompt$) STATIC
      ' Key code numbers
        CONST DOWNARROW = 20480
        CONST ENTER = 13
        CONST ESCAPE = 27
        CONST UPARROW = 18432
      
      ' Determine current video page
        VideoState mode%, columns%, page%
      
      ' Record current cursor location
        cursorRow% = CSRLIN
        cursorCol% = POS(0)
      
      ' Window will be on the next page, if available
        newpage% = page% + 1
        IF newpage% > 7 THEN
            SCREEN , , 0, 0
            PRINT "Error: Windows - not enough video pages"
            SYSTEM
        END IF
      
      ' Copy current page to new page
        PCOPY page%, newpage%
      
      ' Show the current page while building window on new page
        SCREEN , , newpage%, page%
      
      ' Determine array bounds
        lbText% = LBOUND(wText$)
        ubText% = UBOUND(wText$)
      
      ' Check the text array bounds, lower always 1, upper > 0
        IF lbText% <> 1 OR ubText% < 1 THEN
            SCREEN , , 0, 0
            PRINT "Error: Windows - text array dimensioned incorrectly"
            SYSTEM
        END IF
      
      ' Determine longest string in the text array
        maxLen% = 0
        FOR i% = lbText% TO ubText%
            length% = LEN(wText$(i%))
            IF length% > maxLen% THEN
                maxLen% = length%
            END IF
        NEXT i%
      
      ' Determine the bottom right corner of window
        row2% = w.row + ubText% + 1
        col2% = w.col + maxLen% + 3
      
      ' Check that window is on screen
        IF w.row < 1 OR w.col < 1 OR row2% > 25 OR col2% > columns% THEN
            SCREEN , , 0, 0
            PRINT "Error: Windows - part of window is off screen"
            SYSTEM
        END IF
      
      ' Set the edge characters
        SELECT CASE w.edgeLine
        CASE 0
            ul% = 32
            ur% = 32
            ll% = 32
            lr% = 32
            vl% = 32
            hl% = 32
        CASE 1
            ul% = 218
            ur% = 191
            ll% = 192
            lr% = 217
            vl% = 179
            hl% = 196
        CASE 2
            ul% = 201
            ur% = 187
            ll% = 200
            lr% = 188
            vl% = 186
            hl% = 205
        CASE ELSE
            SCREEN , , 0, 0
            PRINT "Error: Windows - Edge line type incorrect"
            SYSTEM
        END SELECT
      
      ' Draw top edge of the box
        LOCATE w.row, w.col, 0
        COLOR w.fgdEdge, w.bgdEdge
        PRINT CHR$(ul%); STRING$(maxLen% + 2, hl%); CHR$(ur%);
      
      ' Draw the body of the window
        FOR r% = w.row + 1 TO row2% - 1
            LOCATE r%, w.col, 0
            COLOR w.fgdEdge, w.bgdEdge
            PRINT CHR$(vl%);
            COLOR w.fgdBody, w.bgdBody
            tmp$ = LEFT$(wText$(r% - w.row) + SPACE$(maxLen%), maxLen%)
            PRINT " "; tmp$; " ";
            COLOR w.fgdEdge, w.bgdEdge
            PRINT CHR$(vl%);
        NEXT r%
      
      ' Draw bottom edge of the box
        LOCATE row2%, w.col, 0
        COLOR w.fgdEdge, w.bgdEdge
        PRINT CHR$(ll%); STRING$(maxLen% + 2, hl%); CHR$(lr%);
      
      ' Center and print top title if present
        IF wTitle$ <> "" THEN
            LOCATE w.row, (w.col + col2% - LEN(wTitle$) + 1) \ 2, 0
            COLOR w.fgdTitle, w.bgdTitle
            PRINT wTitle$;
        END IF
      
      ' Center and print prompt if present
        IF wPrompt$ <> "" THEN
            LOCATE row2%, (w.col + col2% - LEN(wPrompt$) + 1) \ 2, 0
            COLOR w.fgdPrompt, w.bgdPrompt
            PRINT wPrompt$;
        END IF
      
      ' Now make the new page visible and active
        SCREEN , , newpage%, newpage%
      
      ' Take next action based on action code
        SELECT CASE w.action
        CASE 1
          
          ' Get a key code number and return it
            DO
                w.returnCode = InKeyCode%
            LOOP UNTIL w.returnCode
          
        CASE 2
          
          ' Set choice pointer to last selection if known
            IF w.returnCode > 0 AND w.returnCode < ubText% THEN
                ptr% = w.returnCode
            ELSE
                ptr% = 1
            END IF
          
          ' Start with last pointer different, to update highlighting
            IF ptr% > 1 THEN
                lastPtr% = 1
            ELSE
                lastPtr% = 2
            END IF
          
          ' Clear any mouse mickey counts
            MouseMickey horizontal%, vertical%
            mickies% = 0
          
          ' Create unique key selection string
            choice$ = ""
            FOR i% = 1 TO ubText%
                tmp$ = UCASE$(LTRIM$(wText$(i%)))
                DO
                    IF tmp$ <> "" THEN
                        t$ = LEFT$(tmp$, 1)
                        tmp$ = MID$(tmp$, 2)
                        IF INSTR(choice$, t$) = 0 THEN
                            choice$ = choice$ + t$
                        END IF
                    ELSE
                        SCREEN 0, , 0
                        PRINT "Error: Windows - No unique character"
                        SYSTEM
                    END IF
                LOOP UNTIL LEN(choice$) = i%
            NEXT i%
          
          ' Main loop, monitor mouse and keyboard
            DO
              
              ' Add the mouse mickies
                MouseMickey horizontal%, vertical%
                mickies% = mickies% + vertical%
              
              ' Check for enough mickies
                IF mickies% < -17 THEN
                    mickies% = 0
                    IF ptr% > 1 THEN
                        ptr% = ptr% - 1
                    END IF
                ELSEIF mickies% > 17 THEN
                    mickies% = 0
                    IF ptr% < ubText% THEN
                        ptr% = ptr% + 1
                    END IF
                END IF
              
              ' Check keyboard
                kee% = InKeyCode%
                IF kee% >= ASC("a") AND kee% <= ASC("z") THEN
                    kee% = ASC(UCASE$(CHR$(kee%)))
                END IF
                SELECT CASE kee%
                CASE UPARROW
                    IF ptr% > 1 THEN
                        ptr% = ptr% - 1
                    END IF
                CASE DOWNARROW
                    IF ptr% < ubText% THEN
                        ptr% = ptr% + 1
                    END IF
                CASE ENTER
                    w.returnCode = ptr%
                CASE ESCAPE
                    w.returnCode = -1
                CASE ELSE
                    w.returnCode = INSTR(choice$, CHR$(kee%))
                    IF w.returnCode THEN
                        ptr% = w.returnCode
                    END IF
                END SELECT
              
              ' Check the left mouse button
                MouseNow leftButton%, rightButton%, xMouse%, yMouse%
                IF leftButton% THEN
                    w.returnCode = ptr%
                END IF
              
              ' Update the highlight if line has changed
                IF ptr% <> lastPtr% THEN
                    LOCATE lastPtr% + w.row, w.col + 2, 0
                    COLOR w.fgdBody, w.bgdBody
                    tmp$ = LEFT$(wText$(lastPtr%) + SPACE$(maxLen%), maxLen%)
                    PRINT tmp$;
                    LOCATE ptr% + w.row, w.col + 2, 0
                    COLOR w.fgdHighlight, w.bgdHighlight
                    tmp$ = LEFT$(wText$(ptr%) + SPACE$(maxLen%), maxLen%)
                    PRINT tmp$;
                    lastPtr% = ptr%
                END IF
              
            LOOP WHILE w.returnCode = 0
          
        CASE ELSE
            w.returnCode = 0
        END SELECT
      
      ' Reset the cursor position
        LOCATE cursorRow%, cursorCol%
      
    END SUB
  
  ' ************************************************
  ' **  Name:          WindowsPop                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        WINDOWS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Removes last displayed window.
  '
  ' EXAMPLE OF USE:  WindowsPop
  ' PARAMETERS:      (none)
  ' VARIABLES:       mode%      Current video mode
  '                  columns%   Current number of display columns
  '                  page%      Current display page
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB WindowsPop ()
  '
    SUB WindowsPop STATIC
        VideoState mode%, columns%, page%
        IF page% THEN
            SCREEN 0, , page% - 1, page% - 1
        END IF
    END SUB
  

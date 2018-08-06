  ' ************************************************
  ' **  Name:          EDIT                       **
  ' **  Type:          Toolbox                    **
  ' **  Module:        EDIT.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' USAGE:           No command line parameters
  ' .MAK FILE:       EDIT.BAS
  '                  KEYS.BAS
  ' PARAMETERS:      (none)
  ' VARIABLES:       a$         String to be edited by the user
  
    CONST FALSE = 0
    CONST TRUE = NOT FALSE
  
  ' Key code numbers
    CONST BACKSPACE = 8
    CONST CTRLLEFTARROW = 29440
    CONST CTRLRIGHTARROW = 29696
    CONST CTRLY = 25
    CONST CTRLQ = 17
    CONST DELETE = 21248
    CONST DOWNARROW = 20480
    CONST ENDKEY = 20224
    CONST ENTER = 13
    CONST ESCAPE = 27
    CONST HOME = 18176
    CONST INSERTKEY = 20992
    CONST LEFTARROW = 19200
    CONST RIGHTARROW = 19712
    CONST TABKEY = 9
    CONST UPARROW = 18432
  
  ' Functions
    DECLARE FUNCTION KeyCode% ()
  
  ' Subprograms
    DECLARE SUB EditLine (a$, exitCode%)
    DECLARE SUB DrawBox (row1%, col1%, row2%, col2%)
    DECLARE SUB EditBox (a$, row1%, col1%, row2%, col2%)
    DECLARE SUB FormatTwo (a$, b$, col%)
    DECLARE SUB InsertCharacter (x$(), kee$, cp%, rp%, wide%, high%)
  
  ' Demonstrate the EditLine subprogram
    a$ = " Edit this line, and then press Up arrow, Down arrow, or Enter "
    CLS
    COLOR 14, 1
    EditLine a$, exitCode%
    COLOR 7, 0
    PRINT
    PRINT
    PRINT "Result of edit ..."
    COLOR 14, 0
    PRINT a$
    COLOR 7, 0
    PRINT
    SELECT CASE exitCode%
    CASE 0
        PRINT "Enter";
    CASE -1
        PRINT "Down arrow";
    CASE 1
        PRINT "Up arrow";
    CASE ELSE
    END SELECT
    PRINT " key pressed."
  
  ' Demonstrate the EditBox subprogram
    a$ = "Now, edit text inside this box.  Press "
    a$ = a$ + "Esc to end the editing..."
    COLOR 12, 1
    DrawBox 8, 17, 19, 57
    COLOR 11, 1
    EditBox a$, 8, 17, 19, 57
    LOCATE 21, 1
    COLOR 7, 0
    PRINT "Result..."
    COLOR 14, 0
    PRINT a$
    COLOR 7, 0

  ' ************************************************
  ' **  Name:          DrawBox                    **
  ' **  Type:          Subprogram                 **
  ' **  Module:        EDIT.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Draws a double-lined box.
  '
  ' EXAMPLE OF USE:  DrawBox row1%, col1%, row2%, col2%
  ' PARAMETERS:      row1%    Screen text row at upper left corner of the box
  '                  col1%    Screen text column at upper left corner of the box
  '                  row2%    Screen text row at lower right corner of the box
  '                  col2%    Screen text column at lower right corner of the
  '                           box
  ' VARIABLES:       wide%    Inside width of box
  '                  row3%    Loop row number for creating sides of box
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB DrawBox (row1%, col1%, row2%, col2%)
  '
    SUB DrawBox (row1%, col1%, row2%, col2%) STATIC
      
      ' Determine inside width of box
        wide% = col2% - col1% - 1
      
      ' Across the top
        LOCATE row1%, col1%, 0
        PRINT CHR$(201);
        PRINT STRING$(wide%, 205);
        PRINT CHR$(187);
      
      ' Down the sides
        FOR row3% = row1% + 1 TO row2% - 1
            LOCATE row3%, col1%, 0
            PRINT CHR$(186);
            PRINT SPACE$(wide%);
            PRINT CHR$(186);
        NEXT row3%
      
      ' Across the bottom
        LOCATE row2%, col1%, 0
        PRINT CHR$(200);
        PRINT STRING$(wide%, 205);
        PRINT CHR$(188);
      
    END SUB

  ' ************************************************
  ' **  Name:          EditBox                    **
  ' **  Type:          Subprogram                 **
  ' **  Module:        EDIT.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Allows the user to edit text inside a rectangular area.
  '
  ' EXAMPLE OF USE:  EditBox a$, row1%, col1%, row2%, col2%
  ' PARAMETERS:      a$     String to be edited
  '                  row1%  Screen text row at upper left corner of the area
  '                  col1%  Screen text column at upper left corner of the area
  '                  row2%  Screen text row at lower right corner of the area
  '                  col2%  Screen text column at lower right corner of the area
  ' VARIABLES:       r1%    Upper inside row of rectangular area
  '                  r2%    Lower inside row of rectangular area
  '                  c1%    Left inside column of rectangular area
  '                  c2%    Right inside column of rectangular area
  '                  wide%  Width of area
  '                  high%  Height of area
  '                  rp%    Index to current working row
  '                  cp%    Index to current working column
  '                  insert%  Flag for insert/replace mode
  '                  quit%  Flag for quitting the subprogram
  '                  across%  Saved current cursor column
  '                  down%  Saved current cursor row
  '                  x$()  Workspace string array
  '                  i%  Looping index
  '                  b$  Works with a$ to format a$ into x$()
  '                  keyNumber%  Integer code for any key press
  '                  c$  Temporary string workspace
  '                  ds%  Index to double-space groupings
  '                  sp%  Index to character where split of string is to occur
  '                  ctrlQflag%  Indicates Ctrl-Q has been pressed
  '                  kee$  Character entered from keyboard
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION KeyCode% ()
  '                  DECLARE SUB EditBox ($, row1%, col1%, row2%, col2%)
  '                  DECLARE SUB FormatTwo (a$, b$, col%)
  '                  DECLARE SUB InsertCharacter (x$(), kee$, cp%, rp%,
  '                                               wide%, high%)
  '
    SUB EditBox (a$, row1%, col1%, row2%, col2%) STATIC
      
      ' Set up some working variables
        r1% = row1% + 1
        r2% = row2% - 1
        c1% = col1% + 2
        c2% = col2% - 2
        wide% = c2% - c1% + 1
        high% = r2% - r1% + 1
        rp% = 1
        cp% = 1
        insert% = TRUE
        quit% = FALSE
      
      ' Record the current cursor location
        across% = POS(0)
        down% = CSRLIN
      
      ' Dimension a workspace array
        REDIM x$(1 TO high%)
      
      ' Format a$ into array space
        FOR i% = 1 TO high%
            FormatTwo a$, b$, wide%
            x$(i%) = a$
            a$ = b$
        NEXT i%
      
      ' Display the strings
        FOR i% = 1 TO high%
            LOCATE r1% + i% - 1, c1%, 0
            PRINT x$(i%);
        NEXT i%
      
      ' Process each keystroke
        DO
          
          ' Update the current line
            LOCATE r1% + rp% - 1, c1%, 0
            PRINT x$(rp%);
          
          ' Place the cursor
            IF insert% THEN
                LOCATE r1% + rp% - 1, c1% + cp% - 1, 1, 6, 7
            ELSE
                LOCATE r1% + rp% - 1, c1% + cp% - 1, 1, 1, 7
            END IF
          
          ' Grab next keystroke
            keyNumber% = KeyCode%
          
          ' Process the key
            SELECT CASE keyNumber%
              
            CASE INSERTKEY
                IF insert% THEN
                    insert% = FALSE
                ELSE
                    insert% = TRUE
                END IF
              
            CASE BACKSPACE
              
              ' Rub out character to the left
                IF cp% > 1 THEN
                    x$(rp%) = x$(rp%) + " "
                    b$ = LEFT$(x$(rp%), cp% - 2)
                    c$ = MID$(x$(rp%), cp%)
                    x$(rp%) = b$ + c$
                    cp% = cp% - 1
                  
              ' Upper left corner, so reformat the whole box
                ELSEIF rp% = 1 THEN
                  
                  ' Pull all the strings together
                    a$ = ""
                    FOR i% = 1 TO high%
                        a$ = a$ + LTRIM$(RTRIM$(x$(i%))) + " "
                    NEXT i%
                  
                  ' Remove double spaces
                    ds% = INSTR(a$, "  ")
                    DO WHILE ds%
                        a$ = LEFT$(a$, ds% - 1) + MID$(a$, ds% + 1)
                        ds% = INSTR(a$, "  ")
                    LOOP
                  
                  ' Format into the array and display lines
                    FOR i% = 1 TO high%
                        FormatTwo a$, b$, wide%
                        x$(i%) = a$
                        a$ = b$
                        LOCATE r1% + i% - 1, c1%, 0
                        PRINT x$(i%);
                    NEXT i%
                  
              ' Concatenate to the preceding line
                ELSE
                  
                  ' Use the InsertCharacter sub to insert a space
                    rp% = rp% - 1
                    cp% = wide% + 1
                    InsertCharacter x$(), " ", rp%, cp%, wide%, high%
                  
                  ' Remove the extra spaces introduced
                    IF cp% > 2 THEN
                        b$ = LEFT$(x$(rp%), cp% - 3)
                        c$ = MID$(x$(rp%), cp%)
                    ELSE
                        b$ = ""
                        c$ = MID$(x$(rp%), cp% + 1)
                    END IF
                  
                  ' Pull the line pieces together
                    x$(rp%) = LEFT$(b$ + c$ + SPACE$(3), wide%)
                  
                  ' Adjust the cursor position
                    cp% = cp% - 1
                  
                  ' Display the lines
                    FOR i% = 1 TO high%
                        LOCATE r1% + i% - 1, c1%, 0
                        PRINT x$(i%);
                    NEXT i%
                END IF
              
            CASE DELETE
                x$(rp%) = x$(rp%) + " "
                b$ = LEFT$(x$(rp%), cp% - 1)
                c$ = MID$(x$(rp%), cp% + 1)
                x$(rp%) = b$ + c$
              
            CASE UPARROW
                IF rp% > 1 THEN
                    rp% = rp% - 1
                END IF
              
            CASE DOWNARROW
                IF rp% < high% THEN
                    rp% = rp% + 1
                END IF
              
            CASE LEFTARROW
                IF cp% > 1 THEN
                    cp% = cp% - 1
                END IF
              
            CASE RIGHTARROW
                IF cp% < wide% THEN
                    cp% = cp% + 1
                END IF
              
            CASE ENTER
                IF rp% < high% AND x$(high%) = SPACE$(wide%) THEN
                  
                  ' Shuffle lines down
                    FOR i% = high% TO rp% + 1 STEP -1
                        x$(i%) = x$(i% - 1)
                    NEXT i%
                  
                  ' Split current line at cursor
                    sp% = wide% - cp% + 1
                    IF sp% THEN
                        MID$(x$(rp%), cp%, sp%) = SPACE$(sp%)
                    END IF
                  
                  ' Move to next line
                    rp% = rp% + 1
                    x$(rp%) = MID$(x$(rp%), cp%) + SPACE$(cp% - 1)
                    cp% = 1
                  
                  ' Display the modified lines
                    FOR i% = rp% - 1 TO high%
                        LOCATE r1% + i% - 1, c1%, 0
                        PRINT x$(i%);
                    NEXT i%
                  
                ELSE
                  
                  ' Nowhere to push things down
                    BEEP
                  
                END IF
              
            CASE HOME
                cp% = 1
              
            CASE ENDKEY
                cp% = wide% + 1
              
              ' Move back to just after last character
                IF x$(rp%) <> SPACE$(wide%) THEN
                    DO UNTIL MID$(x$(rp%), cp% - 1, 1) <> " "
                        cp% = cp% - 1
                    LOOP
                ELSE
                    cp% = 1
                END IF
              
            CASE CTRLRIGHTARROW
              
              ' Find next space
                DO UNTIL MID$(x$(rp%), cp%, 1) = " " OR cp% = wide%
                    cp% = cp% + 1
                LOOP
              
              ' Find first non-space character
                DO UNTIL MID$(x$(rp%), cp%, 1) <> " " OR cp% = wide%
                    cp% = cp% + 1
                LOOP
              
            CASE CTRLLEFTARROW
              
              ' Find first space to the left
                DO UNTIL MID$(x$(rp%), cp%, 1) = " " OR cp% = 1
                    cp% = cp% - 1
                LOOP
              
              ' Find first non-space character to the left
                DO UNTIL MID$(x$(rp%), cp%, 1) <> " " OR cp% = 1
                    cp% = cp% - 1
                LOOP
              
              ' Find next space to the left
                DO UNTIL MID$(x$(rp%), cp%, 1) = " " OR cp% = 1
                    cp% = cp% - 1
                LOOP
              
              ' Adjust cursor position to first non-space character
                IF cp% > 1 THEN
                    cp% = cp% + 1
                END IF
              
            CASE CTRLY
                IF rp% < high% THEN
                  ' Shuffle lines up, spacing out the last
                    FOR i% = rp% TO high%
                        IF i% < high% THEN
                            x$(i%) = x$(i% + 1)
                        ELSE
                            x$(i%) = SPACE$(wide%)
                        END IF
                        LOCATE r1% + i% - 1, c1%, 0
                        PRINT x$(i%);
                    NEXT i%
                END IF
              
              ' Move cursor to far left
                cp% = 1
              
            CASE CTRLQ
                ctrlQflag% = TRUE
              
            CASE ESCAPE
                quit% = TRUE
              
            CASE IS > 255
                SOUND 999, 1
              
            CASE IS < 32
                SOUND 999, 1
              
            CASE ELSE
                kee$ = CHR$(keyNumber%)
              
              ' Insert mode
                IF insert% THEN
                    InsertCharacter x$(), kee$, rp%, cp%, wide%, high%
                    FOR i% = 1 TO high%
                        LOCATE r1% + i% - 1, c1%, 0
                        PRINT x$(i%);
                    NEXT i%
                  
              ' Must be overstrike mode
                ELSE
                    MID$(x$(rp%), cp%, 1) = kee$
                    IF cp% < wide% + 1 THEN
                        cp% = cp% + 1
                    ELSE
                        IF rp% < high% THEN
                            LOCATE r1% + rp% - 1, c1%, 0
                            PRINT x$(rp%);
                            rp% = rp% + 1
                            cp% = 1
                        END IF
                    END IF
                END IF
              
              ' Correct for bottom right corner problem
                IF rp% > high% THEN
                    cp% = wide%
                    rp% = high%
                END IF
              
              ' Check for Ctrl-Q-Y combination (del to end of line)
                IF kee$ = "y" AND ctrlQflag% THEN
                    cp% = cp% - 1
                    IF cp% = 0 THEN
                        cp% = wide%
                        rp% = rp% - 1
                    END IF
                    sp% = wide% - cp% + 1
                    MID$(x$(rp%), cp%, sp%) = SPACE$(sp%)
                END IF
              
              ' Clear out the possible Ctrl-Q signal
                ctrlQflag% = FALSE
              
            END SELECT
          
        LOOP UNTIL quit%
      
      ' Concatenate the array strings to form the result
        a$ = ""
        FOR i% = 1 TO high%
            a$ = a$ + " " + LTRIM$(RTRIM$(x$(i%)))
        NEXT i%
      
      ' Remove double spaces
        ds% = INSTR(a$, "  ")
        DO WHILE ds%
            a$ = LEFT$(a$, ds% - 1) + MID$(a$, ds% + 1)
            ds% = INSTR(a$, "  ")
        LOOP
      
      ' Trim both ends of spaces
        a$ = LTRIM$(RTRIM$(a$))
      
      ' Restore original cursor position
        LOCATE down%, across%, 1
      
    END SUB

  ' ************************************************
  ' **  Name:          EditLine                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        EDIT.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Allows the user to edit a string at the current cursor position
  ' on the screen.  Keys acted upon are Ctrl-Y, Ctrl-Q-Y, Right
  ' arrow, Left arrow, Ctrl-Left arrow, Ctrl-Right arrow, Home, End,
  ' Insert, Escape, Backspace, and Delete.
  ' Pressing Enter, Up arrow, or Down arrow terminates
  ' the subprogram and returns exitCode% of 0, +1, or -1.
  '
  ' EXAMPLE OF USE:  EditLine a$, exitCode%
  ' PARAMETERS:      a$         String to be edited
  '                  exitCode%  Returned code indicating the terminating
  '                             key press
  ' VARIABLES:       row%       Saved current cursor row
  '                  col%       Saved current cursor column
  '                  length%    Length of a$
  '                  ptr%       Location of cursor during the editing
  '                  insert%    Insert mode toggle
  '                  quit%      Flag for quitting the editing
  '                  original$  Saved copy of starting a$
  '                  keyNumber% Integer code for any key press
  '                  ctrlQflag% Indicates Ctrl-Q key press
  '                  kee$       Character of key just pressed
  '                  sp%        Length of space string
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION KeyCode% ()
  '                  DECLARE SUB EditLine (a$, exitCode%)
  '
    SUB EditLine (a$, exitCode%) STATIC
      
      ' Set up some variables
        row% = CSRLIN
        col% = POS(0)
        length% = LEN(a$)
        ptr% = 0
        insert% = TRUE
        quit% = FALSE
        original$ = a$
      
      ' Main processing loop
        DO
          
          ' Display the line
            LOCATE row%, col%, 0
            PRINT a$;
          
          ' Show appropriate cursor type
            IF insert% THEN
                LOCATE row%, col% + ptr%, 1, 6, 7
            ELSE
                LOCATE row%, col% + ptr%, 1, 1, 7
            END IF
          
          ' Get next keystroke
            keyNumber% = KeyCode%
          
          ' Process the key
            SELECT CASE keyNumber%
              
            CASE INSERTKEY
                IF insert% THEN
                    insert% = FALSE
                ELSE
                    insert% = TRUE
                END IF
              
            CASE BACKSPACE
                IF ptr% THEN
                    a$ = a$ + " "
                    a$ = LEFT$(a$, ptr% - 1) + MID$(a$, ptr% + 1)
                    ptr% = ptr% - 1
                END IF
              
            CASE DELETE
                a$ = a$ + " "
                a$ = LEFT$(a$, ptr%) + MID$(a$, ptr% + 2)
              
            CASE UPARROW
                exitCode% = 1
                quit% = TRUE
              
            CASE DOWNARROW
                exitCode% = -1
                quit% = TRUE
              
            CASE LEFTARROW
                IF ptr% THEN
                    ptr% = ptr% - 1
                END IF
              
            CASE RIGHTARROW
                IF ptr% < length% - 1 THEN
                    ptr% = ptr% + 1
                END IF
              
            CASE ENTER
                exitCode% = 0
                quit% = TRUE
              
            CASE HOME
                ptr% = 0
              
            CASE ENDKEY
                ptr% = length% - 1
              
            CASE CTRLRIGHTARROW
                DO UNTIL MID$(a$, ptr% + 1, 1) = " " OR ptr% = length% - 1
                    ptr% = ptr% + 1
                LOOP
                DO UNTIL MID$(a$, ptr% + 1, 1) <> " " OR ptr% = length% - 1
                    ptr% = ptr% + 1
                LOOP
              
            CASE CTRLLEFTARROW
                DO UNTIL MID$(a$, ptr% + 1, 1) = " " OR ptr% = 0
                    ptr% = ptr% - 1
                LOOP
                DO UNTIL MID$(a$, ptr% + 1, 1) <> " " OR ptr% = 0
                    ptr% = ptr% - 1
                LOOP
                DO UNTIL MID$(a$, ptr% + 1, 1) = " " OR ptr% = 0
                    ptr% = ptr% - 1
                LOOP
                IF ptr% THEN
                    ptr% = ptr% + 1
                END IF
              
            CASE CTRLY
                a$ = SPACE$(length%)
                ptr% = 0
              
            CASE CTRLQ
                ctrlQflag% = TRUE
              
            CASE ESCAPE
                a$ = original$
                ptr% = 0
                insert% = TRUE
              
            CASE IS > 255
                SOUND 999, 1
              
            CASE IS < 32
                SOUND 999, 1
              
            CASE ELSE
              
              ' Convert key code to character string
                kee$ = CHR$(keyNumber%)
              
              ' Insert or overstrike
                IF insert% THEN
                    a$ = LEFT$(a$, ptr%) + kee$ + MID$(a$, ptr% + 1)
                    a$ = LEFT$(a$, length%)
                ELSE
                    IF ptr% < length% THEN
                        MID$(a$, ptr% + 1, 1) = kee$
                    END IF
                END IF
              
              ' Are we up against the wall?
                IF ptr% < length% THEN
                    ptr% = ptr% + 1
                ELSE
                    SOUND 999, 1
                END IF
              
              ' Special check for Ctrl-Q-Y (del to end of line)
                IF kee$ = "y" AND ctrlQflag% THEN
                    IF ptr% <= length% THEN
                        sp% = length% - ptr% + 1
                        MID$(a$, ptr%, sp%) = SPACE$(sp%)
                        ptr% = ptr% - 1
                    END IF
                END IF
              
              ' Clear out the Ctrl-Q signal
                ctrlQflag% = FALSE
              
            END SELECT
          
        LOOP UNTIL quit%
      
    END SUB

  ' ************************************************
  ' **  Name:          FormatTwo                  **
  ' **  Type:          Subprogram                 **
  ' **  Module:        EDIT.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Splits a string into two strings between words,
  ' and with spaces padded to the first string to bring it to
  ' length, col%.
  '
  ' EXAMPLE OF USE:  FormatTwo a$, b$, col%
  ' PARAMETERS:      a$         String to be split
  '                  b$         Returns the tail of the string
  '                  col%       Maximum length of a$ after being split
  ' VARIABLES:       ptr%       Pointer to split point in a$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB FormatTwo (a$, b$, col%)
  '
    SUB FormatTwo (a$, b$, col%) STATIC
      
      ' Be sure string is long enough
        a$ = a$ + SPACE$(col%)
      
      ' Look for rightmost space
        ptr% = col% + 1
        DO WHILE MID$(a$, ptr%, 1) <> " " AND ptr% > 1
            ptr% = ptr% - 1
        LOOP
      
      ' Do the split
        IF ptr% = 1 THEN
            b$ = MID$(a$, col% + 1)
            a$ = LEFT$(a$, col%)
        ELSE
            b$ = MID$(a$, ptr% + 1)
            a$ = LEFT$(a$, ptr% - 1)
        END IF
      
      ' Pad the first string with spaces to length col%
        a$ = LEFT$(a$ + SPACE$(col%), col%)
      
      ' Trim extra spaces from end of second string
        b$ = RTRIM$(b$)
      
    END SUB

  ' ************************************************
  ' **  Name:          InsertCharacter            **
  ' **  Type:          Subprogram                 **
  ' **  Module:        EDIT.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Handles the task of inserting a character
  ' for the EditBox subprogram.
  '
  ' EXAMPLE OF USE:  InsertCharacter x$(), kee$, rp%, cp%, wide%, high%
  ' PARAMETERS:      x$()       Array in EditBox where character is to be
  '                             inserted
  '                  kee$       Character to be inserted
  '                  rp%        Row location of insert
  '                  cp%        Column location of insert
  '                  wide%      Width of rectangular area being edited
  '                  high%      Height of rectangular area being edited
  ' VARIABLES:       dum$       Marker character
  '                  b$         String from array at insertion point
  '                  c$         Right part of string at insertion point
  '                  i%         Looping index
  '                  ds%        Position in string of double spaces
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB InsertCharacter (x$(), kee$, cp%, rp%,
  '                                               wide%, high%)
  '
    SUB InsertCharacter (x$(), kee$, rp%, cp%, wide%, high%) STATIC
      
      ' First, insert a dummy character as a marker
        dum$ = CHR$(255)
        b$ = LEFT$(x$(rp%), cp% - 1)
        c$ = MID$(x$(rp%), cp%)
        b$ = b$ + dum$ + c$
      
      ' If end of string is a space, then drop it
        IF RIGHT$(b$, 1) = " " THEN
            x$(rp%) = LEFT$(b$, wide%)
          
      ' Otherwise, need to adjust the lines
        ELSE
          
          ' If not in the last line, then tack them all together
            IF rp% < high% THEN
                FOR i% = rp% + 1 TO high%
                    b$ = b$ + " " + x$(i%)
                NEXT i%
            END IF
          
          ' Trim both ends
            b$ = LTRIM$(RTRIM$(b$))
          
          ' Remove all double spaces
            ds% = INSTR(b$, "  ")
            DO WHILE ds%
                b$ = LEFT$(b$, ds% - 1) + MID$(b$, ds% + 1)
                ds% = INSTR(b$, "  ")
            LOOP
          
          ' Reformat the lines
            FOR i% = rp% TO high%
                FormatTwo b$, c$, wide%
                x$(i%) = b$
                b$ = c$
            NEXT i%
          
        END IF
      
      ' Find out where that dummy character is now
        FOR rp% = 1 TO high%
            cp% = INSTR(x$(rp%), dum$)
            IF cp% THEN
                EXIT FOR
            END IF
        NEXT rp%
      
      ' Replace the dummy character with the keystroke character
        IF cp% THEN
            MID$(x$(rp%), cp%, 1) = kee$
        END IF
      
      ' Increment the cursor location
        IF cp% < wide% + 1 THEN
            cp% = cp% + 1
        ELSE
            IF rp% < high% THEN
                cp% = 1
                rp% = rp% + 1
            END IF
        END IF
      
    END SUB


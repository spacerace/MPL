  ' ************************************************
  ' **  Name:          MOUSTCRS                   **
  ' **  Type:          Program                    **
  ' **  Module:        MOUSTCRS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' USAGE:           No command line parameters
  ' REQUIREMENTS:    MIXED.QLB/.LIB
  '                  Mouse
  ' .MAK FILE:       MOUSTCRS.BAS
  '                  MOUSSUBS.BAS
  '                  BITS.BAS
  '                  ATTRIB.BAS
  ' PARAMETERS:      (none)
  ' VARIABLES:       screenMask%   Integer bit mask for screen mask
  '                  cursorMask%   Integer bit mask for cursor mask
  '                  leftCount%    Count of left mouse button presses
  '                  xm%           Mouse X position at last left button press
  '                  ym%           Mouse Y position at last left button press
  '                  row%          Code for which screen bit row was selected
  '                  bit%          Bit pattern determined by screen column
  '                                click on
  '                  screenMask$   String of 0s and 1s for bit pattern display
  '                  cursorMask$   String of 0s and 1s for bit pattern display
  '                  i%            Looping index
  '                  Shex$         Hexadecimal representation of the screen mask
  '                  Chex$         Hexadecimal representation of the cursor mask
  
  ' Define constants
    CONST FALSE = 0
    CONST TRUE = NOT FALSE
  
  ' Functions
    DECLARE FUNCTION Bin2BinStr$ (b%)
  
  ' Subprograms
    DECLARE SUB Attrib ()
    DECLARE SUB MouseHide ()
    DECLARE SUB MouseInstall (mouseFlag%)
    DECLARE SUB MousePressLeft (leftCount%, xMouse%, yMouse%)
    DECLARE SUB MouseShow ()
    DECLARE SUB MouseSoftCursor (screenMask%, cursorMask%)
  
  ' Is the mouse out there?
    MouseInstall mouseFlag%
    IF mouseFlag% = 0 THEN
        PRINT "Mouse does not appear to be installed.  Check"
        PRINT "your mouse documentation for proper installation."
        PRINT
        SYSTEM
    END IF
  
  ' Put all attributes on the screen
    Attrib
  
  ' Set masks to initial state
    screenMask% = &H77FF
    cursorMask% = &H7700
  
  ' Create the outlined boxes
    COLOR 14, 0
    PRINT "                  +---+-------+---+--------+----------+--------+"
    PRINT "                  | b | bckgd | i | foregd |   char   |   =    |"
    PRINT "    +-------------+---+-------+---+--------+----------+--------+"
    PRINT "    | screen mask | 0 |  111  | 0 |  111   | 11111111 | &H77FF |"
    PRINT "    | cursor mask | 0 |  111  | 0 |  111   | 00000000 | &H7700 |"
    PRINT "    +-------------+---+-------+---+--------+----------+--------+"
  
  ' Print the instructions
    COLOR 11, 0
    PRINT "Click the mouse on any of the mask bits shown.  Then, try the"
    PRINT "new cursor by moving across the attribute fields above.";
  
  ' Special indication for quitting
    COLOR 15, 0
    LOCATE 17, 1, 0
    PRINT "Click here";
    LOCATE 18, 1, 0
    PRINT "to Quit - ";
    COLOR 10, 0
    PRINT "X";
  
  ' Put mask bits into boxes on screen
    GOSUB PrintScreenMask
    GOSUB PrintCursorMask
  
  ' Activate the mouse
    MouseShow
  
  ' Do the main processing loop until the quit flag is set
    DO
        GOSUB MainLoop
    LOOP UNTIL quitFlag%
  
  ' All done
    MouseHide
    CLS
    SYSTEM
  
  
  ' Main processing loop
MainLoop:
  
  ' Where was mouse when left button was last pressed?
    MousePressLeft leftCount%, xm%, ym%
  
  ' Was it on one of the two important rows of the screen?
    SELECT CASE ym%
    CASE 152
        row% = 1
    CASE 160
        row% = 2
    CASE ELSE
        row% = 0
    END SELECT
  
  ' Was it on an important column?
    SELECT CASE xm%
    CASE 80
        IF ym% = 136 THEN
            quitFlag% = TRUE
        END IF
    CASE 160
        bit% = &H8000
    CASE 200
        bit% = &H4000
    CASE 208
        bit% = &H2000
    CASE 216
        bit% = &H1000
    CASE 256
        bit% = &H800
    CASE 296
        bit% = &H400
    CASE 304
        bit% = &H200
    CASE 312
        bit% = &H100
    CASE 360
        bit% = &H80
    CASE 368
        bit% = &H40
    CASE 376
        bit% = &H20
    CASE 384
        bit% = &H10
    CASE 392
        bit% = &H8
    CASE 400
        bit% = &H4
    CASE 408
        bit% = &H2
    CASE 416
        bit% = &H1
    CASE ELSE
        bit% = 0
    END SELECT
  
  ' Modify the masks and update the cursor
    IF leftCount% THEN
        SELECT CASE row%
        CASE 1
            screenMask% = screenMask% XOR bit%
        CASE 2
            cursorMask% = cursorMask% XOR bit%
        CASE ELSE
        END SELECT
        MouseSoftCursor screenMask%, cursorMask%
        GOSUB PrintScreenMask
        GOSUB PrintCursorMask
    END IF
  
  ' End of main processing loop
    RETURN
  
  ' Put screen mask bits on the screen
PrintScreenMask:
    COLOR 12, 0
    screenMask$ = ""
    screenMask$ = Bin2BinStr$(screenMask%)
    MouseHide
    FOR i% = 0 TO 15
        SELECT CASE i%
        CASE 0 TO 7
            LOCATE 20, 53 - i%, 0
            PRINT MID$(screenMask$, 16 - i%, 1);
        CASE 8 TO 10
            LOCATE 20, 48 - i%, 0
            PRINT MID$(screenMask$, 16 - i%, 1);
        CASE 11
            LOCATE 20, 44 - i%, 0
            PRINT MID$(screenMask$, 16 - i%, 1);
        CASE 12 TO 14
            LOCATE 20, 40 - i%, 0
            PRINT MID$(screenMask$, 16 - i%, 1);
        CASE 15
            LOCATE 20, 36 - i%, 0
            PRINT MID$(screenMask$, 16 - i%, 1);
        CASE ELSE
        END SELECT
    NEXT i%
    shex$ = "&H" + RIGHT$("000" + HEX$(screenMask%), 4)
    LOCATE 20, 57, 0
    COLOR 10, 0
    PRINT shex$;
    MouseShow
    RETURN
  
  ' Put cursor mask bits on the screen
PrintCursorMask:
    COLOR 12, 0
    cursorMask$ = ""
    cursorMask$ = Bin2BinStr$(cursorMask%)
    MouseHide
    FOR i% = 0 TO 15
        SELECT CASE i%
        CASE 0 TO 7
            LOCATE 21, 53 - i%, 0
            PRINT MID$(cursorMask$, 16 - i%, 1);
        CASE 8 TO 10
            LOCATE 21, 48 - i%, 0
            PRINT MID$(cursorMask$, 16 - i%, 1);
        CASE 11
            LOCATE 21, 44 - i%, 0
            PRINT MID$(cursorMask$, 16 - i%, 1);
        CASE 12 TO 14
            LOCATE 21, 40 - i%, 0
            PRINT MID$(cursorMask$, 16 - i%, 1);
        CASE 15
            LOCATE 21, 36 - i%, 0
            PRINT MID$(cursorMask$, 16 - i%, 1);
        CASE ELSE
        END SELECT
    NEXT i%
    chex$ = "&H" + RIGHT$("000" + HEX$(cursorMask%), 4)
    LOCATE 21, 57, 0
    COLOR 10, 0
    PRINT chex$;
    MouseShow
    RETURN
  


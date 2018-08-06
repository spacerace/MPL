  ' ************************************************
  ' **  Name:          MOUSGCRS                   **
  ' **  Type:          Program                    **
  ' **  Module:        MOUSGCRS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Program for the interactive design of graphics-
  ' mode mouse cursor subroutines.
  '
  ' USAGE:          No command line parameters
  ' REQUIREMENTS:   CGA
  '                 MIXED.QLB/.LIB
  '                 Mouse
  ' .MAK FILE:      MOUSGCRS.BAS
  '                 BITS.BAS
  '                 MOUSSUBS.BAS
  ' PARAMETERS:     (none)
  ' VARIABLES:      curs$()       Array of binary cursor string data
  '                 defaultMask$  Pattern mask for the default cursor
  '                 xdef%         Default hot spot X value
  '                 ydef%         Default hot spot Y value
  '                 mask$         Pattern mask for a cursor
  '                 xHot%         Hot spot X value
  '                 yHot%         Hot spot Y value
  '                 maskChr%      Index into the pattern mask
  '                 maskPtr%      Index to the background or foreground mask
  '                               pattern
  '                 y%            Cursor bit pointer, vertical
  '                 x%            Cursor bit pointer, horizontal
  '                 xbox%         X location on screen for cursor bit box
  '                 ybox%         Y location on screen for cursor bit box
  '                 xh%           Screen X location for hot spot
  '                 yh%           Screen Y location for hot spot
  '                 click$        DRAW string for creating the click boxes
  '                 quitFlag%     Indication that user wants to quit
  '                 t$            Copy of TIME$
  '                 toggle%       Once per second toggle for hot spot visibility
  '                 pxl%          Pixel value at the hot spot
  '                 leftButton%   Current state of the left mouse button
  '                 rightButton%  Current state of the right mouse button
  '                 resetBox%     Indicates cursor is in the "Try standard
  '                               cursors" box
  '                 tryBox%       Indicates cursor is in the "Try new cursor"
  '                               box
  '                 subBox%       Indicates cursor is in the "Create cursor
  '                               subroutine" box
  '                 quitBox%      Indicates cursor is in the "Quit" box
  '                 xold%         X location of just-modified pixel box
  '                 yold%         Y location of just-modified pixel box
  '                 ix%           X bit pointer for pixel change
  '                 iy%           Y bit pointer for pixel change
  '                 q$            Double-quote character
  
  ' Define constants
    CONST FALSE = 0
    CONST TRUE = NOT FALSE
  
  ' Subprograms
    DECLARE SUB Cursdflt (mask$, xHot%, yHot%)
    DECLARE SUB Curschek (mask$, xHot%, yHot%)
    DECLARE SUB Curshand (mask$, xHot%, yHot%)
    DECLARE SUB Curshour (mask$, xHot%, yHot%)
    DECLARE SUB Cursjet (mask$, xHot%, yHot%)
    DECLARE SUB Cursleft (mask$, xHot%, yHot%)
    DECLARE SUB Cursplus (mask$, xHot%, yHot%)
    DECLARE SUB Cursup (mask$, xHot%, yHot%)
    DECLARE SUB Cursx (mask$, xHot%, yHot%)
    DECLARE SUB MouseShow ()
    DECLARE SUB MouseNow (lbutton%, rbutton%, xMouse%, yMouse%)
    DECLARE SUB MouseHide ()
    DECLARE SUB MouseMaskTranslate (mask$, xHot%, yHot%, cursor$)
    DECLARE SUB MouseSetGcursor (cursor$)
  
  ' Arrays
    DIM curs$(0 TO 8)
  
  ' Initialization
    SCREEN 2
    CLS
  
  ' Create set of cursors
    Cursdflt defaultMask$, xdef%, ydef%
    MouseMaskTranslate defaultMask$, xdef%, ydef%, curs$(0)
  
    Curschek mask$, xHot%, yHot%
    MouseMaskTranslate mask$, xHot%, yHot%, curs$(1)
  
    Curshand mask$, xHot%, yHot%
    MouseMaskTranslate mask$, xHot%, yHot%, curs$(2)
  
    Curshour mask$, xHot%, yHot%
    MouseMaskTranslate mask$, xHot%, yHot%, curs$(3)
  
    Cursjet mask$, xHot%, yHot%
    MouseMaskTranslate mask$, xHot%, yHot%, curs$(4)
  
    Cursleft mask$, xHot%, yHot%
    MouseMaskTranslate mask$, xHot%, yHot%, curs$(5)
  
    Cursplus mask$, xHot%, yHot%
    MouseMaskTranslate mask$, xHot%, yHot%, curs$(6)
  
    Cursup mask$, xHot%, yHot%
    MouseMaskTranslate mask$, xHot%, yHot%, curs$(7)
  
    Cursx mask$, xHot%, yHot%
    MouseMaskTranslate mask$, xHot%, yHot%, curs$(8)
  
  ' Set the default cursor
    MouseSetGcursor curs$(0)
  
  ' Make the default cursor the starting point for editing
    mask$ = defaultMask$
    xHot% = xdef%
    yHot% = ydef%
  
  ' Place titles above pixel boxes
    LOCATE 2, 22, 0
    PRINT "Screen mask";
    LOCATE 2, 50, 0
    PRINT "Cursor mask";
  
  ' Outline the pixel boxes, filling the "ones" using the Mask$
    maskChr% = 0
    FOR maskPtr% = 0 TO 1
        FOR y% = 1 TO 16
            FOR x% = 1 TO 16
                xbox% = x% * 12 + maskPtr% * 222 + 107
                ybox% = y% * 9 + 10
                maskChr% = maskChr% + 1
                LINE (xbox%, ybox%)-(xbox% + 12, ybox% + 9), 1, B
                IF MID$(mask$, maskChr%, 1) = "1" THEN
                    LINE (xbox% + 3, ybox% + 2)-(xbox% + 9, ybox% + 7), 1, BF
                END IF
                IF maskPtr% = 0 THEN
                    IF x% = xHot% + 1 AND y% = yHot% + 1 THEN
                        xh% = xbox%
                        yh% = ybox%
                    END IF
                END IF
            NEXT x%
        NEXT y%
    NEXT maskPtr%
  
  ' Instruction text at bottom of display
    LOCATE 23, 1
    PRINT TAB(16); "Left button       Right button         Both buttons"
    PRINT TAB(16); "to set pixel      to clear pixel       for hot spot";
  
  ' Print menu items
    LOCATE 3, 2, 0
    PRINT "Try";
    LOCATE 4, 2, 0
    PRINT "standard";
    LOCATE 5, 2, 0
    PRINT "cursors";
    LOCATE 9, 2, 0
    PRINT "Try new";
    LOCATE 10, 2, 0
    PRINT "cursor";
    LOCATE 14, 2, 0
    PRINT "Create"
    LOCATE 15, 2, 0
    PRINT "cursor";
    LOCATE 16, 2, 0
    PRINT "subroutine";
    LOCATE 16, 74, 0
    PRINT "Quit";
  
  ' Make click box draw string
    click$ = "R20D10L20U10BF5BR1F3E6"
  
  ' Draw the click boxes
    DRAW "BM20,45" + click$
    DRAW "BM20,85" + click$
    DRAW "BM20,132" + click$
    DRAW "BM592,132" + click$
  
  ' Make a white cursor testing area
    LOCATE 5, 71
    PRINT "Cursor";
    LOCATE 6, 71
    PRINT "viewing";
    LOCATE 7, 71
    PRINT "area";
    LINE (560, 60)-(610, 100), 1, BF
  
  ' Turn on the mouse
    MouseShow
  
  ' Main processing loop control
    DO
        GOSUB MainLoop
    LOOP UNTIL quitFlag%
  
  ' Exit the loop and end program because Quitflag% has been set
    CLS
    SYSTEM
  
  
  ' Main processing loop
MainLoop:
  
  ' Toggle the hot spot once per second
    IF t$ <> TIME$ THEN
        t$ = TIME$
        IF toggle% = 1 THEN
            toggle% = 0
        ELSE
            toggle% = 1
        END IF
        pxl% = POINT(xh% + 3, yh% + 2) XOR toggle%
        LINE (xh% + 5, yh% + 3)-(xh% + 7, yh% + 6), pxl%, BF
        pxl% = POINT(xh% + 3 + 222, yh% + 2) XOR toggle%
        LINE (xh% + 5 + 222, yh% + 3)-(xh% + 7 + 222, yh% + 6), pxl%, BF
    END IF
  
  ' What is the mouse location and button state right now?
    MouseNow leftButton%, rightButton%, x%, y%
  
  ' Are both buttons being pressed right now?
    IF leftButton% AND rightButton% THEN
        GOSUB WhichBox
        IF xbox% THEN
            GOSUB SetHotSpot
        END IF
    END IF
  
  ' Are we traversing the "Try standard cursors" click box?
    IF x% > 20 AND x% < 40 AND y% > 45 AND y% < 55 THEN
        IF resetBox% = 0 THEN
            MouseHide
            resetBox% = 1
            LINE (17, 43)-(43, 57), 1, B
            MouseShow
        END IF
    ELSE
        IF resetBox% = 1 THEN
            MouseHide
            resetBox% = 0
            LINE (17, 43)-(43, 57), 0, B
            MouseShow
        END IF
    END IF
  
  ' Are we traversing the "Try new cursor" click box?
    IF x% > 20 AND x% < 40 AND y% > 85 AND y% < 95 THEN
        IF tryBox% = 0 THEN
            MouseHide
            tryBox% = 1
            LINE (17, 83)-(43, 97), 1, B
            MouseShow
        END IF
    ELSE
        IF tryBox% = 1 THEN
            MouseHide
            tryBox% = 0
            LINE (17, 83)-(43, 97), 0, B
            MouseShow
        END IF
    END IF
  
  ' Are we traversing the "Create cursor subroutine" click box?
    IF x% > 20 AND x% < 40 AND y% > 132 AND y% < 142 THEN
        IF subBox% = 0 THEN
            MouseHide
            subBox% = 1
            LINE (17, 130)-(43, 144), 1, B
            MouseShow
        END IF
    ELSE
        IF subBox% = 1 THEN
            MouseHide
            subBox% = 0
            LINE (17, 130)-(43, 144), 0, B
            MouseShow
        END IF
    END IF
  
  ' Are we traversing the "Quit" click box?
    IF x% > 592 AND x% < 612 AND y% > 132 AND y% < 142 THEN
        IF quitBox% = 0 THEN
            MouseHide
            quitBox% = 1
            LINE (589, 130)-(615, 144), 1, B
            MouseShow
        END IF
    ELSE
        IF quitBox% = 1 THEN
            MouseHide
            quitBox% = 0
            LINE (589, 130)-(615, 144), 0, B
            MouseShow
        END IF
    END IF
  
  ' If just one button or the other is pressed, then check further
    IF leftButton% XOR rightButton% THEN
        GOSUB ButtonWasPressed
    ELSE
        xold% = 0
        yold% = 0
    END IF
  
  ' End of main loop
    RETURN
  
  
  ' Is the mouse currently pointing at a pixel box?
WhichBox:
    IF x% > 320 THEN
        maskPtr% = 1
        x% = x% - 222
    ELSE
        maskPtr% = 0
    END IF
    ix% = (x% - 107) \ 12
    iy% = (y% - 10) \ 9
    xbox% = 0
    ybox% = 0
    IF ix% >= 1 AND ix% <= 16 THEN
        IF iy% >= 1 AND iy% <= 16 THEN
            xbox% = ix% * 12 + maskPtr% * 222 + 107
            ybox% = iy% * 9 + 10
        END IF
    END IF
    RETURN
  
  
  ' Move the hot spot to the current pixel box
SetHotSpot:
    IF (xbox% <> xh% AND xbox% - 222 <> xh%) OR ybox% <> yh% THEN
        MouseHide
        pxl% = POINT(xh% + 3, yh% + 2)
        LINE (xh% + 5, yh% + 3)-(xh% + 7, yh% + 6), pxl%, BF
        pxl% = POINT(xh% + 3 + 222, yh% + 2)
        LINE (xh% + 5 + 222, yh% + 3)-(xh% + 7 + 222, yh% + 6), pxl%, BF
        MouseShow
        IF xbox% > 320 THEN
            xh% = xbox% - 222
        ELSE
            xh% = xbox%
        END IF
        yh% = ybox%
    END IF
    RETURN
  
  
  ' Process the button press depending on mouse location
ButtonWasPressed:
    IF quitBox% THEN
        GOSUB DoQuitBox
    ELSEIF resetBox% THEN
        GOSUB DoResetCursor
    ELSEIF tryBox% THEN
        GOSUB DoSetNewCursor
    ELSEIF subBox% THEN
        GOSUB DoSetNewCursor
        GOSUB DoCreateSub
    ELSE
        GOSUB DoPixelControl
    END IF
    RETURN
  
  
  ' Button was pressed while mouse was in the "Quit" box
DoQuitBox:
    MouseHide
    quitFlag% = TRUE
    RETURN
  
  
  ' Button was pressed while mouse was in the "Try new cursor" box
DoSetNewCursor:
    MouseHide
    maskChr% = 0
    FOR maskPtr% = 0 TO 1
        FOR y% = 1 TO 16
            FOR x% = 1 TO 16
                xbox% = x% * 12 + maskPtr% * 222 + 107
                ybox% = y% * 9 + 10
                maskChr% = maskChr% + 1
                IF POINT(xbox% + 3, ybox% + 2) THEN
                    MID$(mask$, maskChr%, 1) = "1"
                ELSE
                    MID$(mask$, maskChr%, 1) = "0"
                END IF
                IF xbox% = xh% AND ybox% = yh% THEN
                    xHot% = x% - 1
                    yHot% = y% - 1
                END IF
            NEXT x%
        NEXT y%
    NEXT maskPtr%
    MouseMaskTranslate mask$, xHot%, yHot%, cursor$
    MouseSetGcursor cursor$
    MouseShow
    RETURN
  
  ' Button was pressed while mouse was in the "Try standard cursors" box
DoResetCursor:
    MouseHide
    cursorIndex% = (cursorIndex% + 1) MOD 9
    MouseSetGcursor curs$(cursorIndex%)
    MouseShow
    DO
        MouseNow leftButton%, rightButton%, xMouse%, yMouse%
    LOOP UNTIL leftButton% = 0 AND rightButton% = 0
    RETURN
  
  ' Button was pressed while mouse was in the "Create cursor subroutine" box
DoCreateSub:
    q$ = CHR$(34)
    OPEN "GCURSOR.BAS" FOR OUTPUT AS #1
    PRINT #1, "   ' ************************************************"
    PRINT #1, "   ' **  Name:          Gcursor                    **"
    PRINT #1, "   ' **  Type:          Subprogram                 **"
    PRINT #1, "   ' **  Module:        GCURSOR.BAS                **"
    PRINT #1, "   ' **  Language:      Microsoft QuickBASIC 4.00  **"
    PRINT #1, "   ' ************************************************"
    PRINT #1, "   '"
    PRINT #1, "   SUB Gcursor (mask$, xHot%, yHot%) STATIC"
    PRINT #1, ""
    PRINT #1, "       mask$ = "; q$; q$
    FOR i% = 0 TO 31
        PRINT #1, "       mask$ = mask$ + ";
        PRINT #1, q$; MID$(mask$, 16 * i% + 1, 16); q$
        IF i% = 15 THEN
            PRINT #1, ""
        END IF
    NEXT i%
    PRINT #1, ""
    PRINT #1, "       xHot% ="; STR$(xHot%)
    PRINT #1, "       yHot% ="; STR$(yHot%)
    PRINT #1, ""
    PRINT #1, "   END SUB"
    RETURN
  
  
  ' Set or clear pixel box if mouse is on one
DoPixelControl:
    GOSUB WhichBox
    IF xbox% THEN
        IF xold% <> xbox% OR yold% <> ybox% THEN
            xold% = xbox%
            yold% = ybox%
            MouseHide
            IF leftButton% THEN
                LINE (xbox% + 3, ybox% + 2)-(xbox% + 9, ybox% + 7), 1, BF
            ELSE
                LINE (xbox% + 3, ybox% + 2)-(xbox% + 9, ybox% + 7), 0, BF
            END IF
            MouseShow
        END IF
    END IF
    RETURN
  


  ' ************************************************
  ' **  Name:          MOUSSUBS                   **
  ' **  Type:          Toolbox                    **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Collection of subprograms for using the Microsoft Mouse.
  '
  ' Note:         The assembly-language subroutine named MOUSE.ASM
  '               must be assembled and linked with these routines
  '               or included in the user library loaded with
  '               QuickBASIC.
  ' USAGE:        No command line parameters
  ' REQUIREMENTS: CGA
  '               MIXED.QLB/.LIB
  '               Mouse
  '.MAK FILE:     MOUSSUBS.BAS
  '               BITS.BAS
  ' PARAMETERS:   (none)
  ' VARIABLES:    i%            Looping index
  '               mask$         Pattern mask for each graphics mouse cursor
  '               xHot%         X hot spot location
  '               yHot%         Y hot spot location
  '               curs$         Binary bit pattern for defining mouse cursor
  '               j%            Test for left mouse button press and release
  '               leftButton%   State of left mouse button
  '               rightButton%  State of right mouse button
  '               xMouse%       X location of mouse
  '               yMouse%       Y location of mouse
  '               mflag%        Indicates mouse is available
  '               horizontal%   Horizontal mouse mickies
  '               vertical%     Vertical mouse mickies
  '               xpLeft%       X location of last left button press
  '               ypLeft%       Y location of last left button press
  '               xrLeft%       X location of last left button release
  '               yrLeft%       Y location of last left button release
  '               xpRight%      X location of last right button press
  '               ypRight%      Y location of last right button press
  '               xrRight%      X location of last right button release
  '               yrRight%      Y location of last right button release
  '               t0            Timer value

  ' Functions
    DECLARE FUNCTION BinStr2Bin% (b$)
  
  ' Subprograms
    DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
    DECLARE SUB MouseRange (x1%, y1%, x2%, y2%)
    DECLARE SUB MousePut (xMouse%, yMouse%)
    DECLARE SUB MouseHide ()
    DECLARE SUB MouseInches (horizontal%, vertical%)
    DECLARE SUB MouseInstall (mflag%)
    DECLARE SUB MouseMickey (horizontal%, vertical%)
    DECLARE SUB MousePressLeft (leftCount%, xMouse%, yMouse%)
    DECLARE SUB MousePressRight (rightCount%, xMouse%, yMouse%)
    DECLARE SUB MouseReleaseLeft (leftCount%, xMouse%, yMouse%)
    DECLARE SUB MouseReleaseRight (rightCount%, xMouse%, yMouse%)
    DECLARE SUB MouseWarp (threshold%)
    DECLARE SUB Cursdflt (mask$, xHot%, yHot%)
    DECLARE SUB Curschek (mask$, xHot%, yHot%)
    DECLARE SUB Curshand (mask$, xHot%, yHot%)
    DECLARE SUB Curshour (mask$, xHot%, yHot%)
    DECLARE SUB Cursjet (mask$, xHot%, yHot%)
    DECLARE SUB Cursleft (mask$, xHot%, yHot%)
    DECLARE SUB Cursplus (mask$, xHot%, yHot%)
    DECLARE SUB Cursup (mask$, xHot%, yHot%)
    DECLARE SUB Cursx (mask$, xHot%, yHot%)
    DECLARE SUB MouseMaskTranslate (mask$, xHot%, yHot%, cursor$)
    DECLARE SUB MouseNow (leftButton%, rightButton%, xMouse%, yMouse%)
    DECLARE SUB MouseSetGcursor (cursor$)
    DECLARE SUB MouseShow ()
  
  ' Check for mouse
    SCREEN 2
    CLS
    MouseInstall mflag%
    PRINT "MouseInstall ... "; mflag%
  
  ' Demonstrate the available graphics-mode cursors
    PRINT
    PRINT "Click left mouse button to see the graphics-mode cursors..."
    MouseShow
  
    FOR i% = 1 TO 9
        SELECT CASE i%
        CASE 1
            Curschek mask$, xHot%, yHot%
        CASE 2
            Curshand mask$, xHot%, yHot%
        CASE 3
            Curshour mask$, xHot%, yHot%
        CASE 4
            Cursjet mask$, xHot%, yHot%
        CASE 5
            Cursleft mask$, xHot%, yHot%
        CASE 6
            Cursplus mask$, xHot%, yHot%
        CASE 7
            Cursup mask$, xHot%, yHot%
        CASE 8
            Cursx mask$, xHot%, yHot%
        CASE ELSE
            Cursdflt mask$, xHot%, yHot%
        END SELECT
        MouseMaskTranslate mask$, xHot%, yHot%, curs$
        FOR j% = -1 TO 0
            DO
                MouseNow leftButton%, rightButton%, xMouse%, yMouse%
            LOOP UNTIL leftButton% = j%
        NEXT j%
        MouseSetGcursor curs$
    NEXT i%
  
  ' Mouse hide and show
    PRINT "MouseHide ... (Press any key to continue)"
    MouseHide
    DO
    LOOP UNTIL INKEY$ <> ""
    PRINT "MouseShow ... (Press any key to continue)"
    MouseShow
    DO
    LOOP UNTIL INKEY$ <> ""
  
  ' Mouse inches per screen
    MouseHide
    PRINT
    PRINT "Setting MouseWarp to 9999 to prevent doubling of speed."
    MouseWarp 9999
    PRINT
    PRINT "Setting MouseInches to 8 by 11. (8 inches of mouse motion"
    PRINT "across desk to move across screen, and 11 inches vertical"
    PRINT "mouse motion from top to bottom of screen) ..."
    PRINT
    PRINT "Press any key to continue"
    MouseInches 8, 11
    MouseShow
    DO
    LOOP UNTIL INKEY$ <> ""
  
  ' Resetting the mouse
    MouseHide
    PRINT
    PRINT "Resetting the mouse"
    MouseInstall mflag%
  
  ' Show realtime mouse data
    CLS
    PRINT "Instantaneous mouse information (Press any key to continue)"
    MouseShow
    DO
        MouseMickey horizontal%, vertical%
        MouseNow leftButton%, rightButton%, xMouse%, yMouse%
        MousePressLeft leftCount%, xpLeft%, ypLeft%
        MouseReleaseLeft leftCount%, xrLeft%, yrLeft%
        MousePressRight rightCount%, xpRight%, ypRight%
        MouseReleaseRight rightCount%, xrRight%, yrRight%
        LOCATE 3, 1
        PRINT "Mickies       ";
        PRINT USING "#######, #######"; horizontal%, vertical%
        PRINT "Position      ";
        PRINT USING "#######, #######"; xMouse%, yMouse%
        PRINT
        PRINT "Buttons       ";
        PRINT USING "#######, #######"; leftButton%, rightButton%
        PRINT
        PRINT "Left Press    ";
        PRINT USING "#######, #######"; xpLeft%, ypLeft%
        PRINT "Left Release  ";
        PRINT USING "#######, #######"; xrLeft%, yrLeft%
        PRINT
        PRINT "Right Press   ";
        PRINT USING "#######, #######"; xpRight%, ypRight%
        PRINT "Right Release ";
        PRINT USING "#######, #######"; xrRight%, yrRight%
    LOOP UNTIL INKEY$ <> ""
  
  ' Mouse placement
    CLS
    MouseHide
    PRINT "MousePut..."
    MouseShow
    FOR i% = 1 TO 20
        xMouse% = RND * 639
        yMouse% = RND * 199
        MousePut xMouse%, yMouse%
        t0 = TIMER
        DO
        LOOP UNTIL TIMER - t0 > .2
    NEXT i%
  
  ' Range limiting
    CLS
    MouseHide
    PRINT "Range limited to a rectangular area ..."
    PRINT "Press any key to continue"
    MouseShow
    MouseRange 200, 50, 400, 100
    DO
    LOOP UNTIL INKEY$ <> ""
  
  ' All done
    SCREEN 0
    CLS
  

  ' ************************************************
  ' **  Name:          Curschek                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Defines a graphics-mode mouse cursor (check mark).
  '
  ' EXAMPLE OF USE:  Curschek mask$, xHot%, yHot%
  ' PARAMETERS:      mask$      Pattern mask for creating cursor
  '                  xHot%      X location for cursor hot spot
  '                  yHot%      Y location for cursor hot spot
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Curschek (mask$, xHot%, yHot%)
  '
    SUB Curschek (mask$, xHot%, yHot%) STATIC
      
        mask$ = ""
        mask$ = mask$ + "1111111111110000"
        mask$ = mask$ + "1111111111100000"
        mask$ = mask$ + "1111111111000000"
        mask$ = mask$ + "1111111110000001"
        mask$ = mask$ + "1111111100000011"
        mask$ = mask$ + "0000011000000111"
        mask$ = mask$ + "0000000000001111"
        mask$ = mask$ + "0000000000011111"
        mask$ = mask$ + "1100000000111111"
        mask$ = mask$ + "1111000001111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
      
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000110"
        mask$ = mask$ + "0000000000001100"
        mask$ = mask$ + "0000000000011000"
        mask$ = mask$ + "0000000000110000"
        mask$ = mask$ + "0000000001100000"
        mask$ = mask$ + "0111000011000000"
        mask$ = mask$ + "0001110110000000"
        mask$ = mask$ + "0000011100000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
      
        xHot% = 6
        yHot% = 7
      
    END SUB

  ' ************************************************
  ' **  Name:          Cursdflt                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Defines a default graphics-mode mouse cursor (arrow pointing up and left).
  '
  ' EXAMPLE OF USE:  Cursdflt mask$, xHot%, yHot%
  ' PARAMETERS:      mask$      Pattern mask for creating cursor
  '                  xHot%      X location for cursor hot spot
  '                  yHot%      Y location for cursor hot spot
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATION:   DECLARE SUB Cursdflt (mask$, xHot%, yHot%)
  '
    SUB Cursdflt (mask$, xHot%, yHot%) STATIC
      
        mask$ = ""
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1001111111111111"
        mask$ = mask$ + "1000111111111111"
        mask$ = mask$ + "1000011111111111"
        mask$ = mask$ + "1000001111111111"
        mask$ = mask$ + "1000000111111111"
        mask$ = mask$ + "1000000011111111"
        mask$ = mask$ + "1000000001111111"
        mask$ = mask$ + "1000000000111111"
        mask$ = mask$ + "1000000000011111"
        mask$ = mask$ + "1000000000001111"
        mask$ = mask$ + "1000000000000111"
        mask$ = mask$ + "1000100001111111"
        mask$ = mask$ + "1001100001111111"
        mask$ = mask$ + "1111110000111111"
        mask$ = mask$ + "1111110000111111"
      
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0010000000000000"
        mask$ = mask$ + "0011000000000000"
        mask$ = mask$ + "0011100000000000"
        mask$ = mask$ + "0011110000000000"
        mask$ = mask$ + "0011111000000000"
        mask$ = mask$ + "0011111100000000"
        mask$ = mask$ + "0011111110000000"
        mask$ = mask$ + "0011111111000000"
        mask$ = mask$ + "0011111111100000"
        mask$ = mask$ + "0011111000000000"
        mask$ = mask$ + "0010001100000000"
        mask$ = mask$ + "0000001100000000"
        mask$ = mask$ + "0000000110000000"
        mask$ = mask$ + "0000000110000000"
      
        xHot% = 1
        yHot% = 1
      
    END SUB

  ' ************************************************
  ' **  Name:          Curshand                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Defines a graphics-mode mouse cursor (pointing hand).
  '
  ' EXAMPLE OF USE:  Curshand mask$, xHot%, yHot%
  ' PARAMETERS:      mask$      Pattern mask for creating cursor
  '                  xHot%      X location for cursor hot spot
  '                  yHot%      Y location for cursor hot spot
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Curshand (mask$, xHot%, yHot%)
  '
    SUB Curshand (mask$, xHot%, yHot%) STATIC
      
        mask$ = ""
        mask$ = mask$ + "1110000111111111"
        mask$ = mask$ + "1110000111111111"
        mask$ = mask$ + "1110000111111111"
        mask$ = mask$ + "1110000111111111"
        mask$ = mask$ + "1110000111111111"
        mask$ = mask$ + "1110000000000000"
        mask$ = mask$ + "1110000000000000"
        mask$ = mask$ + "1110000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
      
        mask$ = mask$ + "0001111000000000"
        mask$ = mask$ + "0001001000000000"
        mask$ = mask$ + "0001001000000000"
        mask$ = mask$ + "0001001000000000"
        mask$ = mask$ + "0001001000000000"
        mask$ = mask$ + "0001001111111111"
        mask$ = mask$ + "0001001001001001"
        mask$ = mask$ + "0001001001001001"
        mask$ = mask$ + "1111001001001001"
        mask$ = mask$ + "1001000000000001"
        mask$ = mask$ + "1001000000000001"
        mask$ = mask$ + "1001000000000001"
        mask$ = mask$ + "1000000000000001"
        mask$ = mask$ + "1000000000000001"
        mask$ = mask$ + "1000000000000001"
        mask$ = mask$ + "1111111111111111"
      
        xHot% = 5
        yHot% = 0
      
    END SUB

  ' ************************************************
  ' **  Name:          Curshour                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Defines a graphics-mode mouse cursor (hourglass).
  '
  ' EXAMPLE OF USE:  Curshour mask$, xHot%, yHot%
  ' PARAMETERS:      mask$      Pattern mask for creating cursor
  '                  xHot%      X location for cursor hot spot
  '                  yHot%      Y location for cursor hot spot
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:    DECLARE SUB Curshour (mask$, xHot%, yHot%)
  '
    SUB Curshour (mask$, xHot%, yHot%) STATIC
      
        mask$ = ""
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "1000000000000001"
        mask$ = mask$ + "1100000000000011"
        mask$ = mask$ + "1110000000000111"
        mask$ = mask$ + "1111000000001111"
        mask$ = mask$ + "1110000000000111"
        mask$ = mask$ + "1100000000000011"
        mask$ = mask$ + "1000000000000001"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
      
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0111111111111110"
        mask$ = mask$ + "0110000000000110"
        mask$ = mask$ + "0011000000001100"
        mask$ = mask$ + "0001100000011000"
        mask$ = mask$ + "0000110000110000"
        mask$ = mask$ + "0000011001100000"
        mask$ = mask$ + "0000001111000000"
        mask$ = mask$ + "0000011001100000"
        mask$ = mask$ + "0000110000110000"
        mask$ = mask$ + "0001100110011000"
        mask$ = mask$ + "0011001111001100"
        mask$ = mask$ + "0110011111100110"
        mask$ = mask$ + "0111111111111110"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
      
        xHot% = 7
        yHot% = 7
      
    END SUB

  ' ************************************************
  ' **  Name:          Cursjet                    **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Defines a graphics-mode mouse cursor (jet aircraft).
  '
  ' EXAMPLE OF USE:  Cursjet mask$, xHot%, yHot%
  ' PARAMETERS:      mask$      Pattern mask for creating cursor
  '                  xHot%      X location for cursor hot spot
  '                  yHot%      Y location for cursor hot spot
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Cursjet (mask$, xHot%, yHot%)
  '
    SUB Cursjet (mask$, xHot%, yHot%) STATIC
      
        mask$ = ""
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111011111111"
        mask$ = mask$ + "1111110001111111"
        mask$ = mask$ + "1111100000111111"
        mask$ = mask$ + "1111100000111111"
        mask$ = mask$ + "1111100000111111"
        mask$ = mask$ + "1111000000011111"
        mask$ = mask$ + "1110000000001111"
        mask$ = mask$ + "1100000000000111"
        mask$ = mask$ + "1000000000000011"
        mask$ = mask$ + "1000000000000011"
        mask$ = mask$ + "1111100000111111"
        mask$ = mask$ + "1111100000111111"
        mask$ = mask$ + "1111000000011111"
        mask$ = mask$ + "1110000000001111"
        mask$ = mask$ + "1111111111111111"
      
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000100000000"
        mask$ = mask$ + "0000001110000000"
        mask$ = mask$ + "0000001110000000"
        mask$ = mask$ + "0000001110000000"
        mask$ = mask$ + "0000011111000000"
        mask$ = mask$ + "0000111111100000"
        mask$ = mask$ + "0001111111110000"
        mask$ = mask$ + "0011111111111000"
        mask$ = mask$ + "0110001110001100"
        mask$ = mask$ + "0000001110000000"
        mask$ = mask$ + "0000001110000000"
        mask$ = mask$ + "0000011111000000"
        mask$ = mask$ + "0000110001100000"
        mask$ = mask$ + "0000000000000000"
      
        xHot% = 7
        yHot% = 1
      
    END SUB

  ' ************************************************
  ' **  Name:          Cursleft                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Defines a graphics-mode mouse cursor (left arrow).
  '
  ' EXAMPLE OF USE:  Cursleft mask$, xHot%, yHot%
  ' PARAMETERS:      mask$      Pattern mask for creating cursor
  '                  xHot%      X location for cursor hot spot
  '                  yHot%      Y location for cursor hot spot
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:    DECLARE SUB Cursleft (mask$, xHot%, yHot%)
  '
    SUB Cursleft (mask$, xHot%, yHot%) STATIC
      
        mask$ = ""
        mask$ = mask$ + "1111111000011111"
        mask$ = mask$ + "1111000000011111"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "1111000000011111"
        mask$ = mask$ + "1111111000011111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
      
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000011000000"
        mask$ = mask$ + "0000011111000000"
        mask$ = mask$ + "0111111111111110"
        mask$ = mask$ + "0000011111000000"
        mask$ = mask$ + "0000000011000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
      
        xHot% = 0
        yHot% = 3
      
    END SUB

  ' ************************************************
  ' **  Name:          Cursplus                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Defines a graphics-mode mouse cursor (plus sign).
  '
  ' EXAMPLE OF USE:  Cursplus mask$, xHot%, yHot%
  ' PARAMETERS:      mask$      Pattern mask for creating cursor
  '                  xHot%      X location for cursor hot spot
  '                  yHot%      Y location for cursor hot spot
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Cursplus (mask$, xHot%, yHot%)
  '
    SUB Cursplus (mask$, xHot%, yHot%) STATIC
      
        mask$ = ""
        mask$ = mask$ + "1111110000111111"
        mask$ = mask$ + "1111110000111111"
        mask$ = mask$ + "1111110000111111"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "1111110000111111"
        mask$ = mask$ + "1111110000111111"
        mask$ = mask$ + "1111110000111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
      
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000110000000"
        mask$ = mask$ + "0000000110000000"
        mask$ = mask$ + "0000000110000000"
        mask$ = mask$ + "0111111111111110"
        mask$ = mask$ + "0000000110000000"
        mask$ = mask$ + "0000000110000000"
        mask$ = mask$ + "0000000110000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
      
        xHot% = 7
        yHot% = 4
      
    END SUB

  ' ************************************************
  ' **  Name:          Cursup                     **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Defines a graphics-mode mouse cursor (up arrow).
  '
  ' EXAMPLE OF USE:  Cursup mask$, xHot%, yHot%
  ' PARAMETERS:      mask$      Pattern mask for creating cursor
  '                  xHot%      X location for cursor hot spot
  '                  yHot%      Y location for cursor hot spot
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Cursup (mask$, xHot%, yHot%)
  '
    SUB Cursup (mask$, xHot%, yHot%) STATIC
      
        mask$ = ""
        mask$ = mask$ + "1111100111111111"
        mask$ = mask$ + "1111000011111111"
        mask$ = mask$ + "1110000001111111"
        mask$ = mask$ + "1110000001111111"
        mask$ = mask$ + "1100000000111111"
        mask$ = mask$ + "1100000000111111"
        mask$ = mask$ + "1000000000011111"
        mask$ = mask$ + "1000000000011111"
        mask$ = mask$ + "0000000000001111"
        mask$ = mask$ + "0000000000001111"
        mask$ = mask$ + "1111000011111111"
        mask$ = mask$ + "1111000011111111"
        mask$ = mask$ + "1111000011111111"
        mask$ = mask$ + "1111000011111111"
        mask$ = mask$ + "1111000011111111"
        mask$ = mask$ + "1111000011111111"
      
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000011000000000"
        mask$ = mask$ + "0000111100000000"
        mask$ = mask$ + "0000111100000000"
        mask$ = mask$ + "0001111110000000"
        mask$ = mask$ + "0001111110000000"
        mask$ = mask$ + "0011111111000000"
        mask$ = mask$ + "0011111111000000"
        mask$ = mask$ + "0111111111100000"
        mask$ = mask$ + "0000011000000000"
        mask$ = mask$ + "0000011000000000"
        mask$ = mask$ + "0000011000000000"
        mask$ = mask$ + "0000011000000000"
        mask$ = mask$ + "0000011000000000"
        mask$ = mask$ + "0000011000000000"
        mask$ = mask$ + "0000000000000000"
      
        xHot% = 5
        yHot% = 0
      
    END SUB

  ' ************************************************
  ' **  Name:          Cursx                      **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Defines a graphics-mode mouse cursor (X mark).
  '
  ' EXAMPLE OF USE:  Cursx mask$, xHot%, yHot%
  ' PARAMETERS:      mask$      Pattern mask for creating cursor
  '                  xHot%      X location for cursor hot spot
  '                  yHot%      Y location for cursor hot spot
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Cursx (mask$, xHot%, yHot%)
  '
    SUB Cursx (mask$, xHot%, yHot%) STATIC
      
        mask$ = ""
        mask$ = mask$ + "0000011111100000"
        mask$ = mask$ + "0000000110000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "1100000000000011"
        mask$ = mask$ + "1111000000001111"
        mask$ = mask$ + "1100000000000011"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000110000000"
        mask$ = mask$ + "0000001111000000"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
        mask$ = mask$ + "1111111111111111"
      
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0111000000001110"
        mask$ = mask$ + "0001110000111000"
        mask$ = mask$ + "0000011001100000"
        mask$ = mask$ + "0000001111000000"
        mask$ = mask$ + "0000011001100000"
        mask$ = mask$ + "0001110000111000"
        mask$ = mask$ + "0111000000001110"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
        mask$ = mask$ + "0000000000000000"
      
        xHot% = 7
        yHot% = 4
      
    END SUB

  ' ************************************************
  ' **  Name:          MouseHide                  **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Hides the mouse cursor.
  '
  ' EXAMPLE OF USE:  MouseHide
  '
  ' PARAMETERS:      (none)
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                  DECLARE SUB MouseHide ()
  '
    SUB MouseHide STATIC
        Mouse 2, 0, 0, 0
    END SUB

  ' ************************************************
  ' **  Name:          MouseInches                **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Sets mouse motion ratio in inches per screen.
  '
  ' EXAMPLE OF USE:  MouseInches horizontal%, vertical%
  ' PARAMETERS:      horizontal%   Inches of horizontal mouse motion per
  '                                screen width
  '                  vertical%     Inches of vertical% mouse motion per
  '                                screen height
  ' VARIABLES:       h%            Calculated value to pass to mouse driver
  '                  v%            Calculated value to pass to mouse driver
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                  DECLARE SUB MouseInches (horizontal%, vertical%)
  '
    SUB MouseInches (horizontal%, vertical%) STATIC
        IF horizontal% > 100 THEN
            horizontal% = 100
        END IF
        IF vertical% > 100 THEN
            vertical% = 100
        END IF
        h% = horizontal% * 5 \ 2
        v% = vertical% * 8
        Mouse 15, 0, h%, v%
    END SUB

  ' ************************************************
  ' **  Name:          MouseInstall               **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Determines whether mouse is available and resets all mouse parameters.
  '
  ' EXAMPLE OF USE:  MouseInstall mflag%
  ' PARAMETERS:      mflag%     Returned indication of mouse availability
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                  DECLARE SUB MouseInstall (mflag%)
  '
    SUB MouseInstall (mflag%) STATIC
        mflag% = 0
        Mouse mflag%, 0, 0, 0
    END SUB

  ' ************************************************
  ' **  Name:          MouseLightPen              **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Activates and deactivates lightpen emulation mode.
  '
  ' EXAMPLE OF USE:  MouseLightPen switch%
  ' PARAMETERS:      switch%    non-zero to activate lightpen emulation,
  '                             zero to deactivate
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                  DECLARE SUB MouseLightPen (switch%)
  '
    SUB MouseLightPen (switch%) STATIC
        IF switch% THEN
            Mouse 13, 0, 0, 0
        ELSE
            Mouse 14, 0, 0, 0
        END IF
    END SUB

  ' ************************************************
  ' **  Name:          MouseMaskTranslate         **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Translates mouse graphics cursor Mask$ to Cursor$.
  '
  ' EXAMPLE OF USE:  MouseMaskTranslate mask$, xHot%, yHot%, cursor$
  ' PARAMETERS:      mask$      Pattern mask that defines a mouse
  '                             graphics-mode cursor
  '                  xHot%      X location of the hot spot
  '                  yHot%      Y location of the hot spot
  '                  cursor$    The returned binary buffer string
  '                             for the cursor
  ' VARIABLES:       i%         Looping index
  '                  b%         Integer formed from string bit representations
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB MouseMaskTranslate (mask$, xHot%, yHot%,
  '                              cursor$)
  '
    SUB MouseMaskTranslate (mask$, xHot%, yHot%, cursor$) STATIC
        cursor$ = CHR$(xHot%) + CHR$(yHot%) + STRING$(64, 0)
        IF LEN(mask$) = 512 THEN
            FOR i% = 1 TO 32
                b% = BinStr2Bin%(MID$(mask$, i% * 16 - 15, 16))
                MID$(cursor$, i% + i% + 1, 2) = MKI$(b%)
            NEXT i%
        END IF
    END SUB

  ' ************************************************
  ' **  Name:          MouseMickey                **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Reads mouse mickey counts.
  '
  ' EXAMPLE OF USE:  MouseMickey horizontal%, vertical%
  ' PARAMETERS:      horizontal%   Horizontal motion mickey counts
  '                  vertical%     Vertical motion mickey counts
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                  DECLARE SUB MouseMickey (horizontal, vertical%)
  '
    SUB MouseMickey (horizontal%, vertical%) STATIC
        Mouse 11, 0, horizontal%, vertical%
    END SUB

  ' ************************************************
  ' **  Name:          MouseNow                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the instantaneous state of the mouse.
  '
  ' EXAMPLE OF USE:  MouseNow leftButton%, rightButton%, xMouse%, yMouse%
  ' PARAMETERS:      leftButton%   Indicates left mouse button state
  '                  rightButton%  Indicates right mouse button state
  '                  xMouse%       X location of mouse
  '                  yMouse%       Y location of mouse
  ' VARIABLES:       m2%           Mouse driver parameter containing button
  '                                press information
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                  DECLARE SUB MouseNow (leftButton%, rightButton%,
  '                                        xMouse%, yMouse%)
  '
    SUB MouseNow (leftButton%, rightButton%, xMouse%, yMouse%) STATIC
        Mouse 3, m2%, xMouse%, yMouse%
        leftButton% = ((m2% AND 1) <> 0)
        rightButton% = ((m2% AND 2) <> 0)
    END SUB

  ' ************************************************
  ' **  Name:          MousePressLeft             **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the mouse state at last press of left button.
  '
  ' EXAMPLE OF USE:  MousePressLeft leftCount%, xMouse%, yMouse%
  ' PARAMETERS:      leftCount%    Number of times the left button has been
  '                                pressed since the last call to this
  '                                subprogram
  '                  xMouse%       X location of the mouse at the last press
  '                                of the left button
  '                  yMouse%       Y location of the mouse at the last press
  '                                of the left button
  ' VARIABLES:       m1%           Parameter for call to mouse driver
  ' MODULE LEVEL
  '   DECLARATIONS:    DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                    DECLARE SUB MousePressLeft (leftCount%, xMouse%, yMouse%)
  '
    SUB MousePressLeft (leftCount%, xMouse%, yMouse%) STATIC
        m1% = 5
        leftCount% = 0
        Mouse m1%, leftCount%, xMouse%, yMouse%
    END SUB

  ' ************************************************
  ' **  Name:          MousePressRight            **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the mouse state at last press of right button.
  '
    SUB MousePressRight (rightCount%, xMouse%, yMouse%) STATIC
        m1% = 5
        rightCount% = 1
        Mouse m1%, rightCount%, xMouse%, yMouse%
    END SUB

  ' ************************************************
  ' **  Name:          MousePut                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Sets the mouse position.
  '
  ' EXAMPLE OF USE:  MousePut xMouse%, yMouse%
  ' PARAMETERS:      xMouse%    Horizontal location to place cursor
  '                  yMouse%    Vertical location to place cursor
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:   DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                   DECLARE SUB MousePut (xMouse%, yMouse%)
  '
    SUB MousePut (xMouse%, yMouse%) STATIC
        Mouse 4, 0, xMouse%, yMouse%
    END SUB

  ' ************************************************
  ' **  Name:          MouseRange                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Sets mouse range of motion.
  '
  ' EXAMPLE OF USE:  MouseRange x1%, y1%, x2%, y2%
  ' PARAMETERS:      x1%        Upper left corner X coordinate
  '                  y1%        Upper left corner Y coordinate
  '                  x2%        Lower right corner X coordinate
  '                  y2%        Lower right corner Y coordinate
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:   DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                   DECLARE SUB MouseRange (x1%, y1%, x2%, y2%)
  '
    SUB MouseRange (x1%, y1%, x2%, y2%) STATIC
        Mouse 7, 0, x1%, x2%
        Mouse 8, 0, y1%, y2%
    END SUB

  ' ************************************************
  ' **  Name:          MouseReleaseLeft           **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the mouse state at last release of left button.
  '
  ' EXAMPLE OF USE:  MouseReleaseLeft leftCount%, xMouse%, yMouse%
  ' PARAMETERS:      leftCount%    Number of times the left button has been
  '                                released since the last call to this
  '                                subprogram
  '                  xMouse%       X location of the mouse at the last
  '                                release of the left button
  '                  yMouse%       Y location of the mouse at the last
  '                                release of the left button
  ' VARIABLES:       m1%           Parameter for call to mouse driver
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                  DECLARE SUB MouseReleaseLeft (leftCount%, xMouse%,
  '                                               yMouse%)
  '
    SUB MouseReleaseLeft (leftCount%, xMouse%, yMouse%) STATIC
        m1% = 6
        leftCount% = 0
        Mouse m1%, leftCount%, xMouse%, yMouse%
    END SUB

  ' ************************************************
  ' **  Name:          MouseReleaseRight          **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the mouse state at last release of right button.
  '
  ' EXAMPLE OF USE:  MouseReleaseRight rightCount%, xMouse%, yMouse%
  ' PARAMETERS:      rightCount%   Number of times the right button has been
  '                                released since the last call to this
  '                                subprogram
  '                  xMouse%       X location of the mouse at the last
  '                                release of the right button
  '                  yMouse%       Y location of the mouse at the last
  '                                release of the right button
  ' VARIABLES:       m1%           Parameter for call to mouse driver
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                  DECLARE SUB MouseReleaseRight (rightCount%, xMouse%,
  '                                                 yMouse%)
  '
    SUB MouseReleaseRight (rightCount%, xMouse%, yMouse%) STATIC
        m1% = 6
        rightCount% = 1
        Mouse m1%, rightCount%, xMouse%, yMouse%
    END SUB

  ' ************************************************
  ' **  Name:          MouseSetGcursor            **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Sets mouse graphics cursor using cursor$.
  '
  ' EXAMPLE OF USE:  MouseSetGcursor cursor$
  ' PARAMETERS:      cursor$    Binary format cursor string
  ' VARIABLES:       xHot%      X hot spot location
  '                  yHot%      Y hot spot location
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                  DECLARE SUB MouseSetGcursor (cursor$)
  '
    SUB MouseSetGcursor (cursor$) STATIC
        xHot% = ASC(LEFT$(cursor$, 1))
        yHot% = ASC(MID$(cursor$, 2, 1))
        Mouse 9, xHot%, yHot%, SADD(cursor$) + 2
    END SUB

  ' ************************************************
  ' **  Name:          MouseShow                  **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Shows the mouse cursor.
  '
  ' EXAMPLE OF USE:  MouseShow
  ' PARAMETERS:      (none)
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                  DECLARE SUB MouseShow ()
  '
    SUB MouseShow STATIC
        Mouse 1, 0, 0, 0
    END SUB

  ' ************************************************
  ' **  Name:          MouseSoftCursor            **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Sets text-mode software cursor.
  '
  ' EXAMPLE OF USE:  MouseSoftCursor screenMask%, cursorMask%
  ' PARAMETERS:      screenMask%   Integer bit pattern for the screen mask
  '                  cursorMask%   Integer bit pattern for the cursor mask
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB MouseSoftCursor (screenMaks%, cursorMask%)
  '                  DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '
    SUB MouseSoftCursor (screenMask%, cursorMask%) STATIC
        Mouse 10, 0, screenMask%, cursorMask%
    END SUB

  ' ************************************************
  ' **  Name:          MouseWarp                  **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Sets double-speed threshold.
  '
  ' EXAMPLE OF USE:  MouseWarp threshold%
  ' PARAMETERS:      threshold%    Mickies per second rate of threshold
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
  '                  DECLARE SUB MouseWarp (threshold%)
  '
    SUB MouseWarp (threshold%) STATIC
        Mouse 19, 0, 0, threshold%
    END SUB


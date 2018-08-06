  ' ************************************************
  ' **  Name:          COLORS                     **
  ' **  Type:          Program                    **
  ' **  Module:        COLORS.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Provides interactive selection of a color shade.
  '
  ' USAGE:           No command line parameters
  ' REQUIREMENTS:    VGA or MCGA
  '                  MIXED.QLB/.LIB
  '                  Mouse
  ' .MAK FILE:       COLORS.BAS
  '                  BITS.BAS
  '                  MOUSSUBS.BAS
  ' PARAMETERS:      (none)
  ' VARIABLES:       red!            Intensity of red, from 0 to 1
  '                  green!          Intensity of green, from 0 to 1
  '                  blue!           Intensity of blue, from 0 to 1
  '                  mask$           Mouse graphics cursor definition string
  '                  xHot%           Mouse cursor hot spot X location
  '                  yHot%           Mouse cursor hot spot Y location
  '                  cursor$         Mouse cursor binary definition string
  '                  fill%           Color bar height calculation
  '                  x%              Color bar horizontal left edge
  '                  x2%             Color bar horizontal right edge
  '                  y%              Color bar vertical top edge
  '                  y2%             Color bar vertical bottom edge
  '                  leftButton%     State of left mouse button
  '                  rightButton%    State of right mouse button
  '                  xMouse%         Horizontal mouse location
  '                  yMouse%         Vertical mouse location
  '                  clickFlag%      Toggle for left mouse button state
  '                  xM%             Modified mouse horizontal location
  '                  quitFlag%       Signal to end program
  
  
  ' Logical constants
    CONST FALSE = 0
    CONST TRUE = NOT FALSE
  
  ' Constants
    CONST REDPAL = 1
    CONST BLUEPAL = 2
    CONST GREENPAL = 3
    CONST TESTPAL = 4
    CONST WHITEPAL = 5
    CONST BARPAL = 6
    CONST DX = 15
    CONST DY = 150
    CONST RX = 180
    CONST RY = 30
    CONST GX = RX + DX + DX
    CONST GY = RY
    CONST BX = GX + DX + DX
    CONST BY = RY
  
  ' Functions
    DECLARE FUNCTION Shade& (red!, green!, blue!)
  
  ' Subprograms
    DECLARE SUB MouseHide ()
    DECLARE SUB MouseMaskTranslate (mask$, xHot%, yHot%, cursor$)
    DECLARE SUB MouseSetGcursor (cursor$)
    DECLARE SUB MouseShow ()
    DECLARE SUB Cursleft (mask$, xHot%, yHot%)
    DECLARE SUB MouseNow (leftButton%, rightButton%, xMouse%, yMouse%)
  
  ' Set 256 color mode
    SCREEN 13
  
  ' Set first three colors as pure red, green, blue
    PALETTE REDPAL, Shade&(1!, 0!, 0!)
    PALETTE GREENPAL, Shade&(0!, 1!, 0!)
    PALETTE BLUEPAL, Shade&(0!, 0!, 1!)
  
  ' Set a pure white color choice
    PALETTE WHITEPAL, Shade&(1!, 1!, 1!)
  
  ' Set bar background color
    PALETTE BARPAL, Shade&(0!, 0!, 0!)
  
  ' Set background to light gray
    PALETTE 0, Shade&(.4, .4, .4)
  
  ' Start each intensity at midscale
    red! = .5
    green! = .5
    blue! = .5
  
  ' Set starting shade
    PALETTE TESTPAL, Shade&(red!, green!, blue!)
  
  ' Create ellipse of circle to show current shade selected
    CIRCLE (70, 100), 80, TESTPAL, , , 1.4
    PAINT (70, 100), TESTPAL
  
  ' Create the three color bars
    LINE (RX, RY)-(RX + DX, RY + DY), WHITEPAL, B
    LINE (GX, GY)-(GX + DX, GY + DY), WHITEPAL, B
    LINE (BX, BY)-(BX + DX, BY + DY), WHITEPAL, B
  
  ' Mark place to quit by clicking
    LOCATE 25, 1
    PRINT "(X) "; CHR$(27); " Quit";
  
  ' Make the left arrow mouse cursor
    Cursleft mask$, xHot%, yHot%
    MouseMaskTranslate mask$, xHot%, yHot%, cursor$
    MouseSetGcursor cursor$
  
  ' Main loop
    DO
      
      ' Put title and current shade number at top
        LOCATE 1, 1
        PRINT "COLOR CHOOSER"; TAB(22);
        PRINT USING "##########"; Shade&(red!, green!, blue!)
      
      ' Fill in the red color bar
        fill% = red! * (DY - 3) + 1
        x% = RX + 1
        x2% = RX + DX
        y% = RY + 1
        y2% = RY + DY
        LINE (x%, y%)-(x2% - 1, y2% - fill% - 1), BARPAL, BF
        LINE (x%, y2% - fill%)-(x2% - 1, y2% - 1), REDPAL, BF
      
      ' Fill in the green color bar
        fill% = green! * (DY - 3) + 1
        x% = GX + 1
        x2% = GX + DX
        y% = GY + 1
        y2% = GY + DY
        LINE (x%, y%)-(x2% - 1, y2% - fill% - 1), BARPAL, BF
        LINE (x%, y2% - fill%)-(x2% - 1, y2% - 1), GREENPAL, BF
      
      ' Fill in the blue color bar
        fill% = blue! * (DY - 3) + 1
        x% = BX + 1
        x2% = BX + DX
        y% = BY + 1
        y2% = BY + DY
        LINE (x%, y%)-(x2% - 1, y2% - fill% - 1), BARPAL, BF
        LINE (x%, y2% - fill%)-(x2% - 1, y2% - 1), BLUEPAL, BF
      
      ' Change the shade of the ellipse
        PALETTE TESTPAL, Shade&(red!, green!, blue!)
      
      ' Refresh mouse cursor
        MouseShow
      
      ' Wait for fresh mouse left button click
        DO
            MouseNow leftButton%, rightButton%, xMouse%, yMouse%
            IF leftButton% = FALSE THEN
                clickFlag% = FALSE
            END IF
            IF clickFlag% THEN
                leftButton% = 0
            END IF
        LOOP UNTIL leftButton%
      
      ' Hide mouse and set parameters
        MouseHide
        clickFlag% = TRUE
        xM% = xMouse% \ 2
      
      ' Is mouse in the "Quit" area?
        IF xMouse% < 45 AND yMouse% > 190 THEN
            quitFlag% = TRUE
        END IF
      
      ' Is mouse at the right height to be in a bar?
        IF yMouse% > RY - 2 AND yMouse% < RY + DY + 2 THEN
          
          ' Is mouse in the red bar?
            IF xM% > RX AND xM% < RX + DX THEN
                red! = 1! - (yMouse% - RY) / DY
                IF red! < 0 THEN
                    red! = 0
                ELSEIF red! > 1 THEN
                    red! = 1
                END IF
            END IF
          
          ' Is mouse in the green bar?
            IF xM% > GX AND xM% < GX + DX THEN
                green! = 1! - (yMouse% - RY) / DY
                IF green! < 0 THEN
                    green! = 0
                ELSEIF green! > 1 THEN
                    green! = 1
                END IF
            END IF
          
          ' Is mouse in the blue bar?
            IF xM% > BX AND xM% < BX + DX THEN
                blue! = 1! - (yMouse% - RY) / DY
                IF blue! < 0 THEN
                    blue! = 0
                ELSEIF blue! > 1 THEN
                    blue! = 1
                END IF
            END IF
          
        END IF
      
    LOOP UNTIL quitFlag%
  
    SCREEN 0
    WIDTH 80
    CLS
    END

  ' ************************************************
  ' **  Name:          Shade&                     **
  ' **  Type:          Function                   **
  ' **  Module:        COLORS.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the long integer color number given red,
  ' green, and blue intensity numbers in the range
  ' 0 to 1.
  '
  ' EXAMPLE OF USE:  PALETTE 1, Shade&(red!, green!, blue!)
  ' PARAMETERS:      red!       Intensity of red, from 0 through 1
  '                  green!     Intensity of green, from 0 through 1
  '                  blue!      Intensity of blue, from 0 through 1
  ' VARIABLES:       r&         Red amount
  '                  g&         Green amount
  '                  b&         Blue amount
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Shade& (red!, green!, blue!)
  '
    FUNCTION Shade& (red!, green!, blue!) STATIC
        r& = red! * 63!
        g& = green! * 63!
        b& = blue! * 63!
        Shade& = r& + g& * 256& + b& * 65536
    END FUNCTION


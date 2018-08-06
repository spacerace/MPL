  ' ************************************************
  ' **  Name:          OBJECT                     **
  ' **  Type:          Program                    **
  ' **  Module:        OBJECT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Allows interactive graphics object creation.
  ' Dumps code for another program to be able to create
  ' the graphics object "PUT array" directly.
  '
  ' USAGE:           No command line parameters
  ' REQUIREMENTS:    CGA
  ' .MAK FILE:       OBJECT.BAS
  '                  KEYS.BAS
  '                  EDIT.BAS
  ' PARAMETERS:      (none)
  ' VARIABLES:       quitFlag%     Indicates user is ready to quit
  '                  modeFlag%     Indicates a valid graphics mode was selected
  '                  mode%         Graphics mode
  '                  xMax%         Maximum screen X coordinate
  '                  yMax%         Maximum screen Y coordinate
  '                  fileName$     Name of object creation subprogram file
  '                  exitCode%     Return code from EditLine subprogram
  '                  t$            Temporary work string while reading file
  '                                contents
  '                  a$            The DRAW string
  '                  editFlag%     Indicates an edit of the string is desired
  '               drawErrorFlag%   Indicates an error occurred during the DRAW
  '                  keyNumber%    Integer key code returned by KeyCode%
  '                                function
  '                  okayFlag%     Shared flag for determining array dimensions
  
  ' Logical constants
    CONST FALSE = 0
    CONST TRUE = NOT FALSE
  
  ' Key code constants
    CONST SKEYLC = 115
    CONST SKEYUC = SKEYLC - 32
    CONST QKEYLC = 113
    CONST QKEYUC = QKEYLC - 32
    CONST ESC = 27
  
  ' Color constants
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
  
  ' Functions
    DECLARE FUNCTION KeyCode% ()
  
  ' Subprograms
    DECLARE SUB DrawBox (row1%, col1%, row2%, col2%)
    DECLARE SUB EditBox (a$, row1%, col1%, row2%, col2%)
    DECLARE SUB EditLine (a$, exitCode%)
    DECLARE SUB SaveObject (mode%, xMax%, yMax%, fileName$, a$)
  
  ' Initialization
    SCREEN 0
    CLS
    quitFlag% = FALSE
  
  ' Title
    PRINT "OBJECT - Interactive graphics object editor"
    PRINT
    PRINT
  
  ' Display screen mode table
    PRINT "Adapter       SCREEN modes allowed"
    PRINT "----------    --------------------"
    PRINT "Monochrome    (none)"
    PRINT "Hercules      3"
    PRINT "CGA           1,2"
    PRINT "EGA           1,2,7,8,9"
    PRINT "MCGA          1,2,11,13"
    PRINT "VGA           1,2,7,8,9,10,11,12,13"
    PRINT
  
  ' Ask user for the graphics screen mode
    DO
        PRINT "Enter a SCREEN mode number, ";
        INPUT "based on your graphics adapter "; mode%
        modeFlag% = TRUE
        SELECT CASE mode%
        CASE 1, 7, 13
            xMax% = 319
            yMax% = 199
        CASE 2, 8
            xMax% = 639
            yMax% = 199
        CASE 9, 10
            xMax% = 639
            yMax% = 349
        CASE 11, 12
            xMax% = 639
            yMax% = 479
        CASE 3
            xMax% = 719
            yMax% = 347
        CASE ELSE
            modeFlag% = FALSE
        END SELECT
    LOOP UNTIL modeFlag% = TRUE
  
  ' Ask user for the filename
    fileName$ = "IMAGEARY.BAS" + SPACE$(20)
    SCREEN 0
    WIDTH 80
    CLS
    COLOR WHITE, BLACK
    PRINT "Name of the file where source code will be written:"
    PRINT
    PRINT "Edit the default filename IMAGEARY.BAS ";
    PRINT "if desired, and then press Enter ..."
    PRINT
    PRINT SPACE$(12);
    COLOR YELLOW, BLUE
    EditLine fileName$, exitCode%
    COLOR WHITE, BLACK
  
  ' Try to read in previous contents of the file
    ON ERROR GOTO FileError
    OPEN fileName$ FOR INPUT AS #1
    ON ERROR GOTO 0
    DO UNTIL EOF(1)
        LINE INPUT #1, t$
        IF INSTR(t$, "(DRAW$)") THEN
            t$ = MID$(t$, INSTR(t$, CHR$(34)) + 1)
            t$ = LEFT$(t$, INSTR(t$, CHR$(34)) - 1)
            a$ = a$ + t$
        END IF
    LOOP
    CLOSE #1
  
  ' Main loop
    DO
      
      ' Prepare for DRAW string editing by the user
        SCREEN 0
        WIDTH 80
        CLS
        editFlag% = FALSE
 
      ' Display useful information
        PRINT "OBJECT - Screen mode"; mode%
        PRINT
        PRINT "Edit the DRAW string workspace; then press"
        PRINT "the Esc key to try out your creation..."
        PRINT
        PRINT , "                Cn      Color"
        PRINT , " H   U   E      Mx,y    Move absolute"
        PRINT , "   \ | /        M+|-x,y Move relative"
        PRINT , " L -   - R      An      Angle (1=90,2=180...)"
        PRINT , "   / | \        TAn     Turn angle (-360 to 360)"
        PRINT , " G   D   F      Sn      Scale factor"
        PRINT , "                Pc,b    Paint (color, border)"
        PRINT "(These commands are described in detail in the ";
        PRINT "Microsoft QuickBASIC Language Reference)"
     
      ' Input DRAW string via EditBox subprogram
        COLOR GREEN + BRIGHT, BLUE
        DrawBox 15, 1, 24, 80
        COLOR YELLOW, BLUE
        EditBox a$, 15, 1, 24, 80
      
      ' Try out the DRAW string
        SCREEN mode%
        drawErrorFlag% = FALSE
        ON ERROR GOTO DrawError
        DRAW a$
        ON ERROR GOTO 0
      
      ' Give user idea of what to do
        LOCATE 1, 1
        PRINT "<S>ave, <Esc> to edit, or <Q>uit"
      
      ' Get next valid keystroke
        DO UNTIL editFlag% OR drawErrorFlag% OR quitFlag%
          
          ' Grab key code
            keyNumber% = KeyCode%
          
          ' Process the keystroke
            SELECT CASE keyNumber%
              
            CASE ESC
                editFlag% = TRUE
              
            CASE QKEYLC, QKEYUC
                quitFlag% = TRUE
              
            CASE SKEYLC, SKEYUC
                SaveObject mode%, xMax%, yMax%, fileName$, a$
              
            CASE ELSE
            END SELECT
          
        LOOP
      
    LOOP UNTIL quitFlag%
  
  ' All done
    CLS
    SCREEN 0
    WIDTH 80
    END
  
FileError:
  ' Create the new file
    OPEN fileName$ FOR OUTPUT AS #1
    CLOSE #1
    OPEN fileName$ FOR INPUT AS #1
    RESUME NEXT
  
DrawError:
    drawErrorFlag% = TRUE
    SCREEN 0
    CLS
    PRINT "Your DRAW string caused an error"
    PRINT
    PRINT "Press any key to continue"
    DO
    LOOP UNTIL INKEY$ <> ""
    RESUME NEXT
  
ArrayError:
    okayFlag% = FALSE
    RESUME NEXT

  ' ************************************************
  ' **  Name:          SaveObject                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        OBJECT.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Creates source code file for creating graphics mode
  ' objects for efficient "PUT" graphics.
  '
  ' EXAMPLE OF USE:   SaveObject mode%, xMax%, yMax%, fileName$, a$
  ' PARAMETERS:       mode%      Graphics mode
  '                   xMax%      Maximum X screen coordinate for given
  '                              graphics mode
  '                   yMax%      Maximum Y screen coordinate for given
  '                              graphics mode
  '                   fileName$  Name of source code file to edit and/or
  '                              create
  '                   a$         The DRAW string that creates the object
  '                              initially
  ' VARIABLES:        okayFlag%  Shared flag used to determine array size
  '                   size%      Array sizing
  '                   edge%      Array for efficiently finding edges of object
  '                   stepSize%  Scanning step for search for object edges
  '                   yTop%      Y coordinate at top edge of object
  '                   yBot%      Y coordinate at bottom edge of object
  '                   y1%        Starting edge search Y coordinate
  '                   y2%        Ending edge search Y coordinate
  '                   i%         Looping index
  '                   xLeft%     X coordinate at left edge of object
  '                   xRight%    X coordinate at right edge of object
  '                   x1%        Starting edge search X coordinate
  '                   x2%        Ending edge search X coordinate
  '                   object%()  Array to hold GET object from screen
  '                   objName$   Name of object, derived from filename
  '                   ndx%       Index to any special characters in objName$
  '                   ary$       Name of array, derived from filename
  '                   d$         Works string for building lines for file
  ' MODULE LEVEL
  '   DECLARATIONS: DECLARE FUNCTION SaveObject (mode%, xMax%, yMax%,
  '                                              fileName$, a$)
  '
    SUB SaveObject (mode%, xMax%, yMax%, fileName$, a$) STATIC
      
      ' Shared error trap variable
        SHARED okayFlag%
      
      ' Select the right array size for the mode
        SELECT CASE mode%
        CASE 1, 2
            size% = 93
        CASE 7, 8
            size% = 367
        CASE 9
            size% = 667
        CASE 10
            size% = 334
        CASE 11
            size% = 233
        CASE 12
            size% = 927
        CASE 13
            size% = 161
        CASE ELSE
        END SELECT
      
      ' Build the array space
        DIM edge%(size%)
      
      ' Scan to find top and bottom edges of the object
        stepSize% = 32
        yTop% = yMax%
        yBot% = 0
        y1% = 17
        y2% = yMax%
        DO
            FOR y% = y1% TO y2% STEP stepSize%
                IF y% < yTop% OR y% > yBot% THEN
                    GET (0, y%)-(xMax%, y%), edge%
                    LINE (0, y%)-(xMax%, y%)
                    FOR i% = 2 TO size%
                        IF edge%(i%) THEN
                            IF y% < yTop% THEN
                                yTop% = y%
                            END IF
                            IF y% > yBot% THEN
                                yBot% = y%
                            END IF
                            i% = size%
                        END IF
                    NEXT i%
                    PUT (0, y%), edge%, PSET
                END IF
            NEXT y%
            IF yTop% <= yBot% THEN
                y1% = yTop% - stepSize% * 2
                y2% = yBot% + stepSize% * 2
                IF y1% < 17 THEN
                    y1% = 17
                END IF
                IF y2% > yMax% THEN
                    y2% = yMax%
                END IF
            END IF
            stepSize% = stepSize% \ 2
        LOOP UNTIL stepSize% = 0
      
      ' Scan to find left and right edges of the object
        stepSize% = 32
        xLeft% = xMax%
        xRight% = 0
        x1% = 0
        x2% = xMax%
        DO
            FOR x% = x1% TO x2% STEP stepSize%
                IF x% < xLeft% OR x% > xRight% THEN
                    GET (x%, yTop%)-(x%, yBot%), edge%
                    LINE (x%, yTop%)-(x%, yBot%)
                    FOR i% = 2 TO size%
                        IF edge%(i%) THEN
                            IF x% < xLeft% THEN
                                xLeft% = x%
                            END IF
                            IF x% > xRight% THEN
                                xRight% = x%
                            END IF
                            i% = size%
                        END IF
                    NEXT i%
                    PUT (x%, yTop%), edge%, PSET
                END IF
            NEXT x%
            IF xLeft% <= xRight% THEN
                x1% = xLeft% - stepSize% * 2
                x2% = xRight% + stepSize% * 2
                IF x1% < 0 THEN
                    x1% = 0
                END IF
                IF x2% > xMax% THEN
                    x2% = xMax%
                END IF
            END IF
            stepSize% = stepSize% \ 2
        LOOP UNTIL stepSize% = 0
      
      ' Draw border around the object
        LINE (xLeft% - 1, yTop% - 1)-(xRight% + 1, yBot% + 1), , B
      
      ' Build the right size integer array
        stepSize% = 256
        size% = 3
        DO
            DO
                IF size% < 3 THEN
                    size% = 3
                END IF
                REDIM object%(size%)
                okayFlag% = TRUE
                ON ERROR GOTO ArrayError
                GET (xLeft%, yTop%)-(xRight%, yBot%), object%
                ON ERROR GOTO 0
                IF okayFlag% = FALSE THEN
                    size% = size% + stepSize%
                ELSE
                    IF stepSize% > 1 THEN
                        size% = size% - stepSize%
                    END IF
                END IF
            LOOP UNTIL okayFlag%
            stepSize% = stepSize% \ 2
        LOOP UNTIL stepSize% = 0
      
      ' Make the name of the object
        objName$ = LTRIM$(RTRIM$(fileName$)) + "."
        ndx% = INSTR(objName$, "\")
        DO WHILE ndx%
            objName$ = MID$(objName$, ndx% + 1)
            ndx% = INSTR(objName$, "\")
        LOOP
        ndx% = INSTR(objName$, ":")
        DO WHILE ndx%
            objName$ = MID$(objName$, ndx% + 1)
            ndx% = INSTR(objName$, ":")
        LOOP
        ndx% = INSTR(objName$, ".")
        objName$ = LCASE$(LEFT$(objName$, ndx% - 1))
        IF objName$ = "" THEN
            objName$ = "xxxxxx"
        END IF
      
      ' Make array name
        ary$ = objName$ + "%("
      
      ' Open the file for the new source lines
        OPEN fileName$ FOR OUTPUT AS #1
      
      ' Print the lines
        PRINT #1, " "
        PRINT #1, "  ' " + objName$
        FOR i% = 1 TO LEN(a$) STEP 50
            PRINT #1, "  ' (DRAW$) "; CHR$(34);
            PRINT #1, MID$(a$, i%, 50); CHR$(34)
        NEXT i%
        PRINT #1, "    DIM " + ary$; "0 TO";
        PRINT #1, STR$(size%) + ")"
        PRINT #1, "    FOR i% = 0 TO"; size%
        PRINT #1, "        READ h$"
        PRINT #1, "        " + ary$ + "i%) = VAL(";
        PRINT #1, CHR$(34) + "&H" + CHR$(34);
        PRINT #1, " + h$)"
        PRINT #1, "    NEXT i%"
        FOR i% = 0 TO size%
            IF d$ = "" THEN
                d$ = "    DATA "
            ELSE
                d$ = d$ + ","
            END IF
            d$ = d$ + HEX$(object%(i%))
            IF LEN(d$) > 60 OR i% = size% THEN
                PRINT #1, d$
                d$ = ""
            END IF
        NEXT i%
        PRINT #1, " "
      
      ' Close the file
        CLOSE
      
      ' Erase the border around the object
        LINE (xLeft% - 1, yTop% - 1)-(xRight% + 1, yBot% + 1), 0, B
      
    END SUB


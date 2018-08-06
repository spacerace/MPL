  ' ************************************************
  ' **  Name:          LOOK                       **
  ' **  Type:          Program                    **
  ' **  Module:        LOOK.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' USAGE:           LOOK filename.ext
  ' .MAK FILE:       LOOK.BAS
  '                  KEYS.BAS
  ' PARAMETERS:      filename.ext  Name of file to view
  ' VARIABLES:       a$()          Array of lines from the file
  '                  fileName$     Name of file, from the command line
  '                  lineCount%    Count of lines read from the file
  '                  linePtr%      First file line currently on the display
  '                  i%            Loop index for printing 24 lines
  '                  quitFlag%     Indicates Escape key press
  '                  updateFlag%   Indicates if update of screen is necessary
  
  ' Constants
    CONST FALSE = 0
    CONST TRUE = NOT FALSE
  
  ' Key code numbers
    CONST UPARROW = 18432
    CONST DOWNARROW = 20480
    CONST PGUP = 18688
    CONST PGDN = 20736
    CONST HOME = 18176
    CONST ENDKEY = 20224
    CONST ESCAPE = 27
  
  ' Functions
    DECLARE FUNCTION KeyCode% ()
  
  ' Subprograms
    DECLARE SUB FileRead (fileName$, lineCount%, a$())
  
  ' Dimension string array
  ' NOTE:
  ' Must be dimensioned big enough to read in all lines from the file
    DIM a$(1 TO 2000)
  
  ' Get the command line parameters
    fileName$ = COMMAND$
  
  ' Read in the file
    ON ERROR GOTO FileError
    FileRead fileName$, lineCount%, a$()
    ON ERROR GOTO 0
  
  ' Prepare the screen
    SCREEN 0, 0, 0, 0
    CLS
  
  ' Set line pointer
    linePtr% = 1
  
  ' Main loop
    DO
      
      ' Print information bar at top
        VIEW PRINT 1 TO 1
        COLOR 0, 3
        LOCATE 1, 1
        PRINT " Line:"; LEFT$(STR$(linePtr%) + SPACE$(7), 8);
        PRINT "File: "; LEFT$(fileName$ + SPACE$(19), 19);
        PRINT "Quit: ESC"; SPACE$(3);
        PRINT "Move: "; CHR$(24); " "; CHR$(25); " PGUP PGDN HOME END ";
      
      ' Update the 24 lines of text
        VIEW PRINT 2 TO 25
        COLOR 7, 1
        FOR i% = 0 TO 23
            LOCATE i% + 2, 1
            PRINT LEFT$(a$(i% + linePtr%) + SPACE$(80), 80);
        NEXT i%
      
      ' Wait for a meaningful key to be pressed
        SELECT CASE KeyCode%
        CASE UPARROW
            IF linePtr% > 1 THEN
                linePtr% = linePtr% - 1
            END IF
        CASE DOWNARROW
            IF linePtr% < lineCount% THEN
                linePtr% = linePtr% + 1
            END IF
        CASE PGUP
            IF linePtr% > 1 THEN
                linePtr% = linePtr% - 24
                IF linePtr% < 1 THEN
                    linePtr% = 1
                END IF
            END IF
        CASE PGDN
            IF linePtr% < lineCount% - 24 THEN
                linePtr% = linePtr% + 24
                IF linePtr% > lineCount% THEN
                    linePtr% = lineCount%
                END IF
            END IF
        CASE HOME
            IF linePtr% > 1 THEN
                linePtr% = 1
            END IF
        CASE ENDKEY
            IF linePtr% < lineCount% - 24 THEN
                linePtr% = lineCount% - 24
            END IF
        CASE ESCAPE
            quitFlag% = TRUE
        CASE ELSE
            updateFlag% = FALSE
        END SELECT
      
    LOOP UNTIL quitFlag%
  
  ' Set color back to normal
    COLOR 7, 0
    END
  
FileError:
    PRINT
    PRINT "Usage: LOOK filename.ext"
    SYSTEM
    RESUME NEXT
  
  ' ************************************************
  ' **  Name:          FileRead                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        LOOK.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Reads lines of an ASCII file into a$().  The
  ' lineCount% is set to the number of lines read
  ' in.  If a$() wasn't dimensioned large enough,
  ' then lineCount% will be set to -1.
  '
  ' EXAMPLE OF USE:  FileRead fileName$, lineCount%, a$()
  ' PARAMETERS:      fileName$     Name of file to be read into the array
  '                  lineCount%    Returned count of lines read from the file
  '                  a$()          String array of file contents
  ' VARIABLES:       FileNumber%   Next available free file number
  '                  i%            Index for string array
  ' MODULE LEVEL
  '   DECLARATIONS:    DECLARE SUB FileRead (fileName$, lineCount%, a$())
  '
    SUB FileRead (fileName$, lineCount%, a$()) STATIC
        FileNumber% = FREEFILE
        OPEN fileName$ FOR INPUT AS FileNumber%
        FOR i% = LBOUND(a$) TO UBOUND(a$)
            LINE INPUT #FileNumber%, a$(i%)
            lineCount% = i%
            IF EOF(FileNumber%) THEN
                EXIT FOR
            END IF
        NEXT i%
        IF NOT EOF(FileNumber%) THEN
            lineCount% = -1
        END IF
    END SUB
  

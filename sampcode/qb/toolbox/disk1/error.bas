  ' ************************************************
  ' **  Name:          ERROR                      **
  ' **  Type:          Toolbox                    **
  ' **  Module:        ERROR.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  ' USAGE:           No command line parameters
  ' .MAK FILE:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       (none)
  
  ' Subprogram
    DECLARE SUB ErrorMessage (message$)
  
  ' Demonstrate the subprogram
  
    ErrorMessage "This is a sample message for ErrorMessage"
  

  ' ************************************************
  ' **  Name:          ErrorMessage               **
  ' **  Type:          Subprogram                 **
  ' **  Module:        ERROR.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Displays an error message and then exits to the system.
  '
  ' EXAMPLE OF USE: ErrorMessage "This is a sample message for ErrorMessage"
  ' PARAMETERS:     message$         String to be displayed in the error box
  ' VARIABLES:      lm%              Length of message$ during processing
  '                 col%             Screen character column for left edge
  '                                  of error box
  ' MODULE LEVEL
  '  DECLARATIONS:  DECLARE SUB ErrorMessage (message$)
  '
    SUB ErrorMessage (message$) STATIC
      
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
      
      ' Trim off spaces on each end of message
        message$ = LTRIM$(RTRIM$(message$))
      
      ' Make message length an odd number
        IF LEN(message$) MOD 2 = 0 THEN
            message$ = message$ + " "
        END IF
      
      ' Minimum length of message is 9 characters
        DO WHILE LEN(message$) < 9
            message$ = " " + message$ + " "
        LOOP
      
      ' Maximum length of message is 75
        message$ = LEFT$(message$, 75)
      
      ' Initialization of display
        SCREEN 0
        WIDTH 80
        CLS
      
      ' Calculate screen locations
        lm% = LEN(message$)
        col% = 38 - lm% \ 2
      
      ' Create the error box
        COLOR RED + BRIGHT, RED
        LOCATE 9, col%
        PRINT CHR$(201); STRING$(lm% + 2, 205); CHR$(187)
        LOCATE 10, col%
        PRINT CHR$(186); SPACE$(lm% + 2); CHR$(186)
        LOCATE 11, col%
        PRINT CHR$(186); SPACE$(lm% + 2); CHR$(186)
        LOCATE 12, col%
        PRINT CHR$(200); STRING$(lm% + 2, 205); CHR$(188)
      
      ' The title
        COLOR CYAN + BRIGHT, RED
        LOCATE 10, 36
        PRINT "* ERROR *";
      
      ' The message$
        COLOR YELLOW, RED
        LOCATE 11, col% + 2
        PRINT message$;
      
      ' System will prompt for "any key"
        COLOR WHITE, BLACK
        LOCATE 22, 1
        SYSTEM
      
    END SUB


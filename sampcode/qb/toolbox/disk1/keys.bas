  ' ************************************************
  ' **  Name:          KEYS                       **
  ' **  Type:          Toolbox                    **
  ' **  Module:        KEYS.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Demonstrates keyboard access functions.
  ' USAGE:           No command line parameters
  ' .MAK FILE:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       kee%       Unique integer returned by KeyCode% and
  '                             InKeyCode%
  
    DECLARE FUNCTION KeyCode% ()
    DECLARE FUNCTION InKeyCode% ()
  
    CLS
    PRINT "Press any key to see the unique number returned by KeyCode%."
    PRINT "Press Esc to see InKeyCode% results for 1 second."
    PRINT "Press Esc twice in a row to quit."
    PRINT
  
    DO
        kee% = KeyCode%
        PRINT kee%
        IF kee% = 27 THEN
            t0 = TIMER
            DO
                kee% = InKeyCode%
                PRINT kee%;
                IF kee% THEN
                    PRINT
                END IF
                IF kee% = 27 THEN
                    quitFlag% = -1
                    t0 = t0 - 1
                END IF
            LOOP UNTIL TIMER - t0 > 1
            PRINT
        END IF
    LOOP UNTIL quitFlag%
  
    END

  ' ************************************************
  ' **  Name:          InKeyCode%                 **
  ' **  Type:          Function                   **
  ' **  Module:        KEYS.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a unique integer for any key pressed or
  ' a zero if no key was pressed.
  '
  ' EXAMPLE OF USE:  k% = InKeyCode%
  ' PARAMETERS:      (none)
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION KeyCode% ()
  '
    FUNCTION InKeyCode% STATIC
        InKeyCode% = CVI(INKEY$ + STRING$(2, 0))
    END FUNCTION

  ' ************************************************
  ' **  Name:          KeyCode%                   **
  ' **  Type:          Function                   **
  ' **  Module:        KEYS.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a unique integer for any key pressed.
  '
  ' EXAMPLE OF USE:  k% = KeyCode%
  ' PARAMETERS:      (none)
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION KeyCode% ()
  '
    FUNCTION KeyCode% STATIC
        DO
            k$ = INKEY$
        LOOP UNTIL k$ <> ""
        KeyCode% = CVI(k$ + CHR$(0))
    END FUNCTION


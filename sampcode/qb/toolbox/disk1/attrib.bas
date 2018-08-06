  ' ************************************************
  ' **  Name:          ATTRIB                     **
  ' **  Type:          Program                    **
  ' **  Module:        ATTRIB.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Displays all combinations of text mode character
  ' attributes on the screen for review.
  '
  ' USAGE:           No command line parameters
  ' REQUIREMENTS:    CGA
  ' .MAK FILE:       (none)
  ' FUNCTIONS:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       (none)
  
    DECLARE SUB Attrib ()
  
  ' Call the subprogram
    Attrib
  
  ' All done
    END
  
  ' ************************************************
  ' **  Name:          Attrib                     **
  ' **  Type:          Subprogram                 **
  ' **  Module:        ATTRIB.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Displays table of color attributes for text mode.
  '
  ' EXAMPLE OF USE:  Attrib
  ' PARAMETERS:      (none)
  ' VARIABLES:       bgd%        Background number for COLOR statement
  '                  fgd%        Foreground number for COLOR statement
  ' MODULE LEVEL
  '   DECLARATIONS:              DECLARE SUB Attrib ()
  '
    SUB Attrib STATIC
        SCREEN 0
        CLS
        PRINT "Attributes for the COLOR statement in text mode (SCREEN 0)."
        PRINT "Add 16 to the foreground to cause the character to blink."
        FOR bgd% = 0 TO 7
            COLOR bgd% XOR 7, bgd%
            PRINT
            PRINT "Background%"; STR$(bgd%),
            PRINT "Foreground% ..."; SPACE$(41)
            FOR fgd% = 0 TO 15
                COLOR fgd%, bgd%
                PRINT STR$(fgd%); "  ";
            NEXT fgd%
        NEXT bgd%
        COLOR 7, 0
        PRINT
    END SUB
  

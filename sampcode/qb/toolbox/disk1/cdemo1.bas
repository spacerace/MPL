  ' ************************************************
  ' **  Name:          CDEMO1                     **
  ' **  Type:          Program                    **
  ' **  Module:        CDEMO1.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Demonstrates the QuickC routines presented in
  ' the file CTOOLS1.C.
  '
  ' USAGE:           No command line parameters
  ' REQUIREMENTS:    CGA
  '                  MIXED.QLB/.LIB
  ' .MAK FILE:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       a%(0 TO 1999) Storage space for first text screen
  '                  b%(0 TO 1999) Storage space for second text screen
  '                  i%            Looping index
  '                  sseg%         Word and byte move source segment
  '                                part of address
  '                  soff%         Word and byte move source offset
  '                                part of address
  '                  dseg%         Word and byte move destination segment
  '                                part of address
  '                  doff%         Word and byte move destination offset
  '                                part of address
  '                  nwords%       Number of words to move
  '                  nbytes%       Number of bytes to move
  '                  t$            Copy of TIME$
  '                  quitflag%     Signal to end first demonstration
  
  
  ' Functions
    DECLARE FUNCTION IsItAlnum% CDECL (BYVAL c AS INTEGER)
    DECLARE FUNCTION IsItAlpha% CDECL (BYVAL c AS INTEGER)
    DECLARE FUNCTION IsItAscii% CDECL (BYVAL c AS INTEGER)
    DECLARE FUNCTION IsItCntrl% CDECL (BYVAL c AS INTEGER)
    DECLARE FUNCTION IsItDigit% CDECL (BYVAL c AS INTEGER)
    DECLARE FUNCTION IsItGraph% CDECL (BYVAL c AS INTEGER)
    DECLARE FUNCTION IsItLower% CDECL (BYVAL c AS INTEGER)
    DECLARE FUNCTION IsItPrint% CDECL (BYVAL c AS INTEGER)
    DECLARE FUNCTION IsItPunct% CDECL (BYVAL c AS INTEGER)
    DECLARE FUNCTION IsItSpace% CDECL (BYVAL c AS INTEGER)
    DECLARE FUNCTION IsItUpper% CDECL (BYVAL c AS INTEGER)
    DECLARE FUNCTION IsItXDigit% CDECL (BYVAL c AS INTEGER)
  
  ' Subprograms
    DECLARE SUB MovBytes CDECL (sseg%, soff%, dseg%, doff%, nbytes%)
    DECLARE SUB MovWords CDECL (sseg%, soff%, dseg%, doff%, nwords%)
  
  ' Make two buffers for the first page of video memory
    DIM a%(0 TO 1999), b%(0 TO 1999)
  
  ' Prevent scrolling when printing in row 25, column 80
    VIEW PRINT 1 TO 25
  
  ' Create the first page of text
    CLS
    COLOR 14, 4
    FOR i% = 1 TO 25
        PRINT STRING$(80, 179);
    NEXT i%
    COLOR 15, 1
    LOCATE 11, 25
    PRINT STRING$(30, 32);
    LOCATE 12, 25
    PRINT "    -  Calling MovWords  -    "
    LOCATE 13, 25
    PRINT STRING$(30, 32);
  
  ' Move the screen memory into the first array
    sseg% = &HB800
    soff% = 0
    dseg% = VARSEG(a%(0))
    doff% = VARPTR(a%(0))
    nwords% = 2000
    MovWords sseg%, soff%, dseg%, doff%, nwords%
  
  ' Create the second page of text
    CLS
    COLOR 14, 4
    FOR i% = 1 TO 25
        PRINT STRING$(80, 196);
    NEXT i%
    COLOR 15, 1
    LOCATE 11, 25
    PRINT STRING$(30, 32);
    LOCATE 12, 25
    PRINT "    -  Calling MovBytes  -    "
    LOCATE 13, 25
    PRINT STRING$(30, 32);
  
  ' Move the screen memory into the second array
    sseg% = &HB800
    soff% = 0
    dseg% = VARSEG(b%(0))
    doff% = VARPTR(b%(0))
    nwords% = 2000
    MovWords sseg%, soff%, dseg%, doff%, nwords%
  
  ' Set destination to the video screen memory
    dseg% = &HB800
    doff% = 0
  
  ' Do the following until a key is pressed
    DO
      
      ' Move 2000 words from first array to screen memory
        sseg% = VARSEG(a%(0))
        soff% = VARPTR(a%(0))
        nwords% = 2000
        MovWords sseg%, soff%, dseg%, doff%, nwords%
      
      ' Wait one second
        t$ = TIME$
        DO
            IF INKEY$ <> "" THEN
                t$ = ""
                quitFlag% = 1
            END IF
        LOOP UNTIL TIME$ <> t$
      
      ' Move 4000 bytes from second array to screen memory
        sseg% = VARSEG(b%(0))
        soff% = VARPTR(b%(0))
        nbytes% = 4000
        MovBytes sseg%, soff%, dseg%, doff%, nbytes%
      
      ' Wait one second
        t$ = TIME$
        DO
            IF INKEY$ <> "" THEN
                t$ = ""
                quitFlag% = 1
            END IF
        LOOP UNTIL TIME$ <> t$
      
    LOOP UNTIL quitFlag%
  
  ' Create a table of all 256 characters and their type designations
    FOR i% = 0 TO 255
      
      ' After each screenful display a heading
        IF i% MOD 19 = 0 THEN
          
          ' If not the first heading, prompt user before continuing
            IF i% THEN
                PRINT
                PRINT "Press any key to continue"
                DO WHILE INKEY$ = ""
                LOOP
            END IF
          
          ' Print the heading
            CLS
            PRINT "Char   Alnum Alpha Ascii Cntrl Digit Graph ";
            PRINT "Lower Print Punct Space Upper XDigit"
            PRINT
        END IF
      
      ' Some characters we don't want to display
        SELECT CASE i%
        CASE 7, 8, 9, 10, 11, 12, 13, 29, 30, 31
            PRINT USING "###    "; i%;
        CASE ELSE
            PRINT USING "### \ \"; i%, CHR$(i%);
        END SELECT
      
      ' Display "1" if test is true, "0" otherwise
        PRINT USING "  #   "; 1 + (0 = IsItAlnum%(i%));
        PRINT USING "  #   "; 1 + (0 = IsItAlpha%(i%));
        PRINT USING "  #   "; 1 + (0 = IsItAscii%(i%));
        PRINT USING "  #   "; 1 + (0 = IsItCntrl%(i%));
        PRINT USING "  #   "; 1 + (0 = IsItDigit%(i%));
        PRINT USING "  #   "; 1 + (0 = IsItGraph%(i%));
        PRINT USING "  #   "; 1 + (0 = IsItLower%(i%));
        PRINT USING "  #   "; 1 + (0 = IsItPrint%(i%));
        PRINT USING "  #   "; 1 + (0 = IsItPunct%(i%));
        PRINT USING "  #   "; 1 + (0 = IsItSpace%(i%));
        PRINT USING "  #   "; 1 + (0 = IsItUpper%(i%));
        PRINT USING "  #   "; 1 + (0 = IsItXDigit%(i%))
      
    NEXT i%
    END
  


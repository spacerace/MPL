  ' ************************************************
  ' **  Name:          CIPHER                     **
  ' **  Type:          Program                    **
  ' **  Module:        CIPHER.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' USAGE:   CIPHER  filename.ext key    or    CIPHER /NEWKEY
  ' .MAK FILE:       CIPHER.BAS
  '                  RANDOMS.BAS
  ' PARAMETERS:      filename      Name of file to be ciphered or deciphered
  '                  key           String of one or more words used as the
  '                                cipher key
  ' VARIABLES:       cmd$          Working copy of COMMAND$
  '                  i%            Loop index
  '                  firstSpace%   Location in command line of first character
  '                  fileName$     Name of file to be processed
  '                  key$          String to be used as cipher key
  '                  fileLength&   Length of file to be processed
  '                  a$            Workspace for groups of bytes from the file
  '                  count%        Number of groups of bytes to be processed
  '                  j&            Location in file of each group of bytes
  
  ' Constants
    CONST BYTES = 1000&
  
  ' Functions
    DECLARE FUNCTION NewWord$ ()
    DECLARE FUNCTION Rand& ()
    DECLARE FUNCTION RandInteger% (a%, b%)
  
  ' Subprograms
    DECLARE SUB RandShuffle (key$)
    DECLARE SUB ProcesX (a$)
  
  ' Initialization
    CLS
    PRINT "CIPHER "; COMMAND$
    PRINT
  
  ' Grab the command line parameters
    cmd$ = COMMAND$
  
  ' If no command line parameters, then tell user what's needed
    IF cmd$ = "" THEN
        PRINT
        PRINT "Usage:  CIPHER /NEWKEY"
        PRINT "(or)    CIPHER filename key-string"
        PRINT
        SYSTEM
    END IF
  
  ' If /NEWKEY option, generate a few new words, and then quit
    IF INSTR(cmd$, "/NEWKEY") THEN
      
      ' Clear the screen and describe the output
        CLS
        PRINT "Randomly created words that can be used as cipher keys..."
        PRINT
        RandShuffle DATE$ + TIME$ + STR$(TIMER)
        FOR i% = 1 TO 9
            PRINT NewWord$; " ";
        NEXT i%
        PRINT
        SYSTEM
    END IF
  
  ' Get the filename from the command line
    cmd$ = cmd$ + " "
    firstSpace% = INSTR(cmd$, " ")
    fileName$ = LEFT$(cmd$, firstSpace% - 1)
  
  ' Grab the rest of the command line as the cipher key
    key$ = LTRIM$(MID$(cmd$, firstSpace% + 1))
  
  ' Prepare the pseudorandom numbers using the key for shuffling
    RandShuffle key$
  
  ' Open up the file
    OPEN fileName$ FOR BINARY AS #1
    fileLength& = LOF(1)
  
  ' Process the file in manageable pieces
    a$ = SPACE$(BYTES)
    count% = fileLength& \ BYTES
  
  ' Loop through the file
    FOR i% = 0 TO count%
        j& = i% * BYTES + 1
        IF i% = count% THEN
            a$ = SPACE$(fileLength& - BYTES * count%)
        END IF
        GET #1, j&, a$
        ProcesX a$
        PUT #1, j&, a$
    NEXT i%
  
  ' All done
    SYSTEM
  
  ' ************************************************
  ' **  Name:          NewWord$                   **
  ' **  Type:          Function                   **
  ' **  Module:        CIPHER.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a pseudorandom word of a possibly
  ' speakable form.
  '
  ' EXAMPLE OF USE: PRINT NewWord$
  ' PARAMETERS:     (none)
  ' VARIABLES:      vowel$     String constant listing the set of vowels
  '                 consonant$ String constant listing the set of consonants
  '                 syllables% Random number of syllables for the new word
  '                 i%         Loop index for creating each syllable
  '                 t$         Temporary work string for forming the new word
  ' MODULE LEVEL
  '   DECLARATIONS: DECLARE FUNCTION NewWord$ ()
  '
    FUNCTION NewWord$ STATIC
        CONST vowel$ = "aeiou"
        CONST consonant$ = "bcdfghjklmnpqrstvwxyz"
        syllables% = Rand& MOD 3 + 1
        FOR i% = 1 TO syllables%
            t$ = t$ + MID$(consonant$, RandInteger%(1, 21), 1)
            IF i% = 1 THEN
                t$ = UCASE$(t$)
            END IF
            t$ = t$ + MID$(vowel$, RandInteger%(1, 5), 1)
        NEXT i%
        IF Rand& MOD 2 THEN
            t$ = t$ + MID$(consonant$, RandInteger%(1, 21), 1)
        END IF
        NewWord$ = t$
        t$ = ""
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          ProcesX                    **
  ' **  Type:          Subprogram                 **
  ' **  Module:        CIPHER.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Enciphers a string by XORing with pseudorandom bytes.
  '
  ' EXAMPLE OF USE:  ProcesX a$
  ' PARAMETERS:      a$         String to be ciphered
  ' VARIABLES:       i%         Index into the string
  '                  byte%      Numeric value of each string character
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB ProcesX (a$)
  '
    SUB ProcesX (a$) STATIC
        FOR i% = 1 TO LEN(a$)
            byte% = ASC(MID$(a$, i%, 1)) XOR RandInteger%(0, 255)
            MID$(a$, i%, 1) = CHR$(byte%)
        NEXT i%
    END SUB
  

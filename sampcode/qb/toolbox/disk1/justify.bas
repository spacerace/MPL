  ' ************************************************
  ' **  Name:          JUSTIFY                    **
  ' **  Type:          Toolbox                    **
  ' **  Module:        JUSTIFY.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Demonstrates the Justify subprogram.
  '
  ' USAGE:           No command line parameters
  ' .MAK FILE:       JUSTIFY.BAS
  '                  EDIT.BAS
  '                  PARSE.BAS
  '                  KEYS.BAS
  ' PARAMETERS:      (none)
  ' VARIABLES:       a$         String to be justified
  '                  col%       Number of columns for each example of Justify
  '                  x$         Working copy of a$
  '                  y$         Working string space
  
  
    DECLARE SUB Justify (a$, n%)
    DECLARE SUB ParseLine (x$, sep$, a$())
    DECLARE SUB FormatTwo (a$, b$, col%)
  
    CLS
    a$ = ""
    a$ = a$ + "This paragraph is used to demonstrate the Justify "
    a$ = a$ + "subprogram.  First, the entire paragraph is "
    a$ = a$ + "placed in a single string variable.  This string "
    a$ = a$ + "is then split between words into shorter strings, "
    a$ = a$ + "and these shorter strings are then justified in "
    a$ = a$ + "order to align both the left and right edges of "
    a$ = a$ + "the text."
  
    FOR col% = 50 TO 70 STEP 10
        x$ = a$
        DO
            FormatTwo x$, y$, col%
            IF y$ <> "" THEN
                Justify x$, col%
            END IF
            PRINT x$
            x$ = y$
        LOOP WHILE y$ <> ""
        PRINT
    NEXT col%
  
    END

  ' ************************************************
  ' **  Name:          Justify                    **
  ' **  Type:          Subprogram                 **
  ' **  Module:        JUSTIFY.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Spaces words with extra spaces until line
  ' is n% characters long.
  '
  ' EXAMPLE OF USE:  Justify a$, n%
  ' PARAMETERS:      a$         String to be justified
  '                  n%         Desired string length
  ' VARIABLES:       ary$()     Array to store individual words from the string
  '                  cnt%       Count of non-space characters
  '                  i%         Looping index
  '                  j%         Count of words
  '                  each%      Minimum space count to insert between words
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Justify (a$, n%)
  '                  DECLARE SUB ParseLine (x$, sep$, a$())
  '                  DECLARE SUB FormatTwo (a$, b$, col%)
  '
    SUB Justify (a$, n%) STATIC
      
      ' If string is shorter than n%, don't bother
        IF LEN(a$) < n% THEN
            EXIT SUB
        END IF
      
      ' Array for list of words from original string
        REDIM ary$(1 TO n%)
      
      ' Split line up into individual words
        ParseLine a$, " ", ary$()
      
      ' Count the words and total of non-space characters
        cnt% = 0
        FOR i% = n% TO 1 STEP -1
            cnt% = cnt% + LEN(ary$(i%))
            IF ary$(i%) = "" THEN
                j% = i% - 1
            END IF
        NEXT i%
      
      ' If only one or zero words, there's not much we can do
        IF j% < 2 THEN
            a$ = LEFT$(ary$(1) + SPACE$(n%), n%)
            EXIT SUB
        END IF
      
      ' We want an extra space at the ends of sentences, questions, etc.
        FOR i% = 1 TO j% - 1
            IF INSTR(".!?", RIGHT$(ary$(i%), 1)) THEN
                ary$(i%) = ary$(i%) + " "
                cnt% = cnt% + 1
            END IF
        NEXT i%
      
      ' How many spaces minimum to add to each word?
        each% = (n% - cnt%) \ (j% - 1)
      
      ' Tack on the minimum spaces to each word
        FOR i% = 1 TO j% - 1
            ary$(i%) = ary$(i%) + SPACE$(each%)
            cnt% = cnt% + each%
        NEXT i%
      
      ' Which is quicker, adding remaining spaces, or
      ' adding spaces to all and removing a few of them?
        IF (n% - cnt%) < j% \ 2 THEN
          
          ' We'll add a few spaces at random
            DO UNTIL cnt% = n%
                DO
                    i% = INT(RND * (j% - 1) + 2)
                LOOP UNTIL LEFT$(ary$(i%), 1) <> " "
                ary$(i%) = " " + ary$(i%)
                cnt% = cnt% + 1
            LOOP
          
        ELSE
          
          ' We'll add a space to each, and then remove some at random
            FOR i% = 2 TO j%
                ary$(i%) = " " + ary$(i%)
                cnt% = cnt% + 1
            NEXT i%
          
          ' Now we'll take a few away at random
            DO UNTIL cnt% = n%
                DO
                    i% = INT(RND * (j% - 1) + 2)
                LOOP UNTIL LEFT$(ary$(i%), 1) = " "
                ary$(i%) = MID$(ary$(i%), 2)
                cnt% = cnt% - 1
            LOOP
          
        END IF
      
      ' Glue it all back together
        a$ = ary$(1)
        FOR i% = 2 TO j%
            a$ = a$ + ary$(i%)
        NEXT i%
      
    END SUB


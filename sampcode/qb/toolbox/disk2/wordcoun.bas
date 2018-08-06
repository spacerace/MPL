  ' ************************************************
  ' **  Name:          WORDCOUN                   **
  ' **  Type:          Toolbox                    **
  ' **  Module:        WORDCOUN.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' USAGE:           WORDCOUN filename
  ' .MAK FILE:       (none)
  ' PARAMETERS:      filename      Name of file to be processed
  ' VARIABLES:       fileName$     Name of file from the command line
  '                  sep$          List of characters defined as word separators
  '                  a$            Each line from the file
  '                  totalCount&   Total count of words
  
    DECLARE FUNCTION WordCount% (a$, sep$)
  
  ' Assume a filename has been given on the command line
    fileName$ = COMMAND$
  
  ' Open the file
    OPEN fileName$ FOR INPUT AS #1
  
  ' Define the word-separating characters as space, tab, and comma
    sep$ = " " + CHR$(9) + ","
  
  ' Read in and process each line
    DO
        LINE INPUT #1, a$
        totalCount& = totalCount& + WordCount%(a$, sep$)
    LOOP UNTIL EOF(1)
  
  ' Print the results
    PRINT "There are"; totalCount&; "words in "; fileName$
  
  ' That's all
    END
  
  ' ************************************************
  ' **  Name:          Wordcount%                 **
  ' **  Type:          Function                   **
  ' **  Module:        WORDCOUN.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the number of words in a string.
  '
  ' EXAMPLE OF USE:  WordCount% a$, sep$
  ' PARAMETERS:      a$         String containing words to be counted
  '                  sep$       List of word separation characters
  ' VARIABLES:       count%     Count of words
  '                  flag%      Indicates if scanning is currently inside of a
  '                             word
  '                  la%        length of a$
  '                  i%         Index to each character of a$
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION WordCount% (a$, sep$)
  '
    FUNCTION WordCount% (a$, sep$) STATIC
        count% = 0
        flag% = 0
        la% = LEN(a$)
        IF la% > 0 AND sep$ <> "" THEN
            FOR i% = 1 TO la%
                IF INSTR(sep$, MID$(a$, i%, 1)) THEN
                    IF flag% THEN
                        flag% = 0
                        count% = count% + 1
                    END IF
                ELSE
                    flag% = 1
                END IF
            NEXT i%
        END IF
        WordCount% = count% + flag%
      
    END FUNCTION
  

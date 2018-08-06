  ' ************************************************
  ' **  Name:          BIN2HEX                    **
  ' **  Type:          Program                    **
  ' **  Module:        BIN2HEX.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Reads in any file and writes out a hexadecimal format file
  ' suitable for rebuilding the original file using the HEX2BIN
  ' program.
  '
  ' USAGE:          BIN2HEX inFileName.ext outFileName.ext
  ' .MAK FILE:      BIN2HEX.BAS
  '                 PARSE.BAS
  ' PARAMETERS:     inFileName    Name of file to be duplicated in hexadecimal
  '                               format
  '                 outFileName   Name of hexadecimal format file to be created
  ' VARIABLES:      cmd$          Working copy of the command line
  '                 inFile$       Name of input file
  '                 outFile$      Name of output file
  '                 byte$         Buffer for binary file access
  '                 i&            Index to each byte of input file
  '                 h$            Pair of hexadecimal characters representing
  '                               each byte
  
  
    DECLARE SUB ParseWord (a$, sep$, word$)
  
  ' Initialization
    CLS
    PRINT "BIN2HEX "; COMMAND$
    PRINT
  
  ' Get the input and output filenames from the command line
    cmd$ = COMMAND$
    ParseWord cmd$, " ,", inFile$
    ParseWord cmd$, " ,", outFile$
  
  ' Verify that both filenames were given
    IF outFile$ = "" THEN
        PRINT
        PRINT "Usage: BIN2HEX inFileName outFileName"
        SYSTEM
    END IF
  
  ' Open the input file
    OPEN inFile$ FOR BINARY AS #1 LEN = 1
    IF LOF(1) = 0 THEN
        CLOSE #1
        KILL inFile$
        PRINT
        PRINT "File not found - "; inFile$
        SYSTEM
    END IF
  
  ' Open the output file
    OPEN outFile$ FOR OUTPUT AS #2
  
  ' Process each byte of the file
    byte$ = SPACE$(1)
    FOR i& = 1 TO LOF(1)
        GET #1, , byte$
        h$ = RIGHT$("0" + HEX$(ASC(byte$)), 2)
        PRINT #2, h$; SPACE$(1);
        IF i& = LOF(1) THEN
            PRINT #2, ""
        ELSEIF i& MOD 16 = 0 THEN
            PRINT #2, ""
        ELSEIF i& MOD 8 = 0 THEN
            PRINT #2, "- ";
        END IF
    NEXT i&
  
  ' Clean up and quit
    CLOSE
    END
  


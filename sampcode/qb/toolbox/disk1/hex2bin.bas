  ' ************************************************
  ' **  Name:          HEX2BIN                    **
  ' **  Type:          Program                    **
  ' **  Module:        HEX2BIN.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Reads in a hexadecimal format file and writes out a binary
  ' file created from the hexadecimal byte numbers.
  '
  ' USAGE:           HEX2BIN inFileName.ext outFileName.ext
  ' .MAK FILE:       HEX2BIN.BAS
  '                  PARSE.BAS
  '                  STRINGS.BAS
  ' PARAMETERS:      inFileName.ext   Name of hexadecimal format file to be read
  '                  outFileName.ext  Name of file to be created
  ' VARIABLES:       cmd$       Working copy of the command line
  '                  inFile$    Name of input file
  '                  outFile$   Name of output file
  '                  h$         Pair of hexadecimal characters representing
  '                             each byte
  '                  i%         Index into list of hexadecimal character pairs
  '                  byte$      Buffer for binary file access
  
    DECLARE SUB ParseWord (a$, sep$, word$)
    DECLARE FUNCTION FilterIn$ (a$, set$)
  
  ' Get the input and output filenames from the command line
    cmd$ = COMMAND$
    ParseWord cmd$, " ,", inFile$
    ParseWord cmd$, " ,", outFile$
  
  ' Verify both filenames were given
    IF outFile$ = "" THEN
        PRINT
        PRINT "Usage: HEX2BIN inFileName.ext outFileName.ext"
        SYSTEM
    END IF
  
  ' Open the input file
    OPEN inFile$ FOR INPUT AS #1
  
  ' Truncate the output file if it already exists
    OPEN outFile$ FOR OUTPUT AS #2
    CLOSE #2
  
  ' Now open it for binary output
    OPEN outFile$ FOR BINARY AS #2 LEN = 1
  
  ' Process each line of the hexadecimal file
    DO
        LINE INPUT #1, h$
        h$ = FilterIn$(UCASE$(h$), "0123456789ABCDEF")
        FOR i% = 1 TO LEN(h$) STEP 2
            byte$ = CHR$(VAL("&H" + MID$(h$, i%, 2)))
            PUT #2, , byte$
        NEXT i%
    LOOP WHILE NOT EOF(1)
  
  ' Clean up and quit
    CLOSE
    END
  


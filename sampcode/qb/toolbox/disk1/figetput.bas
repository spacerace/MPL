  ' ************************************************
  ' **  Name:          FIGETPUT                   **
  ' **  Type:          Toolbox                    **
  ' **  Module:        FIGETPUT.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Reads itself (FIGETPUT.BAS) into a string,
  ' converts characters to uppercase, counts occurrences of
  ' the characters "A" through "Z," and saves the
  ' result in a file named FIGETPUT.TST.
  '
  ' USAGE:           No command line parameters
  ' .MAK FILE:       (none)
  ' PARAMETERS:      filename
  ' VARIABLES:       count%()   Tally array for the 26 alpha characters
  '                  fileName$  Name of file to be processed
  '                  a$         Contents of the file
  '                  i%         Looping index
  '                  c%         ASCII value of each file byte
  
  ' Functions
    DECLARE FUNCTION FileGet$ (fileName$)
  
  ' Subprograms
    DECLARE SUB FilePut (fileName$, a$)
  
  ' Dimension array of counts for each ASCII code "A" to "Z"
    DIM count%(65 TO 90)
  
  ' Read in the file (must be no greater than 32767 bytes long)
    a$ = FileGet$("FIGETPUT.BAS")
  
  ' Convert to uppercase
    a$ = UCASE$(a$)
  
  ' Count the letters
    FOR i% = 1 TO LEN(a$)
        c% = ASC(MID$(a$, i%, 1))
        IF c% >= 65 AND c% <= 90 THEN
            count%(c%) = count%(c%) + 1
        END IF
    NEXT i%
  
  ' Output the results
    CLS
    PRINT "Alphabetic character count for FIGETPUT.BAS"
    PRINT
    FOR i% = 65 TO 90
        PRINT CHR$(i%); " -"; count%(i%),
    NEXT i%
  
  ' Write out the new file
    FilePut "FIGETPUT.TST", a$
  
  ' All done
    END

  ' ************************************************
  ' **  Name:          FileGet$                   **
  ' **  Type:          Function                   **
  ' **  Module:        FIGETPUT.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a string containing the contents of a file.
  ' Maximum file length is 32767 bytes.
  '
  ' EXAMPLE OF USE:  a$ = FileGet$(fileName$)
  ' PARAMETERS:      fileName$     Name of file to be accessed
  ' VARIABLES:       fileNumber    Next available free file number
  '                  length&       Length of file
  '                  a$            String for binary read of file
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION FileGet$ (fileName$)
  '
    FUNCTION FileGet$ (fileName$) STATIC
        fileNumber = FREEFILE
        OPEN fileName$ FOR BINARY AS #fileNumber
        length& = LOF(fileNumber)
        IF length& <= 32767 THEN
            a$ = SPACE$(length&)
            GET #fileNumber, , a$
            FileGet$ = a$
            a$ = ""
        ELSE
            PRINT "FileGet$()... file too large"
            SYSTEM
        END IF
        CLOSE #fileNumber
    END FUNCTION

  ' ************************************************
  ' **  Name:          FilePut                    **
  ' **  Type:          Subprogram                 **
  ' **  Module:        FIGETPUT.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Writes contents of a$ into a binary file named fileName$.
  '
  ' EXAMPLE OF USE:  FilePut fileName$, a$
  ' PARAMETERS:      fileName$  Name of file to be written
  '                  a$         Bytes to be placed in the file
  ' VARIABLES:       fileNumber Next available free file number
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB FilePut (fileName$, a$)
  '
    SUB FilePut (fileName$, a$) STATIC
      
      ' Find available file number
        fileNumber = FREEFILE
      
      ' Truncate any previous contents
        OPEN fileName$ FOR OUTPUT AS #fileNumber
        CLOSE #fileNumber
      
      ' Write string to file
        OPEN fileName$ FOR BINARY AS #fileNumber
        PUT #fileNumber, , a$
      
      ' All done
        CLOSE #fileNumber
      
    END SUB


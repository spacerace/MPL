  ' ************************************************
  ' **  Name:          FILEINFO                   **
  ' **  Type:          Toolbox                    **
  ' **  Module:        FILEINFO.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Collection of subprograms and functions for accessing
  ' directory information about files.
  '
  ' USAGE:           No command line parameters
  ' REQUIREMENTS:    MIXED.QLB/.LIB
  ' .MAK FILE:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       path$      Path to files for gathering directory
  '                             information; wildcard characters accepted
  '                  dta$       Disk transfer area buffer string
  '                  result%    Code returned as result of directory search
  '                  file       Structure of type FileDataType
  '                  n%         File count
  
  ' File search attribute bits
    CONST ISNORMAL = 0
    CONST ISREADONLY = 1
    CONST ISHIDDEN = 2
    CONST ISSYSTEM = 4
    CONST ISVOLUMELABEL = 8
    CONST ISSUBDIRECTORY = 16
    CONST ISARCHIVED = 32
  
  ' Here we'll search for normal files and subdirectories
    CONST FILEATTRIBUTE = ISNORMAL + ISSUBDIRECTORY
  
    TYPE RegTypeX
        ax    AS INTEGER
        bx    AS INTEGER
        cx    AS INTEGER
        dx    AS INTEGER
        bp    AS INTEGER
        si    AS INTEGER
        di    AS INTEGER
        flags AS INTEGER
        ds    AS INTEGER
        es    AS INTEGER
    END TYPE
  
    TYPE FileDataType
        finame    AS STRING * 12
        year      AS INTEGER
        month     AS INTEGER
        day       AS INTEGER
        hour      AS INTEGER
        minute    AS INTEGER
        second    AS INTEGER
        attribute AS INTEGER
        size      AS LONG
    END TYPE
  
  ' Subprograms
    DECLARE SUB INTERRUPTX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
    DECLARE SUB FindFirstFile (path$, dta$, result%)
    DECLARE SUB FindNextFile (dta$, result%)
    DECLARE SUB GetFileData (dta$, file AS FileDataType)
  
  ' Data structures
    DIM file AS FileDataType
  
  ' For demonstration purposes, list current directory
    CLS
    path$ = "*.*"
  
  ' Always start by finding the first match
    FindFirstFile path$, dta$, result%
  
  ' Check that the path$ got us off to a good start
    IF result% THEN
        PRINT "Error: FindFirstFile - found no match for path$"
        SYSTEM
    END IF
  
  ' List all the files in this directory
    DO
        IF n% MOD 19 = 0 THEN
            CLS
            PRINT TAB(4); "File"; TAB(18); "Date"; TAB(29); "Time";
            PRINT TAB(39); "Size"; TAB(48); "Attributes"
            PRINT
        END IF
        GetFileData dta$, file
        PRINT file.finame;
        PRINT USING "  ##/##/####"; file.month, file.day, file.year;
        PRINT USING "  ##:##:##"; file.hour, file.minute, file.second;
        PRINT USING "  ########"; file.size;
        PRINT SPACE$(6);
        PRINT "&H";
        PRINT RIGHT$("0" + HEX$(file.attribute), 2)
        n% = n% + 1
        FindNextFile dta$, result%
        IF n% MOD 19 = 0 THEN
            PRINT
            PRINT "Press any key to continue"
            DO
            LOOP WHILE INKEY$ = ""
        END IF
    LOOP UNTIL result%
    PRINT
    PRINT n%; "files found"

  ' ************************************************
  ' **  Name:          FindFirstFile              **
  ' **  Type:          Subprogram                 **
  ' **  Module:        FILEINFO.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Finds first file that matches the path$.
  '
  ' EXAMPLE OF USE:  FindFirstFile path$, dta$, result%
  ' PARAMETERS:      path$      Complete path, including wildcard characters if
  '                             desired, for the directory search
  '                  dta$       Disk transfer area buffer space
  '                  result%    Returned result code for the search
  ' VARIABLES:       reg        Structure of type RegTypeX
  '                  thePath$   Null terminated version of path$
  '                  sgmt%      Current DTA address segment
  '                  ofst%      Current DTA address offset
  ' MODULE LEVEL
  '   DECLARATIONS:  File search attribute bits
  '                     CONST ISNORMAL = 0
  '                     CONST ISREADONLY = 1
  '                     CONST ISHIDDEN = 2
  '                     CONST ISSYSTEM = 4
  '                     CONST ISVOLUMELABEL = 8
  '                     CONST ISSUBDIRECTORY = 16
  '                     CONST ISARCHIVED = 32
  '
  '                     CONST FILEATTRIBUTE = ISNORMAL + ISSUBDIRECTORY
  '
  '                     TYPE RegTypeX
  '                        ax    AS INTEGER
  '                        bx    AS INTEGER
  '                        cx    AS INTEGER
  '                        dx    AS INTEGER
  '                        bp    AS INTEGER
  '                        si    AS INTEGER
  '                        di    AS INTEGER
  '                        flags AS INTEGER
  '                        ds    AS INTEGER
  '                        es    AS INTEGER
  '                     END TYPE
  '
  '   DECLARE SUB INTERRUPTX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
  '   DECLARE SUB FindFirstFile (path$, dta$, result%)
  '
    SUB FindFirstFile (path$, dta$, result%) STATIC
      
      ' Initialization
        DIM reg AS RegTypeX
      
      ' The path must be a null terminated string
        thePath$ = path$ + CHR$(0)
      
      ' Get current DTA address
        reg.ax = &H2F00
        INTERRUPTX &H21, reg, reg
        sgmt% = reg.es
        ofst% = reg.bx
      
      ' Set dta address
        dta$ = SPACE$(43)
        reg.ax = &H1A00
        reg.ds = VARSEG(dta$)
        reg.dx = SADD(dta$)
        INTERRUPTX &H21, reg, reg
      
      ' Find first file match
        reg.ax = &H4E00
        reg.cx = FILEATTRIBUTE
        reg.ds = VARSEG(thePath$)
        reg.dx = SADD(thePath$)
        INTERRUPTX &H21, reg, reg
      
      ' The carry flag tells if a file was found or not
        result% = reg.flags AND 1
      
      ' Reset the original DTA
        reg.ax = &H1A00
        reg.ds = sgmt%
        reg.dx = ofst%
        INTERRUPTX &H21, reg, reg
      
    END SUB

  ' ************************************************
  ' **  Name:          FindNextFile               **
  ' **  Type:          Subprogram                 **
  ' **  Module:        FILEINFO.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Locates next file. FindFirstFile must be called
  ' before this subprogram is called.
  '
  ' EXAMPLE OF USE: FindNextFile dta$, result%
  ' PARAMETERS:      dta$       Previously filled-in Disk Transfer Area
  '                             buffer string
  '                  result%    Result code for the search
  ' VARIABLES:       reg        Structure of type RegTypeX
  '                  thePath$   Null terminated version of path$
  '                  sgmt%      Current DTA address segment
  '                  ofst%      Current DTA address offset
  ' MODULE LEVEL
  '   DECLARATIONS:  CONST ISNORMAL = 0
  '                  CONST ISREADONLY = 1
  '                  CONST ISHIDDEN = 2
  '                  CONST ISSYSTEM = 4
  '                  CONST ISVOLUMELABEL = 8
  '                  CONST ISSUBDIRECTORY = 16
  '                  CONST ISARCHIVED = 32
  '
  '                  CONST FILEATTRIBUTE = ISNORMAL + ISSUBDIRECTORY
  '
  '                     TYPE RegTypeX
  '                        ax    AS INTEGER
  '                        bx    AS INTEGER
  '                        cx    AS INTEGER
  '                        dx    AS INTEGER
  '                        bp    AS INTEGER
  '                        si    AS INTEGER
  '                        di    AS INTEGER
  '                        flags AS INTEGER
  '                        ds    AS INTEGER
  '                        es    AS INTEGER
  '                     END TYPE
  '
  '   DECLARE SUB INTERRUPTX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
  '   DECLARE SUB FindNextFile (dta$, result%)
  '
    SUB FindNextFile (dta$, result%) STATIC
      
      ' Initialization
        DIM reg AS RegTypeX
      
      ' Be sure dta$ was built (FindFirstFile should have been called)
        IF LEN(dta$) <> 43 THEN
            result% = 2
            EXIT SUB
        END IF
      
      ' Get current DTA address
        reg.ax = &H2F00
        INTERRUPTX &H21, reg, reg
        sgmt% = reg.es
        ofst% = reg.bx
      
      ' Set dta address
        reg.ax = &H1A00
        reg.ds = VARSEG(dta$)
        reg.dx = SADD(dta$)
        INTERRUPTX &H21, reg, reg
      
      ' Find next file match
        reg.ax = &H4F00
        reg.cx = FILEATTRIBUTE
        reg.ds = VARSEG(thePath$)
        reg.dx = SADD(thePath$)
        INTERRUPTX &H21, reg, reg
      
      ' The carry flag tells whether a file was found or not
        result% = reg.flags AND 1
      
      ' Reset the original DTA
        reg.ax = &H1A00
        reg.ds = sgmt%
        reg.dx = ofst%
        INTERRUPTX &H21, reg, reg
      
    END SUB

  ' ************************************************
  ' **  Name:          GetFileData                **
  ' **  Type:          Subprogram                 **
  ' **  Module:        FILEINFO.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Extracts the file directory information from a Disk
  ' Transfer Area (dta$) that has been filled in by a
  ' call to either FindFirstFile or FindNextFile.
  '
  ' EXAMPLE OF USE:  GetFileData dta$, file
  ' PARAMETERS:      dta$       Disk Transfer Area buffer string passed back from
  '                             either FindFirstFile or FindNextFile
  ' VARIABLES:       tim&       Time stamp of the file
  '                  dat&       Date stamp of the file
  '                  f$         Filename during extraction
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE FileDataType
  '                     finame    AS STRING * 12
  '                     year      AS INTEGER
  '                     month     AS INTEGER
  '                     day       AS INTEGER
  '                     hour      AS INTEGER
  '                     minute    AS INTEGER
  '                     second    AS INTEGER
  '                     attribute AS INTEGER
  '                     size      AS LONG
  '                  END TYPE
  '
  '                  DECLARE SUB GetFileData (dta$, file AS FileDataType)
  '
    SUB GetFileData (dta$, file AS FileDataType) STATIC
      
        file.attribute = ASC(MID$(dta$, 22, 1))
        tim& = CVI(MID$(dta$, 23, 2))
        IF tim& < 0 THEN
            tim& = tim& + 65536
        END IF
        file.second = tim& AND &H1F
        file.minute = (tim& \ 32) AND &H3F
        file.hour = (tim& \ 2048) AND &H1F
        dat& = CVI(MID$(dta$, 25, 2))
        file.day = dat& AND &H1F
        file.month = (dat& \ 32) AND &HF
        file.year = ((dat& \ 512) AND &H1F) + 1980
        file.size = CVL(MID$(dta$, 27, 4))
        f$ = MID$(dta$, 31) + CHR$(0)
        file.finame = LEFT$(f$, INSTR(f$, CHR$(0)) - 1)
      
    END SUB


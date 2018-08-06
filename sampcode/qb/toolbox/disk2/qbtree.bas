  ' ************************************************
  ' **  Name:          QBTREE                     **
  ' **  Type:          Program                    **
  ' **  Module:        QBTREE.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' This program creates a list of directories and
  ' subdirectories, and all files in them.  If no
  ' command line path is given, the search
  ' begins with the current directory.
  '
  ' USAGE:          QBTREE [path]
  ' REQUIREMENTS:   MIXED.QLB/.LIB
  ' .MAK FILE:      QBTREE.BAS
  '                 FILEINFO.BAS
  ' PARAMETERS:     path       Path for starting directory search
  ' VARIABLES:      path$      Path string, from the command line, or set
  '                            to "*.*"
  '                 indent%    Indention amount for printing
  
  
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
    DECLARE SUB FileTreeSearch (path$, indent%)
  
  ' Create structure for deciphering the DTA file search results
    DIM file AS FileDataType
  
  ' Get the command line path for starting the file search
    path$ = COMMAND$
  
  ' If no path was given, then use "*.*" to search the current directory
    IF path$ = "" THEN
        path$ = "*.*"
    END IF
 
  ' If only a drive was given, then add "*.*"
    IF LEN(path$) = 2 AND RIGHT$(path$, 1) = ":" THEN
        path$ = path$ + "*.*"
    END IF
  
  ' Adjust the given path if necessary
    IF INSTR(path$, "*") = 0 AND INSTR(path$, "?") = 0 THEN
        FindFirstFile path$, dta$, result%
        IF result% = 0 OR RIGHT$(path$, 1) = "\" THEN
            IF RIGHT$(path$, 1) <> "\" THEN
                path$ = path$ + "\"
            END IF
            path$ = path$ + "*.*"
        END IF
    END IF
  
  ' Start with a clean slate
    CLS
  
  ' Call the recursive search subprogram
    FileTreeSearch path$, indent%
  
  ' That's all there is to it
    END
  

  ' ************************************************
  ' **  Name:          FileTreeSearch             **
  ' **  Type:          Subprogram                 **
  ' **  Module:        QBTREE.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Directory searching and listing subprogram for
  ' the QBTREE program.  (recursive)
  '
  ' EXAMPLE OF USE:  FileTreeSearch path$, indent%
  ' PARAMETERS:      path$      Path for search of files
  '                  indent%    Level of indention, function of recursion
  '                             level
  ' VARIABLES:       file       Structure of type FileDataType
  '                  path$      Path for search of files
  '                  dta$       Disk Transfer Area buffer string
  '                  result%    Returned result code from FindFirstFile or
  '                             FindNextFile
  '                  newPath$   Path with added subdirectory for recursive
  '                             search
  ' MODULE LEVEL
  '   DECLARATIONS: TYPE FileDataType
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
  '                  DECLARE SUB FindFirstFile (path$, dta$, result%)
  '                  DECLARE SUB FindNextFile (dta$, result%)
  '                  DECLARE SUB GetFileData (dta$, file AS FileDataType)
  '                  DECLARE SUB FileTreeSearch (path$, indent%)
  '
    SUB FileTreeSearch (path$, indent%)
      
      ' Create structure for deciphering the DTA file search results
        DIM file AS FileDataType
      
      ' Find the first file given the current search path
        FindFirstFile path$, dta$, result%
      
      ' Search through the directory for all files
        DO UNTIL result%
          
          ' Unpack the Disk Transfer Area for file information
            GetFileData dta$, file
          
          ' Skip the "." and ".." files
            IF LEFT$(file.finame, 1) <> "." THEN
              
              ' Print the filename, indented to show tree structure
                PRINT SPACE$(indent% * 4); file.finame;
              
              ' Print any other desired file information here
                PRINT TAB(50); file.size;
                PRINT TAB(58); file.attribute
              
              ' If we found a directory, then recursively search through it
                IF file.attribute AND &H10 THEN
                  
                  ' Modify path$ to add this new directory to the search path
                    newPath$ = path$
                    IF INSTR(newPath$, "\") = 0 THEN
                        newPath$ = "\" + newPath$
                    END IF
                    DO WHILE RIGHT$(newPath$, 1) <> "\"
                        newPath$ = LEFT$(newPath$, LEN(newPath$) - 1)
                    LOOP
                    newPath$ = newPath$ + file.finame + "\*.*"
                  
                  ' Example of recursion here
                    FileTreeSearch newPath$, indent% + 1
                  
                END IF
              
            END IF
          
          ' Try to find the next file in this directory
            FindNextFile dta$, result%
          
        LOOP
      
    END SUB


  ' ************************************************
  ' **  Name:          DOSCALLS                   **
  ' **  Type:          Toolbox                    **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Demonstrates several interrupt calls to MS-DOS.
  '
  ' USAGE:          No command line parameters
  ' REQUIREMENTS:   MS-DOS 3.0 or later
  '                 MIXED.QLB/.LIB
  '.MAK FILE:       (none)
  ' PARAMETERS:     (none)
  ' VARIABLES:      buffer$    String for buffered input demonstration
  '                 x$         Buffered input string
  '                 drive$     Current disk drive name
  '                 desc       Structure of type MediaDescriptorType
  '                 state%     Current status of the Verify state
  '                 oppositeState%   Opposite state for Verify
  '                 disk       Structure of type DiskFreeSpaceType
  '                 country    Structure of type CountryType
  '                 i%         Loop index for creating translation characters
  '                 a$         Characters to be translated
  '                 path$      Current directory
  '                 result%    Result code from call to SetDirectory
  '                 t$         Temporary copy of TIME$
  '                 attr       Structure of type FileAttributesType
  '                 fileName$  Name of file for determining file attributes
  
  
    TYPE RegType
        ax    AS INTEGER
        bx    AS INTEGER
        cx    AS INTEGER
        dx    AS INTEGER
        bp    AS INTEGER
        si    AS INTEGER
        di    AS INTEGER
        flags AS INTEGER
    END TYPE
  
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
  
    TYPE MediaDescriptorType
        sectorsPerAllocationUnit AS INTEGER
        bytesPerSector AS INTEGER
        FATIdentificationByte AS INTEGER
    END TYPE
  
    TYPE DiskFreeSpaceType
        sectorsPerCluster AS INTEGER
        bytesPerSector AS INTEGER
        clustersPerDrive AS LONG
        availableClusters AS LONG
        availableBytes AS LONG
    END TYPE
  
    TYPE CountryType
        dateTimeFormat AS STRING * 11
        currencySymbol AS STRING * 4
        thousandsSeparator AS STRING * 1
        decimalSeparator AS STRING * 1
        dateSeparator AS STRING * 1
        timeSeparator AS STRING * 1
        currencyThenSymbol AS INTEGER
        currencySymbolSpace AS INTEGER
        currencyPlaces AS INTEGER
        hours24 AS INTEGER
        caseMapSegment AS INTEGER
        caseMapOffset AS INTEGER
        dataListSeparator AS STRING * 1
    END TYPE
  
    TYPE FileAttributesType
        readOnly AS INTEGER
        hidden AS INTEGER
        systemFile AS INTEGER
        archive AS INTEGER
        result AS INTEGER
    END TYPE
  
  ' Subprograms
    DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
    DECLARE SUB InterruptX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
    DECLARE SUB SetDrive (drive$)
    DECLARE SUB GetMediaDescriptor (drive$, desc AS MediaDescriptorType)
    DECLARE SUB SetVerifyState (state%)
    DECLARE SUB GetDiskFreeSpace (drive$, disk AS DiskFreeSpaceType)
    DECLARE SUB GetCountry (country AS CountryType)
    DECLARE SUB CaseMap (character%, BYVAL Segment%, BYVAL Offset%)
    DECLARE SUB SetDirectory (path$, result%)
    DECLARE SUB WriteToDevice (handle%, a$, result%)
    DECLARE SUB GetFileAttributes (fileName$, attr AS FileAttributesType)
    DECLARE SUB SetFileAttributes (fileName$, attr AS FileAttributesType)
  
  ' Functions
    DECLARE FUNCTION DOSVersion! ()
    DECLARE FUNCTION BufferedKeyInput$ (n%)
    DECLARE FUNCTION GetDrive$ ()
    DECLARE FUNCTION GetVerifyState% ()
    DECLARE FUNCTION TranslateCountry$ (a$, country AS CountryType)
    DECLARE FUNCTION GetDirectory$ (drive$)
  
  ' Try the Buffered Keyboard Input call
    CLS
    PRINT "BufferedKeyInput$:"
    PRINT "Enter a string of up to nine characters...  ";
    x$ = BufferedKeyInput$(9)
    PRINT
    PRINT "Here's the nine-character string result... ";
    PRINT CHR$(34); x$; CHR$(34)
  
  ' Get the MS-DOS version number
    PRINT
    PRINT "DosVersion!:"
    PRINT "DOS Version number is "; DOSVersion!
  
  ' Demonstrate the GetDrive and SetDrive routines
    PRINT
    PRINT "GetDrive$ and SetDrive:"
    drive$ = GetDrive$
    PRINT "The current drive is "; drive$
    PRINT "Setting the current drive to A:"
    SetDrive "A:"
    PRINT "Now the current drive is "; GetDrive$
    PRINT "Setting the current drive back to "; drive$
    SetDrive drive$
    PRINT "Now the current drive is "; GetDrive$
  
  ' Call the MS-DOS "Media Descriptor" function for the current drive
    PRINT
    PRINT "GetMediaDescriptor"
    DIM desc AS MediaDescriptorType
    GetMediaDescriptor drive$, desc
    PRINT "Drive                        "; drive$
    PRINT "Sectors per allocation unit "; desc.sectorsPerAllocationUnit
    PRINT "Bytes per sector            "; desc.bytesPerSector
    PRINT "FAT identification byte      &H"; HEX$(desc.FATIdentificationByte)
  
  ' Wait for user
    PRINT
    PRINT
    PRINT "Press any key to continue"
    DO
    LOOP UNTIL INKEY$ <> ""
    CLS
  
  ' Demonstrate the GetVerifyState and SetVerifyState routines
    PRINT
    PRINT "GetVerifyState% and SetVerifyState:"
    state% = GetVerifyState%
    PRINT "Current verify state is"; state%
    oppositeState% = 1 AND NOT state%
    SetVerifyState oppositeState%
    PRINT "Now the verify state is"; GetVerifyState%
    SetVerifyState state%
    PRINT "Now the verify state is"; GetVerifyState%
  
  ' Determine free space on the current drive
    PRINT
    PRINT "GetDiskFreeSpace:"
    DIM disk AS DiskFreeSpaceType
    GetDiskFreeSpace drive$, disk
    PRINT "Sectors per cluster     "; disk.sectorsPerCluster
    PRINT "Bytes per sector        "; disk.bytesPerSector
    PRINT "Total clusters on drive "; disk.clustersPerDrive
    PRINT "Available clusters      "; disk.availableClusters
    PRINT "Available bytes         "; disk.availableBytes
  
  ' Wait for user
    PRINT
    PRINT
    PRINT "Press any key to continue"
    DO
    LOOP UNTIL INKEY$ <> ""
    CLS
  
  ' Get country-dependent information
    PRINT
    PRINT "GetCountry:"
    DIM country AS CountryType
    GetCountry country
    PRINT "Date and time format    "; country.dateTimeFormat
    PRINT "Currency symbol         "; country.currencySymbol
    PRINT "Thousands separator     "; country.thousandsSeparator
    PRINT "Decimal separator       "; country.decimalSeparator
    PRINT "Date separator          "; country.dateSeparator
    PRINT "Time separator          "; country.timeSeparator
    PRINT "Currency before symbol "; country.currencyThenSymbol
    PRINT "Currency symbol space  "; country.currencySymbolSpace
    PRINT "Currency decimal places"; country.currencyPlaces
    PRINT "24-hour time           "; country.hours24
    PRINT "Case map segment       "; country.caseMapSegment
    PRINT "Case map offset        "; country.caseMapOffset
    PRINT "Data list separator     "; country.dataListSeparator
  
  ' Let's translate lowercase characters for the current country
    PRINT
    PRINT "TranslateCountry$:"
    FOR i% = 128 TO 175
        a$ = a$ + CHR$(i%)
    NEXT i%
    PRINT "Character codes 128 to 175, before and after translation... "
    PRINT a$
    PRINT TranslateCountry$(a$, country)
  
  ' Wait for user
    PRINT
    PRINT
    PRINT "Press any key to continue"
    DO
    LOOP UNTIL INKEY$ <> ""
    CLS
  
  ' Demonstrate the SetDirectory and GetDirectory routines
    PRINT
    PRINT "GetDirectory$ and SetDirectory:"
    path$ = GetDirectory$(drive$)
    PRINT "Current directory is "; path$
    SetDirectory GetDrive$ + "\", result%
    PRINT "Now the directory is "; GetDirectory$(drive$)
    SetDirectory path$, result%
    PRINT "Now the directory is "; GetDirectory$(drive$)
  
  ' Write to a file or device
    PRINT
    PRINT "WriteToDevice:"
    PRINT "Writing a 'bell' character to the CRT"
    WriteToDevice 1, CHR$(7), result%
    t$ = TIME$
    DO
    LOOP UNTIL t$ <> TIME$
    PRINT "Writing a 'bell' character to the printer"
    WriteToDevice 4, CHR$(7), result%
  
  ' Wait for user
    PRINT
    PRINT
    PRINT "Press any key to continue"
    DO
    LOOP UNTIL INKEY$ <> ""
    CLS
  
  ' Demonstrate the GetFileAttributes and SetFileAttributes routines
    PRINT
    PRINT "GetFileAttributes and SetFileAttributes:"
    DIM attr AS FileAttributesType
    fileName$ = "C:\IBMDOS.COM"
    GetFileAttributes fileName$, attr
    PRINT "File attributes for "; fileName$
    PRINT "Result of call "; attr.result
    PRINT "Read only      "; attr.readOnly
    PRINT "Hidden         "; attr.hidden
    PRINT "System         "; attr.systemFile
    PRINT "Archive        "; attr.archive
    PRINT
    attr.hidden = 0
    SetFileAttributes fileName$, attr
    GetFileAttributes fileName$, attr
    PRINT "File attributes for "; fileName$
    PRINT "Result of call "; attr.result
    PRINT "Read only      "; attr.readOnly
    PRINT "Hidden         "; attr.hidden
    PRINT "System         "; attr.systemFile
    PRINT "Archive        "; attr.archive
    PRINT
    attr.hidden = 1
    SetFileAttributes fileName$, attr
    GetFileAttributes fileName$, attr
    PRINT "File attributes for "; fileName$
    PRINT "Result of call "; attr.result
    PRINT "Read only      "; attr.readOnly
    PRINT "Hidden         "; attr.hidden
    PRINT "System         "; attr.systemFile
    PRINT "Archive        "; attr.archive
    PRINT
  

  ' ************************************************
  ' **  Name:          BufferedKeyInput$          **
  ' **  Type:          Function                   **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Calls the "Buffered Keyboard Input" MS-DOS function
  ' and returns the entered string of characters.
  '
  ' EXAMPLE OF USE:  x$ = BufferedKeyInput$(n%)
  ' PARAMETERS:      buffer$    Buffer for keyboard input
  ' VARIABLES:       regX       Structure of type RegTypeX
  '                  bufSize%   Length of buffer$
  '                  b$         Working copy of buffer$
  '                  count%     Count of characters entered
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegTypeX
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                     ds    AS INTEGER
  '                     es    AS INTEGER
  '                  END TYPE
  '
  '   DECLARE SUB InterruptX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
  '   DECLARE FUNCTION BufferedKeyInput$ (n%)
  '
    FUNCTION BufferedKeyInput$ (n%) STATIC
        DIM regX AS RegTypeX
        b$ = CHR$(n% + 1) + SPACE$(n% + 1)
        regX.ax = &HA00
        regX.ds = VARSEG(b$)
        regX.dx = SADD(b$)
        InterruptX &H21, regX, regX
        count% = ASC(MID$(b$, 2, 1))
        BufferedKeyInput$ = MID$(b$, 3, count%) + SPACE$(n% - count%)
    END FUNCTION

  ' ************************************************
  ' **  Name:          DOSVersion!                **
  ' **  Type:          Function                   **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the version number of MS-DOS.
  '
  ' EXAMPLE OF USE:  PRINT "MS-DOS Version number is "; DOSVersion!
  ' PARAMETERS:      (none)
  ' VARIABLES:       reg        Structure of type RegType
  '                  major%     Integer part of the MS-DOS version number
  '                  minor%     Fractional part of the MS-DOS version number
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegType
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                  END TYPE
  '
  '      DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
  '      DECLARE FUNCTION DOSVersion! ()
  '
    FUNCTION DOSVersion! STATIC
        DIM reg AS RegType
        reg.ax = &H3000
        Interrupt &H21, reg, reg
        major% = reg.ax MOD 256
        minor% = reg.ax \ 256
        DOSVersion! = major% + minor% / 100!
    END FUNCTION

  ' ************************************************
  ' **  Name:          GetCountry                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns country-dependent information as defined
  ' by MS-DOS.
  '
  ' EXAMPLE OF USE:  GetCountry country
  ' PARAMETERS:      country    Structure of type CountryType
  ' VARIABLES:       regX       Structure of type RegTypeX
  '                  c$         Buffer for data returned from interrupt
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegTypeX
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                     ds    AS INTEGER
  '                     es    AS INTEGER
  '                  END TYPE
  '
  '                  TYPE CountryType
  '                     DateTimeFormat AS STRING * 11
  '                     CurrencySymbol AS STRING * 4
  '                     ThousandsSeparator AS STRING * 1
  '                     DecimalSeparator AS STRING * 1
  '                     DateSeparator AS STRING * 1
  '                     TimeSeparator AS STRING * 1
  '                     CurrencyThenSymbol AS INTEGER
  '                     CurrencySymbolSpace AS INTEGER
  '                     CurrencyPlaces AS INTEGER
  '                     Hours24 AS INTEGER
  '                     caseMapSegment AS INTEGER
  '                     caseMapOffset AS INTEGER
  '                     DataListSeparator AS STRING * 1
  '                  END TYPE
  '
  '   DECLARE SUB InterruptX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
  '   DECLARE SUB GetCountry (country AS CountryType)
  '
    SUB GetCountry (country AS CountryType)
        DIM regX AS RegTypeX
        regX.ax = &H3800
        c$ = SPACE$(32)
        regX.ds = VARSEG(c$)
        regX.dx = SADD(c$)
        InterruptX &H21, regX, regX
        SELECT CASE CVI(LEFT$(c$, 2))
        CASE 0
            country.dateTimeFormat = "h:m:s m/d/y"
        CASE 1
            country.dateTimeFormat = "h:m:s d/m/y"
        CASE 2
            country.dateTimeFormat = "y/m/d h:m:s"
        CASE ELSE
            country.dateTimeFormat = "h:m:s m/d/y"
        END SELECT
        country.currencySymbol = MID$(c$, 3, 4)
        country.thousandsSeparator = MID$(c$, 8, 1)
        country.decimalSeparator = MID$(c$, 10, 1)
        country.dateSeparator = MID$(c$, 12, 1)
        country.timeSeparator = MID$(c$, 14, 1)
        country.currencyThenSymbol = ASC(MID$(c$, 16)) AND 1
        country.currencySymbolSpace = (ASC(MID$(c$, 16)) AND 2) \ 2
        country.currencyPlaces = ASC(MID$(c$, 17))
        country.hours24 = ASC(MID$(c$, 18))
        country.caseMapSegment = CVI(MID$(c$, 21, 2))
        country.caseMapOffset = CVI(MID$(c$, 19, 2))
        country.dataListSeparator = MID$(c$, 23, 1)
    END SUB

  ' ************************************************
  ' **  Name:          GetDirectory$              **
  ' **  Type:          Function                   **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the name of the current directory for any drive.
  '
  ' EXAMPLE OF USE:  path$ = GetDirectory$(drive$)
  ' PARAMETERS:      drive$     Drive of concern, or null string for default
  '                             drive
  ' VARIABLES:       regX       Structure of type RegTypeX
  '                  d$         Working copy of drive$
  '                  p$         Buffer space for returned path
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegTypeX
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                     ds    AS INTEGER
  '                     es    AS INTEGER
  '                  END TYPE
  '
  '   DECLARE SUB InterruptX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
  '   DECLARE FUNCTION GetDirectory$ (drive$)
  '
    FUNCTION GetDirectory$ (drive$) STATIC
        DIM regX AS RegTypeX
        IF drive$ = "" THEN
            d$ = GetDrive$
        ELSE
            d$ = UCASE$(drive$)
        END IF
        drive% = ASC(d$) - 64
        regX.dx = drive%
        regX.ax = &H4700
        p$ = SPACE$(64)
        regX.ds = VARSEG(p$)
        regX.si = SADD(p$)
        InterruptX &H21, regX, regX
        p$ = LEFT$(p$, INSTR(p$, CHR$(0)) - 1)
        GetDirectory$ = LEFT$(d$, 1) + ":\" + p$
        IF regX.flags AND 1 THEN
            GetDirectory$ = ""
        END IF
    END FUNCTION

  ' ************************************************
  ' **  Name:          GetDiskFreeSpace           **
  ' **  Type:          Subprogram                 **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Get information about a drive's organization, including
  ' total number of bytes available.
  '
  ' EXAMPLE OF USE:  GetDiskFreeSpace drive$, disk
  ' PARAMETERS:      drive$     Disk drive designation
  '                  disk       Structure of type DiskFreeSpaceType
  ' VARIABLES:       reg        Structure of type RegType
  '                  drive%     Numeric drive designation
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegType
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                  END TYPE
  '
  '                  TYPE DiskFreeSpaceType
  '                     sectorsPerCluster AS INTEGER
  '                     bytesPerSector AS INTEGER
  '                     clustersPerDrive AS LONG
  '                     availableClusters AS LONG
  '                     availableBytes AS LONG
  '                  END TYPE
  '
  '      DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
  '      DECLARE SUB GetDiskFreeSpace (drive$, disk AS DiskFreeSpaceType)
  '
    SUB GetDiskFreeSpace (drive$, disk AS DiskFreeSpaceType)
        DIM reg AS RegType
        IF drive$ <> "" THEN
            drive% = ASC(UCASE$(drive$)) - 64
        ELSE
            drive% = 0
        END IF
        IF drive% >= 0 THEN
            reg.dx = drive%
        ELSE
            reg.dx = 0
        END IF
        reg.ax = &H3600
        Interrupt &H21, reg, reg
        disk.sectorsPerCluster = reg.ax
        disk.bytesPerSector = reg.cx
        IF reg.dx >= 0 THEN
            disk.clustersPerDrive = reg.dx
        ELSE
            disk.clustersPerDrive = reg.dx + 65536
        END IF
        IF reg.bx >= 0 THEN
            disk.availableClusters = reg.bx
        ELSE
            disk.availableClusters = reg.bx + 65536
        END IF
        disk.availableBytes = disk.availableClusters * reg.ax * reg.cx
    END SUB

  ' ************************************************
  ' **  Name:          GetDrive$                  **
  ' **  Type:          Function                   **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the current disk drive name, such as "A:".
  '
  ' EXAMPLE OF USE:  drive$ = GetDrive$
  ' PARAMETERS:      (none)
  ' VARIABLES:       reg        Structure of type RegType
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegType
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                  END TYPE
  '
  '      DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
  '      DECLARE FUNCTION GetDrive$ ()
  '
    FUNCTION GetDrive$ STATIC
        DIM reg AS RegType
        reg.ax = &H1900
        Interrupt &H21, reg, reg
        GetDrive$ = CHR$((reg.ax AND &HFF) + 65) + ":"
    END FUNCTION

  ' ************************************************
  ' **  Name:          GetFileAttributes          **
  ' **  Type:          Subprogram                 **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the file attribute settings for a file.
  '
  ' EXAMPLE OF USE:  GetFileAttributes fileName$, attr
  ' PARAMETERS:      fileName$  Name of file
  '                  attr       Structure of type FileAttributesType
  ' VARIABLES:       regX       Structure of type RegTypeX
  '                  f$         Null terminated copy of fileName$
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegTypeX
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                     ds    AS INTEGER
  '                     es    AS INTEGER
  '                  END TYPE
  '
  '                  TYPE FileAttributesType
  '                     readOnly AS INTEGER
  '                     hidden AS INTEGER
  '                     systemFile AS INTEGER
  '                     archive AS INTEGER
  '                     result AS INTEGER
  '                  END TYPE
  '
  '   DECLARE SUB InterruptX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
  '   DECLARE SUB GetFileAttributes (fileName$, attr AS FileAttributesType)
  '
    SUB GetFileAttributes (fileName$, attr AS FileAttributesType) STATIC
        DIM regX AS RegTypeX
        regX.ax = &H4300
        f$ = fileName$ + CHR$(0)
        regX.ds = VARSEG(f$)
        regX.dx = SADD(f$)
        InterruptX &H21, regX, regX
        IF regX.flags AND 1 THEN
            attr.result = regX.ax
        ELSE
            attr.result = 0
        END IF
        attr.readOnly = regX.cx AND 1
        attr.hidden = (regX.cx \ 2) AND 1
        attr.systemFile = (regX.cx \ 4) AND 1
        attr.archive = (regX.cx \ 32) AND 1
    END SUB

  ' ************************************************
  ' **  Name:          GetMediaDescriptor         **
  ' **  Type:          Subprogram                 **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Calls the MS-DOS "Get Media Descriptor" function for
  ' the indicated drive.  Results are returned in a
  ' structure of type MediaDescriptorType.
  '
  ' EXAMPLE OF USE:  GetMediaDescriptor drive$, desc
  ' PARAMETERS:      drive$     Drive designation, such as "A:"
  '                  desc       Structure of type MediaDescriptorType
  ' VARIABLES:       regX       Structure of type RegTypeX
  '                  drive%     Numeric drive designation
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegTypeX
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                     ds    AS INTEGER
  '                     es    AS INTEGER
  '                  END TYPE
  '
  '                  TYPE MediaDescriptorType
  '                     sectorsPerAllocationUnit AS INTEGER
  '                     bytesPerSector AS INTEGER
  '                     FATIdentificationByte AS INTEGER
  '                  END TYPE
  '
  '   DECLARE SUB InterruptX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
  '   DECLARE SUB GetMediaDescriptor (drive$, desc AS MediaDescriptorType)
  '
    SUB GetMediaDescriptor (drive$, desc AS MediaDescriptorType) STATIC
        DIM regX AS RegTypeX
        IF drive$ <> "" THEN
            drive% = ASC(UCASE$(drive$)) - 64
        ELSE
            drive% = 0
        END IF
        IF drive% >= 0 THEN
            regX.dx = drive%
        ELSE
            regX.dx = 0
        END IF
        regX.ax = &H1C00
        InterruptX &H21, regX, regX
        desc.sectorsPerAllocationUnit = regX.ax AND &HFF
        desc.bytesPerSector = regX.cx
        DEF SEG = regX.ds
        desc.FATIdentificationByte = PEEK(regX.bx)
        DEF SEG
    END SUB

  ' ************************************************
  ' **  Name:          GetVerifyState%            **
  ' **  Type:          Function                   **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the current state of the MS-DOS "Verify After
  ' Write" flag.
  '
  ' EXAMPLE OF USE:  state% = GetVerifyState%
  ' PARAMETERS:      (none)
  ' VARIABLES:       reg        Structure of type RegType
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegTypeX
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                  END TYPE
  '
  '      DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
  '      DECLARE FUNCTION GetVerifyState% ()
  '
    FUNCTION GetVerifyState% STATIC
        DIM reg AS RegType
        reg.ax = &H5400
        Interrupt &H21, reg, reg
        GetVerifyState% = reg.ax AND &HFF
    END FUNCTION

  ' ************************************************
  ' **  Name:          SetDirectory               **
  ' **  Type:          Subprogram                 **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Sets the current directory.
  '
  ' EXAMPLE OF USE:  SetDirectory path$, result%
  ' PARAMETERS:      path$      The path to the directory
  '                  result%    Returned error code, zero if successful
  ' VARIABLES:       regX       Structure of type RegTypeX
  '                  p$         Null terminated copy of path$
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegTypeX
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                     ds    AS INTEGER
  '                     es    AS INTEGER
  '                  END TYPE
  '
  '   DECLARE SUB InterruptX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
  '   DECLARE SUB SetDirectory (path$, result%)
  '
    SUB SetDirectory (path$, result%) STATIC
        DIM regX AS RegTypeX
        regX.ax = &H3B00
        p$ = path$ + CHR$(0)
        regX.ds = VARSEG(p$)
        regX.dx = SADD(p$)
        InterruptX &H21, regX, regX
        IF regX.flags AND 1 THEN
            result% = regX.ax
        ELSE
            result% = 0
        END IF
    END SUB

  ' ************************************************
  ' **  Name:          SetDrive                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Calls MS-DOS to set the current drive.
  '
  ' EXAMPLE OF USE:  SetDrive d$
  ' PARAMETERS:      d$         Drive designation, such as "A:"
  ' VARIABLES:       reg        Structure of type RegType
  '                  drive%     Numeric value of drive
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegTypeX
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                  END TYPE
  '
  '      DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
  '      DECLARE SUB SetDrive (drive$)
  '
    SUB SetDrive (drive$) STATIC
        DIM reg AS RegType
        IF drive$ <> "" THEN
            drive% = ASC(UCASE$(drive$)) - 65
        ELSE
            drive% = 0
        END IF
        IF drive% >= 0 THEN
            reg.dx = drive%
        ELSE
            reg.dx = 0
        END IF
        reg.ax = &HE00
        Interrupt &H21, reg, reg
    END SUB

  ' ************************************************
  ' **  Name:          SetFileAttributes          **
  ' **  Type:          Subprogram                 **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Sets attribute bits for a file.
  '
  ' EXAMPLE OF USE:  SetFileAttributes fileName$, attr
  ' PARAMETERS:      fileName$  Name of file
  '                  attr       Structure of type FileAttributesType
  ' VARIABLES:       regX       Structure of type RegTypeX
  '                  f$         Null terminated copy of fileName$
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegTypeX
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                     ds    AS INTEGER
  '                     es    AS INTEGER
  '                  END TYPE
  '
  '                 TYPE FileAttributesType
  '                    readOnly AS INTEGER
  '                    hidden AS INTEGER
  '                    systemFile AS INTEGER
  '                    archive AS INTEGER
  '                    result AS INTEGER
  '                 END TYPE
  '
  '   DECLARE SUB InterruptX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
  '   DECLARE SUB SetFileAttributes (fileName$, attr AS FileAttributesType)
  '
    SUB SetFileAttributes (fileName$, attr AS FileAttributesType)
        DIM regX AS RegTypeX
        regX.ax = &H4301
        IF attr.readOnly THEN
            regX.cx = 1
        ELSE
            regX.cx = 0
        END IF
        IF attr.hidden THEN
            regX.cx = regX.cx + 2
        END IF
        IF attr.systemFile THEN
            regX.cx = regX.cx + 4
        END IF
        IF attr.archive THEN
            regX.cx = regX.cx + 32
        END IF
        f$ = fileName$ + CHR$(0)
        regX.ds = VARSEG(f$)
        regX.dx = SADD(f$)
        InterruptX &H21, regX, regX
        IF regX.flags AND 1 THEN
            attr.result = regX.ax
        ELSE
            attr.result = 0
        END IF
    END SUB

  ' ************************************************
  ' **  Name:          SetVerifyState             **
  ' **  Type:          Subprogram                 **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Sets or clears the "Verify After Write" MS-DOS flag.
  '
  ' EXAMPLE OF USE:  SetVerifyState state%
  ' PARAMETERS:      state%     If 0, resets Verify;  If non-zero,
  '                             then sets Verify on
  ' VARIABLES:       reg        Structure of type RegType
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegTypeX
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                  END TYPE
  '
  '      DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
  '      DECLARE SUB SetVerifyState (state%)
  '
    SUB SetVerifyState (state%) STATIC
        DIM reg AS RegType
        IF state% THEN
            reg.ax = &H2E01
        ELSE
            reg.ax = &H2E00
        END IF
        Interrupt &H21, reg, reg
    END SUB

  ' ************************************************
  ' **  Name:          TranslateCountry$          **
  ' **  Type:          Function                   **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a string of characters translated according to
  ' the current country setting of MS-DOS.
  '
  ' EXAMPLE OF USE:  b$ = TranslateCountry$(a$, country)
  ' PARAMETERS:      a$         String to be translated
  '                  country    Structure of type CountryType
  ' VARIABLES:       i%         Index to each character of a$
  '                  c%         Byte value of each character in a$
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE CountryType
  '                     DateTimeFormat AS STRING * 11
  '                     CurrencySymbol AS STRING * 4
  '                     ThousandsSeparator AS STRING * 1
  '                     DecimalSeparator AS STRING * 1
  '                     DateSeparator AS STRING * 1
  '                     TimeSeparator AS STRING * 1
  '                     CurrencyThenSymbol AS INTEGER
  '                     CurrencySymbolSpace AS INTEGER
  '                     CurrencyPlaces AS INTEGER
  '                     Hours24 AS INTEGER
  '                     caseMapSegment AS INTEGER
  '                     caseMapOffset AS INTEGER
  '                     DataListSeparator AS STRING * 1
  '                  END TYPE
  '
  '           DECLARE SUB CaseMap (character%, BYVAL Segment%, BYVAL Offset%)
  '           DECLARE FUNCTION TranslateCountry$ (a$, country AS CountryType)
  '
    FUNCTION TranslateCountry$ (a$, country AS CountryType) STATIC
        FOR i% = 1 TO LEN(a$)
            c% = ASC(MID$(a$, i%))
            CaseMap c%, country.caseMapSegment, country.caseMapOffset
            MID$(a$, i%, 1) = CHR$(c%)
        NEXT i%
        TranslateCountry$ = a$
    END FUNCTION

  ' ************************************************
  ' **  Name:          WriteToDevice              **
  ' **  Type:          Subprogram                 **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Writes bytes to a file or device.
  '
  ' EXAMPLE OF USE:  WriteToDevice handle%, a$, result%
  ' PARAMETERS:      handle%    File or device handle
  '                  a$         String to be output
  '                  result%    Error code returned from MS-DOS
  ' VARIABLES:       regX       Structure of type RegTypeX
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegTypeX
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                     ds    AS INTEGER
  '                     es    AS INTEGER
  '                  END TYPE
  '
  '   DECLARE SUB InterruptX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
  '   DECLARE SUB WriteToDevice (handle%, a$, result%)
  '
    SUB WriteToDevice (handle%, a$, result%) STATIC
        DIM regX AS RegTypeX
        regX.ax = &H4000
        regX.cx = LEN(a$)
        regX.bx = handle%
        regX.ds = VARSEG(a$)
        regX.dx = SADD(a$)
        InterruptX &H21, regX, regX
        IF regX.flags AND 1 THEN
            result% = regX.ax
        ELSEIF regX.ax <> LEN(a$) THEN
            result% = -1
        ELSE
            result% = 0
        END IF
    END SUB


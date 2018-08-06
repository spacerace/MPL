  ' ************************************************
  ' **  Name:          BIOSCALL                   **
  ' **  Type:          Toolbox                    **
  ' **  Module:        BIOSCALL.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Demonstrates several interrupt calls to the ROM BIOS.
  '
  ' USAGE: No command line parameters
  ' REQUIREMENTS:    MIXED.QLB/.LIB
  ' .MAK FILE:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       i%         Loop index for creating lines to scroll
  '                  equip      Structure of type EquipmentType
  '                  mode%      Video mode returned by VideoState
  '                  columns%   Video columns returned by VideoState
  '                  page%      Video page returned by VideoState
  '                  shift      Structure of type ShiftType
  
  
  ' Constants
    CONST FALSE = 0
    CONST TRUE = NOT FALSE
  
  ' Declare the Type structures
    TYPE RegType
        ax    AS INTEGER
        bx    AS INTEGER
        cx    AS INTEGER
        dx    AS INTEGER
        Bp    AS INTEGER
        si    AS INTEGER
        di    AS INTEGER
        flags AS INTEGER
    END TYPE
  
    TYPE RegTypeX
        ax    AS INTEGER
        bx    AS INTEGER
        cx    AS INTEGER
        dx    AS INTEGER
        Bp    AS INTEGER
        si    AS INTEGER
        di    AS INTEGER
        flags AS INTEGER
        ds    AS INTEGER
        es    AS INTEGER
    END TYPE
  
    TYPE EquipmentType
        printers     AS INTEGER
        gameAdapter  AS INTEGER
        serial       AS INTEGER
        floppies     AS INTEGER
        initialVideo AS INTEGER
        coprocessor  AS INTEGER
    END TYPE
  
    TYPE ShiftType
        right           AS INTEGER
        left            AS INTEGER
        ctrl            AS INTEGER
        alt             AS INTEGER
        scrollLockState AS INTEGER
        numLockState    AS INTEGER
        capsLockState   AS INTEGER
        insertState     AS INTEGER
    END TYPE
  
    DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
    DECLARE SUB InterruptX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
    DECLARE SUB PrintScreen ()
    DECLARE SUB Scroll (row1%, col1%, row2%, col2%, lines%, attribute%)
    DECLARE SUB Equipment (equip AS EquipmentType)
    DECLARE SUB VideoState (mode%, columns%, page%)
    DECLARE SUB GetShiftStates (shift AS ShiftType)
    DECLARE SUB ReBoot ()
  
  ' Demonstrate the Scroll subprogram
    CLS
    FOR i% = 1 TO 15
        COLOR i%, i% - 1
        PRINT STRING$(25, i% + 64)
    NEXT i%
    COLOR 7, 0
    PRINT
    PRINT "Press <Enter> to scroll part of the screen"
    DO
    LOOP UNTIL INKEY$ = CHR$(13)
    Scroll 2, 3, 6, 16, 3, SCREEN(2, 3, 1)
  
  ' Wait for user before continuing
    PRINT
    PRINT "Press any key to continue"
    DO
    LOOP UNTIL INKEY$ <> ""
    CLS
  
  ' Determine the equipment information
    DIM equip AS EquipmentType
    Equipment equip
    PRINT "Printers:", equip.printers
    PRINT "Game adapter:", equip.gameAdapter
    PRINT "Serial IO:", equip.serial
    PRINT "Floppies:", equip.floppies
    PRINT "Video:", equip.initialVideo
    PRINT "Coprocessor:", equip.coprocessor
  
  ' Determine the current video state
    PRINT
    VideoState mode%, columns%, page%
    PRINT "Video mode:", mode%
    PRINT "Text columns:", columns%
    PRINT "Video page:", page%
  
  ' Wait for user before continuing
    PRINT
    PRINT "Press any key to continue"
    DO
    LOOP UNTIL INKEY$ <> ""
  
  ' Demonstrate the shift key states
    CLS
    PRINT "(Press shift keys, then <Enter> to continue...)"
    DIM shift AS ShiftType
    DO
        LOCATE 4, 1
        PRINT "Shift states:"
        GetShiftStates shift
        PRINT
        PRINT "Left shift:", shift.left
        PRINT "Right shift:", shift.right
        PRINT "Ctrl:", shift.ctrl
        PRINT "Alt:", shift.alt
        PRINT "Scroll Lock:", shift.scrollLockState
        PRINT "Num Lock:", shift.numLockState
        PRINT "Caps Lock:", shift.capsLockState
        PRINT "Insert:", shift.insertState
    LOOP UNTIL INKEY$ = CHR$(13)
  
  ' Uncomment the following line to cause a screen dump to printer....
  ' PrintScreen
  
  ' Uncomment the following line only if you want to reboot....
  ' ReBoot
  
    END
  

  ' ************************************************
  ' **  Name:          Equipment                  **
  ' **  Type:          Subprogram                 **
  ' **  Module:        BIOSCALL.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns equipment configuration information from BIOS.
  '
  ' EXAMPLE OF USE:  Equipment equip
  ' PARAMETERS:      equip      Structure of type EquipmentType
  ' VARIABLES:       reg        Structure of type RegType
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegType
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     Bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                  END TYPE
  '
  '                  TYPE EquipmentType
  '                     printers     AS INTEGER
  '                     gameAdapter  AS INTEGER
  '                     serial       AS INTEGER
  '                     floppies     AS INTEGER
  '                     initialVideo AS INTEGER
  '                     coprocessor  AS INTEGER
  '                  END TYPE
  '
  '     DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
  '     DECLARE SUB Equipment (equip AS EquipmentType)
  '
    SUB Equipment (equip AS EquipmentType) STATIC
        DIM reg AS RegType
        Interrupt &H11, reg, reg
        equip.printers = (reg.ax AND &HC000&) \ 16384
        equip.gameAdapter = (reg.ax AND &H1000) \ 4096
        equip.serial = (reg.ax AND &HE00) \ 512
        equip.floppies = (reg.ax AND &HC0) \ 64 + 1
        equip.initialVideo = (reg.ax AND &H30) \ 16
        equip.coprocessor = (reg.ax AND 2) \ 2
    END SUB

  ' ************************************************
  ' **  Name:          GetShiftStates             **
  ' **  Type:          Subprogram                 **
  ' **  Module:        BIOSCALL.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Return state of the various shift keys.
  '
  ' EXAMPLE OF USE:  GetShiftStates shift
  ' PARAMETERS:      shift      Structure of type ShiftType
  ' VARIABLES:       reg        Structure of type RegType
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegType
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     Bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                  END TYPE
  '
  '                  TYPE ShiftType
  '                     right           AS INTEGER
  '                     left            AS INTEGER
  '                     ctrl            AS INTEGER
  '                     alt             AS INTEGER
  '                     scrollLockState AS INTEGER
  '                     numLockState    AS INTEGER
  '                     capsLockState   AS INTEGER
  '                     insertState     AS INTEGER
  '                  END TYPE
  '
  '      DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
  '      DECLARE SUB GetShiftStates (shift AS ShiftType)
  '
    SUB GetShiftStates (shift AS ShiftType) STATIC
        DIM reg AS RegType
        reg.ax = &H200
        Interrupt &H16, reg, reg
        shift.right = reg.ax AND 1
        shift.left = (reg.ax AND 2) \ 2
        shift.ctrl = (reg.ax AND 4) \ 4
        shift.alt = (reg.ax AND 8) \ 8
        shift.scrollLockState = (reg.ax AND 16) \ 16
        shift.numLockState = (reg.ax AND 32) \ 32
        shift.capsLockState = (reg.ax AND 64) \ 64
        shift.insertState = (reg.ax AND 128) \ 128
    END SUB

  ' ************************************************
  ' **  Name:          PrintScreen                **
  ' **  Type:          Subprogram                 **
  ' **  Module:        BIOSCALL.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Activates interrupt 5 to cause a dump of the
  ' screen's contents to the printer.
  '
  ' EXAMPLE OF USE:  PrintScreen
  ' PARAMETERS:      (none)
  ' VARIABLES:       reg        Structure of type RegType
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegType
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     Bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                  END TYPE
  '
  '      DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
  '      DECLARE SUB PrintScreen ()
  '
    SUB PrintScreen STATIC
        DIM reg AS RegType
        Interrupt 5, reg, reg
    END SUB

  ' ************************************************
  ' **  Name:          ReBoot                     **
  ' **  Type:          Subprogram                 **
  ' **  Module:        BIOSCALL.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Causes the computer to reboot.
  '
  ' EXAMPLE OF USE:  ReBoot
  ' PARAMETERS:      (none)
  ' VARIABLES:       reg        Structure of type RegType
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegType
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     Bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                  END TYPE
  '
  '      DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
  '      DECLARE SUB ReBoot ()
  '
    SUB ReBoot STATIC
        DIM reg AS RegType
        Interrupt &H19, reg, reg
    END SUB

  ' ************************************************
  ' **  Name:          Scroll                     **
  ' **  Type:          Subprogram                 **
  ' **  Module:        BIOSCALL.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Scrolls the screen in the rectangular area defined
  ' by the row and col parameters.  Positive line count
  ' moves the lines up, leaving blank lines at bottom;
  ' negative line count moves the lines down.
  '
  ' EXAMPLE OF USE:  Scroll row1%, col1%, row2%, col2%, lines%, attr%
  ' PARAMETERS:      row1%    Upper left character row defining rectangular
  '                           scroll area
  '                  col1     Upper left character column defining rectangular
  '                           scroll area
  '                  row2%    Lower right character row defining rectangular
  '                           scroll area
  '                  col2%    Lower right character column defining
  '                           rectangular scroll area
  '                  lines%   Number of character lines to scroll
  '                  attr%    Color attribute byte to be used in new text
  '                           lines scrolled onto the screen
  ' VARIABLES:       reg      Structure of type RegType
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegType
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     Bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                  END TYPE
  '      DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
  '      DECLARE SUB Scroll (row1%, col1%, row2%, col2%, lines%, attribute%)
  '
    SUB Scroll (row1%, col1%, row2%, col2%, lines%, attribute%) STATIC
        DIM reg AS RegType
        IF lines% > 0 THEN
            reg.ax = &H600 + lines% MOD 256
        ELSE
            reg.ax = &H700 + ABS(lines%) MOD 256
        END IF
        reg.bx = (attribute% * 256&) AND &HFF00
        reg.cx = (row1% - 1) * 256 + col1% - 1
        reg.dx = (row2% - 1) * 256 + col2% - 1
        Interrupt &H10, reg, reg
    END SUB

  ' ************************************************
  ' **  Name:          VideoState                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        BIOSCALL.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Determines the current video mode parameters.
  '
  ' EXAMPLE OF USE:  VideoState mode%, columns%, page%
  ' PARAMETERS:      mode%      Current video mode
  '                  columns%   Current number of text columns
  '                  page%      Current active display page
  ' VARIABLES:       reg        Structure of type RegType
  ' MODULE LEVEL
  '   DECLARATIONS:  TYPE RegType
  '                     ax    AS INTEGER
  '                     bx    AS INTEGER
  '                     cx    AS INTEGER
  '                     dx    AS INTEGER
  '                     Bp    AS INTEGER
  '                     si    AS INTEGER
  '                     di    AS INTEGER
  '                     flags AS INTEGER
  '                  END TYPE
  '
  '      DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
  '      DECLARE SUB VideoState (mode%, columns%, page%)
  '
    SUB VideoState (mode%, columns%, page%) STATIC
        DIM reg AS RegType
        reg.ax = &HF00
        Interrupt &H10, reg, reg
        mode% = reg.ax AND &HFF
        columns% = (CLNG(reg.ax) AND &HFF00) \ 256
        page% = (CLNG(reg.bx) AND &HFF00) \ 256
    END SUB


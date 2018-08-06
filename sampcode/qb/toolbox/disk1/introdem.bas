  ' ************************************************
  ' **  Name:          INTRODEM                   **
  ' **  Type:          Program                    **
  ' **  Module:        INTRODEM.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Quick introductory demonstration created from the
  ' QuickBASIC Toolbox routines.
  ' USAGE:  Run as a main program to see the demonstration.
  ' REQUIREMENTS: CGA or VGA
  ' .MAK FILE CONTENTS: (none)
  ' FUNCTIONS:    Collision%            Date2Day%           Date2Julian&
  '               Date2Month%           Date2Year%          DayOfTheWeek$
  '               DOSVersion!           GetDirectory$       GetDrive$
  '               GetVerifyState%       InKeyCode%          Julian2Date$
  '               KeyCode%              MDY2Date$           MenuString%
  '               MonthName$
  ' SUBPROGRAMS:  Attrib                DrawBox             EditLine
  '               Equipment             FindFirstFile       FindNextFile
  '               GetDiskFreeSpace      GetFileData         GetShiftStates
  '               Interrupt             InterruptX          Mouse
  '               MouseMickey           OneMonthCalendar    SpaceWorms
  '               TextGet               TextPut             VideoState
  ' PARAMETERS:   (none)
  ' VARIABLES:    disk          Structure of type DiskFreeSpaceType
  '               equip         Structure of type EquipmentType
  '               file          Structure of type FileDataType
  '               shift         Structure of type ShiftType
  '               wormship1%()  First image of UFO
  '               i%            Looping index
  '               h$            Hexadecimal notation character string
  '               wormship2%()  Second image of UFO
  '               title$()      Storage for title boxes
  '               row%()        Row location of each title box
  '               col%()        Column location of each title box
  '               row2%         Row location at opposite corner of title box
  '               col2%         Column location at opposite corner of title box
  '               t0            Timer value at start of timing loop
  '               path$         String for finding directory contents
  '               result%       Returned code for finding each file name
  '               dta$          Disk transfer area buffer string
  '               mode%         Current video mode number
  '               columns%      Current number of video columns
  '               page%         Current video page number
  '               drive$        Default drive string for getting disk
  '                               information
  '               quitFlag%     Indicates user wants to quit
  
  ' Constants
    CONST FALSE = 0
    CONST TRUE = NOT FALSE
  
  ' Define color constants
    CONST BLACK = 0
    CONST BLUE = 1
    CONST GREEN = 2
    CONST CYAN = 3
    CONST RED = 4
    CONST MAGENTA = 5
    CONST BROWN = 6
    CONST WHITE = 7
    CONST BRIGHT = 8
    CONST BLINK = 16
    CONST YELLOW = BROWN + BRIGHT
  
  ' Key code numbers
    CONST BACKSPACE = 8
    CONST CTRLLEFTARROW = 29440
    CONST CTRLRIGHTARROW = 29696
    CONST CTRLY = 25
    CONST CTRLQ = 17
    CONST DELETE = 21248
    CONST DOWNARROW = 20480
    CONST ENDKEY = 20224
    CONST ENTER = 13
    CONST ESCAPE = 27
    CONST HOME = 18176
    CONST INSERTKEY = 20992
    CONST LEFTARROW = 19200
    CONST RIGHTARROW = 19712
    CONST TABKEY = 9
    CONST UPARROW = 18432
  
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
  
    TYPE DiskFreeSpaceType
        sectorsPerCluster AS INTEGER
        bytesPerSector AS INTEGER
        clustersPerDrive AS LONG
        availableClusters AS LONG
        availableBytes AS LONG
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
  
  ' Functions
    DECLARE FUNCTION Collision% (object%(), backGround%())
    DECLARE FUNCTION Date2Day% (dat$)
    DECLARE FUNCTION Date2Julian& (dat$)
    DECLARE FUNCTION Date2Month% (dat$)
    DECLARE FUNCTION Date2Year% (dat$)
    DECLARE FUNCTION DayOfTheWeek$ (dat$)
    DECLARE FUNCTION DOSVersion! ()
    DECLARE FUNCTION GetDirectory$ (drive$)
    DECLARE FUNCTION GetDrive$ ()
    DECLARE FUNCTION GetVerifyState% ()
    DECLARE FUNCTION InKeyCode% ()
    DECLARE FUNCTION Julian2Date$ (julian&)
    DECLARE FUNCTION KeyCode% ()
    DECLARE FUNCTION MDY2Date$ (month%, day%, year%)
    DECLARE FUNCTION MenuString% CDECL (row%, col%, a$)
    DECLARE FUNCTION MonthName$ (dat$)
  
  ' Subprograms
    DECLARE SUB Attrib ()
    DECLARE SUB DrawBox (row1%, col1%, row2%, col2%)
    DECLARE SUB EditLine (a$, exitCode%)
    DECLARE SUB Equipment (equip AS ANY)
    DECLARE SUB FindFirstFile (path$, dta$, result%)
    DECLARE SUB FindNextFile (dta$, result%)
    DECLARE SUB GetDiskFreeSpace (drive$, disk AS ANY)
    DECLARE SUB GetFileData (dta$, file AS ANY)
    DECLARE SUB GetShiftStates (shift AS ANY)
    DECLARE SUB Interrupt (intnum%, inreg AS RegType, outreg AS RegType)
    DECLARE SUB InterruptX (intnum%, inreg AS RegTypeX, outreg AS RegTypeX)
    DECLARE SUB Mouse (m1%, m2%, m3%, m4%)
    DECLARE SUB MouseMickey (horizontal%, vertical%)
    DECLARE SUB OneMonthCalendar (dat$, row%, col%)
    DECLARE SUB SpaceWorms ()
    DECLARE SUB TextGet CDECL (r1%, c1%, r2%, c2%, a$)
    DECLARE SUB TextPut CDECL (r1%, c1%, r2%, c2%, a$)
    DECLARE SUB VideoState (mode%, columns%, page%)
  
  ' Data structures
    DIM disk AS DiskFreeSpaceType
    DIM equip AS EquipmentType
    DIM file AS FileDataType
    DIM shift AS ShiftType
  
  ' Worm ship image 1
  ' (DRAW$) "c1u2e2r4f2d2 l8 be1p1,1 bf1br6 c2r6f3g3l20h3e3r18 "
  ' (DRAW$) "bd3 p3,2 c2 l1 bl3 l1 bl3 l1 bl3 l1 bl3 l1 bd7 br1"
  ' (DRAW$) " e3r10 f3"
    DIM wormship1%(0 TO 54)
    FOR i% = 0 TO 54
        READ h$
        wormship1%(i%) = VAL("&H" + h$)
    NEXT i%
    DATA 36,F,0,5501,0,0,500,4055,0,0,5515,50,0,1500,5055,0,AA02
    DATA AAAA,AAAA,B00,FFFF,FFFF,80FF,FF2F,FFFF,FFFF,BFE0,AFAF
    DATA AFAF,F8AF,FF2F,FFFF,FFFF,BE0,FFFF,FFFF,80FF,AA02,AAAA
    DATA AAAA,0,AA00,A8AA,0,200,0,2,0,8,0,80,2000,0,2000,0
  
  ' Worm ship image 2
  ' (DRAW$) "c1u2e2r4f2d2 l8 be1p1,1 bf1br6 c2r6f3g3l20h3e3r18 "
  ' (DRAW$) "bd3 p3,2 c2 br2 l1 bl3 l1 bl3 l1 bl3 l1 bl3 l1 bl3"
  ' (DRAW$) " l1 bd7 br3 e3r10 f3"
    DIM wormship2%(0 TO 54)
    FOR i% = 0 TO 54
        READ h$
        wormship2%(i%) = VAL("&H" + h$)
    NEXT i%
    DATA 36,F,0,5501,0,0,400,4000,0,0,10,10,0,1000,1000,0,AA02
    DATA AAAA,AAAA,B00,FFFF,FFFF,80FF,FF2F,FFFF,FFFF,BAE0,FAFA
    DATA FAFA,F8FA,FF2F,FFFF,FFFF,BE0,FFFF,FFFF,80FF,AA02,AAAA
    DATA AAAA,0,AA00,A8AA,0,200,0,2,0,8,0,80,2000,0,2000,0
  
  ' Initialization
    DIM title$(0 TO 17), row%(0 TO 17), col%(0 TO 17)
    FOR i% = 0 TO 17
        title$(i%) = SPACE$(432)
    NEXT i%
    COLOR BLACK, WHITE
    CLS
    menu$ = "  Files  Equipment  Game  Attributes  Calendar  Quit  "
  
  ' Main loop
    DO
      
      ' Create main screen
        COLOR BLACK, WHITE
        DrawBox 1, 1, 25, 80
        DrawBox 2, 3, 24, 78
        DrawBox 3, 5, 23, 76
      
      ' Build the title box
        COLOR YELLOW, BLUE
        DrawBox 9, 29, 17, 52
        COLOR BRIGHT + MAGENTA, BLUE
        LOCATE 10, 35
        PRINT "Introducing"
        COLOR BRIGHT + GREEN, BLUE
        LOCATE 12, 39
        PRINT "THE"
        LOCATE 13, 34
        PRINT "QuickBASIC 4.0"
        LOCATE 14, 37
        PRINT "TOOLBOX"
      
      ' Grab a copy of the title box
        TextGet 9, 29, 17, 52, title$(0)
        row%(0) = 9
        col%(0) = 29
      
      ' Randomly place 17 title boxes, saving the background
        FOR i% = 1 TO 17
            row%(i%) = INT(RND * 16) + 1
            col%(i%) = INT(RND * 56) + 1
            row2% = row%(i%) + 8
            col2% = col%(i%) + 23
            TextGet row%(i%), col%(i%), row2%, col2%, title$(i%)
            TextPut row%(i%), col%(i%), row2%, col2%, title$(0)
        NEXT i%
      
      ' Delay for half a second
        t0 = TIMER
        DO
        LOOP WHILE TIMER - t0 < .5
      
      ' Replace the backgrounds
        FOR i% = 17 TO 0 STEP -1
            row2% = row%(i%) + 8
            col2% = col%(i%) + 23
            TextPut row%(i%), col%(i%), row2%, col2%, title$(i%)
        NEXT i%
      
      ' Now for the main menu
        LOCATE , , 0
        SELECT CASE MenuString%(6, 12, menu$)
          
      ' Current drive, path, and directory listing
        CASE 1
            COLOR BRIGHT + WHITE, CYAN
            CLS
            PRINT "Enter path (or just press ";
            PRINT CHR$(17); CHR$(196); CHR$(217); ")... "
            path$ = GetDirectory$("")
            IF RIGHT$(path$, 1) <> "\" THEN
                path$ = path$ + "\"
            END IF
            path$ = LEFT$(path$ + "*.*" + SPACE$(70), 70)
            COLOR YELLOW, BLUE
            LOCATE 2, 5
            EditLine path$, exitCode%
            path$ = LTRIM$(RTRIM$(path$))
            IF path$ = "" OR RIGHT$(path$, 1) = "\" THEN
                path$ = path$ + "*.*"
            END IF
            COLOR BLACK, CYAN
            PRINT
            PRINT
            FindFirstFile path$, dta$, result%
            DO UNTIL result%
                GetFileData dta$, file
                IF file.attribute AND &H10 THEN
                    PRINT "*"; file.finame,
                ELSE
                    PRINT " "; file.finame,
                END IF
                FindNextFile dta$, result%
            LOOP
            PRINT
            PRINT
            COLOR BRIGHT + WHITE, CYAN
            PRINT "Press any key to continue"
            DO
            LOOP WHILE INKEY$ = ""
            COLOR BLACK, WHITE
            CLS
          
      ' Equipment information
        CASE 2
            COLOR BRIGHT + GREEN, BLUE
            CLS
            PRINT "Information about your hardware and software..."
            COLOR YELLOW, BLUE
            PRINT
            PRINT "Current version of DOS is "; DOSVersion!
            Equipment equip
            PRINT "Number of printers", equip.printers
            PRINT "Game adapter", , equip.gameAdapter
            PRINT "Serial I/O ports", equip.serial
            PRINT "Floppy disk drives", equip.floppies
            PRINT "Initial video state", equip.initialVideo
            PRINT "Numerical coprocessor", equip.coprocessor
            VideoState mode%, columns%, page%
            PRINT "Video mode number", mode%
            PRINT "Video width", , columns%
            PRINT "Video page", , page%
            GetShiftStates shift
            PRINT "Scroll lock", , shift.scrollLockState
            PRINT "Num lock", , shift.numLockState
            PRINT "Caps lock", , shift.capsLockState
            PRINT "Insert lock", , shift.insertState
            PRINT "Disk verify state", GetVerifyState%
            GetDiskFreeSpace drive$, disk
            PRINT "Disk sectors per cluster", disk.sectorsPerCluster
            PRINT "Disk bytes per sector", disk.bytesPerSector
            PRINT "Disk clusters on drive", disk.clustersPerDrive
            PRINT "Disk available clusters", disk.availableClusters
            PRINT "Disk available bytes", disk.availableBytes
            PRINT "Current complete path", " "; RTRIM$(GetDirectory$(""))
            LOCATE 15, 45
            COLOR BRIGHT + WHITE, BLACK
            PRINT " Press any key to continue "
            DO
            LOOP WHILE INKEY$ = ""
            COLOR WHITE, BLACK
            CLS
          
      ' Worms from space
        CASE 3
            SpaceWorms
          
      ' Color attributes
        CASE 4
            Attrib
            LOCATE 23, 1
            PRINT "Press any key to continue"
            DO
            LOOP WHILE INKEY$ = ""
            CLS
          
      ' Calendar sheet for this month
        CASE 5
            COLOR YELLOW, CYAN
            CLS
            COLOR YELLOW, BLUE
            DrawBox 6, 21, 19, 60
            COLOR BRIGHT + WHITE, BLUE
            OneMonthCalendar DATE$, 9, 26
            LOCATE 23, 27
            PRINT "Press any key to continue"
            DO
            LOOP WHILE INKEY$ = ""
            COLOR WHITE, BLACK
            CLS
          
      ' Must be time to quit
        CASE ELSE
            quitFlag% = TRUE
          
        END SELECT
      
    LOOP UNTIL quitFlag%
  
  ' All done
    COLOR WHITE, BLACK
    CLS
    END
  
  ' ************************************************
  ' **  Name:          Attrib                     **
  ' **  Type:          Subprogram                 **
  ' **  Module:        ATTRIB.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Displays table of color attributes for text mode
  '
    SUB Attrib STATIC
        SCREEN 0
        CLS
        PRINT "Attributes for the COLOR statement in text mode (SCREEN 0)."
        PRINT "Add 16 to the foreground to cause the character to blink."
        FOR bgd% = 0 TO 7
            COLOR bgd% XOR 7, bgd%
            PRINT
            PRINT "Background%"; STR$(bgd%),
            PRINT "Foreground% ..."; SPACE$(41)
            FOR fgd% = 0 TO 15
                COLOR fgd%, bgd%
                PRINT STR$(fgd%); "  ";
            NEXT fgd%
        NEXT bgd%
        COLOR 7, 0
        PRINT
    END SUB
  
  ' ************************************************
  ' **  Name:          Collision%                 **
  ' **  Type:          Function                   **
  ' **  Module:        GAMES.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns TRUE if any non-zero pixels occur in the
  ' same byte of video memory, as saved in the object%()
  ' and backGround%() arrays.  The arrays must be the
  ' same size.
  '
    FUNCTION Collision% (object%(), backGround%()) STATIC
        lo% = LBOUND(object%)
        uo% = UBOUND(object%)
        lb% = LBOUND(backGround%)
        ub% = UBOUND(backGround%)
        IF lo% <> lb% OR uo% <> ub% THEN
            PRINT "Error: Collision - The object and background"
            PRINT "graphics arrays have different dimensions."
            SYSTEM
        END IF
        FOR i% = lo% + 2 TO uo%
            IF object%(i%) THEN
                IF backGround%(i%) THEN
                    Collision% = TRUE
                    EXIT FUNCTION
                END IF
            END IF
        NEXT i%
        Collision% = FALSE
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Date2Day%                  **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the day number given a date in the
  ' QuickBASIC string format "MM-DD-YYYY".
  '
    FUNCTION Date2Day% (dat$) STATIC
        Date2Day% = VAL(MID$(dat$, 4, 2))
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Date2Julian&               **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the astronomical Julian day number given a
  ' date in the QuickBASIC string format "MM-DD-YYYY".
  '
    FUNCTION Date2Julian& (dat$) STATIC
        month% = Date2Month%(dat$)
        day% = Date2Day%(dat$)
        year% = Date2Year%(dat$)
        IF year% < 1583 THEN
            PRINT "Date2Julian: Year is less than 1583"
            SYSTEM
        END IF
        IF month% > 2 THEN
            month% = month% - 3
        ELSE
            month% = month% + 9
            year% = year% - 1
        END IF
        ta& = 146097 * (year% \ 100) \ 4
        tb& = 1461& * (year% MOD 100) \ 4
        tc& = (153 * month% + 2) \ 5 + day% + 1721119
        Date2Julian& = ta& + tb& + tc&
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Date2Month%                **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the month number given a date in the
  ' QuickBASIC string format "MM-DD-YYYY".
  '
    FUNCTION Date2Month% (dat$) STATIC
        Date2Month% = VAL(MID$(dat$, 1, 2))
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Date2Year%                 **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the year number given a date in the
  ' QuickBASIC string format "MM-DD-YYYY".
  '
    FUNCTION Date2Year% (dat$) STATIC
        Date2Year% = VAL(MID$(dat$, 7))
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          DayOfTheWeek$              **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a string stating the day of the week.
  ' Input is a date expressed in the QuickBASIC string
  ' format "MM-DD-YYYY".
  '
    FUNCTION DayOfTheWeek$ (dat$) STATIC
        SELECT CASE Date2Julian&(dat$) MOD 7
        CASE 0
            DayOfTheWeek$ = "Monday"
        CASE 1
            DayOfTheWeek$ = "Tuesday"
        CASE 2
            DayOfTheWeek$ = "Wednesday"
        CASE 3
            DayOfTheWeek$ = "Thursday"
        CASE 4
            DayOfTheWeek$ = "Friday"
        CASE 5
            DayOfTheWeek$ = "Saturday"
        CASE 6
            DayOfTheWeek$ = "Sunday"
        END SELECT
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          DOSVersion!                **
  ' **  Type:          Function                   **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the version number of DOS
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
  ' **  Name:          DrawBox                    **
  ' **  Type:          Subprogram                 **
  ' **  Module:        EDIT.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Draw a double line box
  '
    SUB DrawBox (row1%, col1%, row2%, col2%) STATIC
      
      ' Determine inside width of box
        wide% = col2% - col1% - 1
      
      ' Across the top
        LOCATE row1%, col1%, 0
        PRINT CHR$(201);
        PRINT STRING$(wide%, 205);
        PRINT CHR$(187);
      
      ' down the sides
        FOR row3% = row1% + 1 TO row2% - 1
            LOCATE row3%, col1%, 0
            PRINT CHR$(186);
            PRINT SPACE$(wide%);
            PRINT CHR$(186);
        NEXT row3%
      
      ' Across the bottom
        LOCATE row2%, col1%, 0
        PRINT CHR$(200);
        PRINT STRING$(wide%, 205);
        PRINT CHR$(188);
      
    END SUB
  
  ' ************************************************
  ' **  Name:          EditLine                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        EDIT.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Allows editing of a string at the current cursor position
  ' on the screen.  Keys acted upon include Right-arrow,
  ' Left-arrow, Ctrl-left-arrow, Ctrl-right-arrow, Home, End,
  ' Insert, Escape, Enter, Backspace, and Delete.
  ' Pressing the Enter, Up-arrow, or Down-arrow terminates
  ' the subprogram and returns exitCode% of 0, +1, or -1.
  '
    SUB EditLine (a$, exitCode%) STATIC
      
      ' Set up some variables
        row% = CSRLIN
        col% = POS(0)
        length% = LEN(a$)
        ptr% = 0
        insert% = TRUE
        quit% = FALSE
        original$ = a$
      
      ' Main processing loop
        DO
          
          ' Display the line
            LOCATE row%, col%, 0
            PRINT a$;
          
          ' Show appropriate cursor type
            IF insert% THEN
                LOCATE row%, col% + ptr%, 1, 6, 7
            ELSE
                LOCATE row%, col% + ptr%, 1, 1, 7
            END IF
          
          ' Get next key stroke
            keyNumber% = KeyCode%
          
          ' Process the key
            SELECT CASE keyNumber%
              
            CASE INSERTKEY
                IF insert% THEN
                    insert% = FALSE
                ELSE
                    insert% = TRUE
                END IF
              
            CASE BACKSPACE
                IF ptr% THEN
                    a$ = a$ + " "
                    a$ = LEFT$(a$, ptr% - 1) + MID$(a$, ptr% + 1)
                    ptr% = ptr% - 1
                END IF
              
            CASE DELETE
                a$ = a$ + " "
                a$ = LEFT$(a$, ptr%) + MID$(a$, ptr% + 2)
              
            CASE UPARROW
                exitCode% = 1
                quit% = TRUE
              
            CASE DOWNARROW
                exitCode% = -1
                quit% = TRUE
              
            CASE LEFTARROW
                IF ptr% THEN
                    ptr% = ptr% - 1
                END IF
              
            CASE RIGHTARROW
                IF ptr% < length% - 1 THEN
                    ptr% = ptr% + 1
                END IF
              
            CASE ENTER
                exitCode% = 0
                quit% = TRUE
              
            CASE HOME
                ptr% = 0
              
            CASE ENDKEY
                ptr% = length% - 1
              
            CASE CTRLRIGHTARROW
                DO UNTIL MID$(a$, ptr% + 1, 1) = " " OR ptr% = length% - 1
                    ptr% = ptr% + 1
                LOOP
                DO UNTIL MID$(a$, ptr% + 1, 1) <> " " OR ptr% = length% - 1
                    ptr% = ptr% + 1
                LOOP
              
            CASE CTRLLEFTARROW
                DO UNTIL MID$(a$, ptr% + 1, 1) = " " OR ptr% = 0
                    ptr% = ptr% - 1
                LOOP
                DO UNTIL MID$(a$, ptr% + 1, 1) <> " " OR ptr% = 0
                    ptr% = ptr% - 1
                LOOP
                DO UNTIL MID$(a$, ptr% + 1, 1) = " " OR ptr% = 0
                    ptr% = ptr% - 1
                LOOP
                IF ptr% THEN
                    ptr% = ptr% + 1
                END IF
              
            CASE CTRLY
                a$ = SPACE$(length%)
                ptr% = 0
              
            CASE CTRLQ
                ctrlQflag% = TRUE
              
            CASE ESCAPE
                a$ = original$
                ptr% = 0
                insert% = TRUE
              
            CASE IS > 255
                SOUND 999, 1
              
            CASE IS < 32
                SOUND 999, 1
              
            CASE ELSE
              
              ' Convert key code to character string
                kee$ = CHR$(keyNumber%)
              
              ' Insert or overstrike
                IF insert% THEN
                    a$ = LEFT$(a$, ptr%) + kee$ + MID$(a$, ptr% + 1)
                    a$ = LEFT$(a$, length%)
                ELSE
                    IF ptr% < length% THEN
                        MID$(a$, ptr% + 1, 1) = kee$
                    END IF
                END IF
              
              ' Are we up against the wall?
                IF ptr% < length% THEN
                    ptr% = ptr% + 1
                ELSE
                    SOUND 999, 1
                END IF
              
              ' Special check for Ctrl-q-y (del to end of line)
                IF kee$ = "y" AND ctrlQflag% THEN
                    IF ptr% <= length% THEN
                        sp% = length% - ptr% + 1
                        MID$(a$, ptr%, sp%) = SPACE$(sp%)
                        ptr% = ptr% - 1
                    END IF
                END IF
              
              ' Clear out the Ctrl-q signal
                ctrlQflag% = FALSE
              
            END SELECT
          
        LOOP UNTIL quit%
      
    END SUB
  
  ' ************************************************
  ' **  Name:          Equipment                  **
  ' **  Type:          Subprogram                 **
  ' **  Module:        BIOSCALL.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns equipment configuration information from BIOS
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
  ' **  Name:          FindFirstFile              **
  ' **  Type:          Subprogram                 **
  ' **  Module:        FILEINFO.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Finds first file that matches the path$
  '
    SUB FindFirstFile (path$, dta$, result%) STATIC
      
      ' Initialization
        DIM reg AS RegTypeX
      
      ' The path must be a null terminated string
        thePath$ = path$ + CHR$(0)
      
      ' Get current DTA address
        reg.ax = &H2F00
        InterruptX &H21, reg, reg
        sgmt% = reg.es
        ofst% = reg.bx
      
      ' Set dta address
        dta$ = SPACE$(43)
        reg.ax = &H1A00
        reg.ds = VARSEG(dta$)
        reg.dx = SADD(dta$)
        InterruptX &H21, reg, reg
      
      ' Find first file match
        reg.ax = &H4E00
        reg.cx = FILEATTRIBUTE
        reg.ds = VARSEG(thePath$)
        reg.dx = SADD(thePath$)
        InterruptX &H21, reg, reg
      
      ' The carry flag tells if a file was found or not
        result% = reg.flags AND 1
      
      ' Reset the original DTA
        reg.ax = &H1A00
        reg.ds = sgmt%
        reg.dx = ofst%
        InterruptX &H21, reg, reg
      
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
    SUB FindNextFile (dta$, result%) STATIC
      
      ' Initialization
        DIM reg AS RegTypeX
      
      ' Make sure dta$ was built (FindFirstFile should have been called)
        IF LEN(dta$) <> 43 THEN
            result% = 2
            EXIT SUB
        END IF
      
      ' Get current DTA address
        reg.ax = &H2F00
        InterruptX &H21, reg, reg
        sgmt% = reg.es
        ofst% = reg.bx
      
      ' Set dta address
        reg.ax = &H1A00
        reg.ds = VARSEG(dta$)
        reg.dx = SADD(dta$)
        InterruptX &H21, reg, reg
      
      ' Find next file match
        reg.ax = &H4F00
        reg.cx = FILEATTRIBUTE
        reg.ds = VARSEG(thePath$)
        reg.dx = SADD(thePath$)
        InterruptX &H21, reg, reg
      
      ' The carry flag tells if a file was found or not
        result% = reg.flags AND 1
      
      ' Reset the original DTA
        reg.ax = &H1A00
        reg.ds = sgmt%
        reg.dx = ofst%
        InterruptX &H21, reg, reg
      
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
  ' Returns the current disk drive name, such as "A:"
  '
    FUNCTION GetDrive$ STATIC
        DIM reg AS RegType
        reg.ax = &H1900
        Interrupt &H21, reg, reg
        GetDrive$ = CHR$((reg.ax AND &HFF) + 65) + ":"
    END FUNCTION
  
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
  
  ' ************************************************
  ' **  Name:          GetShiftStates             **
  ' **  Type:          Subprogram                 **
  ' **  Module:        BIOSCALL.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns state of the various shift keys and states.
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
  ' **  Name:          GetVerifyState%            **
  ' **  Type:          Function                   **
  ' **  Module:        DOSCALLS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
    ' Returns the current state of the DOS "Verify After
    ' Write" flag.
  '
    FUNCTION GetVerifyState% STATIC
        DIM reg AS RegType
        reg.ax = &H5400
        Interrupt &H21, reg, reg
        GetVerifyState% = reg.ax AND &HFF
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          InKeyCode%                 **
  ' **  Type:          Function                   **
  ' **  Module:        EDIT.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a unique integer for any key pressed, or
  ' a zero if no key has been pressed.
  '
    FUNCTION InKeyCode% STATIC
        InKeyCode% = CVI(INKEY$ + STRING$(2, 0))
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          Julian2Date$               **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a date in the QuickBASIC string format
  ' "MM-DD-YYYY" as calculated from a Julian day number.
  '
    FUNCTION Julian2Date$ (julian&) STATIC
      
        x& = 4 * julian& - 6884477
        y& = (x& \ 146097) * 100
        d& = (x& MOD 146097) \ 4
      
        x& = 4 * d& + 3
        y& = (x& \ 1461) + y&
        d& = (x& MOD 1461) \ 4 + 1
      
        x& = 5 * d& - 3
        m& = x& \ 153 + 1
        d& = (x& MOD 153) \ 5 + 1
      
        IF m& < 11 THEN
            month% = m& + 2
        ELSE
            month% = m& - 10
        END IF
        day% = d&
        year% = y& + m& \ 11
      
        dat$ = MDY2Date$(month%, day%, year%)
        Julian2Date$ = dat$
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          KeyCode%                   **
  ' **  Type:          Function                   **
  ' **  Module:        EDIT.BAS                   **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a unique integer for any key pressed.
  '
    FUNCTION KeyCode% STATIC
        DO
            k$ = INKEY$
        LOOP UNTIL k$ <> ""
        KeyCode% = CVI(k$ + CHR$(0))
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          MDY2Date$                  **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Converts month%, day%, and year% to a date string
  ' in the QuickBASIC string format "MM-DD-YYYY".
  '
    FUNCTION MDY2Date$ (month%, day%, year%) STATIC
        y$ = RIGHT$("000" + MID$(STR$(year%), 2), 4)
        m$ = RIGHT$("0" + MID$(STR$(month%), 2), 2)
        d$ = RIGHT$("0" + MID$(STR$(day%), 2), 2)
        MDY2Date$ = m$ + "-" + d$ + "-" + y$
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          MonthName$                 **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a string stating the month as indicated
  ' in dat$ (QuickBASIC string format "MM-DD-YYYY").
  '
    FUNCTION MonthName$ (dat$) STATIC
      
        IF LEN(dat$) <> 10 THEN
            dat$ = "MM-DD-YYYY"
        END IF
      
        SELECT CASE LEFT$(dat$, 2)
        CASE "01"
            MonthName$ = "January"
        CASE "02"
            MonthName$ = "February"
        CASE "03"
            MonthName$ = "March"
        CASE "04"
            MonthName$ = "April"
        CASE "05"
            MonthName$ = "May"
        CASE "06"
            MonthName$ = "June"
        CASE "07"
            MonthName$ = "July"
        CASE "08"
            MonthName$ = "August"
        CASE "09"
            MonthName$ = "September"
        CASE "10"
            MonthName$ = "October"
        CASE "11"
            MonthName$ = "November"
        CASE "12"
            MonthName$ = "December"
        CASE ELSE
            MonthName$ = "?MonthName?"
        END SELECT
      
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          MouseMickey                **
  ' **  Type:          Subprogram                 **
  ' **  Module:        MOUSSUBS.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Read mouse mickey counts
  '
    SUB MouseMickey (horizontal%, vertical%) STATIC
        Mouse 11, 0, horizontal%, vertical%
    END SUB
  
  ' ************************************************
  ' **  Name:          OneMonthCalendar           **
  ' **  Type:          Subprogram                 **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Prints a small, one month calendar at the row%
  ' and col% indicated.
  '
    SUB OneMonthCalendar (dat$, row%, col%) STATIC
        mname$ = MonthName$(dat$)
        LOCATE row%, col% + 12 - LEN(mname$) \ 2
        PRINT mname$; ","; Date2Year%(dat$)
        month% = Date2Month%(dat$)
        day% = 1
        year% = Date2Year%(dat$)
        dat1$ = MDY2Date$(month%, day%, year%)
        j& = Date2Julian&(dat1$)
        heading$ = " Sun Mon Tue Wed Thu Fri Sat"
        wa% = INSTR(heading$, LEFT$(DayOfTheWeek$(dat1$), 3)) \ 4
        LOCATE row% + 1, col%
        PRINT heading$
        rowloc% = row% + 2
        LOCATE rowloc%, col% + 4 * wa%
        DO
            PRINT USING "####"; day%;
            IF wa% = 6 THEN
                rowloc% = rowloc% + 1
                LOCATE rowloc%, col%
            END IF
            wa% = (wa% + 1) MOD 7
            j& = j& + 1
            day% = Date2Day%(Julian2Date$(j&))
        LOOP UNTIL day% = 1
        PRINT
    END SUB
  
  ' ************************************************
  ' **  Name:          SpaceWorms                 **
  ' **  Type:          Subprogram                 **
  ' **  Module:        INTRODEM.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' A simple game that demonstrates object creation and
  ' collision detection routines.
  '
    SUB SpaceWorms STATIC
      
      ' Constants
        CONST WORMS = 2
        CONST WSEGS = 999
        CONST PI = 3.141593
        CONST RADIUS = 2
        CONST MICKFACTOR = 5
        CONST DOWNARROW = 20480
        CONST LEFTARROW = 19200
        CONST RIGHTARROW = 19712
        CONST UPARROW = 18432
        CONST HOMEKEY = 18176
        CONST ENDKEY = 20224
        CONST PGUP = 18688
        CONST PGDN = 20736
      
      ' The worm ship arrays were built in module level code
        SHARED wormship1%(), wormship2%()
      
      ' Initialization
        SCREEN 1
        COLOR BLUE, 1
        CLS
        RANDOMIZE TIMER
      
      ' Dimension arrays for worm data
        REDIM row%(WORMS, WSEGS - 1)
        REDIM col%(WORMS, WSEGS - 1)
        REDIM h(WORMS)
        REDIM p%(WORMS)
        REDIM bgd%(0 TO 54)
      
      ' Starting conditions
        t% = 0
        timeStart = TIMER
        t0 = timeStart
        n% = 0
        nseg% = 20
        zapped% = 0
        wx% = 160
        wy% = 100
        newx% = wx%
        newy% = wy%
      
      ' Put starting ship image on the screen
        PUT (wx%, wy%), wormship1%
      
      ' Set starting random worm headings
        FOR i% = 0 TO WORMS
            h(i%) = RND * PI
        NEXT i%
      
      ' Main loop
        DO
          
          ' Check for mouse movement
            MouseMickey horizontal%, vertical%
            newx% = (newx% + 293 + horizontal% / MICKFACTOR) MOD 293
            newy% = (newy% + 185 + vertical% / MICKFACTOR) MOD 185
          
          ' Check for cursor keys
            SELECT CASE InKeyCode%
            CASE UPARROW
                newy% = (newy% + 175) MOD 185
            CASE DOWNARROW
                newy% = (newy% + 10) MOD 185
            CASE LEFTARROW
                newx% = (newx% + 283) MOD 293
            CASE RIGHTARROW
                newx% = (newx% + 10) MOD 293
            CASE HOMEKEY
                newy% = (newy% + 175) MOD 185
                newx% = (newx% + 283) MOD 293
            CASE ENDKEY
                newy% = (newy% + 10) MOD 185
                newx% = (newx% + 283) MOD 293
            CASE PGUP
                newx% = (newx% + 10) MOD 293
                newy% = (newy% + 175) MOD 185
            CASE PGDN
                newx% = (newx% + 10) MOD 293
                newy% = (newy% + 10) MOD 185
            CASE ELSE
            END SELECT
          
          ' Draw the worm ship
            t% = NOT t%
            IF t% THEN
                PUT (wx%, wy%), wormship1%
                wx% = newx%
                wy% = newy%
                GET (wx%, wy%)-(wx% + 23, wy% + 14), bgd%
                PUT (wx%, wy%), wormship2%
                zapped% = Collision%(wormship2%(), bgd%())
            ELSE
                PUT (wx%, wy%), wormship2%
                wx% = newx%
                wy% = newy%
                GET (wx%, wy%)-(wx% + 23, wy% + 14), bgd%
                PUT (wx%, wy%), wormship1%
                zapped% = Collision%(wormship1%(), bgd%())
            END IF
          
          
          ' Sprout new worm or add segments every five seconds
            IF TIMER - t0 > 5 THEN
                IF n% < WORMS THEN
                    n% = n% + 1
                ELSE
                    IF nseg% < WSEGS THEN
                        nseg% = nseg% + 5
                    END IF
                END IF
                t0 = TIMER
            END IF
          
          ' Randomly adjust the worm headings
            FOR i% = 0 TO n%
                h(i%) = h(i%) + RND - .5
            NEXT i%
          
          ' Move each worm
            FOR i% = 0 TO n%
                p2% = p%(i%)
                p1% = (p2% + nseg% - 1) MOD nseg%
                p3% = (p2% + nseg% + 1) MOD nseg%
                CIRCLE (row%(i%, p2%), col%(i%, p2%)), RADIUS, 0
                row%(i%, p2%) = (row%(i%, p1%) + COS(h(i%)) * 5 + 320) MOD 320
                col%(i%, p2%) = (col%(i%, p1%) + SIN(h(i%)) * 5 + 200) MOD 200
                CIRCLE (row%(i%, p2%), col%(i%, p2%)), RADIUS, 3
                p%(i%) = p3%
            NEXT i%
          
        LOOP UNTIL zapped%
      
      ' Display results
        LOCATE 1, 1
        elapsed = TIMER - timeStart
        PRINT USING "You stayed alive for####.# seconds!"; elapsed
        PRINT "Press <Enter> to continue..."
        DO
            backGround% = (backGround% + 1) MOD 8
            COLOR backGround%
            t0 = TIMER
            DO
            LOOP WHILE TIMER - t0 < .3
        LOOP UNTIL INKEY$ = CHR$(13)
      
      ' All done
        SCREEN 0
        WIDTH 80
      
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
    SUB VideoState (mode%, columns%, page%) STATIC
        DIM reg AS RegType
        reg.ax = &HF00
        Interrupt &H10, reg, reg
        mode% = reg.ax AND &HFF
        columns% = (CLNG(reg.ax) AND &HFF00) \ 256
        page% = (CLNG(reg.bx) AND &HFF00) \ 256
    END SUB
  

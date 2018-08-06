  ' ************************************************
  ' **  Name:          CDEMO2                     **
  ' **  Type:          Program                    **
  ' **  Module:        CDEMO2.BAS                 **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' USAGE:           No command line parameters
  ' REQUIREMENTS:    CGA
  '                  MIXED.QLB/.LIB
  ' .MAK FILE:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       m$            Menu string
  '                  word%         Integer to be packed with two bytes
  '                  hi%           Most significant byte unpacked from an
  '                                integer
  '                  lo%           Least significant byte unpacked from an
  '                                integer
  '                  a$            Workspace for TextGet and TextPut
  '                  b$            Workspace for TextGet and TextPut
  '                  n%            Timing constant for TextPut demonstration
  '                  row%          Row location to put small "window" using
  '                                TextPut
  '                  col%          Column location to put small "window" using
  '                                TextPut
  '                  t0            Timer variable
  '                  x$            String variable for bit shifting
  '                  i%            Looping index
  
  
  ' Functions
    DECLARE FUNCTION MenuString% CDECL (row%, col%, a$)
    DECLARE FUNCTION BitShiftleft% CDECL (a$)
    DECLARE FUNCTION BitShiftRight% CDECL (a$)
    DECLARE FUNCTION NumberOfBits& CDECL (a$)
  
  ' Subprograms
    DECLARE SUB PackWord CDECL (word%, hi%, lo%)
    DECLARE SUB UnPackWord CDECL (word%, hi%, lo%)
    DECLARE SUB TextGet CDECL (r1%, c1%, r2%, c2%, a$)
    DECLARE SUB TextPut CDECL (r1%, c1%, r2%, c2%, a$)
  
  ' Build menu string
    m$ = "Packword Unpackword Textget Textput "
    m$ = m$ + "Bitshiftleft Bitshiftright Numberofbits Quit"
  
  ' Let user repeatedly select the demonstrations
    DO
        COLOR 15, 1
        CLS
        PRINT
        PRINT
        PRINT "MenuString function..."
        PRINT
        PRINT "Select one of the CTOOLS2 demonstrations by ";
        PRINT "pressing the Left arrow,"
        PRINT "Right arrow, first letter of the choice, or Enter keys."
      
      ' Use MenuString to choose demonstrations
        SELECT CASE MenuString%(1, 1, m$)
          
      ' PackWord demonstration
        CASE 1
          
            CLS
            PRINT "PackWord word%, 255, 255  ...  word% = ";
            PackWord word%, 255, 255
            PRINT word%
            PRINT "PackWord word%,   0,   1  ...  word% = ";
            PackWord word%, 0, 1
            PRINT word%
            PRINT "PackWord word%,   1,   0  ...  word% = ";
            PackWord word%, 1, 0
            PRINT word%
          
            PRINT
            PRINT "Press any key to continue..."
          
            DO
            LOOP UNTIL INKEY$ <> ""
          
      ' UnPackWord demonstration
        CASE 2
          
            CLS
            PRINT "UnPackWord  -1, hi%, lo%  ...  hi%, lo% =";
            UnPackWord -1, hi%, lo%
            PRINT hi%; lo%
            PRINT "UnPackWord   1, hi%, lo%  ...  hi%, lo% =";
            UnPackWord 1, hi%, lo%
            PRINT hi%; lo%
            PRINT "UnPackWord 256, hi%, lo%  ...  hi%, lo% =";
            UnPackWord 256, hi%, lo%
            PRINT hi%; lo%
          
            PRINT
            PRINT "Press any key to continue..."
          
            DO
            LOOP UNTIL INKEY$ <> ""
          
      ' TextGet and TextPut demonstration
        CASE 3, 4
          
          ' TextGet a line of text
            CLS
            PRINT "A Vertical Message"
            a$ = SPACE$(36)
            TextGet 1, 1, 1, 18, a$
          
          ' TextPut it back, but stretch it vertically
            TextPut 6, 1, 23, 1, a$
          
          ' Now just a normal line of text at top
            LOCATE 1, 1
            PRINT "TextGet and TextPut - Press any key to stop"
          
          ' Create first of two colorful text patterns
            COLOR 14, 4
            LOCATE 13, 13, 0
            PRINT CHR$(201); CHR$(205); CHR$(209); CHR$(205); CHR$(187)
            LOCATE 14, 13, 0
            PRINT CHR$(199); CHR$(196); CHR$(197); CHR$(196); CHR$(182)
            LOCATE 15, 13, 0
            PRINT CHR$(200); CHR$(205); CHR$(207); CHR$(205); CHR$(188)
            a$ = SPACE$(30)
            TextGet 13, 13, 15, 17, a$
          
          ' Create second of two colorful text patterns
            COLOR 10, 1
            LOCATE 13, 13, 0
            PRINT CHR$(218); CHR$(196); CHR$(210); CHR$(196); CHR$(191)
            LOCATE 14, 13, 0
            PRINT CHR$(198); CHR$(205); CHR$(206); CHR$(205); CHR$(181)
            LOCATE 15, 13, 0
            PRINT CHR$(192); CHR$(196); CHR$(208); CHR$(196); CHR$(217)
            b$ = SPACE$(30)
            TextGet 13, 13, 15, 17, b$
          
          ' Randomly pop up little "windows"
            n% = 0
            DO
                row% = INT(RND * 21 + 3)
                col% = INT(RND * 73 + 4)
                TextPut row%, col%, row% + 2, col% + 4, a$
                row% = INT(RND * 21 + 3)
                col% = INT(RND * 73 + 4)
                TextPut row%, col%, row% + 2, col% + 4, b$
                IF n% < 10 THEN
                    n% = n% + 1
                    t0 = TIMER
                    DO
                    LOOP UNTIL TIMER > t0 + (10 - n%) / 10
                END IF
            LOOP UNTIL INKEY$ <> ""
          
      ' BitShiftLeft demonstration
        CASE 5
          
            CLS
            x$ = "This string will be shifted left 8 bits"
            PRINT x$
            FOR i% = 1 TO 8
                PRINT "bit ="; BitShiftleft%(x$)
            NEXT i%
            PRINT x$
          
            PRINT
            PRINT "Press any key to continue..."
          
            DO
            LOOP UNTIL INKEY$ <> ""
          
      ' BitShiftRight demonstration
        CASE 6
          
            CLS
            x$ = "This string will be shifted right 8 bits"
            PRINT x$
            FOR i% = 1 TO 8
                PRINT "bit ="; BitShiftRight%(x$)
            NEXT i%
            PRINT x$
          
            PRINT
            PRINT "Press any key to continue..."
          
            DO
            LOOP UNTIL INKEY$ <> ""
          
      ' BitShiftRight demonstration
        CASE 7
          
            CLS
            x$ = "The number of bits in this string is ..."
            PRINT x$
            PRINT NumberOfBits&(x$)
          
            PRINT
            PRINT "Press any key to continue..."
          
            DO
            LOOP UNTIL INKEY$ <> ""
          
      ' Must be time to quit
        CASE ELSE
            COLOR 7, 0
            CLS
            END
        END SELECT
    LOOP
  


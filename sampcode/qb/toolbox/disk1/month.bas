  ' ************************************************
  ' **  Name:          MONTH                      **
  ' **  Type:          Program                    **
  ' **  Module:        MONTH.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Creates and displays a three-month calendar.
  ' USAGE:           No command line parameters
  ' .MAK FILE:       MONTH.BAS
  '                  CALENDAR.BAS
  ' PARAMETERS:      (none)
  ' VARIABLES:       year%      Year of concern
  '                  month%     Month of concern
  '                  quitFlag%  Indicates that program is to terminate
  '                  day%       Day near middle of the month
  '                  d2$        Date for second calendar month
  '                  j2&        Julian day number for second calendar month
  '                  d1$        Date for first calendar month
  '                  j1&        Julian day number for first calendar month
  '                  d3$        Date for third calendar month
  '                  j3&        Julian day number for third calendar month
  '                  k$         Key press character
  
  ' Constants
    CONST FALSE = 0
    CONST TRUE = NOT FALSE
  
  ' Functions
    DECLARE FUNCTION Date2Julian& (dat$)
    DECLARE FUNCTION MDY2Date$ (month%, day%, year%)
    DECLARE FUNCTION Date2Year% (dat$)
    DECLARE FUNCTION Date2Month% (dat$)
    DECLARE FUNCTION Julian2Date$ (julian&)
  
  ' Subprograms
    DECLARE SUB OneMonthCalendar (dat$, row%, col%)
  
  ' Get today's month and year
    year% = Date2Year%(DATE$)
    month% = Date2Month%(DATE$)
  
  ' Make calendars until the Esc key is pressed
    DO UNTIL quitFlag%
      
      ' Get Julian day number for about the middle of the month
        day% = 15
        d2$ = MDY2Date$(month%, day%, year%)
        j2& = Date2Julian&(d2$)
      
      ' Get last month's date
        j1& = j2& - 30
        d1$ = Julian2Date$(j1&)
      
      ' Get next month's date
        j3& = j2& + 30
        d3$ = Julian2Date$(j3&)
      
      ' Display the heading
        CLS
        LOCATE 1, 57
        PRINT "THREE-MONTH CALENDAR"
        LOCATE 2, 57
        PRINT "QuickBASIC 4.0"
      
      ' Create the three calendar sheets
        OneMonthCalendar d1$, 1, 1
        OneMonthCalendar d2$, 8, 25
        OneMonthCalendar d3$, 15, 49
      
      ' Display the instructions
        LOCATE 17, 1
        PRINT "Press <Y> to increment the year"
        LOCATE 18, 1
        PRINT "Press <y> to decrement the year"
        LOCATE 19, 1
        PRINT "Press <M> to increment the months"
        LOCATE 20, 1
        PRINT "Press <m> to decrement the months"
        LOCATE 22, 1
        PRINT "Press the Esc key to quit"
      
      ' Wait for a keystroke
        DO
            k$ = INKEY$
        LOOP UNTIL k$ <> ""
      
      ' Check for appropriate keystroke
        SELECT CASE k$
        CASE "y"
            year% = year% - 1
        CASE "Y"
            year% = year% + 1
        CASE "m"
            month% = month% - 3
        CASE "M"
            month% = month% + 3
        CASE CHR$(27)
            quitFlag% = TRUE
        CASE ELSE
        END SELECT
      
      ' Adjust month for proper range
        IF month% < 1 THEN
            month% = month% + 12
            year% = year% - 1
        ELSEIF month% > 12 THEN
            month% = month% - 12
            year% = year% + 1
        END IF
      
    LOOP
  
  ' All done
    END
  

  ' ************************************************
  ' **  Name:          CALENDAR                   **
  ' **  Type:          Toolbox                    **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' USAGE: No command line parameters
  ' .MAK FILE:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       month%     Month for demonstration
  '                  day%       Day for demonstration
  '                  year%      Year for demonstration
  '                  dat$       Date for demonstration
  '                  j&         Julian day number
  '                  tim$       System time right now
  '                  hour%      Hour right now
  '                  minute%    Minute right now
  '                  second%    Second right now
  '                  sec&       Seconds since last second of 1979
  
  
    CONST FALSE = 0
    CONST TRUE = NOT FALSE
  
  ' Functions
    DECLARE FUNCTION CheckDate% (dat$)
    DECLARE FUNCTION Date2Day% (dat$)
    DECLARE FUNCTION Date2Julian& (dat$)
    DECLARE FUNCTION Date2Month% (dat$)
    DECLARE FUNCTION Date2Year% (dat$)
    DECLARE FUNCTION DayOfTheCentury& (dat$)
    DECLARE FUNCTION DayOfTheWeek$ (dat$)
    DECLARE FUNCTION DayOfTheYear% (dat$)
    DECLARE FUNCTION DaysBetweenDates& (dat1$, dat2$)
    DECLARE FUNCTION HMS2Time$ (hour%, minute%, second%)
    DECLARE FUNCTION Julian2Date$ (julian&)
    DECLARE FUNCTION MDY2Date$ (month%, day%, year%)
    DECLARE FUNCTION MonthName$ (dat$)
    DECLARE FUNCTION Second2Date$ (second&)
    DECLARE FUNCTION Second2Time$ (second&)
    DECLARE FUNCTION Time2Hour% (tim$)
    DECLARE FUNCTION Time2Minute% (tim$)
    DECLARE FUNCTION Time2Second% (tim$)
    DECLARE FUNCTION TimeDate2Second& (tim$, dat$)
  
  ' Subprograms
    DECLARE SUB OneMonthCalendar (dat$, row%, col%)
  
  ' Let's choose the fourth of July for the demonstration
    CLS
    PRINT "All about the fourth of July for this year..."
    month% = 7
    day% = 4
    year% = Date2Year%(DATE$)
  
  ' Demonstrate the conversion to dat$
    PRINT
    dat$ = MDY2Date$(month%, day%, year%)
    PRINT "QuickBASIC string format for this date is "; dat$
  
  ' Check the validity of this date
    IF CheckDate%(dat$) = FALSE THEN
        PRINT "The date you entered is faulty... " + dat$
        SYSTEM
    END IF
  
  ' Day of the week and name of the month
    PRINT "The day of the week is "; DayOfTheWeek$(dat$); "."
  
  ' Astronomical Julian day number
    j& = Date2Julian&(dat$)
    PRINT "The Julian day number is"; j&
  
  ' Conversion of Julian number to date
    PRINT "Date for the given Julian number is "; Julian2Date$(j&); "."
  
  ' Convert the date string to numbers
    PRINT "The month, day, and year numbers are ";
    PRINT Date2Month%(dat$); ","; Date2Day%(dat$); ","; Date2Year%(dat$)
  
  ' The month name
    PRINT "The month name is "; MonthName$(dat$)
  
  ' Day of the year
    PRINT "The day of the year is"; DayOfTheYear%(dat$)
  
  ' Day of the century
    PRINT "The day of the century is"; DayOfTheCentury&(dat$)
  
  ' Days from right now
    IF Date2Julian&(dat$) < Date2Julian&(DATE$) THEN
        PRINT "That was"; DaysBetweenDates&(dat$, DATE$); "days ago."
    ELSEIF Date2Julian&(dat$) > Date2Julian&(DATE$) THEN
        PRINT "That is"; DaysBetweenDates&(dat$, DATE$); "days from now."
    ELSE
        PRINT "The date you entered is today's date."
    END IF
  
  ' Print a one-month calendar
    OneMonthCalendar dat$, 14, 25
  
  ' Wait for user
    LOCATE 23, 1
    PRINT "Press any key to continue"
    DO
    LOOP UNTIL INKEY$ <> ""
    CLS
  
  ' Demonstrate extracting hour, minute, and second from tim$
    dat$ = DATE$
    tim$ = TIME$
    hour% = Time2Hour%(tim$)
    minute% = Time2Minute%(tim$)
    second% = Time2Second%(tim$)
    PRINT "The date today... "; dat$
    PRINT "The time now  ... "; tim$
    PRINT "The hour, minute, and second numbers are ";
    PRINT hour%; ","; minute%; ","; second%
  
  ' Now put it all back together again
    PRINT "Time string created from hour, minute, and second is ";
    PRINT HMS2Time$(hour%, minute%, second%)
  
  ' Seconds since end of 1979
    dat$ = DATE$
    PRINT "The number of seconds since the last second of 1979 is";
    sec& = TimeDate2Second&(tim$, dat$)
    PRINT sec&
    PRINT "From this number we can extract the date and time..."
    PRINT Second2Date$(sec&); " and "; Second2Time$(sec&); "."
  

  ' ************************************************
  ' **  Name:          CheckDate%                 **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns TRUE if the given date represents a real
  ' date or FALSE if the date is in error.
  '
  ' EXAMPLE OF USE:  test% = CheckDate%(dat$)
  ' PARAMETERS:      dat$       Date to be checked
  ' VARIABLES:       julian&    Julian day number for the date
  '                  test$      Date string for given Julian day number
  ' MODULE LEVEL
  '   DECLARATIONS:  CONST FALSE = 0
  '                  CONST TRUE = NOT FALSE
  '
  '                  DECLARE FUNCTION CheckDate% (dat$)
  '                  DECLARE FUNCTION Date2Julian& (dat$)
  '                  DECLARE FUNCTION Julian2Date$ (julian&)
  '
    FUNCTION CheckDate% (dat$) STATIC
        julian& = Date2Julian&(dat$)
        test$ = Julian2Date$(julian&)
        IF dat$ = test$ THEN
            CheckDate% = TRUE
        ELSE
            CheckDate% = FALSE
        END IF
    END FUNCTION

  ' ************************************************
  ' **  Name:          Date2Day%                  **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the day number given a date in the
  ' QuickBASIC string format MM-DD-YYYY.
  '
  ' EXAMPLE OF USE:  day% = Date2Day%(dat$)
  ' PARAMETERS:      dat$       Date of concern
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Date2Day% (dat$)
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
  ' date in the QuickBASIC string format MM-DD-YYYY.
  '
  ' EXAMPLE OF USE:  j& = Date2Julian&(dat$)
  ' PARAMETERS:      dat$       Date of concern
  ' VARIABLES:       month%     Month number for given date
  '                  day%       Day number for given date
  '                  year%      Year number for given date
  '                  ta&        First term of the Julian day number calculation
  '                  tb&        Second term of the Julian day number calculation
  '                  tc&        Third term of the Julian day number calculation
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Date2Day% (dat$)
  '                  DECLARE FUNCTION Date2Julian& (dat$)
  '                  DECLARE FUNCTION Date2Month% (dat$)
  '                  DECLARE FUNCTION Date2Year% (dat$)
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
  ' QuickBASIC string format MM-DD-YYYY.
  '
  ' EXAMPLE OF USE:  month% = Date2Month%(dat$)
  ' PARAMETERS:      dat$       Date of concern
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Date2Month% (dat$)
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
  ' QuickBASIC string format MM-DD-YYYY.
  '
  ' EXAMPLE OF USE:  year% = Date2Year%(dat$)
  ' PARAMETERS:      dat$       Date of concern
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Date2Year% (dat$)
  '
    FUNCTION Date2Year% (dat$) STATIC
        Date2Year% = VAL(MID$(dat$, 7))
    END FUNCTION

  ' ************************************************
  ' **  Name:          DayOfTheCentury%           **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the number of the day of the century.
  '
  ' EXAMPLE OF USE:  cDay& = DayOfTheCentury&(dat$)
  ' PARAMETERS:      dat$       Date of concern
  ' VARIABLES:       year%      Year for given date
  '                  dat1$      Date for last day of previous century
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION DayOfTheCentury& (dat$)
  '
    FUNCTION DayOfTheCentury& (dat$)
        year% = Date2Year%(dat$)
        dat1$ = MDY2Date$(12, 31, year% - (year% MOD 100) - 1)
        DayOfTheCentury& = DaysBetweenDates&(dat1$, dat$)
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
  ' format MM-DD-YYYY.
  '
  ' EXAMPLE OF USE:  PRINT "The day of the week is "; DayOfTheWeek$(dat$)
  ' PARAMETERS:      dat$       Date of concern
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION DayOfTheWeek$ (dat$)
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
  ' **  Name:          DayOfTheYear%              **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the number of the day of the year (1-366).
  '
  ' EXAMPLE OF USE:  PRINT "The day of the year is"; DayOfTheYear%(dat$)
  ' PARAMETERS:      dat$       Date of concern
  ' VARIABLES:       dat1$      Date of last day of previous year
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION DayOfTheYear% (dat$)
  '
    FUNCTION DayOfTheYear% (dat$) STATIC
        dat1$ = MDY2Date$(12, 31, Date2Year%(dat$) - 1)
        DayOfTheYear% = DaysBetweenDates&(dat1$, dat$)
    END FUNCTION

  ' ************************************************
  ' **  Name:          DaysBetweenDates&          **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the number of days between any two dates.
  '
  ' EXAMPLE OF USE:  days& = DaysBetweenDates&(dat1$, dat2$)
  ' PARAMETERS:      dat1$      First date
  '                  dat2$      Second date
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION DaysBetweenDates& (dat1$, dat2$)
  '
    FUNCTION DaysBetweenDates& (dat1$, dat2$) STATIC
        DaysBetweenDates& = ABS(Date2Julian&(dat1$) - Date2Julian&(dat2$))
    END FUNCTION

  ' ************************************************
  ' **  Name:          HMS2Time$                  **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the time in the QuickBASIC string format
  ' HH:MM:SS given hour%, minute%, and second%.
  '
  ' EXAMPLE OF USE:  PRINT HMS2Time$(hour%, minute%, second%)
  ' PARAMETERS:      hour%      Hour number
  '                  minute%    Minutes number
  '                  second%    Seconds number
  ' VARIABLES:       t$         Workspace for building the time string
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION HMS2Time$ (hour%, minute%, second%)
  '
    FUNCTION HMS2Time$ (hour%, minute%, second%) STATIC
        t$ = RIGHT$("0" + MID$(STR$(hour%), 2), 2) + ":"
        t$ = t$ + RIGHT$("0" + MID$(STR$(minute%), 2), 2) + ":"
        HMS2Time$ = t$ + RIGHT$("0" + MID$(STR$(second%), 2), 2)
    END FUNCTION

  ' ************************************************
  ' **  Name:          Julian2Date$               **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a date in the QuickBASIC string format
  ' MM-DD-YYYY as calculated from a Julian day number.
  '
  ' EXAMPLE OF USE:
  '        PRINT "Date for the given Julian number is ";Julian2Date$(j&)
  ' PARAMETERS:      j&         Julian day number
  ' VARIABLES:       x&         Temporary calculation variable
  '                  y&         Temporary calculation variable
  '                  d&         Day number in long integer form
  '                  m&         Month number before adjustment
  '                  month%     Month number
  '                  year%      Year number
  '                  day%       Day number
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Julian2Date$ (julian&)
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
  ' **  Name:          MDY2Date$                  **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Converts month%, day%, and year% to a date string
  ' in the QuickBASIC string format MM-DD-YYYY.
  '
  ' EXAMPLE OF USE:  dat$ = MDY2Date$(month%, day%, year%)
  ' PARAMETERS:      month%     Month for the date
  '                  day%       Day of the month
  '                  year%      Year number
  ' VARIABLES:       y$         Temporary year string
  '                  m$         Temporary month string
  '                  d$         Temporary day string
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION MDY2Date$ (month%, day%, year%)
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
  ' in dat$ (QuickBASIC string format MM-DD-YYYY).
  '
  ' EXAMPLE OF USE:  PRINT MonthName$(dat$)
  ' PARAMETERS:      dat$       Date of concern
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION MonthName$ (dat$)
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
  ' **  Name:          OneMonthCalendar           **
  ' **  Type:          Subprogram                 **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Prints a small, one-month calendar at the row%
  ' and col% indicated.
  '
  ' EXAMPLE OF USE:  OneMonthCalendar dat$, row%, col%
  ' PARAMETERS:      dat$       Date of concern
  '                  row%       Screen row for upper left corner of calendar
  '                  col%       Screen column for upper left corner of calendar
  ' VARIABLES:       mname$     Name of given month
  '                  month%     Month number
  '                  day%       Day number
  '                  year%      Year number
  '                  dat1$      Date for first of the given month
  '                  j&         Julian day number for each day of the month
  '                  heading$   Title line for calendar
  '                  wa%        Day of the week for each day of the month
  '                  rowloc%    Row for printing each day number
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB OneMonthCalendar (dat$, row%, col%)
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
  ' **  Name:          Second2Date$               **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the date in the QuickBASIC string format
  ' MM-DD-YYYY given a number of seconds since the
  ' last second of 1979.  Use Second2Time$ to find
  ' the time of day at the indicated second.
  '
  ' EXAMPLE OF USE:  dat$ = Second2Date$(second&)
  ' PARAMETERS:      second&    Number of seconds since the last second of 1979
  ' VARIABLES:       days&      Julian day number of the date
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Second2Date$ (second&)
  '
    FUNCTION Second2Date$ (second&) STATIC
        days& = second& \ 86400 + 2444240
        Second2Date$ = Julian2Date$(days&)
    END FUNCTION

  ' ************************************************
  ' **  Name:          Second2Time$               **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the time in the QuickBASIC string format
  ' HH:MM:SS given the number of seconds since the
  ' last second of 1979.  Use Second2Date$ to find
  ' the date at the indicated second.
  '
  ' EXAMPLE OF USE:  tim$ = Second2Time$(second&)
  ' PARAMETERS:      second&    Number of seconds since the last second of 1979
  ' VARIABLES:       time&      Number of seconds in current day
  '                  second%    Current second of the minute
  '                  minute%    Current minute of the hour
  '                  hour%      Current hour of the day
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Second2Time$ (second&)
  '
    FUNCTION Second2Time$ (second&) STATIC
        IF second& > 0 THEN
            time& = second& MOD 86400
            second% = time& MOD 60
            time& = time& \ 60
            minute% = time& MOD 60
            hour% = time& \ 60
            Second2Time$ = HMS2Time$(hour%, minute%, second%)
        ELSE
            Second2Time$ = "HH:MM:SS"
        END IF
    END FUNCTION

  ' ************************************************
  ' **  Name:          Time2Hour%                 **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the hour number as indicated in a time
  ' string in the format HH:MM:SS.
  '
  ' EXAMPLE OF USE:  hour% = Time2Hour%(tim$)
  ' PARAMETERS:      tim$       Time of concern
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Time2Hour% (tim$)
  '
    FUNCTION Time2Hour% (tim$) STATIC
        Time2Hour% = VAL(LEFT$(tim$, 2))
    END FUNCTION

  ' ************************************************
  ' **  Name:          Time2Minute%               **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the minute number as indicated in a time
  ' string in the format HH:MM:SS.
  '
  ' EXAMPLE OF USE:  minute% = Time2Minute%(tim$)
  ' PARAMETERS:      tim$       Time of concern
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Time2Minute% (tim$)
  '
    FUNCTION Time2Minute% (tim$) STATIC
        Time2Minute% = VAL(MID$(tim$, 4, 2))
    END FUNCTION

  ' ************************************************
  ' **  Name:          Time2Second%               **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the second number as indicated in a time
  ' string in the format HH:MM:SS.
  '
  ' EXAMPLE OF USE:  second% = Time2Second%(tim$)
  ' PARAMETERS:      tim$       Time of concern
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Time2Second% (tim$)
  '
    FUNCTION Time2Second% (tim$) STATIC
        Time2Second% = VAL(MID$(tim$, 7))
    END FUNCTION

  ' ************************************************
  ' **  Name:          TimeDate2Second&           **
  ' **  Type:          Function                   **
  ' **  Module:        CALENDAR.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the number of seconds since the last
  ' second of 1979.  If the date is not in the years
  ' 1980 to 2047, an error message is output.
  '
  ' EXAMPLE OF USE:  sec& = TimeDate2Second&(tim$, dat$)
  ' PARAMETERS:      tim$       Time of concern
  '                  dat$       Date of concern
  ' VARIABLES:       days&      Days since 12-31-1979
  '                  hour%      Hour of the day
  '                  minute%    Minute of the hour
  '                  second%    Second of the minute
  '                  secs&      Working number of total seconds
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION TimeDate2Second& (tim$, dat$)
  '
    FUNCTION TimeDate2Second& (tim$, dat$) STATIC
        days& = Date2Julian&(dat$) - 2444240
        hour% = VAL(LEFT$(tim$, 2))
        minute% = VAL(MID$(tim$, 4, 2))
        second% = VAL(RIGHT$(tim$, 2))
        secs& = CLNG(hour%) * 3600 + minute% * 60 + second%
        IF days& >= 0 AND days& < 24857 THEN
            TimeDate2Second& = days& * 86400 + secs&
        ELSE
            PRINT "TimeDate2Second: Not in range 1980 to 2047"
            SYSTEM
        END IF
    END FUNCTION


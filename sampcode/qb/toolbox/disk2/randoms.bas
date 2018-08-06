  ' ************************************************
  ' **  Name:          RANDOMS                    **
  ' **  Type:          Toolbox                    **
  ' **  Module:        RANDOMS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  ' USAGE:           No command line parameters
  ' .MAK FILE:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       i%      Loop index for generating pseudorandom numbers
  
    DECLARE FUNCTION Rand& ()
    DECLARE FUNCTION RandExponential! (mean!)
    DECLARE FUNCTION RandFrac! ()
    DECLARE FUNCTION RandInteger% (a%, b%)
    DECLARE FUNCTION RandNormal! (mean!, stddev!)
    DECLARE FUNCTION RandReal! (x!, y!)
  
    DECLARE SUB RandShuffle (key$)
  
  ' Array of long integers for generating all randoms
    DIM SHARED r&(1 TO 100)
  
  ' Clear the screen
    CLS
  
  ' Shuffle the random number generator, creating a
  ' unique sequence for every possible second
    RandShuffle DATE$ + TIME$
  
    PRINT "Rand&"
    FOR i% = 1 TO 5
        PRINT Rand&,
    NEXT i%
    PRINT
  
    PRINT "RandInteger%(0, 9)"
    FOR i% = 1 TO 5
        PRINT RandInteger%(0, 9),
    NEXT i%
    PRINT
  
    PRINT "RandReal!(-10!, 10!)"
    FOR i% = 1 TO 5
        PRINT RandReal!(-10!, 10!),
    NEXT i%
    PRINT
  
    PRINT "RandExponential!(100!)"
    FOR i% = 1 TO 5
        PRINT RandExponential!(100!),
    NEXT i%
    PRINT
  
    PRINT "RandNormal!(100!, 10!)"
    FOR i% = 1 TO 5
        PRINT RandNormal!(100!, 10!),
    NEXT i%
    PRINT
  
    PRINT "RandFrac!"
    FOR i% = 1 TO 5
        PRINT RandFrac!,
    NEXT i%
    PRINT
  
  ' ************************************************
  ' **  Name:          Rand&                      **
  ' **  Type:          Function                   **
  ' **  Module:        RANDOMS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a pseudorandom long integer in the range
  ' 0 through 999999999.
  '
  ' EXAMPLE OF USE:  n& = Rand&
  ' PARAMETERS:      (none)
  ' VARIABLES:       i%         First index into random number table
  '                  j%         Second index into random number table
  '                  t&         Working variable
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Rand& ()
  '                  DIM SHARED r&(1 TO 100)
  '
    FUNCTION Rand& STATIC
      
      ' Get the pointers into the table
        i% = r&(98)
        j% = r&(99)
      
      ' Subtract the two table values
        t& = r&(i%) - r&(j%)
      
      ' Adjust result if less than zero
        IF t& < 0 THEN
            t& = t& + 1000000000
        END IF
      
      ' Replace table entry with new random number
        r&(i%) = t&
      
      ' Decrement first index, keeping in range 1 through 55
        IF i% > 1 THEN
            r&(98) = i% - 1
        ELSE
            r&(98) = 55
        END IF
      
      ' Decrement second index, keeping in range 1 through 55
        IF j% > 1 THEN
            r&(99) = j% - 1
        ELSE
            r&(99) = 55
        END IF
      
      ' Use last random number to index into shuffle table
        i% = r&(100) MOD 42 + 56
      
      ' Grab random from table as current random number
        r&(100) = r&(i%)
      
      ' Put new calculated random into table
        r&(i%) = t&
      
      ' Return the random number grabbed from the table
        Rand& = r&(100)
      
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          RandExponential!           **
  ' **  Type:          Function                   **
  ' **  Module:        RANDOMS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns an exponentially distributed pseudorandom,
  ' single-precision number given the mean of the
  ' distribution.
  '
  ' EXAMPLE OF USE:  x! = RandExponential!(mean!)
  ' PARAMETERS:      mean!          The mean of the exponential distribution
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION RandExponential! (mean!)
  '
    FUNCTION RandExponential! (mean!) STATIC
        RandExponential! = -mean! * LOG(RandFrac!)
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          RandFrac!                  **
  ' **  Type:          Function                   **
  ' **  Module:        RANDOMS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a pseudorandom, single-precision number
  ' in the range 0 through 1.
  '
  ' EXAMPLE OF USE:  x! = RandFrac!
  ' PARAMETERS:      (none)
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION RandFrac! ()
  '
    FUNCTION RandFrac! STATIC
        RandFrac! = Rand& / 1E+09
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          RandInteger%               **
  ' **  Type:          Function                   **
  ' **  Module:        RANDOMS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a pseudorandom integer in the range
  ' a% to b% inclusive.
  '
  ' EXAMPLE OF USE:  n% = RandInteger%(a%, b%)
  ' PARAMETERS:      a%           Minimum value for returned integer
  '                  b%           Maximum value for returned integer
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION RandInteger% (a%, b%)
  '
    FUNCTION RandInteger% (a%, b%) STATIC
        RandInteger% = a% + (Rand& MOD (b% - a% + 1))
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          RandNormal!                **
  ' **  Type:          Function                   **
  ' **  Module:        RANDOMS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a normally distributed single-precision,
  ' pseudorandom number given the mean and standard deviation.
  '
  ' EXAMPLE OF USE:  x! = RandNormal!(mean!, stddev!)
  ' PARAMETERS:      mean!           Mean of the distribution of returned
  '                                  values
  '                  stddev!         Standard deviation of the distribution
  ' VARIABLES:       u1!             Pseudorandom positive real value
  '                                  less than 1
  '                  u2!             Pseudorandom positive real value
  '                                  less than 1
  '                  x!              Working value
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION RandNormal! (mean!, stddev!)
  '
    FUNCTION RandNormal! (mean!, stddev!) STATIC
        u1! = RandFrac!
        u2! = RandFrac!
        x! = SQR(-2! * LOG(u1!)) * COS(6.283185 * u2)
        RandNormal! = mean! + stddev! * x!
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          RandReal!                  **
  ' **  Type:          Function                   **
  ' **  Module:        RANDOMS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns a pseudorandom, single-precision real
  ' number in the range x! to y!.
  ' EXAMPLE OF USE:  z! = RandReal!(x!, y!)
  ' PARAMETERS:      x!           Minimum for returned value
  '                  y!           Maximum for returned value
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION RandReal! (x!, y!)
  '
    FUNCTION RandReal! (x!, y!) STATIC
        RandReal! = x! + (y! - x!) * (Rand& / 1E+09)
    END FUNCTION
  
  ' ************************************************
  ' **  Name:          RandShuffle                **
  ' **  Type:          Subprogram                 **
  ' **  Module:        RANDOMS.BAS                **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Creates original table of pseudorandom long integers
  ' for use by the function Rand&.  The contents of key$
  ' are used to seed the table.
  '
  ' EXAMPLE OF USE:  RandShuffle(key$)
  ' PARAMETERS:      key$            String used to seed the generator
  '          r&(1 TO 100) (shared)   Array of long integers for
  '                                  generating pseudorandom numbers
  ' VARIABLES:       k$              Modified key string
  '                  i%              Index into k$, index into table
  '                  j%              Index into table
  '                  k%              Loop count for warming up generator
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB RandShuffle (key$)
  '
    SUB RandShuffle (key$) STATIC
      
      ' Form 97-character string, with key$ as part of it
        k$ = LEFT$("Abra Ca Da Bra" + key$ + SPACE$(83), 97)
      
      ' Use each character to seed table
        FOR i% = 1 TO 97
            r&(i%) = ASC(MID$(k$, i%, 1)) * 8171717 + i% * 997&
        NEXT i%
      
      ' Preserve string space
        k$ = ""
      
      ' Initialize pointers into table
        i% = 97
        j% = 12
      
      ' Randomize the table to get it warmed up
        FOR k% = 1 TO 997
          
          ' Subtract entries pointed to by i% and j%
            r&(i%) = r&(i%) - r&(j%)
          
          ' Adjust result if less than zero
            IF r&(i%) < 0 THEN
                r&(i%) = r&(i%) + 1000000000
            END IF
          
          ' Decrement first index, keeping in range of 1 through 97
            IF i% > 1 THEN
                i% = i% - 1
            ELSE
                i% = 97
            END IF
          
          ' Decrement second index, keeping in range of 1 through 97
            IF j% > 1 THEN
                j% = j% - 1
            ELSE
                j% = 97
            END IF
          
        NEXT k%
      
      ' Initialize pointers for use by Rand& function
        r&(98) = 55
        r&(99) = 24
      
      ' Initialize pointer for shuffle table lookup by Rand& function
        r&(100) = 77
      
    END SUB
  

  ' ************************************************
  ' **  Name:          TRIANGLE                   **
  ' **  Type:          Toolbox                    **
  ' **  Module:        TRIANGLE.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' USAGE:          No command line parameters
  ' REQUIREMENTS:   CGA
  ' .MAK FILE:      TRIANGLE.BAS
  '                 QCALMATH.BAS
  ' PARAMETERS:     (none)
  ' VARIABLES:      sA$        User input of side a
  '                 sB$        User input of side b
  '                 sC$        User input of side c
  '                 aA$        User input of angle A
  '                 aB$        User input of angle B
  '                 aC$        User input of angle C
  '                 sA#        Side A
  '                 sB#        Side B
  '                 sC#        Side C
  '                 aA#        Angle A
  '                 aB#        Angle B
  '                 aC#        Angle C
  
  ' Functions
    DECLARE FUNCTION Deg2Rad# (deg#)
    DECLARE FUNCTION Rad2Deg# (rad#)
    DECLARE FUNCTION ArcCosine# (x#)
    DECLARE FUNCTION ArcSine# (x#)
    DECLARE FUNCTION TriangleArea# (sA#, sB#, sC#)
  
  ' Subprograms
    DECLARE SUB Triangle (sA#, sB#, sC#, aA#, aB#, aC#)
  
  ' Initialization
    SCREEN 2
    CLS
    PRINT "TRIANGLE"
  
  ' Draw a representative triangle
    WINDOW (0, 0)-(1, 1)
    LINE (.3, .7)-(.8, .7)
    LINE -(.4, 1)
    LINE -(.3, .7)
  
  ' Label the triangle sides
    LOCATE 4, 26
    PRINT "a"
    LOCATE 3, 48
    PRINT "b"
    LOCATE 9, 42
    PRINT "c"
  
  ' Label the triangle angles
    LOCATE 7, 55
    PRINT "A"
    LOCATE 7, 28
    PRINT "B"
    LOCATE 2, 33
    PRINT "C"
  
  ' Ask user for the known data
    LOCATE 12, 1
    PRINT "Enter known sides and angles (deg),"
    PRINT "and press Enter for unknowns..."
    LOCATE 16, 1
    LINE INPUT "Side  a  "; sA$
    LINE INPUT "Side  b  "; sB$
    LINE INPUT "Side  c  "; sC$
    PRINT
    LINE INPUT "Angle A  "; aA$
    LINE INPUT "Angle B  "; aB$
    LINE INPUT "Angle C  "; aC$
    PRINT
  
  ' Convert to numeric values
    sA# = VAL(sA$)
    sB# = VAL(sB$)
    sC# = VAL(sC$)
    aA# = Deg2Rad#(VAL(aA$))
    aB# = Deg2Rad#(VAL(aB$))
    aC# = Deg2Rad#(VAL(aC$))
  
  ' Solve for the unknowns
    Triangle sA#, sB#, sC#, aA#, aB#, aC#
  
  ' Output the results
    LOCATE 16, 1
    PRINT "Side  a  "; sA#
    PRINT "Side  b  "; sB#
    PRINT "Side  c  "; sC#
    PRINT
    PRINT "Angle A  "; Rad2Deg#(aA#); "Deg"
    PRINT "Angle B  "; Rad2Deg#(aB#); "Deg"
    PRINT "Angle C  "; Rad2Deg#(aC#); "Deg"
    LOCATE 20, 40
    PRINT "Area = "; TriangleArea#(sA#, sB#, sC#)

  ' All done
    LOCATE 24, 1
    PRINT "Press any key to continue";
    DO
    LOOP WHILE INKEY$ = ""
    SCREEN 0
    END

  ' ************************************************
  ' **  Name:          Deg2Rad#                   **
  ' **  Type:          Function                   **
  ' **  Module:        TRIANGLE.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Converts degree angular units to radians.
  '
  ' EXAMPLE OF USE:  r# = Deg2Rad#(deg#)
  ' PARAMETERS:      deg#       Degrees
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Deg2Rad# (deg#)
  '
    FUNCTION Deg2Rad# (deg#) STATIC
        Deg2Rad# = deg# / 57.29577951308232#
    END FUNCTION

  ' ************************************************
  ' **  Name:          Rad2Deg#                   **
  ' **  Type:          Function                   **
  ' **  Module:        TRIANGLE.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Converts radian angular units to degrees.
  '
  ' EXAMPLE OF USE:  d# = Rad2Deg#(rad#)
  ' PARAMETERS:      rad#       Radians
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Rad2Deg# (rad#)
  '
    FUNCTION Rad2Deg# (rad#) STATIC
        Rad2Deg# = rad# * 57.29577951308232#
    END FUNCTION

  ' ************************************************
  ' **  Name:          Triangle                   **
  ' **  Type:          Subprogram                 **
  ' **  Module:        TRIANGLE.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Calculates all sides and angles of a triangle,
  ' assuming enough sides and angles are given.
  '
  ' EXAMPLE OF USE:  Triangle sA#, sB#, sC#, aA#, aB#, aC#
  ' PARAMETERS:      sA#        Side A
  '                  sB#        Side B
  '                  sC#        Side C
  '                  aA#        Angle A
  '                  aB#        Angle B
  '                  aC#        Angle C
  ' VARIABLES:       i%         Looping index
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Triangle (sA#, sB#, sC#, aA#, aB#, aC#)
  '
    SUB Triangle (sA#, sB#, sC#, aA#, aB#, aC#) STATIC
      
        FOR i% = 1 TO 18
          
            IF aA# = 0# THEN
                IF sA# <> 0# AND sB# <> 0# AND sC# <> 0# THEN
                    t# = sB# * sB# + sC# * sC# - sA# * sA#
                    aA# = ArcCosine#(t# / 2# / sB# / sC#)
                END IF
            END IF
          
            IF aB# = 0# THEN
                IF sA# <> 0# AND sB# <> 0# AND aA# <> 0# THEN
                    aB# = ArcSine#(sB# * SIN(aA#) / sA#)
                END IF
            END IF
          
            IF aC# = 0# THEN
                IF aA# <> 0# AND aB# <> 0# THEN
                    aC# = 3.141592653589793# - aA# - aB#
                END IF
            END IF
          
            IF sB# = 0# THEN
                IF sA# <> 0# AND aB# <> 0# AND aA# <> 0# THEN
                    sB# = sA# * SIN(aB#) / SIN(aA#)
                END IF
            END IF
          
            IF sC# = 0# THEN
                IF sA# <> 0# AND sB# <> 0# AND aC# <> 0# THEN
                    t# = sA# * sA# + sB# * sB#
                    sC# = SQR(t# - 2# * sA# * sB# * COS(aC#))
                END IF
            END IF
          
            IF i% MOD 2 THEN
                SWAP sB#, sC#
                SWAP aB#, aC#
            ELSE
                SWAP sA#, sB#
                SWAP aA#, aB#
            END IF
          
        NEXT i%
      
    END SUB

  ' ************************************************
  ' **  Name:          TriangleArea#              **
  ' **  Type:          Function                   **
  ' **  Module:        TRIANGLE.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the area of a triangle given the three sides.
  '
  ' EXAMPLE OF USE:  TriangleArea# sA#, sB#, sC#
  ' PARAMETERS:      sA#        Side A
  '                  sB#        Side B
  '                  sC#        Side C
  ' VARIABLES:       s#         Sum of the three sides of the triangle
  '                             divided by two
  '                  t1#        Temporary variable
  '                  t2#        Temporary variable
  '                  t3#        Temporary variable
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION TriangleArea# (sA#, sB#, sC#)
  '
    FUNCTION TriangleArea# (sA#, sB#, sC#) STATIC
        s# = (sA# + sB# + sC#) / 2#
        t1# = s# - sA#
        t2# = s# - sB#
        t3# = s# - sC#
        TriangleArea# = SQR(s# * t1# * t2# * t3#)
    END FUNCTION


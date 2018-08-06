  ' ************************************************
  ' **  Name:          CARTESIA                   **
  ' **  Type:          Toolbox                    **
  ' **  Module:        CARTESIA.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Demonstrates a set of functions and subprograms
  ' dealing with Cartesian coordinates.
  '
  ' USAGE:           No command line parameters
  ' .MAK FILE:       (none)
  ' PARAMETERS:      (none)
  ' VARIABLES:       x!     X value of Cartesian coordinate
  '                  y!     Y value of Cartesian coordinate
  '                  r!     Polar notation distance from origin
  '                  theta! Polar notation angle from X axis
  
    DECLARE FUNCTION Angle! (x!, y!)
    DECLARE FUNCTION Magnitude! (x!, y!)
  
    DECLARE SUB Pol2Rec (r!, theta!, x!, y!)
    DECLARE SUB Rec2Pol (x!, y!, r!, theta!)
  
    CLS
    INPUT "Enter X  ", x!
    INPUT "Enter Y  ", y!
    PRINT
    PRINT "Magnitude!(x!, y!)", Magnitude!(x!, y!)
    PRINT "Angle!(x!, y!)", Angle!(x!, y!)
    PRINT
    Rec2Pol x!, y!, r!, theta!
    PRINT "Rec2Pol", , r!; theta!
    Pol2Rec r!, theta!, x!, y!
    PRINT "Pol2Rec", , x!; y!

  ' ************************************************
  ' **  Name:          Angle!                     **
  ' **  Type:          Function                   **
  ' **  Module:        CARTESIA.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the angle (in radians) between the X axis
  ' and the line from the origin to the point x!,y!
  '
  ' EXAMPLE OF USE:  a! = Angle!(x!, y!)
  ' PARAMETERS:      x!         X part of the Cartesian coordinate
  '                  y!         Y part of the Cartesian coordinate
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Angle! (x!, y!)
  '
    FUNCTION Angle! (x!, y!) STATIC
      
        CONST PI = 3.141593
        CONST HALFPI = PI / 2
      
        IF x! = 0! THEN
            IF y! > 0! THEN
                Angle! = HALFPI
            ELSEIF y! < 0! THEN
                Angle! = -HALFPI
            ELSE
                Angle! = 0!
            END IF
        ELSEIF y! = 0! THEN
            IF x! < 0! THEN
                Angle! = PI
            ELSE
                Angle! = 0!
            END IF
        ELSE
            IF x! < 0! THEN
                IF y! > 0! THEN
                    Angle! = ATN(y! / x!) + PI
                ELSE
                    Angle! = ATN(y! / x!) - PI
                END IF
            ELSE
                Angle! = ATN(y! / x!)
            END IF
        END IF
      
    END FUNCTION

  ' ************************************************
  ' **  Name:          Magnitude!                 **
  ' **  Type:          Function                   **
  ' **  Module:        CARTESIA.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the distance from the origin to the
  ' point x!,y!
  '
  ' EXAMPLE OF USE:  r! =  Magnitude!(x!, y!)
  ' PARAMETERS:      x!         X part of the Cartesian coordinate
  '                  y!         Y part of the Cartesian coordinate
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Magnitude! (x!, y!)
  '
    FUNCTION Magnitude! (x!, y!) STATIC
        Magnitude! = SQR(x! * x! + y! * y!)
    END FUNCTION

  ' ************************************************
  ' **  Name:          Pol2rec                    **
  ' **  Type:          Subprogram                 **
  ' **  Module:        CARTESIA.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Converts polar coordinates to Cartesian notation.
  '
  ' EXAMPLE OF USE:  Pol2Rec r!, theta!, x!, y!
  ' PARAMETERS:      r!         Distance of point from the origin
  '                  theta!     Angle of point from the X axis
  '                  x!         X coordinate of the point
  '                  y!         Y coordinate of the point
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB Pol2Rec (r!, theta!, x!, y!)
  '
    SUB Pol2Rec (r!, theta!, x!, y!) STATIC
        x! = r! * COS(theta!)
        y! = r! * SIN(theta!)
    END SUB

  ' ************************************************
  ' **  Name:          Rec2pol                    **
  ' **  Type:          Subprogram                 **
  ' **  Module:        CARTESIA.BAS               **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Converts Cartesian coordinates to polar notation.
  '
  ' EXAMPLE OF USE:  Rec2Pol x!, y!, r!, theta!
  ' PARAMETERS:      x!         X coordinate of the point
  '                  y!         Y coordinate of the point
  '                  r!         Distance of point from the origin
  '                  theta!     Angle of point from the X axis
  ' VARIABLES:       (none)
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Angle! (x!, y!)
  '                  DECLARE FUNCTION Magnitude! (x!, y!)
  '                  DECLARE SUB Rec2Pol (x!, y!, r!, theta!)
  '
    SUB Rec2Pol (x!, y!, r!, theta!) STATIC
        r! = Magnitude!(x!, y!)
        theta! = Angle!(x!, y!)
    END SUB


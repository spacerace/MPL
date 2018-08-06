  ' ************************************************
  ' **  Name:          GAMES                      **
  ' **  Type:          Toolbox                    **
  ' **  Module:        GAMES.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' USAGE:          No command line parameters
  ' REQUIREMENTS:   CGA
  ' .MAK FILE:      (none)
  ' PARAMETERS:     (none)
  ' VARIABLES:      a$             String containing the 26 letters of the
  '                                alphabet
  '                 x%             Lower bound for array a%()
  '                 y%             Upper bound for array a%()
  '                 a%()           Array of numbers to be shuffled
  '                 i%             Looping index
  '                 size%          Dimension of bouncing ball array
  '                 object%()      Array for GET and PUT of bouncing ball
  '                 backGround%()  Array for GET and PUT of background
  '                 dx%            X velocity of bouncing ball
  '                 dy%            Y velocity of bouncing ball
  '                 px%            X coordinate of bouncing ball
  '                 py%            Y coordinate of bouncing ball
  '                 testNumber%    One of four bounce direction tests
  '                 test%          Result of the Collision% test
  
  
  ' Constants
    CONST FALSE = 0
    CONST TRUE = NOT FALSE
  
  ' Functions
    DECLARE FUNCTION Shuffle$ (a$)
    DECLARE FUNCTION Dice% (numberOfDice%)
    DECLARE FUNCTION Card$ (cardNumber%)
    DECLARE FUNCTION Collision% (object%(), backGround%())
  
  ' Subprograms
    DECLARE SUB FillArray (a%())
    DECLARE SUB ShuffleArray (a%())
  
  ' Demonstration of the Shuffle$ function
    CLS
    RANDOMIZE TIMER
    a$ = "abcdefghijklmnopqrstuvwxyz"
    PRINT "a$           = "; a$
    PRINT "Shuffle$(a$) = "; Shuffle$(a$)
    PRINT
  
  ' Demonstration of the FillArray subprogram
    x% = -7
    y% = 12
    DIM a%(x% TO y%)
    PRINT "FillArray a%()   where DIM a%( -7 TO 12) ..."
    FillArray a%()
    FOR i% = x% TO y%
        PRINT a%(i%);
    NEXT i%
    PRINT
  
  ' Demonstration of the ShuffleArray subprogram
    PRINT
    PRINT "ShuffleArray a%() ..."
    ShuffleArray a%()
    FOR i% = x% TO y%
        PRINT a%(i%);
    NEXT i%
    PRINT
  
  ' Demonstration of the Dice% function
    PRINT
    PRINT "Dice%(2)..."
    FOR i% = 1 TO 20
        PRINT Dice%(2);
    NEXT i%
    PRINT
  
  ' Deal a hand of seven cards
    PRINT
    PRINT "Seven random cards, without replacement..."
    REDIM a%(1 TO 54)
    FillArray a%()
    ShuffleArray a%()
    FOR i% = 1 TO 7
        PRINT Card$(a%(i%))
    NEXT i%
    PRINT
  
  ' Wait for user to press a key
    PRINT
    PRINT "Press any key to continue"
    DO
    LOOP WHILE INKEY$ = ""
  
  ' Demonstration of the Collision% function
    size% = 6
    DIM object%(size%), backGround%(size%)
  
  ' Set medium resolution graphics mode
    SCREEN 1
  
  ' Create the bouncing ball
    CIRCLE (2, 2), 2, 3
    PAINT (2, 2), 3
    GET (0, 0)-(4, 4), object%
  
  ' Make solid border around screen
    LINE (14, 18)-(305, 187), 1, B
    PAINT (0, 0), 1
  
    PRINT " Collision% function... Press any key to quit "
  
  ' Make three obstacles
    CIRCLE (115, 78), 33, 2, , , .6
    PAINT (115, 78), 2
    CIRCLE (205, 78), 33, 2, , , .6
    PAINT (205, 78), 2
    LINE (90, 145)-(230, 155), 2, BF
  
  ' Initialize position and velocity of the object
    dx% = 1
    dy% = 1
    px% = 160
    py% = 44
    PUT (px%, py%), object%
  
  ' Move the object around the screen, avoiding collisions,
  ' until any key is pressed
    DO
        testNumber% = 0
        DO
            PUT (px%, py%), object%
            px% = px% + dx%
            py% = py% + dy%
            GET (px%, py%)-(px% + 4, py% + 4), backGround%
            PUT (px%, py%), object%
            test% = Collision%(object%(), backGround%())
            IF test% THEN
                testNumber% = testNumber% + 1
                PUT (px%, py%), object%
                px% = px% - dx%
                py% = py% - dy%
                SELECT CASE testNumber%
                CASE 1
                    dx% = -dx%
                CASE 2
                    dx% = -dx%
                    dy% = -dy%
                CASE 3
                    dy% = -dy%
                CASE ELSE
                END SELECT
                PUT (px%, py%), object%
            END IF
        LOOP UNTIL test% = 0
    LOOP UNTIL INKEY$ <> ""
  
  ' Clean up a little
    SCREEN 0
    WIDTH 80
    CLS
    SYSTEM
  

  ' ************************************************
  ' **  Name:          Card$                      **
  ' **  Type:          Function                   **
  ' **  Module:        GAMES.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the name of a playing card given a number
  ' from 1 to 52.  Any other number returns "Joker."
  '
  ' EXAMPLE OF USE:  PRINT Card$(n%)
  ' PARAMETERS:      n%         Number from 1 to 52 representing a card (any
  '                             other number returns a Joker)
  ' VARIABLES:       suit$      Name of one of the four card suits
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Card$ (cardNumber%)
  '
    FUNCTION Card$ (cardNumber%)
      
        SELECT CASE (cardNumber% - 1) \ 13      ' Which suit?
        CASE 0
            suit$ = " of Spades"
        CASE 1
            suit$ = " of Clubs"
        CASE 2
            suit$ = " of Hearts"
        CASE 3
            suit$ = " of Diamonds"
        CASE ELSE
            Card$ = "Joker"
            EXIT FUNCTION
        END SELECT
      
        SELECT CASE (cardNumber% - 1) MOD 13    ' Which card?
        CASE 0
            Card$ = "Ace" + suit$
        CASE 1 TO 9
            Card$ = MID$(STR$(cardNumber% MOD 13), 2) + suit$
        CASE 10
            Card$ = "Jack" + suit$
        CASE 11
            Card$ = "Queen" + suit$
        CASE 12
            Card$ = "King" + suit$
        END SELECT
      
    END FUNCTION

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
  ' EXAMPLE OF USE:  test% = Collision%(object%(), backGround%())
  ' PARAMETERS:      object%()       First array, filled in with the GET
  '                                  statement
  '                  backGround%()   Second array, filled in with the GET
  '                                  statement
  ' VARIABLES:       lo%             Lower bound of first array
  '                  up%             Upper bound of first array
  '                  lb%             Lower bound of second array
  '                  ub%             Upper bound of second array
  '                  i%              Index to integers in each array
  ' MODULE LEVEL
  '   DECLARATIONS:  CONST FALSE = 0
  '                  CONST TRUE = NOT FALSE
  '                  DECLARE FUNCTION Collision% (object%(), backGround%())
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
  ' **  Name:          Dice%                      **
  ' **  Type:          Function                   **
  ' **  Module:        GAMES.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Returns the total of the dots showing when any
  ' desired number of dice are rolled.
  '
  ' EXAMPLE OF USE:  total% = Dice%(n%)
  ' PARAMETERS:      n%         Number of dice
  ' VARIABLES:       toss%      Loop index for throwing the n% dice
  '                  total%     Total of the dots showing
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Dice% (numberOfDice%)
  '
    FUNCTION Dice% (numberOfDice%)
        IF numberOfDice% < 1 THEN
            PRINT "Error: Dice%() - Can't throw fewer than one die"
            SYSTEM
        END IF
        FOR toss% = 1 TO numberOfDice%
            total% = total% + INT(RND * 6) + 1
        NEXT toss%
        Dice% = total%
    END FUNCTION

  ' ************************************************
  ' **  Name:          FillArray                  **
  ' **  Type:          Subprogram                 **
  ' **  Module:        GAMES.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Initializes an integer array by putting i% into
  ' each i%th element.
  '
  ' EXAMPLE OF USE:  FillArray a%()
  ' PARAMETERS:      a%()       Array to be filled with a sequence of numbers
  ' VARIABLES:       i%         Looping index
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB FillArray (a%())
  '
    SUB FillArray (a%()) STATIC
        FOR i% = LBOUND(a%) TO UBOUND(a%)
            a%(i%) = i%
        NEXT i%
    END SUB

  ' ************************************************
  ' **  Name:          Shuffle$                   **
  ' **  Type:          Function                   **
  ' **  Module:        GAMES.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Randomizes the order of the character bytes in a$.
  '
  ' EXAMPLE OF USE:  b$ = Shuffle$(a$)
  ' PARAMETERS:      a$         String to be shuffled
  ' VARIABLES:       x$         Working string space
  '                  lenx%      Number of bytes in the string
  '                  i%         Pointer to each byte
  '                  j%         Pointer to randomly selected byte
  '                  t$         Temporary byte-swapping string
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE FUNCTION Shuffle$ (a$)
  '
    FUNCTION Shuffle$ (a$) STATIC
        x$ = a$
        lenx% = LEN(x$)
        FOR i% = 1 TO lenx%
            j% = INT(RND * lenx% + 1)
            t$ = MID$(x$, i%, 1)
            MID$(x$, i%, 1) = MID$(x$, j%, 1)
            MID$(x$, j%, 1) = t$
        NEXT i%
        Shuffle$ = x$
        x$ = ""
    END FUNCTION

  ' ************************************************
  ' **  Name:          ShuffleArray               **
  ' **  Type:          Subprogram                 **
  ' **  Module:        GAMES.BAS                  **
  ' **  Language:      Microsoft QuickBASIC 4.00  **
  ' ************************************************
  '
  ' Randomizes the order of the integers in a%()
  ' by swapping contents in a pseudorandom order.
  '
  ' EXAMPLE OF USE:  ShuffleArray a%()
  ' PARAMETERS:      a%()       Array to be shuffled
  ' VARIABLES:       lb%        Lower bound of the array
  '                  ub%        Upper bound of the array
  '                  range%     Number of array entries
  '                  i%         Looping index
  '
  ' MODULE LEVEL
  '   DECLARATIONS:  DECLARE SUB ShuffleArray (a%())
  '
    SUB ShuffleArray (a%()) STATIC
        lb% = LBOUND(a%)
        ub% = UBOUND(a%)
        range% = ub% - lb% + 1
        FOR i% = lb% TO ub%
            SWAP a%(i%), a%(INT(RND * range% + lb%))
        NEXT i%
    END SUB


'***** DEMO.BAS - demonstrates the BASIC and assembler OS/2 routines
'
'This routine can call either assembly language or BASIC modules.
'Include the statements marked "*** BASIC only" if using BASIC modules,
'and those marked "*** Assembly only" if using assembly language routines.
'

DEFINT A-Z

DECLARE FUNCTION FileCount%(FileSpec$)
DECLARE FUNCTION GetDir$(Drive$)
DECLARE FUNCTION GetDrive%()

DECLARE SUB ReadNames(Spec$, Array$())      '*** BASIC only
DECLARE SUB ReadNames(Spec$, BYVAL Element) '*** Assembly only
DECLARE SUB SetDrive(Drive$)

INPUT "Enter a file specification or press Enter for *.*:  ", Spec$

IF (Spec$ = "") THEN			  'Pressed Enter, use *.*
    Spec$ = "*.*"
ENDIF

Spec$ = Spec$ + CHR$(0)                   '*** Assembly only -- append NULL

Count = FileCount%(Spec$)                 'count the number of matching files

IF (Count > 0) THEN

    DIM Array$(1 TO Count)                'make an array to hold them

    FOR X = 1 TO Count                    '*** Assembly only
        Array$(X) = SPACE$(12)		  'Make array 12 characters long
    NEXT

    ReadNames Spec$, Array$()             '*** BASIC only
    ReadNames Spec$, VARPTR(Array$(1))    '*** Assembly only

    FOR X = 1 TO Count                    'print the filenames
        PRINT Array$(X),
    NEXT

ELSE

    PRINT "No files found"		  'No matching files

ENDIF

SaveDrive$ = CHR$(GetDrive%)              'save the current drive
PRINT
PRINT "The current drive is " SaveDrive$          'print it
PRINT "and its directory is " GetDir$(SaveDrive$) 'print current directory
INPUT "Enter a new drive to move to: ", NewDrive$ 'prompt for a new drive
SetDrive NewDrive$                        'set it

PRINT "The current drive is: " CHR$(GetDrive%)     'print the new drive

PRINT "Moving back to "SaveDrive$	           'inform user
SetDrive SaveDrive$                                'restore original drive

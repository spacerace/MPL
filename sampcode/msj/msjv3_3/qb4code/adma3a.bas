'	Microsoft Systems Journal
'	Volume 3; Issue 3; May, 1988


'	Code Listings For:

'	ADMA3A
'	pp. 49-62

'	Author(s): Augie Hansen
'	Title:     New Compiler Technology Boosts Microsoft QuickBASIC 4.0 
'	           Productivity




'	Figure 7
'	========



'====================================================================
' ADM3A Terminal Emulator
' Version 1.0
'
' This program emulates a Lear Siegler adm3a video terminal.  The
' emulator gives PC users the ability to run full-screen video
' programs on UNIX and XENIX systems, and others that support a
' video terminal interface.
'
' Author: Augie Hansen
' Released: 1-14-88
'====================================================================

DEFINT A-Z

'--- Subprogram declarations.
DECLARE SUB BreakSignal ()
DECLARE SUB Delay (Period!)
DECLARE SUB Dial ()
DECLARE SUB DoCommand (CmdKey$)
DECLARE SUB InitScreen ()
DECLARE SUB ProcessInput (Code$)

'--- Manifest constants.
CONST TRUE = -1, FALSE = NOT TRUE
CONST BLACK = 0, BLUE = 1, GREEN = 2, CYAN = 3
CONST RED = 4, MAGENTA = 5, BROWN = 6, WHITE = 7
CONST BRIGHT = 8, BLINK = 128
CONST CURSOROFF = 0, CURSORON = 1
CONST BUFSIZE = 512
CONST SPACE = 32
CONST ROWS = 25, COLS = 80
CONST BANNERCOL = 4, COMMANDCOL = 33
CONST TESTMODE = 0

'--- Screen management data.
TYPE WinType
	Top AS INTEGER
	Left AS INTEGER
	Bottom AS INTEGER
	Right AS INTEGER
	Fgnd AS INTEGER
	Bkgnd AS INTEGER
	Standout AS INTEGER
END TYPE

DIM CmdWin AS WinType
DIM ViewWin AS WinType

CmdWin.Top = 1
CmdWin.Bottom = 1
CmdWin.Left = 1
CmdWin.Right = 80
CmdWin.Fgnd = BLACK
CmdWin.Bkgnd = WHITE
CmdWin.Standout = BROWN + BRIGHT

ViewWin.Top = 2
ViewWin.Bottom = 25
ViewWin.Left = 1
ViewWin.Right = 80
ViewWin.Fgnd = WHITE
ViewWin.Bkgnd = BLUE
ViewWin.Standout = WHITE + BRIGHT

'--- Set cursor-positioning offsets.
RowOffset = SPACE - ViewWin.Top
ColOffset = SPACE - ViewWin.Left

'--- Install an error-recovery mechanism.
ON ERROR GOTO ErrorRecovery

'--- Set up the emulator screen.
InitScreen

'--- Set communications parameters.
Parm$ = ENVIRON$("COMPARMS")            ' Check environment.
IF Parm$ = "" THEN
	Parm$ = "COM2:1200,E,7,1"       ' Use defaults.
END IF

Port$ = LEFT$(Parm$, 4)
IF Port$ = "COM1" THEN
	PortAddress = &H3FB
ELSE
	PortAddress = &H2FB
END IF
BreakMask = &H40                        ' Break control bits

'--- Open the communications channel.
OPEN Parm$ FOR RANDOM AS #1 LEN = BUFSIZE

'--------------------------------------------------------------------
' Main communications loop.
'
' Check the keyboard for input.  Send all normal characters typed by
' the user to the communications port for transmission to the remote
' system. If the user presses any of the emulator command keys, run
' the associated procedure.
'--------------------------------------------------------------------
Main:
EscapeFlag = FALSE
DO
	'--- Process keyboard input for commands and characters.
	UserKey$ = INKEY$
	IF LEN(UserKey$) > 1 THEN
		DoCommand UserKey$
	ELSEIF UserKey$ <> "" THEN
		'--- Send the character to the remote system.
		PRINT #1, UserKey$;
	END IF

	'--- Check the communications line for received characters.
	DO
		IF EOF(1) THEN
			EXIT DO
		END IF
		Received$ = INPUT$(1, #1) ' Read a single character.

		'--- Look for cursor-positioning command.
		IF EscapeFlag = TRUE THEN
			IF Received$ = "=" THEN
			    CursorRow = ASC(INPUT$(1, #1))-RowOffset
			    CursorCol = ASC(INPUT$(1, #1))-ColOffset
			    LOCATE CursorRow, CursorCol
			ELSE
			    PRINT CHR$(27); ' The retained Esc code.
			    PRINT Received$;
			END IF
			EscapeFlag = FALSE
		ELSE
			ProcessInput Received$
		END IF
	LOOP
LOOP

END

ErrorRecovery:
	RESUME Main

'
'====================================================================
' BreakSignal
'
' Send a "break" signal to the communications port.
'====================================================================
'
SUB BreakSignal
	SHARED PortAddress, BreakMask

	'--- Set the break bit.
	OUT PortAddress, (INP(PortAddress) OR BreakMask)

	'--- Mark time for the break period.
	Delay .5
       
	'--- Clear the break bit.
	OUT PortAddress, (INP(PortAddress) AND NOT BreakMask)
END SUB

'
'====================================================================
' Delay
'
' Produce a specified delay.  The delay period is specified in
' seconds as a single-precision number with tenth-second precision.
'====================================================================
'
SUB Delay (Period!) STATIC
	Start! = TIMER

	'--- Loop for specified period.  Abort if clock rolls over.
	DO
		Now! = TIMER
		IF (Now! - Start! < Period!) OR (Now! < Start!) THEN
			EXIT SUB
		END IF
	LOOP
END SUB

'
'====================================================================
' Dial
'
' Ask the user for a telephone number and dial it.
'====================================================================
'
SUB Dial
	INPUT "Number: ", Phone$
	PRINT #1, "ATDT" + Phone$
END SUB

'
'====================================================================
' DoCommand
'
' Examine the extended key code to see whether it is an Emulator
' program command.  If it is, execute the requested command.  If it
' is not, return to the caller without doing anything.
'====================================================================
'
SUB DoCommand (CmdKey$) STATIC
	SHARED CmdWin AS WinType
	SELECT CASE ASC(RIGHT$(CmdKey$, 1))
		CASE 16 ' Alt+q -- Quit the emulator.
			' Close the communications channel.
			CLOSE
			' Restore full screen.
			VIEW PRINT
			' Clear the screen and "home" the cursor.
			COLOR WHITE, BLACK
			CLS
			LOCATE CmdWin.Top, CmdWin.Left, CURSORON
			END
		CASE 32 ' Alt-d -- Dial a number
			Dial
		CASE 48 ' Alt+b -- Send break signal.
			BreakSignal
		CASE 83 ' PC keyboard Del key -- Send an ASCII DEL
			PRINT #1, CHR$(127);
		CASE ELSE
			' Unknown command -- ignore it.
	END SELECT
END SUB

'
'====================================================================
' InitScreen
'
' Set up command bar (1 line), guarantee that the cursor is turned
' on, and establish the active terminal display window (24 lines).
'====================================================================
'
SUB InitScreen STATIC
	SHARED CmdWin AS WinType, ViewWin AS WinType

	'--- Initialize the screen for text and 80 columns.
	SCREEN TEXTMODE
	WIDTH COLS, ROWS
	COLOR WHITE, BLACK
	CLS
       
	'--- Draw the command window on the top line.
	LOCATE CmdWin.Top, CmdWin.Left, CURSORON
	COLOR CmdWin.Fgnd, CmdWin.Bkgnd
	PRINT SPACE$(CmdWin.Right - CmdWin.Left + 1)
       
	'--- Display the program banner.
	LOCATE CmdWin.Top, CmdWin.Left + BANNERCOL
	COLOR CmdWin.Standout, CmdWin.Bkgnd
	PRINT "ADM3A EMULATOR";

	'--- Display a command summary.
	LOCATE CmdWin.Top, CmdWin.Left + COMMANDCOL
	COLOR CmdWin.Fgnd, CmdWin.Bkgnd
	PRINT "Break (Alt+b)   Dial (Alt+d)   Quit (Alt+q)"

	'--- Initialize the terminal screen.
	VIEW PRINT ViewWin.Top TO ViewWin.Bottom
	COLOR ViewWin.Fgnd, ViewWin.Bkgnd
	CLS
END SUB

'
'====================================================================
' ProcessInput
'
' Check input from the communucations line and analyze it.  Act on
' any adm3a terminal commands codes.  Pass anything else unchanged
' to the terminal screen.
'====================================================================
'
SUB ProcessInput (Code$) STATIC
	SHARED EscapeFlag, ViewWin AS WinType
	SELECT CASE ASC(Code$)
		CASE 8 ' ASCII backspace character
			IF POS(0) > ViewWin.Left THEN
				' Non-destructive backspace
				LOCATE , POS(0) - 1
			END IF
		CASE 10 ' ^J -- New-line character
			IF CSRLIN < ViewWin.Bottom THEN
				LOCATE CSRLIN + 1
			ELSE
				PRINT Code$;
			END IF
		CASE 11 ' ^K -- Upline command
			IF CSRLIN > ViewWin.Top THEN
				LOCATE CSRLIN - 1
			END IF
		CASE 12 ' ^L -- Form-feed character
			' adm3a use as non-destructive space
			IF POS(0) < ViewWin.Right THEN
				LOCATE , POS(0) + 1
			ELSEIF (POS(0) = ViewWin.Right) AND _
				    (CSRLIN < ViewWin.Bottom) THEN
				LOCATE CSRLIN + 1, ViewWin.Left
			END IF
		CASE 13
			LOCATE , ViewWin.Left
		CASE 26 ' ^Z -- clear the screen
			CLS
		CASE 27 ' Esc -- Could be start of cursor sequence
			EscapeFlag = TRUE
		CASE 30 ' ^^ -- Cursor to home position
			LOCATE ViewWin.Top, ViewWin.Left
		CASE ELSE
			PRINT Code$;
	END SELECT
END SUB


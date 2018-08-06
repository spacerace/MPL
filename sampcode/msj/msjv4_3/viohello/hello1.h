/* hello1.h	RHS	12/14/88
 *
 * OS/2	and	1988 version of	K&R's hello.c
 * demonstrates	multiple threads
 */


	/* Defines and Typedefs	*********************************/
#if	!defined(TRUE)
#define	TRUE				1
#endif

#if	!defined(FALSE)
#define	FALSE				0
#endif

#define	LINES25				4					/* height in lines of frames*/
#define	LINES43				6
#define	LINES50				7

#define	RANDOMIZER			5
#define	MAXFRAMES			28					/* limit to	max	possible	*/
#define	RAND()				(rand()	% maxframes);
#define	THREADSTACK			400					/* size	of stack each thread*/
#define	IDCOL				15
#define	ESC					0x1b
#define	BLINKING			0x80
#define	NOBLINKING			(~BLINKING)
#define	BRIGHT				0x08
#define	NORMAL				(~BRIGHT)
#define	VIOHDL				0
#define	KBDHDL				0
#define	MAXUSERNAME			40
#define	BLANK				' '
#define	LEFTMARKER			16
#define	RIGHTMARKER			17
#define	BACKGROUND(n)		(n << 4)
#define	COLORS(foreground,background)	(foreground	| BACKGROUND(background))
#define	FIELDROW			10
#define	FIELDCOL			20
#define	BACKGROUNDMASK		0x70
#define	UPARROW				72
#define	DOWNARROW			80
#define	SAVE				TRUE
#define	RESTORE				FALSE
#define	SAVECUR				0x0001
#define	RESTORECUR			0x0002
#define	HIDECUR				0x0004
#define	BYTES_PER_ROW		40
#define	CGA_MAGENTA			2
#define	UPPER_START_PIXEL	85
#define	LEFT_START_PIXEL	130
#define	BANK2OFFSET			8192
#define	CGABUFSIZE			16384
#define	MAPSIZE				31

#define	Sizeof(stru)		(stru.cb = sizeof(stru))

#define	C0					(COLORS(BLACK,WHITE))
#define	C1					(COLORS(BLUE, CYAN)	| BRIGHT)
#define	C2					(COLORS(GREEN,MAGENTA) | BRIGHT)
#define	C3					(COLORS(CYAN,BLUE) | BRIGHT)
#define	C4					(COLORS(BROWN, GREEN) |	BRIGHT)
#define	C5					(COLORS(WHITE, MAGENTA)	| BRIGHT)
#define	C6					(COLORS(RED, BLUE) | BRIGHT)
#define	C7					(COLORS(BLACK,BLACK))

enum _colors
	{
	BLACK, BLUE, GREEN,	CYAN, RED, MAGENTA,	BROWN, WHITE
	};

typedef	struct _cell
	{
	char character;
	char attribute;
	} CELL;

typedef	struct _frame							/* frame structure			*/
	{
	unsigned	frame_cleared;
	unsigned	row;
	unsigned	col;
	unsigned	threadid;
	long		startSem;
	unsigned char attr;
	char		filler;							/* insures stack on	even byte*/
	char		threadstack[THREADSTACK];
	} FRAME;


	/* Function	Prototypes ***************************/
void hello_thread(FRAME	far	*frameptr);
void box_thread(FRAME far *frameptr);
void keyboard_thread(void);
void main(int argc,	char **argv);
void termination(void);
void initialization(void);
void startup(int argc, char	**argv);
void flicker_frames(void);
void chasing_frames(void);
void WaitForThreadDeath(int	i);
char *adapter(unsigned n);
char *display(unsigned n);
void Background(int	n);
void Screen(int	save);
void Cursor(unsigned hide);
int	 get_user_name(void);
void displayconfig(void);
void togglebackground(void);
void putpixel(int row, int col,	unsigned color);
void clear_graphics_screen(register	unsigned char far *bank1);
void SaveRestoreScreen(void);
void graphics(void);



	/************ end of hello1.h ********************/


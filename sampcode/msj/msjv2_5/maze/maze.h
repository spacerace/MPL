/*

Figure 15
=========

*/

/*
 * DDE MAZE - HEADER FILE
 *
 * LANGUAGE : Microsoft C
 * MODEL		: Small
 * STATUS	: Operational
 *
 * 09/22/87 - Kevin P. Welch - initial creation.
 *
 */
 
#define MAZE_COLS			3
#define MAZE_ROWS			5

#define SC_GRAB_BALL		0x0100
#define SC_GRAB_FOCUS	0x0101

#define HI				HIWORD(lPrm)
#define LO				LOWORD(lPrm)

#define RGB_BLACK			RGB(0x00,0x00,0x00)
#define RGB_WHITE			RGB(0xFF,0xFF,0xFF)

/*
 * BALL DATA STRUCTURE DEFINITIONS
 *
 */

#define BALL_WIDTH	6
#define BALL_HEIGHT	6

#define RANDOM_MOTION	(((rand()%2)*2)-1)

#define H_BOUNCE(a,b)	((a<=0)?1:((a>=b)?-1:Ball.iHorzMotion))
#define V_BOUNCE(a,b)	((a<=0)?1:((a>=b)?-1:Ball.iVertMotion))

#define ADVISE_OFF	(!Link[i].bAdviseBall)

#define OUTSIDE_LEFT	(Ball.rPosn.left<=Link[i].rHole.left)
#define OUTSIDE_RIGHT	(Link[i].rHole.right<=Ball.rPosn.right)
#define OUTSIDE_TOP	(Ball.rPosn.top<=Link[i].rHole.top)
#define OUTSIDE_BOTTOM	(Link[i].rHole.bottom<=Ball.rPosn.bottom)

#define OUTSIDE_WIDTH	((OUTSIDE_LEFT)||(OUTSIDE_RIGHT))
#define OUTSIDE_HEIGHT	((OUTSIDE_TOP)||(OUTSIDE_BOTTOM))

#define OUTSIDE_HOLE	((ADVISE_OFF)||(OUTSIDE_WIDTH)||(OUTSIDE_HEIGHT))

typedef struct {
	RECT		rPosn;		/* current ball position */
	long		lTimeIn;	/* time ball entered maze */
	int		iHorzMotion;	/* vertical ball motion offset */
	int		iVertMotion;	/* horizontal ball motion offset */
	BOOL		bIsBouncing;	/* ball bouncing flag */
} BALL;

/*
 * MAZE DATA STRUCTURE DEFINITIONS
 *
 */

#define DISPLAY_WIDTH			(Maze.wWidth+BALL_WIDTH)
#define DISPLAY_HEIGHT			(Maze.wHeight+BALL_HEIGHT)

typedef struct {
	WORD		wNum;		/* maze window number */
	WORD		wLinks;		/* number of maze links */
	WORD		wWidth;		/* width of maze client area */
	WORD		wHeight;	/* height of maze client area */
	BOOL		bInitiate;	/* in initiate flag */
	BOOL		bGrabBall;	/* grab the ball flag */
	BOOL		bGrabFocus;	/* grab the focus flag */
	WORD		wGoingAway;	/* maze going away counter */
} MAZE;


/*
 * COMMUNICATION LINK DATA STRUCTURE DEFINITIONS
 *
 */

#define MAX_LINK			32

#define HOLE_WIDTH			20
#define HOLE_HEIGHT			14

typedef struct {
	HWND		hWnd;		/* client window handle */
	WORD		wNum;		/* client window number */
	RECT		rHole;		/* client hole position */
	BOOL		bAdviseBall;	/* advise client of ball flag */
	BOOL		bAdviseStat;	/* advise client of stats flag */
} LINK;


/*
 * DYNAMIC DATA EXCHANGE DEFINITIONS
 *
 */
 
#define ACCEPTED		0x8000
#define REJECTED		0x0000

#define WM_DDE_INITIATE 	0x03e0
#define WM_DDE_TERMINATE	0x03e1
#define WM_DDE_ADVISE     	0x03e2
#define WM_DDE_UNADVISE   	0x03e3
#define WM_DDE_ACK        	0x03e4
#define WM_DDE_DATA       	0x03e5
#define WM_DDE_REQUEST   	0x03e6
#define WM_DDE_POKE       	0x03e7
#define WM_DDE_EXECUTE    	0x03e8

#define SEND(a,b,c)		SendMessage(a,b,hWnd,c)
#define POST(a,b,c)		PostMessage(a,b,hWnd,c)

#define DDE_INITIATE(a,b,c)	SEND(a,WM_DDE_INITIATE,MAKELONG(b,c))
#define DDE_TERMINATE(a)	POST(a,WM_DDE_TERMINATE,0L)
#define DDE_ADVISE(a,b)		Advise(a,hWnd,b)
#define DDE_UNADVISE(a,b)	POST(a,WM_DDE_UNADVISE,MAKELONG(0,b))
#define DDE_ACK(a,b,c)		POST(a,WM_DDE_ACK,MAKELONG(b,c))
#define DDE_DATA(a,b,c,d,e)	Transmit(a,hWnd,WM_DDE_DATA,b,c,d,e)
#define DDE_POKE(a,b,c,d,e)	Transmit(a,hWnd,WM_DDE_POKE,b,c,d,e)

typedef struct {
	WORD	fEmpty:12;	/* reserved for future use */
	WORD	fResponse:1;	/* in response to request */
	WORD	fRelease:1;	/* release data */
	WORD	fNoData:1;	/* null data handle ok */
	WORD	fAck:1;		/* Ack expected */
	WORD	cfFormat;	/* clipboard data format */
	BYTE	info[30];	/* data buffer */
} DATA;

typedef DATA *		PDATA;
typedef DATA FAR *	LPDATA;

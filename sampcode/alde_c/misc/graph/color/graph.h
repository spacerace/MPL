/******************************************************************
 *								  *
 *   GRAPH.H							  *
 *								  *
 *   Max R. D�rsteler	  Dec 1983				  *
 *   12405 Village Square Terrace				  *
 *   Rockville Md. 20852					  *
 ******************************************************************
 */

/* Constants */

/* IBM color code table */
/* This table conforms to the colorcodes of the 6845 color register */
/* in 16-color modes. Unfortunately, MSDOS2.0 is using another color code */
/* table if using MSDOS graphics routines. */
#define BLACK	0x0
#define BLUE	0x1
#define GREEN	0x2
#define CYAN	0x3
#define RED	0x4
#define MAGENTA 0x5	 /* MAGENTA */
#define BROWN	0x6
#define WHITE	0x7
#define GRAY	0x8
#define LBLUE	0x9	 /* light blue */
#define LGREEN	0xa
#define LCYAN	0xb
#define LRED	0xc
#define LMAGENTA 0xd
#define YELLOW	0xe
#define LWHITE	0xf	/* high intensity white */

/* Color-select register */
#define COLREG	0x3d9	/* Color-select register I/O adress */
#define EMODREG 0x3dd	/* Extended mode/plane-select register I/O adress */
#define EMRES	0x10	/* Extended medium resolution graphics mode */
#define EHRES	0x20	/* Extended high resolution graphics mode */
#define EALTPLN 0x40	/* Extended mode alternate plane (1: plane 1)*/


/* TEXTMOD, MRESMOD, HRESMOD for use as input to */
/* subroutine screen(mode) to set screen modes using BIOS video-IO */
#define TEXTMOD 3	/* 25x80 color text mode */
#define MRESMOD 4	/* 320x200 color graphics mode */
#define HRESMOD 6	/* 640x320 graphics mode */
#define EMRESMOD 12	/* extended 320x200 color graphics mode */
#define EHRESMOD 14	/* extended 640x200 color graphics mode */

/* Flags for _graphflag */
#define FILLOFF 00
#define FILLON	01

/* Flagcodes for motion directions */
#define XPLUS	001	/* right */
#define XMINUS	002	/* left */
#define YPLUS	004	/* up */
#define YMINUS	010	/* down */

/* Codes for marker symbols */
#define POINT	 0	/* small dot */
#define SQUARE	 1	/* empty square */
#define FSQUARE  2	/* filled square */
#define DIAMOND  3	/* empty diamond */
#define FDIAMOND 4	/* filled diamond */
#define CROSS	 5	/* cross */
#define XCROSS	 6	/* x */
#define CERCLE	 7	/* open circle */
#define FCERCLE  8	/* filled circle */
#define BSQUARE  9	/* big open square */
#define BDIAMOND 10	/* big open diamond */

#define NSYMBOLS 11	/* Number of available symbol types */

/* definitions related to polygon drawing routines */
#define MAXEDGES	50	/* Maximal number of polygon sides */
#define SCANDECR	 1	/* Size of scanline decrement */

/* list of actual polygon edges used by polygon filling routines */
struct EDGE {
	float ymax;		/* upper y coordinate: y-top */
	float ymin;		/* lower y coordinate: y-bottom */
	float xa;		/* x coordinate to ymax: x-top */
	float dx;		/* inverse slope of side */
};

/* screen definitions in pixels */
struct FRAME {
	float heightstart;/* starting column index of screen/frame */
	float widthstart; /* starting row index of screen */
	float heightend;  /* ending column index of screen */
	float widthend;   /* ending row index of screen */
	float height;	  /* heigth of screen/frame in pixels */
	float width;	  /* width of screen/frame in pixels */
	float charwidth;  /* width of current characters */
	float charheight; /* height of current characters */
	float trnx, trny; /* translation parameters */
	float sclx, scly; /* scale parameters */
	float angl;	  /* rotation angle */
};

/* character control variables in screen pixels */
struct ICHARSPEC {
	int icwidth ;	     /* character width & height in pixels */
	int icheight;	     /* (default 8x8 matrix on IBM display */
	int icdirflg;	     /* character direction flag */
};

/* character control variables for inpreter subroutines in pixels */
/* Macros */

/* DOS Version 2.00 screen control sequences, textmode only */
#define CLS "\033[2J\n" /* Erases whole screen, cursor goes home */
#define cls() (printf(CLS))
#define max(x,y)	((x) >= (y) ? (x) : (y))
#define min(x,y)	((x) <= (y) ? (x) : (y))

/* Functions */

/* Assembler subroutines */
extern _screen(), _dot(), clear();
/* Screen initialisation */
extern screen();
/* Integer screen pixel graphics */
extern dot(), doline(), dosymbol(), dochar();
/* Floating point graphics in normalized coordinates */
extern setcolor(), setbackg(), setsymbol(), setfill();
extern setcharsize(), setchardir();
extern moveabs2(), lineabs2(), markabs2(), polyabs2();
extern markrel2(), moverel2(), linerel2(), polyrel2();
extern text();

/* Pixel patterns of marker symbols (1x1 to 5x5 matrix) */
char MARKER[NSYMBOLS][5] = {
	{ '\0', '\0', '\010', '\0', '\0'},              /* small dot */
	{ '\0', '\034', '\024', '\034', '\0'},          /* empty square */
	{ '\0', '\034', '\034', '\034', '\0'},          /* filled square */
	{ '\0', '\010', '\024', '\010', '\0'},          /* empty diamond */
	{ '\0', '\010', '\034', '\010', '\0'},          /* filled diamond */
	{ '\010', '\010', '\076', '\010', '\010'},      /* cross */
	{ '\0', '\024', '\010', '\024', '\0'},          /* x */
	{ '\034', '\042', '\042', '\042', '\034'},      /* open circle */
	{ '\034', '\076', '\076', '\076', '\034'},      /* filled circle */
	{ '\076', '\042', '\042', '\042', '\076'},      /* big open square */
	{ '\010', '\024', '\042', '\024', '\010'}};     /* big open diamond */

/* Graphic status registers /flags */

int _graphmode;       /* currrent graphic mode */
int _graphflag;       /* flag register */
int _colreg;	      /* status of color-select register */
int _color;	      /* current color used */
int _symbol;	      /* current marker symbol */
int _background;      /* current background color */
int _ixold, _iyold;   /* coordinates of last point in pixels */
float _xold, _yold;   /* coordinates of last point in relative coordinates */

struct FRAME display; /* current display parameters (set by screen())*/
/* current display character parameters in pixels */
struct ICHARSPEC icspec = {8, 8, YPLUS};
struct EDGE pol[MAXEDGES] = {0.0};  /* polygon edge parameter list */
double hmatrix[3][3] = {
	{ 1, 0, 0 },
	{ 0, 1, 0 },
	{ 0, 0, 1 }
};

/* End of graphics include file graph.h */
                                                                                                          
/*** ext.h - extension definitions
*
*   Copyright <C> 1988, Microsoft Corporation
*
* Purpose:
*
* NOTES:
*
*   1) THIS FILE IS SHIPPED WITH THE PRODUCT. This note, the file history and
*      all code within "#ifdef EDITOR" conditionals should be REMOVED before
*      shipping.
*
*      ALSO: be VERY carefull what gets put into this file. Technically, if it
*      is NOT required for extension writers, it does NOT belong here.
*
*   2) Certain sections of this file should be removed for delivery with
*      version 1.01.
*
* Revision History:
*
*   16-Jan-1987 mz  Add pascal typing
*		    Exportable switch lists
*   10-Feb-1987 mz  Add fExecute
*   22-Oct-1987 mz  Correct definitions as headers
*   23-Oct-1987 bw  long LINE
*   15-Dec-1987 bw  Add ReadCmd and ReadChar
*		    Add KbHook and KbUnHook
*		    Add toPIF() macro
*   02-Mar-1988 ln  Add Falloc, Fdalloc, mgetenv, GetEditorObject
*		    Add _loadds & _export attribute
*   16-Mar-1988 ln  Add event code
*   25-Mar-1988 ln  Change kbhook/unhook to pre/postspawn
*   28-Apr-1988 ln  Add color interfaces.
*   12-May-1988 ln  Add menu interfaces & definitions.
*   01-Jun-1988 ln  V101 changes
*   21-Jun-1988 ln  Remove V101 changes, as extensions will get everything
*		    Update documentation & order. Removed buffer def.
*   07-Jul-1988 ln  Changes for inclusion in the editor itself.
*   14-Jul-1988 ln  Add rn struct
*   26-Jul-1988 bw  CW: KeyData structure
*   01-Aug-1988 ln  Restored buffer def
*
*************************************************************************/

/*************************************************************************
 *
 *  Macro Definitions
 *
 * BUFLEN is the maximum line length that can be passed or will be returned
 * by the editor.
 */
#define BUFLEN	251
/*
 * EXPORT defines the attributes required for extension functions. _loadds
 * is used such that extensions can be compiled /Aw as well as /Au.
 */
#define EXPORT		_loadds far
#ifdef EDITOR
#define EXTERNAL	far
#else
#define EXTERNAL	_loadds far
#endif
/*
 * RQ_... are various request types supported for Get/Set EditorObject
 */
#define RQ_FILE 	0x1000		/* GetEditorObject: File request*/
#define RQ_FILE_HANDLE	0x1000		/*	File Handle		*/
#define RQ_FILE_NAME	0x1100		/*	ASCIIZ filename 	*/
#define RQ_FILE_FLAGS	0x1200		/*	flags			*/
#define RQ_WIN		0x2000		/* Window request		*/
#define RQ_WIN_HANDLE	0x2000		/*	Window Handle		*/
#define RQ_WIN_CONTENTS 0x2100		/*	Window Contents 	*/
#define RQ_WIN_CUR	0x2200		/*	Current Window Index	*/
#define RQ_COLOR	0x9000		/* Color request		*/
#define RQ_CLIP 	0xf000		/* clipboard type		*/

/*
 * toPif is used when placing numeric or boolean switches in the swiDesc table
 * to eliminate C 5.X compiler warnings.
 *
 * For example: { "Switchname", toPIF(switchvar), SWI_BOOLEAN },
 */
#define toPIF(x)  (PIF)(long)(void far *)&x
/*
 * Editor color table endicies. (Colors 25-35 are unassigned and available for
 * extension use).
 */
#define FGCOLOR 	20		/* foreground (normal) color	*/
#define HGCOLOR 	21		/* highlighted region color	*/
#define INFCOLOR	22		/* information color		*/
#define ERRCOLOR	23		/* error message color		*/
#define STACOLOR	24		/* status line color		*/

/*************************************************************************
 *
 *  General type Definitions
 */
typedef char flagType;			/* Boolean value		*/
typedef int  COL;			/* column or position with line */
typedef long LINE;			/* line number within file	*/
#ifndef EDITOR
typedef unsigned PFILE; 		/* editor file handle		*/
typedef unsigned PWIN;			/* editor window handle 	*/
#endif

typedef char buffer[BUFLEN];		/* line buffer			*/

typedef struct {			/* file location		*/
    LINE    lin;			/* - line number		*/
    COL     col;			/* - column			*/
    } fl;

typedef struct {			/* screen location		*/
    int     lin;			/* - line number		*/
    int     col;			/* - column			*/
    } sl;

typedef struct {			/* file range			*/
    fl	    flFirst;			/* - Lower line, or leftmost col*/
    fl	    flLast;			/* - Higher, or rightmost	*/
    } rn;

struct lineAttr {			/* Line color attribute info	*/
    unsigned char attr; 		/* - Attribute of piece 	*/
    unsigned char len;			/* - Bytes in colored piece	*/
    };

/*************************************************************************
 *
 * Function definition table definitions
 */
#if 0 /* defined(CW) UNDONE: this needs to be resolved! */
typedef unsigned long CMDDATA;
#else
typedef unsigned CMDDATA;
#endif

struct cmdDesc {			/* function definition entry	*/
    char far *name;			/* - pointer to name of fcn	*/
    flagType (pascal EXTERNAL *func)(); /* - pointer to function	*/
    CMDDATA  arg;			/* - used internally by editor	*/
    unsigned argType;			/* - user args allowed		*/
    };
typedef struct cmdDesc far *PCMD;

#ifdef EDITOR
					/* CW Key info structure	*/
typedef struct KeyData {		/* Key information		*/
    unsigned ascii;			/* ASCII code, or .vkey if none */
    unsigned scan;			/* scan code			*/
    unsigned vkey;			/* CW virtual key code		*/
    unsigned shift;			/* Shift state 0 - 3		*/
    PCMD pFunc; 			/* command key will invoke	*/
    char name[30];			/* full name for key		*/
    } KeyData;

typedef unsigned KeyHandle;
#endif

#define NOARG	    0x0001		/* no argument specified	*/
#define TEXTARG     0x0002		/* text specified		*/
#define NULLARG     0x0004		/* arg + no cursor movement	*/
#define NULLEOL     0x0008		/* null arg => text from arg->eol*/
#define NULLEOW     0x0010		/* null arg => text from arg->end word*/
#define LINEARG     0x0020		/* range of entire lines	*/
#define STREAMARG   0x0040		/* from low-to-high, viewed 1-D */
#define BOXARG	    0x0080		/* box delimited by arg, cursor */

#define NUMARG	    0x0100		/* text => delta to y position	*/
#define MARKARG     0x0200		/* text => mark at end of arg	*/

#define BOXSTR	    0x0400		/* single-line box => text	*/

#define MODIFIES    0x1000		/* modifies file		*/
#define KEEPMETA    0x2000		/* do not eat meta flag 	*/
#define WINDOWFUNC  0x4000		/* moves window 		*/
#define CURSORFUNC  0x8000		/* moves cursor 		*/

/*************************************************************************
 *
 * Switch definition table defintions
 */
typedef flagType (pascal EXTERNAL *PIF)(char far *);

union swiAct {				/* switch location or routine	*/
    flagType (pascal EXTERNAL *pFunc)(char far *); /* - routine for text*/
    int far *ival;			/* - integer value for NUMERIC	*/
    flagType far *fval; 		/* - flag value for BOOLEAN	*/
    };

struct swiDesc {			/* switch definition entry	*/
    char far *name;			/* - pointer to name of switch	*/
    union swiAct act;			/* - pointer to value or fcn	*/
    int type;				/* - flags defining switch type */
    };
typedef struct swiDesc far *PSWI;

#define SWI_BOOLEAN 0			/* Boolean switch		*/
#define SWI_NUMERIC 1			/* hex or decimal switch	*/
#define SWI_SCREEN  4			/* switch affects screen	*/
#define SWI_SPECIAL 5			/* textual switch		*/
#define RADIX10 (0x0A << 8)		/* numeric switch is decimal	*/
#define RADIX16 (0x10 << 8)		/* numeric switch is hex	*/

/*************************************************************************
 *
 * Argument defininition structures.
 *
 * We define a structure for each of the argument types that may be passed to
 * an extension function. Then, we define the structure argType which is
 * used to pass these arguments around in a union.
 */
struct	noargType {			/* no argument specified	*/
    LINE    y;				/* - cursor line		*/
    COL     x;				/* - cursor column		*/
    };

struct textargType {			/* text argument specified	*/
    int     cArg;			/* - count of <arg>s pressed	*/
    LINE    y;				/* - cursor line		*/
    COL     x;				/* - cursor column		*/
    char far *pText;			/* - ptr to text of arg 	*/
    };

struct	nullargType {			/* null argument specified	*/
    int     cArg;			/* - count of <arg>s pressed	*/
    LINE    y;				/* - cursor line		*/
    COL     x;				/* - cursor column		*/
    };

struct lineargType {			/* line argument specified	*/
    int     cArg;			/* - count of <arg>s pressed	*/
    LINE yStart;			/* - starting line of range	*/
    LINE yEnd;				/* - ending line of range	*/
    };

struct streamargType {			/* stream argument specified	*/
    int     cArg;			/* - count of <arg>s pressed	*/
    LINE yStart;			/* - starting line of region	*/
    COL  xStart;			/* - starting column of region	*/
    LINE yEnd;				/* - ending line of region	*/
    COL  xEnd;				/* - ending column of region	*/
    };

struct boxargType {			/* box argument specified	*/
    int     cArg;			/* - count of <arg>s pressed	*/
    LINE yTop;				/* - top line of box		*/
    LINE yBottom;			/* - bottom line of bix 	*/
    COL  xLeft; 			/* - left column of box 	*/
    COL  xRight;			/* - right column of box	*/
    };

struct	argType {
    int     argType;
    union   {
	struct	noargType	noarg;
	struct	textargType	textarg;
	struct	nullargType	nullarg;
	struct	lineargType	linearg;
	struct	streamargType	streamarg;
	struct	boxargType	boxarg;
	} arg;
    };
typedef struct argType ARG;

/*************************************************************************
 *
 * Get/Set EditorObject data structures
 */
typedef struct {			/* define window contents	*/
    PFILE	pFile;			/* - handle of file displayed	*/
    sl		slMac;			/* - on-screen limits of window */
    sl		slPos;			/* - position of upper left	*/
    fl		flPos;			/* - upper left corner wrt file */
    } winContents;
/*
 * FILE flags values
 */
#define DIRTY	    0x01		/* file had been modified	*/
#define FAKE	    0x02		/* file is a pseudo file	*/
#define REAL	    0x04		/* file has been read from disk */
#define DOSFILE     0x08		/* file has CR-LF		*/
#define TEMP	    0x10		/* file is a temp file		*/
#define NEW	    0x20		/* file has been created by editor*/
#define REFRESH     0x40		/* file needs to be refreshed	*/
#define READONLY    0x80		/* file may not be editted	*/

#define DISKRO	    0x0100		/* file on disk is read only	*/

/*************************************************************************
 *
 * Event processing definitions
 */
typedef union { 			/* arguments to event dispatches*/
    unsigned int key;			/* - key for key events 	*/
    PFILE	pfile;			/* file handle for file events	*/
    } EVTargs;

struct eventType {			/* event definition struct	*/
    unsigned	evtType;		/* - type			*/
    flagType (pascal EXPORT *func)(EVTargs);	/* - handler		*/
    struct eventType far *pEVTNext;	/* - next handler in list	*/
    PFILE	focus;			/* - applicable focus		*/
    EVTargs	arg;			/* - applicable agruments	*/
    };
typedef struct eventType EVT;

#define EVT_RAWKEY	1		/* ALL keystrokes		*/
#define EVT_KEY 	2		/* Editting keystrokes		*/
#define EVT_GETFOCUS	3		/* file GETs focus.		*/
#define EVT_LOOSEFOCUS	4		/* file looses focus.		*/
#define EVT_EXIT	5		/* about to exit.		*/
#define EVT_SHELL	6		/* about to sell or compile	*/
#define EVT_UNLOAD	7		/* about to be unloaded.	*/
#define EVT_IDLE	8		/* idle event			*/
#define EVT_CANCEL	9		/* do-nothing cancel		*/
#define EVT_REFRESH	10		/* about to refresh a file	*/
/*
** Events handled through OTHER interfaces.
*/
#define EVT_MENU	100		/* menu about to be viewed	*/
#define EVT_MENUSELECT	101		/* menu selected		*/

/*************************************************************************
 *
 * CW Menu, window and dialog definitions
 *
 * action constants for ChangeMenu
 */
#define MNU_DISABLE	1		/* disable (grey) menu item	*/
#define MNU_ENABLE	2		/* enable menu item		*/
#define MNU_DELETE	3		/* delete menu item		*/
#define MNU_RENAME	4		/* rename menu item		*/
#define MNU_ACCEL	5		/* define menu item accelerator */
/*
 * Message box types.
 */
#define MBOX_OK 		1	/* <OK> 			*/
#define MBOX_YESNOCANCEL	2	/* <YES> <NO> <CANCEL>		*/
#define MBOX_RETRYCANCEL	3	/* <RETRY> <CANCEL>		*/
#define MBOX_OKCANCEL		4	/* <OK> <CANCEL>		*/
#define MBOX_ABORT		5	/* <ABORT>			*/
#define MBOX_YESNO		6	/* <YES> <NO>			*/
#define MBOX_RETRY		7	/* <RETRY>			*/
#define MBOX_TYPE		0x0f	/* message type 		*/
#define MBOX_BEEP		0x10	/* beep when displayed		*/
#define MBOX_CAPTION		0x20	/* 1st param is caption 	*/

/*************************************************************************
 *
 * Editor lowlevel function prototypes.
 *
 * This list defines the routines within the editor which may be called
 * by extension functions.
 */
#ifndef EDITOR
flagType    pascal Replace	    (char, COL, LINE, PFILE, flagType);
void	    pascal MoveCur	    (COL, LINE);
void	    pascal DelLine	    (PFILE, LINE, LINE);
void	    pascal DelBox	    (PFILE, COL, LINE, COL, LINE);
void	    pascal DelStream	    (PFILE, COL, LINE, COL, LINE);
PFILE	    pascal AddFile	    (char far *);
void	    pascal DelFile	    (PFILE);
flagType    pascal RemoveFile	    (PFILE);
void	    pascal CopyBox	    (PFILE, PFILE, COL, LINE, COL, LINE, COL, LINE);
void	    pascal CopyLine	    (PFILE, PFILE, LINE, LINE, LINE);
void	    pascal CopyStream	    (PFILE, PFILE, COL, LINE, COL, LINE, COL, LINE);
void	    pascal pFileToTop	    (PFILE);
void	    pascal Display	    (void);
void	    pascal GetCursor	    (COL far *, LINE far *);
LINE	    pascal FileLength	    (PFILE);
int	    pascal GetLine	    (LINE, char far *, PFILE);
PFILE	    pascal FileNameToHandle (char far *, char far *);
flagType    pascal FileRead	    (char far *, PFILE);
flagType    pascal FileWrite	    (char far *, PFILE);
flagType    pascal SetKey	    (char far *, char far *);
int	    pascal DoMessage	    (char far *);
void	    pascal PutLine	    (LINE, char far *, PFILE);
flagType    pascal BadArg	    (void);
flagType    pascal fExecute	    (char far *);
PCMD	    pascal ReadCmd	    (void);
long	    pascal ReadChar	    (void);
void	    pascal KbHook	    (void);
void	    pascal KbUnHook	    (void);
char far *  pascal Falloc	    (long);
void	    pascal Fdalloc	    (char far *);
char far *  pascal mgetenv	    (char far *);
flagType    pascal GetEditorObject  (unsigned, unsigned, void far *);
flagType    pascal SetEditorObject  (unsigned, unsigned, void far *);
flagType    pascal DeclareEvent     (unsigned, EVTargs);
void	    pascal RegisterEvent    (EVT *);
void	    pascal DeRegisterEvent  (EVT *);
PSWI	    pascal FindSwitch	    (char far *);
int	    pascal search	    (PFILE, flagType, flagType, char far *, COL far *, LINE far *);
int	    pascal REsearch	    (PFILE, flagType, flagType, char far *, COL far *, LINE far *);
char far *  pascal NameToKeys	    (char far *, char far *);
flagType    pascal GetColor	    (LINE, struct lineAttr far *, PFILE);
void	    pascal PutColor	    (LINE, struct lineAttr far *, PFILE);
int	    pascal AddMenu	    (char far *, char far *, void (pascal EXPORT *)(), flagType);
int	    pascal AddMenuItem	    (int, char far *, char far *);
flagType    pascal ChangeMenu	    (int, int, char far *);
int	    pascal MessageBox	    (char far *, char far *, char far *, int);
#endif

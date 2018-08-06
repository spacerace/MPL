/*  ds.h  - definitions for DS */

#include    <doscalls.h>
#include    <subcalls.h>

#define     FALSE   0
#define     TRUE    1

#define     VioHandle	0

#define     MAX_IMAGE_ROWS	200
#define     MAX_COLS		 80

#define     FILE_NAME_LEN	 13
#define     MAX_PATH_LEN	(64+FILE_NAME_LEN)

#define     DosAttrReadOnly	0x01
#define     DosAttrHidden	0x02
#define     DosAttrSystem	0x04
#define     DosAttrLabel	0x08
#define     DosAttrSubDir	0x10
#define     DosAttrArchive	0x20

#define     DosAttrAll		0x3F

#define     DosErrorFileNotFound    2
#define     DosErrorNoMoreFiles    18

#define     D_INDENT	FILE_NAME_LEN
#define     RT_CHAR	0xC8
#define     LT_CHAR	0xBC
#define     BR_CHAR	0xC9
#define     BL_CHAR	0xBB
#define     BT_CHAR	0xBA
#define     LR_CHAR	0xCD
#define     BRT_CHAR	0xCC

#define     optKey	0x0054
#define     helpKey	0x003B
#define     enterKey	0x0D00
#define     upKey	0x0048
#define     pgUpKey	0x0049
#define     downKey	0x0050
#define     pgDownKey	0x0051
#define     exitKey	0x0043
#define     leftKey	0x004B
#define     rightKey	0x004D

#define     WINDOW_TOP	    2		/* Start tree window on third row */
#define     WINDOW_LEFT     0		/* Start tree window at left column */

#define     titleC	    0
#define     nameC	    1
#define     cursorC	    2
#define     barC	    3
#define     statusC	    4
#define     popupC	    5
#define     errorC	    6
#define     blankC	    7

#define     N_COLORS	    8

#define     INIT_SYMBOL 	"USER"
#define     INIT_FILE		"DS.INI"
#define     INIT_FILE_HEADER	"DS Initialization File - Ben Slivka\n"


typedef struct FileFindBuf FileSearch;


#define     SB_LENGTH	43


typedef struct DirStruct {
    char	     d_name[FILE_NAME_LEN]; /* Directory name */
    int 	     d_row;		/* Row to display name */
    int 	     d_col;		/* Column to display name */
    struct DirStruct *d_parent; 	/* Parent directory at higher level */
    struct DirStruct *d_prev;		/* Previous directory at same level */
    struct DirStruct *d_next;		/* Next directory at same level */
    struct DirStruct *d_child;		/* First directory at lower level */
    struct DirStruct *d_link;		/* Breadth-first link */
} Directory;

typedef struct {
    unsigned char    theChar;
    unsigned char    theAttr;
} cellType;

typedef struct {
    unsigned a_fore   : 3;
    unsigned a_bright : 1;
    unsigned a_back   : 3;
    unsigned a_blink  : 1;
} AttrStruct;


/*  Stuff from VIO.H */

typedef struct {
    unsigned char   ch; 	/* character */
    unsigned char   at; 	/* attribute */
} Cell;

typedef unsigned char Attr;
/*
#define     cefs(a) (Cell far *)(a)
*/
#define     cefs(a) (char far *)(a) /* Bogus for subcalls.h */

#define     afs(a) (Attr far *)(a)
#define     chfs(a) (char far *)(a)
#define     ifs(a) (int far *)(a)
#define     ufs(a) (unsigned int far *)(a)

/***	Attribute definitions	*/

#define     black	0
#define     blue	1
#define     green	2
#define     cyan	3
#define     red 	4
#define     magenta	5
#define     yellow	6
#define     white	7
#define     bright	8
#define     blinking	8

/***	attr - compute a character attribute
*
*	ENTRY:	f = foreground attribute
*		b = backround attribute
*
*	EXIT:	returns combined attribute
*/

#define     attr(f,b)  ((b)<<4) + (f)

/*  option.c - Display option screen */

#include "ds.h"
#include "vars.h"

extern decorateTree (Directory *);


char optScreen[][63] = {
   /* 123456789 123456789 123456789 123456789 123456789 123456789 1 */
    "ษอออออออออออออออออออออออ[ Set Colors ]อออออออออออออออออออออออป", /*  0 */
    "บ                                                            บ", /*  1 */
    "บ            Fore-     Back-                                 บ", /*  2 */
    "บ            ground    ground   Bright?  Blink?  Sample      บ", /*  3 */
    "บ          ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ   บ", /*  4 */
    "บ   title  ณ xxxxxxx   xxxxxxx    yes     yes    DosShell    บ", /*  5 */
    "บ   name   ณ                                     filename    บ", /*  6 */
    "บ   cursor ณ                                     filename    บ", /*  7 */
    "บ   bar    ณ                                     ศอออออออ    บ", /*  8 */
    "บ   status ณ                                     Info        บ", /*  9 */
    "บ   popup  ณ                                     screens     บ", /* 10 */
    "บ   error  ณ                                     Danger      บ", /* 11 */
    "บ   blank  ณ                                     <      >    บ", /* 12 */
    "บ                                                            บ", /* 13 */
    "บ   Use cursor keys to move between fields.                  บ", /* 14 */
    "บ   Use PgUp and PgDn to change a field.                     บ", /* 15 */
    "บ                                                            บ", /* 16 */
    "ศออออออออออออออออออ[ Press Enter to return ]อออออออออออออออออผ"  /* 17 */
};

#define     O_WIDTH	62
#define     O_HEIGHT	18

int O_ROW;  /* ((N_of_Rows - O_HEIGHT)/2) */
int O_COL;  /* ((N_of_Cols - O_WIDTH)/2) */

int  O_Start_Row;   /*	(O_ROW + 5)	*/
int  O_FORE_COL;    /*	(O_COL + 13)	*/
int  O_BACK_COL;    /*	(O_COL + 23)	*/
int  O_BRIGHT_COL;  /*	(O_COL + 34)	*/
int  O_BLINK_COL;   /*	(O_COL + 42)	*/
int  O_SAMPLE_COL;  /*	(O_COL + 49)	*/

#define     SAMPLE_LEN	    8

#define     CUR_COLS	    4
int curCol[CUR_COLS];			/* Cursor columns for color screen */

#define     C_NAME_LEN	    7
#define     C_NAMES	    8
char cName[C_NAMES][C_NAME_LEN+1] = {
    "black ",	/* 0  000 */
    "blue  ",	/* 1  001 */
    "green ",	/* 2  010 */
    "cyan  ",	/* 3  011 */
    "red   ",	/* 4  100 */
    "purple",	/* 5  101 */
    "yellow",	/* 6  110 */
    "white "	/* 7  111 */
};

#define     TOGGLE_LEN	    3
#define     T_NAMES	    8
char tName[T_NAMES][TOGGLE_LEN+1] = {
    " no",	/* 0 */
    "yes"	/* 1 */
};


int color[N_COLORS] = {
    attr(green,black),			/* titleC  */
    attr(cyan,black),			/* nameC   */
    attr(bright+yellow,black),		/* cursorC */
    attr(red,black),			/* barC    */
    attr(green,black),			/* statusC */
    attr(blue,white),			/* popupC  */
    attr(red,black+blinking),		/* errorC  */
    attr(white,black)			/* blankC  */
};


/***	optionInit - Initialize option screen stuff
*
*
*/
optionInit ()
{
    O_ROW = (N_of_Rows - O_HEIGHT)/2;
    O_COL = (N_of_Cols - O_WIDTH)/2;
    O_Start_Row  =  (O_ROW + 5);
    O_FORE_COL	 =  (O_COL + 13);
    O_BACK_COL	 =  (O_COL + 23);
    O_BRIGHT_COL =  (O_COL + 34);
    O_BLINK_COL  =  (O_COL + 42);
    O_SAMPLE_COL =  (O_COL + 49);

    curCol[0] = O_FORE_COL;
    curCol[1] = O_BACK_COL;
    curCol[2] = O_BRIGHT_COL;
    curCol[3] = O_BLINK_COL;

}   /* optionInit */


/***	showOption - display option screen and record user modifications
*
*
*/
showOption ()
{
    int  row,col,key;
    Attr attrib, fore, back, brite, blink;
    int  cRow,cCol;
    Attr oldColor[N_COLORS];
    int  i;
    Attr oldPopupC;
    Attr a;
    unsigned colorChanged;

    oldPopupC = color[popupC];		    /* Use old blank color */
    for (i=0; i<N_COLORS; i++)		    /* Remember old colors */
	oldColor[i] = color[i];

    a = oldPopupC;
    for (row=0; row<O_HEIGHT; row++)
	VIOWRTCHARSTRATT (chfs(optScreen[row]), O_WIDTH, row+O_ROW,O_COL,
			  afs(&a), VioHandle);

    for (row=0; row<N_COLORS; row++)
	showRow (row, oldPopupC);

    key = 0;
    cRow = 0;
    cCol = 0;
    while (key != enterKey) {
	VIOSETCURPOS (O_Start_Row+cRow,curCol[cCol]-1, VioHandle);
	key = getKey();
	switch (key) {
	    case upKey:
		if (!cRow--)
		    cRow = N_COLORS - 1;
		break;
	    case downKey:
		if (cRow++ >= (N_COLORS-1))
		    cRow = 0;
		break;
	    case leftKey:
		if (!cCol--)
		    cCol = CUR_COLS - 1;
		break;
	    case rightKey:
		if (cCol++ >= (CUR_COLS-1))
		    cCol = 0;
		break;
	    case pgUpKey:
		modColor (cRow,cCol, 1, oldPopupC);
		break;
	    case pgDownKey:
		modColor (cRow,cCol,-1, oldPopupC);
		break;
	    default:
		break;
	}   /* switch */
    }	/* while */

/* See if colors changed in the tree */

    colorChanged = FALSE;		    /* No color change (yet) */
    if (color[nameC] != oldColor[nameC])
	colorChanged = TRUE;
    if (color[barC] != oldColor[barC])
	colorChanged = TRUE;
    if (color[blankC] != oldColor[blankC])
	colorChanged = TRUE;

    if (colorChanged)			/* Tree color changed, must re-image */
	decorateTree (root);

/* See if any colors changed */

    colorChanged = 0;			/* No color change (yet) */
    for (i=0; i<N_COLORS; i++)		/* See if ANY colors changed */
	colorChanged += ((oldColor[i] == color[i]) ? 0 : 1);

    if (colorChanged)			/* At least one color changed */
	stateModified = TRUE;

}   /* showOption */


/***	modColor - Modify color on option screen
*
*
*/
modColor (row,col,inc,oldPopupC)
int row,col,inc,oldPopupC;
{
    int attrib, fore, back, brite, blink;

    attrToParts (color[row], &fore, &back, &brite, &blink);
    if (inc > 0)		    /* Increment value */
	 switch (col) {
	    case 0:
		if (fore++ > (C_NAMES-1))
		    fore = 0;
		break;
	    case 1:
		if (back++ > (C_NAMES-1))
		    back = 0;
		break;
	    case 2:
		if (brite++ > (T_NAMES-1))
		    brite = 0;
		break;
	    case 3:
		if (blink++ > (T_NAMES-1))
		    blink = 0;
		break;
	}   /* case */
    else
	 switch (col) {
	    case 0:
		if (!fore--)
		    fore = C_NAMES-1;
		break;
	    case 1:
		if (!back--)
		    back = C_NAMES-1;
		break;
	    case 2:
		if (!brite--)
		    brite = T_NAMES-1;
		break;
	    case 3:
		if (!blink--)
		    blink = T_NAMES-1;
		break;
	};  /* case */
    partsToAttr (&color[row], fore, back, brite, blink);
    showRow (row, oldPopupC);
}   /* modColor */


/***	showRow - display a row of the option screen
*
*
*/
showRow (row,oldPopupC)
int row;
int oldPopupC;
{
    Attr attrib, fore, back, brite, blink;
    Attr a;

    a = oldPopupC;

    attrToParts (color[row], &fore, &back, &brite, &blink);

    VIOWRTCHARSTRATT (chfs(cName[fore]), C_NAME_LEN,
			row+O_Start_Row,O_FORE_COL, afs(&a), VioHandle);

    VIOWRTCHARSTRATT (chfs(cName[back]), C_NAME_LEN,
			row+O_Start_Row,O_BACK_COL, afs(&a), VioHandle);

    VIOWRTCHARSTRATT (chfs(tName[brite]), TOGGLE_LEN,
			row+O_Start_Row,O_BRIGHT_COL, afs(&a), VioHandle);

    VIOWRTCHARSTRATT (chfs(tName[blink]), TOGGLE_LEN,
			row+O_Start_Row,O_BLINK_COL, afs(&a), VioHandle);

    a = color[row];
    VIOWRTNATTR (afs(&a), SAMPLE_LEN, row+O_Start_Row,O_SAMPLE_COL, VioHandle);

}   /* showRow */


/***	attrToParts - Break cell attribute into component parts
*
*
*/
attrToParts (attrib, fore, back, brite, blink)
AttrStruct attrib;
int *fore, *back, *brite, *blink;
{
    *fore  = attrib.a_fore;
    *brite = attrib.a_bright;
    *back  = attrib.a_back;
    *blink = attrib.a_blink;
}   /* attrToParts */


/***	partsToAttr - Build cell attribute from component parts
*
*
*/
partsToAttr (attrib, fore, back, brite, blink)
AttrStruct *attrib;
int fore, back, brite, blink;
{
    attrib->a_fore   = fore;
    attrib->a_bright = brite;
    attrib->a_back   = back;
    attrib->a_blink  = blink;
}   /* partsToAttr */

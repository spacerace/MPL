/*  help.c - Show help screen */

#include "ds.h"
#include "vars.h"


char helpScreen[][63] = {
   /* 123456789 123456789 123456789 123456789 123456789 123456789 1 */
    "ษอออออออออออออหอออออออออออออ[ Help ]อออออออออออออหอออออออออออป", /*  0 */
    "บ 21 Aug 1986 บ      Display Subdirectories      บ Ver 0.030 บ", /*  1 */
    "ฬอออออออออออออสออออออออออออออออออออออออออออออออออสอออออออออออน", /*  2 */
    "บ                        Cursor Keys                         บ", /*  3 */
    "บ                                                            บ", /*  4 */
    "บ     up - move to upper sibling     left - move to parent   บ", /*  5 */
    "บ   down - move to lower sibling    right - move to child    บ", /*  6 */
    "บ                                                            บ", /*  7 */
    "บ   Shift F1:  Set Colors                                    บ", /*  8 */
    "บ         F9:  Exit                                          บ", /*  9 */
    "บ      Enter:  Spawn shell at current directory              บ", /* 10 */
    "บ                                                            บ", /* 11 */
    "ศอออออออออออออออออ[ Press any key to return ]ออออออออออออออออผ"  /* 12 */
};

#define     H_WIDTH	62
#define     H_HEIGHT	13

int H_ROW;  /* (N_of_Rows - H_HEIGHT)/2 */
int H_COL;  /* (N_of_Cols - H_WIDTH)/2 */


/***	helpInit - initialize stuff for the help screen
*
*
*/
helpInit ()
{
    H_ROW = (N_of_Rows - H_HEIGHT)/2;
    H_COL = (N_of_Cols - H_WIDTH)/2;
}   /* helpInit */


/***	showHelp - display the help screen
*
*
*/
showHelp ()
{
    int row,col;
    Attr a;

    a = color[popupC];

    for (row=0; row<H_HEIGHT; row++)
	VIOWRTCHARSTRATT (chfs(helpScreen[row]), H_WIDTH, row+H_ROW,H_COL,
			  afs(&a), VioHandle);
    getch();			    /* Pause */
}   /* showHelp */

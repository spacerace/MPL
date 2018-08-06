/*  help.c - Show help screen */

#include "ds.h"
#include "vars.h"


char helpScreen[][63] = {
   /* 123456789 123456789 123456789 123456789 123456789 123456789 1 */
    "����������������������������[ Help ]������������������������ͻ", /*  0 */
    "� 21 Aug 1986 �      Display Subdirectories      � Ver 0.030 �", /*  1 */
    "������������������������������������������������������������͹", /*  2 */
    "�                        Cursor Keys                         �", /*  3 */
    "�                                                            �", /*  4 */
    "�     up - move to upper sibling     left - move to parent   �", /*  5 */
    "�   down - move to lower sibling    right - move to child    �", /*  6 */
    "�                                                            �", /*  7 */
    "�   Shift F1:  Set Colors                                    �", /*  8 */
    "�         F9:  Exit                                          �", /*  9 */
    "�      Enter:  Spawn shell at current directory              �", /* 10 */
    "�                                                            �", /* 11 */
    "������������������[ Press any key to return ]���������������ͼ"  /* 12 */
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

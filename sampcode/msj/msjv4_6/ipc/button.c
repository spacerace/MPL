/* button.c RHS 7/15/89
 *
 * text-button functions
 */

#define	INCL_SUB
#include<os2.h>
#include<string.h>
#include"button.h"

#define	VIOHDL				0

extern BUTTON buttonlist[];


void InitButtons(void)
	{
	BUTTON *b = buttonlist;

	for( ; b->text; b++)
		ButtonInit(b);
	}


void ButtonInit(BUTTON *b)
	{
	b->endrow = (b->startrow+2);                 /* startrow+#of ptrs-1     */
												            /* startcol+strlen of text-1*/
	b->endcol = (b->startcol+strlen(b->text)+1);
	}


void ResetButtons(void)
	{
	BUTTON *b = buttonlist;

	for( ; b->text; b++)
		{
		b->state = 0;
		ButtonPaint(b,b->attribute);
		}
	}

void findbutton(char *text,BUTTON **bptr)
	{
	BUTTON *b = buttonlist;

	for( ; b->text; b++)
		if(!strncmp(b->text,text,strlen(text)))
			{
			*bptr = b;
			return;
			}
	}

void DisplayButtons(void)
	{
	BUTTON *b = buttonlist;

	for( ; b->text; b++)
		ButtonDisplay(b);
	}

void ButtonDisplay(BUTTON *b)
	{
	BYTE	cell[2];
	USHORT	row = b->startrow;
	USHORT	endcol = b->endcol;
	USHORT	startcol = b->startcol;
	char	*text = b->text;

	USHORT	len = endcol - startcol - 1;

	cell[0] = 'Í';
	cell[1] = b->attribute;
												            /* write the 1st corner char*/
	VioWrtCharStrAtt("Õ",1,row,startcol,&cell[1],VIOHDL);
												            /* write the top line      */
	VioWrtNCell(cell,len,row,startcol+1,VIOHDL);
                                                /* write the 2nd corner char*/
	VioWrtCharStrAtt("¸",1,row,endcol,&cell[1],VIOHDL);
            												/* write the left border   */
	VioWrtCharStrAtt("³",1,++row,startcol,&cell[1],VIOHDL);
				              								/* write the message       */
	VioWrtCharStrAtt(text,strlen(text),row,startcol+1,&cell[1],VIOHDL);
								              				/* write the right border  */
	VioWrtCharStrAtt("³",1,row,endcol,&cell[1],VIOHDL);
            												/* write the 3rd corner    */
	VioWrtCharStrAtt("Ô",1,++row,startcol,&cell[1],VIOHDL);
				            								/* write the bottom line   */
	VioWrtNCell(cell,len,row,startcol+1,VIOHDL);
								            				/* write the 4th corner    */
	VioWrtCharStrAtt("¾",1,row,endcol,&cell[1],VIOHDL);
	}


void ButtonPaint(BUTTON *b, BYTE attribute)
	{
	USHORT	row = b->startrow;
	USHORT	col = b->startcol;
	USHORT	endrow = b->endrow;
	USHORT	num = b->endcol-col+1;

	for( ; row <= endrow; row++)
		VioWrtNAttr(&attribute,num,row,col,VIOHDL);
	}




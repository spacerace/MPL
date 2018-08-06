/* button.c RHS
 *
 * text-button functions
 */

#define    INCL_SUB
#include<os2.h>
#include<string.h>
#include"button.h"

#define    VIOHDL                0

extern BUTTON buttonlist[];


void InitButtons(void)
    {
    BUTTON *b = buttonlist;

    for( ; b->text; b++)
        ButtonInit(b);
    }


void ButtonInit(BUTTON *b)
    {
    b->endrow = (b->startrow+2);        // startrow+#of ptrs-1     
                                        // startcol+strlen of text-1
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
    BYTE      cell[2],cell2[2];
    USHORT    row = b->startrow;
    USHORT    endcol = b->endcol;
    USHORT    startcol = b->startcol;
    char      *text = b->title;

    USHORT    len = endcol - startcol - 1;

    cell[0] = 205;                      // line character
    cell[1] = b->attribute;
    cell2[1] = '\0';
                                        // write the 1st corner char
    cell2[0] = 201;
    VioWrtCharStrAtt(cell2,1,row,startcol,&cell[1],VIOHDL); 
                                        // write the top line
    VioWrtNCell(cell,len,row,startcol+1,VIOHDL);
                                        // write the 2nd corner char
    cell2[0] = 187;
    VioWrtCharStrAtt(cell2,1,row,endcol,&cell[1],VIOHDL);

    if(*text)                           // if title, write it
        VioWrtCharStrAtt(text,strlen(text),row,startcol+2,&cell[1],
            VIOHDL);
                                        // write the left border
    cell2[0] = 186;
    VioWrtCharStrAtt(cell2,1,++row,startcol,&cell[1],VIOHDL);
    text = b->text;                     // reset pointer
                                        //
                                        // write the message
    VioWrtCharStrAtt(text,strlen(text),row,startcol+1,&cell[1],
        VIOHDL);
                                        // write the right border  
    VioWrtCharStrAtt(cell2,1,row,endcol,&cell[1],VIOHDL);
                                        // write the 3rd corner    
    cell2[0] = 200;
    VioWrtCharStrAtt(cell2,1,++row,startcol,&cell[1],VIOHDL);
                                        // write the bottom line   
    VioWrtNCell(cell,len,row,startcol+1,VIOHDL);
                                        // write the 4th corner    
    cell2[0] = 188;
    VioWrtCharStrAtt(cell2,1,row,endcol,&cell[1],VIOHDL);
    }


void ButtonPaint(BUTTON *b, BYTE attribute)
    {
    USHORT    row = b->startrow;
    USHORT    col = b->startcol;
    USHORT    endrow = b->endrow;
    USHORT    num = b->endcol-col+1;

    for( ; row <= endrow; row++)
        VioWrtNAttr(&attribute,num,row,col,VIOHDL);
    }


void ButtonRead(BUTTON *b,char *contents)
    {
    int len = b->endcol - b->startcol - 1;

    VioReadCharStr((PCH)contents,&len,b->startrow+1,b->startcol+1,
        VIOHDL);
    contents[len] = '\0';
    for(len--; len >= 0 && contents[len]==' '; len--);
    len++;
    contents[len] = '\0';
    }

BUTTON *IsButton(USHORT row, USHORT col)
    {
    BUTTON *b = buttonlist;

    for( ; b->text; b++)
        if(row >= b->startrow
            && row <= b->endrow
            && col >= b->startcol
            && col <= b->endcol)
            return b;
    return NULL;
    }


/* WPRO2.C - From page 362 of "Microsoft C Programming for      */
/* the IBM" by Robert Lafore. This adds some more features to   */
/* the WPRO1.C simple word processor on this disk. Hit Alt-     */
/* u to toggle underline on and off. I changed the switch       */
/* statement to enable the right arrow key when cursor is       */
/* at the last column.                                          */
/****************************************************************/

#include "dos.h"           /* for REGS definition */
#define TRUE 1
#define COMAX 80           /* max number of columns */
#define R_ARRO 77
#define L_ARRO 75
#define BK_SPC 8
#define ALT_U 22           /* [Alt] and [u] keys */
#define VIDEO 0x10         /* video ROM BIOS service */
#define NORM 0x07          /* normal attribute */
#define UNDR 0x01          /* underline attribute */
int col = 0;               /* cursor position */
int length = 0;            /* length of phrase */
int far *farptr;           /* pointer to screen memory */
union REGS regs;           /* for ROM BIOS calls */

main()
{
char ch, attr = NORM;

   farptr = (int far *) 0xB0000000;    /*start of screen mem*/
   clear();
   cursor();
   while(TRUE) {
      if ((ch = getch()) == 0) {       /*if extended code key*/
         ch = getch();                 /*read extended code*/
         switch(ch) {
            case R_ARRO: {
               if (col < length) {
                  ++col;
                  break;
               }
               else if (col >= length) {
                  ++col;
                  ++length;
                  break;
               }
            }
            case L_ARRO:
               if (col > 0)
                  --col;
               break;
            case ALT_U:
               attr = (attr == NORM) ? UNDR : NORM;
         }
      }
      else
         switch(ch) {
            case BK_SPC:
               if (length > 0)
                  delete();
               break;
            default:
               if (length < COMAX)
                  insert(ch, attr);
         }
      cursor();
   }                    /* end while */
}                       /* end main */

/* cursor() */    /* move cursor to row = 0, col */
cursor()
{
regs.h.ah = 2;       /* 'set cursor pos' service */
regs.h.dl = col;     /* column varies */
regs.h.dh = 0;       /* always top row */
regs.h.bh = 0;       /* page zero */

   int86(VIDEO, &regs, &regs);     /* call video interrupt */
}

/* insert() */    /*inserts cursor at cursor position */
insert(ch, attr)
char ch, attr;
{
int j;

   for (j = length; j > col; j--)         /* shift chars right */
      *(farptr + j) = *(farptr + j -1);   /*     to make room */
   *(farptr + col) = ch | attr << 8;      /* insert char */
   ++length;                              /* increment count */
   ++col;                                 /* move cursor right */
}

/* delete() */    /* deletes char at pos 1 left of cursor */
delete()
{
int j;

   for(j = col; j <= length; j++)         /* shift chars left */
      *(farptr + j - 1) = *(farptr + j);
   --length;                              /* decrement count */
   --col;                                 /* move cursor left */
}

/* clear() */  /*clears screen by inserting 0 at every location */
clear()
{
int j;

   for(j = 0; j < 2000; j++)
      *(farptr + j) = 0x0700;    /* zeros with attr = 07 */
}


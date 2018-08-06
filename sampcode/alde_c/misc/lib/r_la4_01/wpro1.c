/* WPRO1.C - From page 355 of "Microsoft C Programming for      */
/* the IBM" by Robert Lafore. This is a rudimentary one line    */
/* word processor.                                              */
/****************************************************************/

#include "dos.h"           /*for REGS definition*/
#define TRUE 1
#define COMAX 80           /*max num of cols*/
#define R_ARRO 77
#define L_ARRO 75
#define VIDEO 0x10         /*video ROM BIOS service*/
int col = 0;               /*cursor position*/
int far *farptr;
union REGS regs;           /*for ROM BIOS calls*/

main()
{
char ch;

   farptr = (int far *) 0xB0000000;    /*start of screen memory*/
   clear();
   cursor();                           /*position cursor*/
   while(TRUE) {
      if ((ch = getch()) == 0) {
         ch = getch();                 /*read extended code*/
         switch(ch) {
            case R_ARRO:
               if (col < COMAX)
                  ++col;
               break;
            case L_ARRO:
               if (col > 0)
                  --col;
               break;
         }
      }
      else                             /*not extended code*/
         if (col < COMAX)
            insert(ch);                /*print char at col*/
      cursor();                        /*reset cursor*/
   }
}

/* cursor */  /*move cursor to row = 0, col */
cursor()
{

   regs.h.ah = 2;                /* 'set cursor position service */
   regs.h.dl = col;              /*column varies*/
   regs.h.dh = 0;                /*always top row*/
   regs.h.bh = 0;                /*page zero*/
   int86(VIDEO, &regs, &regs);   /*call video int*/
}

/* insert() */  /*inserts char at cursor position*/
insert(ch)
char ch;
{
int j;

   *(farptr + col) = ch | 0x700;       /*insert char*/
   ++col;                              /*move cursor right*/
}

/* clear */      /*clears screen using direct memory access*/
clear()
{
int j;

   for(j = 0; j < 2000; j++)           /*fill screen mem with*/
      *(farptr + j) = 0x0700;          /*zeros   (attrib=07) */
}


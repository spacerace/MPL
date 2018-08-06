/*      Small functions which use DISPIO BIOS calls and include file       */
/* DISPIO.H.                                                               */
/*                                                                         */
/* set_dfull()                  - Set up display parameters except         */
/*                                the attribute (d_attr).                  */
/*                                                                         */
/* d_tomono()                   - Change monitor to mono                   */
/* d_toc80()                    - Change monitor to 80 column color        */
/* d_toc40()                    - Change monitor to 40 column color        */
/*                                                                         */
/* i = d_newmode(newmode)       - Change mode to newmode.  i = 1 if mode   */
/*                                newmode is valid for this monitor, 0     */
/*                                otherwise.                               */
/*                                                                         */
/* i = d_newpage(newpage)       - Change active page to newpage.  i = 1    */
/*                                if newpage is valid for this mode, 0     */
/*                                otherwise.                               */
/*                                                                         */
/* clrscr()                     - Clear the current window.  Leave the     */
/*                                cursor in the upper left hand corner.    */
/*                                                                         */
/* d_erasel()                   - Erase current line.  Leave cursor at its */
/*                                start.                                   */
/*                                                                         */
/* d_ereol()                    - Erase from the cursor to the end of the  */
/*                                current line.  Do not move the cursor.   */
/*                                                                         */
/* d_nextc()                    - Move the cursor to the next position.    */
/*                                If at the end of the window, scroll up   */
/*                                one line and move to the lower left      */
/*                                corner.                                  */
/*                                                                         */
/* d_nextl()                    - Move the cursor to the next line.  If    */
/*                                at the end of the window, scroll up one  */
/*                                line and move to the lower left corner.  */
/*                                                                         */
/* d_putc(ch)                   - Write a single character at the cursor   */
/*                                position and move the cursore to the     */
/*                                next position.  If at the end of the     */
/*                                window, scroll up one line and move to   */
/*                                the lower left hand corner.              */
/*                                                                         */
/* i = d_puts(s)                - Write the string s, starting at the      */
/*                                cursor position.  If the window would    */
/*                                overflow, scroll it up enough lines to   */
/*                                fit.  The cursor ends one position       */
/*                                after the end of the string.  The        */
/*                                function returns the number of           */
/*                                characters written.                      */
/*                                                                         */
/*      Draw vertical lines                                                */
/*      If length > 0, draw down, but not beyond the bottom of the screen  */
/*      If length < 0, draw up, but not beyond the top of the screen       */
/*      The functions return the number of characters written, 0 for       */
/* error.  They leave the cursor one beyone the end of the line drawn.     */
/* verline(ch, length)          - draw a vertical line of length length    */
/*                                starting at the current cursor position  */
/* vline(row, col, ch, length) - draw a vertical line of length length     */
/*                                characters ch, starting at row, col.     */
/*                                Checks row and col for validity.         */
/*      row is in the range 0 - 24.                                        */
/*      col is either in range 0 - 39 or 0 - 79                            */
/* _vline(row, col, ch, length) - draw a vertical line of length length    */
/*                                characters ch, starting at row, col.     */
/*                                Does not check row and col.              */
/*                                                                         */
/*      Draw horizontal lines                                              */
/*      If width > 0, draw down, but not beyond the bottom of the screen   */
/*      If width < 0, draw up, but not beyond the top of the screen        */
/*      The functions return the number of characters written, 0 for       */
/* error.  They leave the cursor one beyone the end of the line drawn.     */
/* horline(ch, width)           - draw a horizontal line of width width    */
/*                                starting at the current cursor position  */
/* hline(row, col, ch, width)   - draw a horizontal line of width width    */
/*                                characters ch, starting at row, col.     */
/*                                Checks row and col for validity.         */
/*      row is in the range 0 - 24.                                        */
/*      col is either in range 0 - 39 or 0 - 79                            */
/* _hline(row, col, ch, width)  - draw a horizontal line of width width    */
/*                                characters ch, starting at row, col.     */
/*                                Does not check row and col.              */
/*                                                                         */
/* Lew Paper                                                               */
/*                                                                         */
/*                                                                         */

#include "dispio.h"

#undef PAGE
#define PAGE d_page

#undef WRTCHAR
#define WRTCHAR(CH) write_ac(d_attr, CH, PAGE, 1);

/*  Initial values for display parameters.  Change to suit the application */
/*  These are for the full screen of the monochrome monitor*/
#define INIT_DMODE     7        /* Monochrome */
#define INIT_DCOLS    80        /* 80 columns wide */
#define INIT_DPAGE     0        /* Page 0 */
#define INIT_DTOP      0        /* Top row is 0 */
#define INIT_DBOTTOM  24        /* Bottom row is 24 */
#define INIT_DLEFT     0        /* Left hand column is 0 */
#define INIT_DRIGHT   79        /* Right hand column is 79 */
#define INIT_DULC      0        /* Upper left hand corner */
#define INIT_DLRC 0x184f        /* Lower right hand corner, (24<<8) + 79 */
#define INIT_DATTR     7        /* Standard attribute */

#define MAXSTRING 254

#define SCRL_UP1 dispio(0x0601, STD_CHAR, d_ulc, d_lrc)

/* Display parameters.  Note that they can be declared external in other
   modules */
int d_mode = INIT_DMODE;
int d_cols = INIT_DCOLS;
int d_page = INIT_DPAGE;
int d_top = INIT_DTOP;
int d_bottom = INIT_DBOTTOM;
int d_left = INIT_DLEFT;
int d_right = INIT_DRIGHT;
int d_ulc = INIT_DULC;
int d_lrc = INIT_DLRC;
int d_attr = INIT_DATTR;

int set_dfull()

{
   
   dispio(GET_STATE, &d_mode, &d_cols, &d_page);
   d_top = d_left = d_ulc = 0;
   d_bottom = 24;
   d_right = d_cols - 1;
   d_lrc = (24 << 8) + d_right;
   
}

int d_tomono() 

{
   
   mono_dsp();
   set_dfull();
   
}

int d_toc80() 

{
   
   c80_disp();
   set_dfull();
   
}

int d_toc40() 

{
   
   c40_disp();
   set_dfull();
   
}

int d_newmode(newmode)

int newmode;

{
   
   if (d_mode == 7 || newmode < 0 || newmode > 6) return(0);
   dispio(newmode);
   set_dfull();
   return(1);
   
}

int d_newpage(new_page)

int new_page;

{
   
   if (new_page < 0) return(0);
   switch (d_mode) {
      case 2: case 3:
         if (new_page > 3) return(0);
         break;
      case 0: case 1:
         if (new_page > 7) return(0);
         break;
      default: return(0);
   }
   dispio(SELECT_PAGE + new_page);
   d_page = new_page;
   return(1);
   
}

int clrscr() {
   
   if (d_top == d_bottom) {
      CURPOS(d_top, d_left);
      write_ac(STD_CHAR, ' ', PAGE, d_right - d_left + 1);
   }
   /* SCROLL_UP has number of columns = 0, so the next line 
      clears the window */
      else dispio(SCROLL_UP, STD_CHAR, d_ulc, d_lrc); 
   dispio(SET_CUR, PAGE, d_ulc);
   
}

int d_erasel()

{
   
   int row, col;
   
   read_loc(PAGE, &row, &col);
   CURPOS(row, d_left);
   write_ac(STD_CHAR, ' ', PAGE, d_right - d_left + 1);
   CURPOS(row, d_left);

}

int d_ereol()

{
   
   int row, col;
   
   read_loc(PAGE, &row, &col);
   write_ac(STD_CHAR, ' ', PAGE, d_right - col + 1);
   CURPOS(row, col);
}

int d_nextc()

{
   
   int row, col;
   
   read_loc(PAGE, &row, &col);
   if (col < d_right) ++col;
   else {
      col = d_left;
      if (row < d_bottom) ++row;
      else SCRL_UP1;
   }
   CURPOS(row, col);
   
}

int d_nextl()

{
   
   int row, col;
   
   read_loc(PAGE, &row, &col);
   col = d_left;
   if (row < d_bottom) ++row;
   else SCRL_UP1;
   CURPOS(row, col);
   
}

int d_putc(ch)

char ch;

{
   
   if (ch == '\n') d_nextl();
   else {
      WRTCHAR(ch);
      d_nextc();
   }
   
}

int d_puts(s)

char *s;

{
   
   char *i, c;
   int row, col, j;
   
   read_loc(PAGE, &row, &col);
   
   for (i = s, j = MAXSTRING; j; --j, ++i) {
      if (!(c = *i)) break; /* End of string */
      if (c != '\n') {
         WRTCHAR(*i);
         if (col < d_right) {  /* Stay on same line */
            ++col;
            CURPOS(row, col);
            continue;
         }
      }
      col = d_left;              /* Go to next line */
      if (row < d_bottom) ++row;
      else SCRL_UP1;
      CURPOS(row, col);
   }
   return(i - s);
}

int verline(ch, length)

char ch;
int length;

{
   int row, col;
   
   read_loc(d_page, &row, &col);
   return(_vline(row, col, ch, length));
   
}

int vline(row, col, ch, length)

char ch;
int row, col, length;

{
   
   if (row < d_top || row > d_bottom || col < d_left || col > d_right) 
       return(0);
   return(_vline(row, col, ch, length));
   
}

int _vline(row, col, ch, length)

char ch;
int row, col, length;

{
   
   int start_r, end_r, count, return_r;
   
   if (length == 0) return(0);
   
   if (length > 0) {
      start_r = row;
      end_r = start_r + length - 1;
      if (end_r > d_bottom) end_r = d_bottom;  /* Don't go beyond bottom */
      count = end_r - start_r + 1;
      return_r = end_r + (end_r < d_bottom);   /* Don't go beyond bottom */
   }
   else {
      start_r = row + length + 1;  /* length < 0, so start_r < row */
      if (start_r < d_top) start_r = d_top;  /* Don't go beyond top */
      end_r = row;
      count = start_r - end_r - 1;
      return_r = start_r -(start_r > d_top); /* Don't go beyone top */
   }
   
   for ( ; start_r <= end_r; ++start_r) {
      CURPOS(start_r, col);
      WRTCHAR(ch);
   }
   
   CURPOS(return_r, col);
   return(count);
   
}

int horline(ch, width)

char ch;
int width;

{
   int row, col;
   
   read_loc(d_page, &row, &col);
   return(_hline(row, col, ch, width));
   
}

int hline(row, col, ch, width)

char ch;
int row, col, width;

{
   
   if (row < d_top || row > d_bottom || col < d_left || col > d_right) 
       return(0);
   return(_hline(row, col, ch, width));
   
}

int _hline(row, col, ch, width)

char ch;
int row, col, width;

{
   
   int start_c, end_c, count, return_c;
   
   if (width == 0) return(0);
   
   if (width > 0) {
      start_c = col;
      end_c = start_c + width - 1;
      if (end_c > d_right) end_c = d_right;  /* Don't go beyond right end */
      return_c = end_c + (end_c < d_right);   /* Don't go beyond right end */
   }
   else {
      start_c = col + width + 1;  /* width < 0, so start_c < col */
      if (start_c < d_left) start_c = d_left;  /* Don't go beyond left end */
      end_c = col;
      return_c = start_c -(start_c > d_left); /* Don't go beyone left end */
   }
   
   CURPOS(row, start_c);
   count = end_c - start_c + 1;
   write_ac(d_attr, ch, PAGE, count);
   
   CURPOS(row, return_c);
   return(width > 0 ? count: -count);
   
}


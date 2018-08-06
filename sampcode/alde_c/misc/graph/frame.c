/**********************************************************
 *   This function builds a nice looking frame for a      *
 *   menu.  The function is passed the parameters for     *
 *   the upper left corner row, upper left corner column  *
 *   the height of the frame and the width.  Depending    *
 *   on your particular compiler characteristics you      *
 *   will have to substitute its functions for clear-     *
 *   ing the screen and locating at the correct row       *
 *   and column on the screen. the functions you will     *
 *   need to replace with your own are scr_clr() and      *
 *   scr_rowcol(x, y).                                    *
 *                                                        *
 *          Program By                                    *
 *          Lynn Long                                     *
 *          Tulsa, OK RBBS "C" Bulletin Board             *
 *          918-664-8737                                  *
 *********************************************************/









#define			ULCOR			201
#define			URCOR			187
#define			LLCOR			200
#define			LRCOR			188
#define			VBAR			186
#define			HBAR			205

frame(row, col, hgt, wdth)
int row, col, hgt, wdth;

{
	int x, y;
    

	scr_clr();                                /*  insert your code here to clear screen  */
	scr_rowcol(row, col);                     /*  insert your function to locate row and col */
	putchar(ULCOR);
    for(x = col + 1; x <=(col + wdth -1); x++)
     	putchar(HBAR);
        putchar(URCOR);
        for(x = row + 1; x <=(row + hgt - 1); x++){
        scr_rowcol(x, col);    
        putchar(VBAR);
        scr_rowcol(x, col+wdth);
        putchar(VBAR);
   }
    scr_rowcol(x, col);  
    putchar(LLCOR);
    for(x= col + 1; x <=(col + wdth -1); x++)
    	putchar(HBAR);
    	putchar(LRCOR);	
} 

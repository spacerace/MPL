
/*                        Listing 4
                    C to Assembly Interface
      Computer Language, Vol. 2, No. 2 (February, 1985), pp. 49-59
        Modified to remove duplicate leading part, L.P., 5/29/85
   Split into DSPIO.H for #define's and macros and TDSPIO.C, L.P., 5/29/85
*/

#include "DISPIO.H"

main()
{
 int retvals [4] ;
 int index ;
  SET_MODE ;
  hline(0,0,219,80) ;   /* draw box around screen */
  hline(24,0,219,80) ;   /* i.e., manual window using nonwindow characters */ 
  verline(0,0,219,24) ;
  verline(0,79,219,24) ;
  window(3,3,6,5) ;
  window(10,10,5,6) ;
  window(7,17,10,11) ;
  window(15,40,10,6) ;
  window(5,50,20,16) ;
      /* read cursor position */
  for (index=0; index <4; index++ ) {
    curpos(index,index+1) ;
    dispio(read_position,PAGE,0,0,retvals) ;
    printf("For page PAGE, Row:%u, Col:%u, cursor mode=%o,%o \n",
      retvals[3]>>8, (retvals[3]<<8)>>8, retvals[2]>>8,(retvals[2]<<8)>>8) ; 
  }
     /* get current video state */
  dispio(get_state,PAGE,0,0,retvals) ;
  printf("Mode: %u, Columns: %u, Active Page: %u \n",
     (retvals[0]<<8)>>8, retvals[0]>>8, retvals[1]>>8 ) ;
}
window(row,col,width,depth) /* start at row,col, i.e. left upper corner */
char row,col ;
{
     /* first do 4 corners */
  curpos(row,col) ;
  wrtchar(214) ;
  curpos(row+depth,col) ;
  wrtchar(211) ; 
  curpos(row,col+width-1) ;
  wrtchar(183) ;
  curpos(row+depth,col+width-1) ;
  wrtchar(189) ;
     /* then connect corners with horizontal and vertical lines */
  hline(row,col+1,196,width-2) ;
  hline(row+depth,col+1,196,width-2) ;
  verline(row+1,col,186,depth-1) ;
  verline(row+1,col+width-1,186,depth-1) ;
}
verline(row,col,ch,len)
char row, col, ch, len ;
{
  char cnt ;
     for (cnt=len; cnt > 0; cnt--) {
        curpos(row++,col) ;
        wrtchar(ch) ;
     }
}

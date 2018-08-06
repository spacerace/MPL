#include "windows.h"
main()
{
WINDOWPTR w1;                   /* window handle */
int batrib;                     /* border atrib */
int watrib;                     /* window atrib */
register i;

/*
 * Set attributes:
 *
 *      border - blue/white box
 *      window - white background/black letters
 *
*/

  batrib = v_setatr(BLUE,WHITE,0,0);
  watrib = v_setatr(WHITE,BLACK,0,0);

/*
 * Open window at 0,0 - 25 cells wide and 10 cells high
*/

  w1 = wn_open(0,0,0,25,10,watrib,batrib);
  if(!w1) exit();

/*
 * Print the famous string and wait for key to be struck.
 * Close window on key strike.. exit.
*/

  wn_printf(w1,"Hello World...");
  v_getch();
  wn_close(w1);
}

/* End */


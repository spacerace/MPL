#include <color.h>

main()
 {
  int c = 0;
  int w;
  int w2;
  save_cursor();
  color(WHT_F+BLU_B);
  w = wopen(2,3,10,10,1);
  color(BLU_F+WHT_B);
  w2 = wopen(7,7,20,30,3);
  for ( c = 0; c < 5; ++c )
   {
    wactivate(w);
    delay(25);
    wactivate(w2);
    delay(25);
   }
  for ( c = 0; c < 50; ++c )
      wprintf(w2,"wprintf() - %02d\n",c);
  wprintf(w2,"wprintf() - %02d",c);
  getch();
  wblank(w2,3);
  getch();
  wprint(w2,"\f  Header, Static.\n");
  whline(w2,1);
  wputat(w2,wrow(w2)-1,0,"  Footer, Static.");
  whline(w2,wrow(w2)-2);
  wfreeze(w2,2,wrow(w2)-3);
  wgotoxy(w2,2,0);
  getch();
  for ( c = 0; c < 50; ++c )
      wprintf(w2,"wprintf() - %02d\n",c);
  wprintf(w2,"wprintf() - %02d",c);
  getch();
  wcloseall();
  restore_cursor();
 }

delay(x)
 int x;
 {
  int i = 0;
  while ( i < x )
        ++x;
 }

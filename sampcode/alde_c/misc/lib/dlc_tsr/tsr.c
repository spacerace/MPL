/*
In December of 1986 Roy Sherrill, President of Datalight, asked me if I 
could produce a Terminate and Stay Resident (TSR) library function for 
Datalight C.  He explained that a consultant programmer known by him to 
be capable of producing such a library function estimated 4 to 6 months 
effort.  I scoffed at that and produced my first version of a fully 
operational TSR library function over the following weekend.  A 
demonstration copy was forwarded to Datalight and Roy Sherrill 
encouraged me to enhance that version with a screen saver function, all 
the while strongly implying that Datalight wished to use the resultant 
library function as a product.

Roy Sherrill was either not serious or is in the progress of reverse 
engineering the .OBJ files he received from me.  In either case, his 
time is up.  I offer the enclosed files to any C programmer who finds 
them useful with no strings or encumberances of any kind.  In other 
words, the enclosed files are offered free and clear to the public 
domain to whomever desires to use them.

Anyone desiring the source code for TSR.OBJ should contact me 
personally.

John J. Newlin
4060-288 Rosenda Ct.
San Diego, CA 92122
(619) 455-6225

A note to Roy Sherrill:  It won't be Microsoft that treads on 
Datalight's grave - it will be Borland!!!
*/


/* minimum configuration is _stack = 2048 - 470h paragraphs 
   with no screen save */
#include <stdio.h>
int _stack = 6048;
main()
{
  if (check_tsr() == 112)  /* 112 = Alt-F9 */
  /* check_tsr returns an integer equal to the hot key value of any
     Datalight TSR routines currently in memory.  It is used to determine
     if your TSR has already been installed */
  {
     puts("TSR installed in memory\07");
     exit(1);
  }
  puts("DATALIGHT C memory resident demo installed");
  tsr_init(112,0x0500);
  /* tsr_init is called with two integer values.  The first is the 
  extended scan code of the activation ("hot") key.  The second is the 
  amount of memory (in 16 byte paragraphs) needed to support the TSR.
  Memory calculation must be accomplished by the programmer prior to 
  final compilation and linking. */

}
tsr_main()
/* tsr_main is the function called by the TSR when the "hot" key is 
sensed.  It MUST have this name.  It may call any other functions and 
acts as the primary gateway between the resident handler and the main 
program code.  Place your desired code here. */

{ int key,attr;
  char vmode,*msg;
  static char *buffer[4000];
  /* This buffer is used to save the entire screen but it is possible to
     use savescrn and restscrn to save and selected portions of the screen.
     For example, savescrn(10,10,20,8,buffer) would save a rectangular
     portion of the screen with the upper left hand corner at 10,10, a
     width of 20 columns, and a depth of 8 rows.  A buffer on the heap just
     large enough to contain the portion of the screen desired to be saved
     may be employed by using the intrinsic call calloc.  The amount of
     memory required is computed as width * depth * 2.
  */

  savecurs();
  /* Saves the position and the type of the current cursor */

  savescrn(1,1,80,25,buffer);
  /* This function saves a selected rectangular portion of the screen.  In
     this case the entire screen is saved.  The function declaration
     (restscrn is the same) is:

               void savescrn(col,row,width,depth,buffer);
               int col,row,width,depth;
               char *buffer;

     where col is the column coordinate of the upper left hand corner of the
     rectangle, row is the row coordinate of the upper left hand corner of
     the rectangle, width is the width of the rectangle in columns, and depth
     is the height of the rectangle in rows (normalized so that the top left 
     corner of the full screen is column 1, row 1)
  */

  cls(14); /* clears the entire screen with an attribute of 0Eh.  Declared
              as :
                   void cls(attribute);
                   int attribute;
           */

  /*
      The type of video monitor is detected and adjusted for automatically
      upon each call to savescrn.
  */

  msg = "TERMINATE & STAY RESIDENT DEMONSTRATION";

  gotoxy(21,6);  /* position the cursor - normalized to (1,1) for upper left
                    corner of the full screen.  Declared as :
                         
                           void gotoxy(col,row);
                           int col,row;
                  */

  puts(msg);
  msg = "DATALIGHT C";
  gotoxy(35,8);
  puts(msg);
  msg = "by John Newlin";
  gotoxy(33,10);
  puts(msg);
  msg = "Copyright (C) 1987";
  gotoxy(31,12);
  puts(msg);
  msg = "Any key continues";
  gotoxy(32,14);
  puts(msg);
  msg = "End key deinstalls";
  gotoxy(31,16);
  puts(msg);
  key = getkey(); /* my personal keyboard interrogator */
  if (key == 335) 
    { 
      restscrn(1,1,80,25,buffer);  /* restore the saved screen */
      restcurs();  /* restore the saved cursor type and position */
      remove();                   /* remove this TSR */
    }
  restscrn(1,1,80,25,buffer);      /* restore the saved screen */
  restcurs();                     /* restore the cursor type and position */
}


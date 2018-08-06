#include <graph.h>
#include <stdio.h>
#include <memory.h>
#include <dos.h>
#include <malloc.h>



/* DEMONSTRATION OF SAVE_SCREEN AND GET_SCREEN  */
/* THESE ROUTINES DEMONSTRATE HOW TO QUICKLY SAVE */
/* AND RETREIVE THE 16 COLOR 640X350 EGA SCREEN */
/* THERE IS MINIMAL ERROR CHECKING IN THE CODE */
/* WRITTEN BY R.W. SIMPSON   DEC 21, 1987 */
/*            EDEN PRAIRIE, MN  */
/* MICROSOFT C VERSION 5 SMALL MODEL */
/* PROGRAM REQUIRES 128K EGA CARD AND MONITOR  */





main ()

{

      char filename[13];
      int flag;            /* error flag */


      printf("\nEnter the filename for the test >");
      scanf("%s",filename);




      if ((_setvideomode(_ERESCOLOR)) == 0)
	    exit();


      _setcolor(12);


      _rectangle(_GFILLINTERIOR, 80, 50, 240, 150);

      _setcolor(14);
      _rectangle(_GFILLINTERIOR,100,200,340,340);

      flag = save_screen(filename);

      if (flag != 0)
	  {  _setvideomode(_DEFAULTMODE);
	     printf("\nERROR SAVING SCREEN # %i",flag);
	     exit();
	  }

      _clearscreen(_GCLEARSCREEN);

      printf("\n Now to retrieve the screen \n Hit a key to procede ");

      while (!kbhit());
      getch();

      flag = get_screen(filename);

      if (flag !=0)
	 { _setvideomode(_DEFAULTMODE);
	   printf("\nERROR READING IMAGE # %i",flag);
	   exit();
	 }


      _settextposition(20,1);
      _outtext("Done, hit a key to terminate");
      while (!kbhit());
      getch();

      _setvideomode(_DEFAULTMODE);
      printf("Program terminated without error");

 }

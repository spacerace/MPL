/* TIMER.C - From page 511 of "Microsoft C Programming for      */
/* the IBM" by Robert Lafore. Checks the initime() and timeout()*/
/* functions from the same page.                                */
/****************************************************************/

main()
{
long intval;

   while(1)  {
      printf("\n\nEnter interval (hundreds of a second): ");
      scanf("%D", &intval);
      initime(intval);              /*initialize timer*/
      printf("\n\nStart.\n");
      while(!timeout())
         ;
      printf("\n\nEnd.\n");
   }
}

/* initime() */   /* initializes time delay for timeout() function */
/* delays number of hundredths of a second in argument */
/* increments every 5 or 6 hundredths sec */

#include <types.h>      /*defines time_b type in timeb*/
#include <timeb.h>      /*defines time_b structure*/
struct timeb xtime;     /*structure for time info*/
long time1, time2;      /*start & running times*/
long intval;            /*interval in hundredths*/

initime(hsecs)
long hsecs;
{

   intval = (hsecs < 12) ? 12 : hsecs;    /*minimum is 12*/
   ftime(&xtime);
   time1 = (long)xtime.millitm/10 + xtime.time*100;
}

/* timeout() */   /* Returns 1 to function timer() */
/* if time interval exceeded, 0 otherwise. */
timeout()
{

   ftime(&xtime);
   time2 = (long)xtime.millitm/10 + xtime.time*100;
   return((time2-time1 > intval) ? 1 : 0);
}



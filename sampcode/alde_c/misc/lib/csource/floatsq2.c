                                         /* Chapter 5 - Program 7 */
#include "stdio.h"   /* Prototypes for standard Input/Outputs     */

float sqr(float inval);
float glsqr(void);

float z;   /* This is a global variable */

main()
{
int index;
float x,y;

   for (index = 0;index <= 7;index++){
      x = index;  /* convert int to float */
      y = sqr(x); /* square x to a floating point variable */
      printf("The square of %d is %10.4f\n",index,y);
   }

   for (index = 0; index <= 7;index++) {
      z = index;
      y = glsqr();
      printf("The square of %d is %10.4f\n",index,y);
   }
}

float sqr(float inval)  /* square a float, return a float */
{
float square;

   square = inval * inval;
   return(square);
}

float glsqr(void)    /* square a float, return a float */
{
   return(z*z);
}

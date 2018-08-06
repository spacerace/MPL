#include <stdio.h>

void delay(seconds)
   unsigned int seconds;
{
   long dtime;

   dtime = time(NULL) + seconds;

   while (dtime != time(NULL));
}

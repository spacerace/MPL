/* VOLUME.C: Calculate sphere's volume. */

#include <stdio.h>
#define PI 3.14

float sphere( int rad );

main()
{
   float volume;
   int radius = 3;
   volume = sphere( radius );
   printf( "Volume: %f\n", volume );
}

float sphere( int rad )
{
   float result;
   result = rad * rad * rad;
   result = 4 * PI * result;
   result = result / 3;
   return result;
}

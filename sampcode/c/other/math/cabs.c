/* CABS.C illustrates functions:
 *      cabs            hypot
 */

#include <math.h>
#include <stdio.h>

main()
{
    static struct complex ne = {  3.0,  4.0 }, se = { -3.0, -4.0 },
                          sw = { -3.0, -4.0 }, nw = { -3.0,  4.0 };

    printf( "Absolute %4.1lf + %4.1lfi:\t\t%4.1f\n",
            ne.x, ne.y, cabs( ne ) );
    printf( "Absolute %4.1lf + %4.1lfi:\t\t%4.1f\n",
            sw.x, sw.y, cabs( sw ) );

    printf( "Hypotenuse of %4.1lf and %4.1lf:\t%4.1f\n",
            se.x, se.y, hypot( se.x, se.y ) );
    printf( "Hypotenuse of %4.1lf and %4.1lf:\t%4.1f\n",
            nw.x, nw.y, hypot( nw.x, nw.y ) );
}

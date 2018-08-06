/* tester.c -- demonstrates the assert() macro        */
/* Program list: tester.c (to link math functions)    */

#include <assert.h>
#include <stdio.h>
#include <math.h>
main()
{
    float s1 = 3.0;
    float s2 = 4.0;
    float sumsq;
    float hypot;

    sumsq = s1*s1 - s2*s2;
    assert(sumsq >= 0);
    hypot = sqrt(sumsq);
    printf("hypotenuse is %.2f\n", hypot);
}

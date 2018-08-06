/* mathtest.c -- driver for do_math()                 */
/* Program list: mathtest.c (to link math functions)  */

#include <stdio.h>
double do_math(double);
main()
{
    double input, result;

    printf("Enter a number: ");
    while (scanf("%lf", &input) == 1)
	{
        result = do_math(input);
        printf("input = %.2e, result = %.2e\n", input,
                result);
        printf("Next number (q to quit): ");
	}
}

#include <math.h>
double do_math(x)
double x;
{
    return (sin (x) * exp (-x));
}


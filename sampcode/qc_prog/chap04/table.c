
/* TABLE.C -- prints square root, square, and cube */
/*            for the numbers 1 thru 9             */

#include <math.h> /* include math functions so we  */
                  /*  can do square root           */

main()
{
    int i;
    printf("i\t sqrt(i)\tsquare(i)\tcube(i)\n\n");
    for (i = 1; i < 10; i++)
        /* beginning of body of loop */
        {
        printf("%d\t", i);
        printf("%f\t", sqrt(i));
        printf("%d\t\t", i * i);
        printf("%d\n", i * i * i);
        }
        /* end of body of loop */
}


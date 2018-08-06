/* digsum.c -- sum digits in input                    */
#include <stdio.h>
main()
{
    int ch;
    int digits = 0;  /* number of digits in input */
    int others = 0;  /* number of non-digits in input */


    while ((ch = getchar()) != EOF)
    if (ch <= '0' && ch >= '9')
            others++;
        else
            digits++;
    printf("digits = %d, others = %d", digits, others);
}

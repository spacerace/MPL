/* break.c -- shows how to get out of loop with BREAK */

#include <stdio.h>
#define TRUE 1

main()
{
    int number;
    while (TRUE) /* endless loop */
        {
        /* get a random number between 0 and 32767 */
        number = rand();
        printf("%d\n", number);

        /* break out of loop if random number */
        /* is greater than 32000              */
        if (number > 32000)
            break; /* exit WHILE loop */
        }
    printf("Broken out of WHILE loop.\n");
}

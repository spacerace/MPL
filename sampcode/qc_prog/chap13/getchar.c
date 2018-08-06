/*      getchar.c -- using getchar()         */

#include <stdio.h>
main()
{
    int count = 1;

    printf("Please enter a word.\n");
    while (getchar() != '\n')       /* here it is */
        printf("%d.. ", count++);
    printf("\n%d characters altogether\n", count - 1);
}

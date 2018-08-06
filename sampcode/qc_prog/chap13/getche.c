/*     getche.c -- using getche()             */
#include <conio.h>     /* note different file included */
main()
{
    int count = 1;

    printf("Please enter a word.\n");
    while (getche() != '\r')    /* changed comparison */
        printf("%d.. ", count++); 
    printf("\n%d characters altogether\n", count - 1);
}

/*     getch.c -- using getch()             */
#include <conio.h>
main()
{
    int count = 1;

    printf("Please enter a word.\n");
    while (getch() != '\r')
        printf("%d.. ", count++);
    printf("\n%d characters altogether\n", count - 1);
}

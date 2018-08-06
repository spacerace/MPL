/* backward.c -- the backwards word displayer         */

#include <stdio.h>
#define SIZE 5
char word[SIZE] = "trap";
main()
{
    unsigned int index;

    printf("%s backwards is ", word);
    for (index = SIZE - 2; index >= 0; index--)
	putchar(word[index]);
    putchar('\n');
}


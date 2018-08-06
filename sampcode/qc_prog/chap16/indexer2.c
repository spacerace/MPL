/* indexer2.c -- use indices to display an array      */

#include <stdio.h>
int code1[] = {2, 4, 6, 8};
int code2[] = {1, 3, 7, 9};
int code3[] = {5, 10, 15, 20};
main()
{
    int index;
    int size = (sizeof code2) / (sizeof (int));
                /* get number of elements in array */

    for ( index = 1; index <= size; index++)
       printf("%3d ", code2[index]);
    putchar('\n');
}

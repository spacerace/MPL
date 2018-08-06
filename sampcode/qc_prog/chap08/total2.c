/* total2.c -- how to build an array on the fly       */
/*             using sbrk()                           */

#include <stdio.h>        /* for NULL   */
#include <malloc.h>       /* for size_t */

main()
    {
    int *iptr, count = 0, i, total;
    size_t bytes = sizeof(int);

    /* Start the array with room for one value. */
    iptr = sbrk(0);
    if (sbrk(bytes) == (int *)-1)
        {
        printf("Oops, sbrk failed\n");
        exit(1);
        }

    printf("Enter as many integer values as you want.\n");
    printf("I will build an array on the fly with them.\n");
    printf("(Any non-number means you are done.)\n");

    while (scanf("%d", &iptr[count]) == 1)
        {
        ++count;
        /* Enlarge the array. */
        if (sbrk(bytes) == (int *)-1)
            {
            printf("Oops, sbrk failed\n");
            exit(1);
            }
        }
    total = 0;
    for (i = 0; i < count; i++)
        total += iptr[i];
    /* just print the total this time */
    printf( "%d\n", total);
}

/* total.c  -- how to build an array on the fly    */

#include <stdio.h>         /* for NULL   */
#include <malloc.h>        /* for size_t */

main()
{
    int *iptr, count = 0, i, total;
    size_t bytes = sizeof(int);

    /* Start the array with room for one value. */
    if ((iptr = malloc(bytes)) == NULL)
        {
        printf("Oops, malloc failed\n");
        exit(1);
        }

    printf("Enter as many integer values as you want.\n");
    printf("I will build an array on the fly with them.\n");
    printf("(Any non-number means you are done.)\n");

    while (scanf("%d", &iptr[count]) == 1)
        {
        ++count;
        /* Enlarge the array. */
        if ((iptr = realloc(iptr,bytes*(count+1))) == NULL)
            {
            printf("Oops, realloc failed\n");
            exit(1);
            }
        }
    total = 0;
    printf("You entered:\n");
    for (i = 0; i < count; i++)
        {
        printf("iptr[%d] = %d\n", i, iptr[i]);
        total += iptr[i];
        }
    printf("\nTotal: %d\n", total);
    return (0);
}

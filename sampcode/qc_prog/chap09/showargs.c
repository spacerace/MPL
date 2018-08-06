/* showargs.c  --  shows how to access the arguments   */
/*                 passed to main()                    */

#include <stdio.h>        /* for NULL */

main(argc, argv)
int argc;
char *argv[];
{
    int i;

    printf("argc = %d\n", argc);
    printf("\n");

    for (i = 0; i < argc; ++i)
        {
        printf("argv[%d] -> \"%s\"\n", i, argv[i]);
        }
    printf("argv[%d] -> NULL\n", i);
    printf("\n");

}

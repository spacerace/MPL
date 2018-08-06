/* show2.c  --     shows how to use main()'s envp */

#include <stdio.h>        /* for NULL */

main(argc, argv, envp)
int argc;
char *argv[], *envp[];
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

    for (i= 0; envp[i] != NULL; ++i)
        {
        printf("envp[%d] -> \"%s\"\n", i, envp[i]);
        }
    printf("envp[%d] -> NULL\n", i);

}

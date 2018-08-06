/* reverse.c -- demonstrates an array of pointers     */
/*              by reversing lines of text            */

#include <stdio.h>         /* for NULL   */
#include <malloc.h>        /* for size_t */

#define MAXL 20

main()
{
    char *cptrs[MAXL];     /* array of pointers */
    char *cp;
    int count, i, j, ch;
    extern char *Getbyte();


    printf("Type in several lines of text, and I will\n");
    printf("print them back out in reverse order.\n");
    printf("(Any blank line ends input):\n");

    for (i = 0; i < MAXL; ++i)
        {
        cp = Getbyte();
        cptrs[i] = cp;     /* assign address to pointer */
        count = 0;
        while ((ch = getchar()) != '\n') /* gather line */
            {
            *cp = ch;
            cp = Getbyte();
            ++count;
            }
        *cp = '\0';
        if (count == 0)    /* all done if blank line */
            break;
        }
    printf("---------<reversed>---------\n");
    for (j = i-1; j >= 0; --j)
        {
        printf("%s\n", cptrs[j]);
        }
}

char *Getbyte(void)
{
    char *cp;

    if ((cp = sbrk(1)) == (char *)-1)
        {
        printf("Panic: sbrk failed\n");
        exit(1);
        }
    return (cp);
}

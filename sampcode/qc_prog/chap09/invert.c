/* invert.c  --  combines character classification and */
/*               transformation to invert text         */

#include <stdio.h>         /* for NULL           */
#include <ctype.h>         /* for toupper, et al */

main()
{
    char buf[BUFSIZ];
    int i;

    printf("Type in a line of text and I will invert it.\n");

    if (gets(buf) == NULL)
        exit(1);

    /* Print the string backwards. */
    for (i = (strlen(buf) -1); i >= 0; --i)
        {
        if (isupper(buf[i]))            /* upper to lower */
            putchar(tolower(buf[i]));
        else if (islower(buf[i]))       /* lower to upper */
            putchar(toupper(buf[i]));
        else
            putchar(buf[i]);
        }
    putchar('\n');

}

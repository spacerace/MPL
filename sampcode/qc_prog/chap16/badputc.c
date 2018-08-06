/* badputc.c -- misusing putc()                  */

#include <stdio.h>
main()
{
    FILE *fp;
    int ch;

    if ((fp = fopen("junk", "w")) == NULL)
        exit(1);

    while ((ch = getchar()) != EOF)
        putc(fp, ch);
    fclose(fp);
}


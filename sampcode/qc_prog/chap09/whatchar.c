/* whatchar.c  --  demonstrates the character         */
/*                 classification routines in ctype.h */

#include <stdio.h>        /* for NULL and BUFSIZ */
#include <ctype.h>        /* for iscntl(), et al */
#define MAXL 20

main()
{
    char buf[BUFSIZ];
    int i;

    printf("Enter a line of text (20 chars max):\n");
    if (gets(buf) == NULL)
        exit(1);

    for (i = 0; i < MAXL; ++i)
        {
        if (buf[i] == '\0')
            break;
        printf("'%c' ->", buf[i]);
        if (isalpha(buf[i]))   printf(" isalpha");
        if (isascii(buf[i]))   printf(" isascii");
        if (iscntrl(buf[i]))   printf(" iscntrl");
        if (isgraph(buf[i]))   printf(" isgraph");
        if (isprint(buf[i]))   printf(" isprint");
        if (isdigit(buf[i]))   printf(" isdigit");
        if (isupper(buf[i]))   printf(" isupper");
        if (islower(buf[i]))   printf(" islower");
        if (ispunct(buf[i]))   printf(" ispunct");
        if (isspace(buf[i]))   printf(" isspace");
        if (isalnum(buf[i]))   printf(" isalnum");
        if (isxdigit(buf[i]))  printf(" isxdigit");
        printf("\n");
        }
}

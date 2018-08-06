/* ccopy.c  --  Copies a file, cutting blank lines and  */
/*              leading space from lines of copy        */

#include <stdio.h>        /* for FILE, BUFSIZ, NULL */
#include <ctype.h>        /* for iswhite()          */

main(argc, argv)
int argc;
char *argv[];
{
    FILE *fp_in, *fp_out;
    char buf[BUFSIZ];
    char *cp;

    if (argc != 3)
        {
        printf("usage: ccopy infile outfile\n");
        exit(1);
        }
    if ((fp_in = fopen(argv[1], "r")) == NULL)
        {
        printf("Can't open %s for reading.\n", argv[1]);
        exit(1);
        }
    if ((fp_out = fopen(argv[2], "w")) == NULL)
        {
        printf("Can't open %s for writing.\n", argv[2]);
        exit(1);
        }

    printf("Copying and Crushing: %s->%s ...", 
            argv[1], argv[2]);

    while (fgets(buf, BUFSIZ, fp_in) != NULL)
        {
        cp = buf;
        if (*cp == '\n')    /* blank line */
            continue;
        while (isspace(*cp))
            {
            ++cp;
            }
        if (*cp == '\0')    /* empty line */
            continue;
        if (fputs(cp, fp_out) == EOF)
            {
            printf("\nError writing %s.\n", argv[2]);
            exit(1);
            }
        }
    printf("Done\n");
}

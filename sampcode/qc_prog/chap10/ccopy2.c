/* ccopy2.c  -- copies a file, cutting blank lines and    */
/*              leading space from lines of copy          */

/*              Modified to demonstrate stdout and stderr */

#include <stdio.h>        /* for FILE, BUFSIZ, NULL */
#include <ctype.h>        /* for iswhite()          */

main(argc, argv)
int argc;
char *argv[];
{
    FILE *fp_in, *fp_out;
    char buf[BUFSIZ];
    char *cp;

    if (argc < 2)
        {
        fprintf(stderr, "usage: ccopy infile {outfile}\n");
        exit(1);
        }
    if ((fp_in = fopen(argv[1], "r")) == NULL)
        {
        fprintf(stderr, "\"%s\": Can't open.\n", argv[1]);
        exit(1);
        }
    if (argc == 3)
        {
        if ((fp_out = fopen(argv[2], "w")) == NULL)
            {
            fprintf(stderr, "\"%s\": Can't open.\n", argv[2]);
            exit(1);
            }
        }
    else
        fp_out = stdout;

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
            fprintf(stderr, "Error writing.\n");
            exit(1);
            }
        }
    if (! feof(fp_in))      /* error reading? */
        {
        fprintf(stderr, "\"%s\": Error reading.\n", argv[1]);
        exit(1);
        }
}

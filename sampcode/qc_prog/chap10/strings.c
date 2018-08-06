/* strings.c  --  opens a file and searches it for */
/*                possible strings                 */

#include <stdio.h>       /* for FILE, BUFSIZ & EOF */
#include <ctype.h>       /* for isprint()          */

main(argc, argv)
int argc;
char *argv[];
{
    FILE *fp;
    char buf[BUFSIZ];
    int ch, count;

    if (argc != 2)
        {
        fprintf(stderr, "usage: strings file\n");
        exit(1);
        }
    if ((fp = fopen(argv[1], "rb")) == NULL)
        {
        fprintf(stderr, "Can't open %s\n", argv[1]);
        exit(1);
        }

    count = 0;
    while ((ch = fgetc(fp)) != EOF)
        {
        if (! isprint(ch) || count >= (BUFSIZ - 1))
            {
            if (count > 5)
                {
                buf[count] = 0;
                puts(buf);
                }
            count = 0;
            continue;
            }
        buf[count++] = ch;
        }
}

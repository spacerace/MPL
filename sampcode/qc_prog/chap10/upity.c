/* upity.c --  makes an uppercase copy of a file using */
/*             fread() and fwrite()                    */

#include <string.h>        /* for strrchr() */
#include <stdio.h>         /* for NULL      */
#include <malloc.h>        /* for malloc()  */
#include <ctype.h>         /* for isupper() */

#define HUNK 512

main(argc, argv)
int argc;
char *argv[];
{
    char *cp, newname[128], *np;
    FILE *fp;
    int  hunks = 0, bytes = 0, totbytes = 0;
    int  i;
    if (argc != 2)
        {
        printf("usage: upity file\n");
        exit(1);
        }

    if ((fp = fopen(argv[1], "rb")) == NULL)
        {
        printf("\"%s\": Can't open.\n", argv[1]);
        exit(1);
        }
    if ((cp = malloc(HUNK)) == NULL)
        {
        printf("Malloc Failed.\n");
        exit(1);
        }

    while ((bytes = fread(cp + (HUNK * hunks), 1, HUNK, fp)) == HUNK)
        {
        totbytes += bytes;
        ++hunks;
        if ((cp = realloc(cp, HUNK + (HUNK * hunks))) == NULL)
            {
            printf("Realloc Failed.\n");
            exit(1);
            }
        }
    if (bytes < 0)
        {
        printf("\"%s\": Error Reading.\n", argv[1]);
        exit(1);
        }
    totbytes += bytes;

    for (i = 0; i < totbytes; ++i)
        if (islower(cp[i]))
            cp[i] = toupper(cp[i]);
    
    (void)fclose(fp);

    if ((np = strrchr(argv[1], '.')) != NULL)
        *np = '\0';
    strcpy(newname, argv[1]);
    strcat(newname, ".up");
    if ((fp = fopen(newname, "wb")) == NULL)
        {
        printf("\"%s\": Can't open.\n", argv[1]);
        exit(1);
        }

    if (fwrite(cp, 1, totbytes, fp) != totbytes)
        {
        printf("\"%s\": Error writing.\n", argv[1]);
        exit(1);
        }

}

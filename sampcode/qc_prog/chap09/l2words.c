/* l2words.c  --  employ an array of pointers to   */
/*                strings to bust a line of text   */
/*                into its component words         */

#include <stdio.h>          /* for NULL and BUFSIZ */

main()
{
    char **Line2words();    /* declare function type */
    char **list;            /* pointer to pointer    */
    char buf[BUFSIZ];       /* buffer for input      */
    int  count, i, quote_flag;

    printf("Enter a line of text and I will break\n");
    printf("it up for you.\n");

    if (gets(buf) == NULL)
        exit(1);
    
    list = Line2words(buf, &count);

    for (i = 0; i < count; i++)
        {
        quote_flag = 0;
        printf("<");
        if (list[i] != buf)
            {
            if( list[i][-1] == '"')    /* negative subscript */
                {
                ++quote_flag;
                printf("\"");
                }
            }
        printf("%s", list[i]);

        if (quote_flag)
            printf("\"");

        printf(">\n");
        }
}

#define MAXW 64

char **Line2words(char *line, int  *count)
{
    static char *words[MAXW];
    int  index;

    index = 0;        /* zero internal index */

    while (*line != '\0')
        {
        /* turn spaces and tabs into zeros */
        if (*line == ' ' || *line == '\t')
            {
            *(line++) = '\0';
            continue;
            }
        words[index] = line++;    /* found a word */

        /* is it quoted? */
        if ( *(words[index]) == '"')
            {
            /* Yes, advance pointer to just past quote. */
            ++words[index];

            /* find next quote. */
            while (*line && *line != '"')
                {
                ++line;
                }

            /* and turn it into a '\0'. */
            if (*line)
                *(line++) = '\0';
            }
        else
            {
            /* otherwise skip to next space */
            while (*line && *line != ' ' && *line != '\t')
                {
                ++line;
                }
            }
        if (++index == MAXW)
            break;
        }
    *count = index;        /* set count via pointer   */
    return (words);        /* return address of array */
}

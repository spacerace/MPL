/* strpool.c  -- dumps the string pool to show how     */
/*               quoted strings are stored             */

#define PHRASE \
"This is one long sentence that the compiler \
combines into a single string."

char Start[]        = "start";
char Long_phrase[]  = PHRASE;
char Short_phrase[] = "This is a short phrase";
char Cent_string[]  = "\x9b";

main()
{
    static char local_phrase[] = "This is local";
    char *cp;

    printf("Dump of the string pool:\n");
    printf("-----------------------\n");

    printf("\"");                /* print leading quote */

    /*
     * Note that the address of a string can be
     * assigned to a pointer: cp = Start
     */
    for (cp = Start; *cp != '^'; ++cp)
        {
        if (*cp == '\0')        /* print '\0' as a quote */
            printf("\"\n\"");
        else if (*cp == '\n' )  /* print '\n' as '\' 'n' */
            printf("\\n");
        else
            printf("%c", *cp);
        }
    printf("^");                /* marks end */
}

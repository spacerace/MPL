/* static.c -- demonstrates a static variable */
/*             that holds count of lines,     */
/*             words, and characters          */

main()
    {
    void countline();
    printf("Type some lines of text.\n");
    printf("Start a line with a . to quit.\n\n");

    while (getche() != '.')
        countline();  /* accumulate word and */
                      /* line counts         */
    }

void countline()
    {
    static int words = 0; /* static variables */
    static int chars = 0;
    char ch;
    ++chars; /* count char typed when */
             /* function was called   */

    while ((ch = getche()) != '\r')
        {
        ++chars;
        if (ch == ' ')
            ++words;
        }
        ++words; /* count last word */

    printf("\nWords so far %d. Chars. so far %d\n", words, chars);
    }

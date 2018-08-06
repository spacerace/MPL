/* scanline.c  -- demonstrates how scanf() reads  */
/*                the individual words of a line  */

#define INTRO \
"Type in lines of text. They will be printed out\n\
one word per line, thus demonstrating scanf().\n\
(Type Ctrl-Z to Quit)\n"

main()
{
    char buf[512];    /* should be big enough */

    printf(INTRO);

    /*
     * scanf() returns the number of items
     * its control string matched.
     */
    while (scanf("%s", buf) == 1)
        {
        printf("%s\n", buf);
        }
    
}

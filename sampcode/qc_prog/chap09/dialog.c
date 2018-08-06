/* dialog.c  -- a conversation using gets() and puts()  */

#include <stdio.h>          /* for NULL and BUFSIZ */

#define THE_QUESTION \
"And what is your view on the current price of corn\n\
and the stability of our trade import balance?"

main()
{
    char name[BUFSIZ],
         buf[BUFSIZ];
    extern char *gets();

    name[0] = '\0';         /* clear the name */

    puts("\n\nHi there. And what is your name?");

    if (gets(name) != NULL && name[0] != '\0')
        {
        printf("\nPleased to meet you, %s.\n", name);
        puts(THE_QUESTION);

        /*
         * force an extra <enter> before replying.
         */
        do
            {
            if (gets(buf) == NULL)
                break;

            } while (*buf != '\0');     /* wait for empty line */

        puts("Sorry. I needed to think about that.");
        printf("Nice talking to you, %s.\n", name);
        }
    else
        puts("How rude!");

    puts("Goodbye.");
}

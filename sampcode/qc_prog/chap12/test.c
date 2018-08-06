/* test.c -- test the routines in basic.lib */
/* Program list: test.c and basic.lib       */

#include <stdio.h>

main()
{
    static char string[] = "This is a test.";
    char *cp, *Leftstr(), *Midstr(), *Rightstr();

    printf("Testing: \"%s\"\n", string);

    if ((cp = Leftstr(string, 4)) == NULL)
        {
        printf("Error in Leftstr()\n");
        exit(1);
        }
    printf("Leftstr() returned: \"%s\"\n", cp);

    if ((cp = Midstr(string, 4, 5)) == NULL)
        {
        printf("Error in Midstr()\n");
        exit(1);
        }
    printf("Midstr() returned: \"%s\"\n", cp);

    if ((cp = Rightstr(string, 5)) == NULL)
        {
        printf("Error in Rightstr()\n");
        exit(1);
        }
    printf("Rightstr() returned: \"%s\"\n", cp);
}

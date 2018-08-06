/* ibmiq.c -- a short dialog                          */

#include <stdio.h>
main()
{
    char name[80];
    int iq;

    printf("Enter your first name: -> ");
    scanf("%s", name);
    printf("Enter your IQ: -> ");
    scanf("%d", iq);
    printf("Well, %s, my IQ is %d!", name, 2 * iq - 1 );
}

/* acme.c  --     illustrate an assortment of the     */
/*                C library string-handling routines  */

#include <stdio.h>         /* for NULL */
#include <string.h>        /* for strchr(), et al */

#define NAME_PATTERN \
"first<space>last  or\n\
first<space>middle<space>last"

#define ADDRESS_PATTERN \
"number<space>street<comma><space>city<comma>"

char Buf[BUFSIZ];        /* global I/O buffer */

main()
{
    char *ocp, *cp, *first, *last, *street, *city;
    void Prompt(), Cant();

    printf("Acme Employment Questionaire\n");

    /*
     * Expect first<space>last or
     *        first<space>middle<space>last
     */
    Prompt("Full Name");
     /* search forward for a space */
    if ((cp = strchr(Buf,' ')) == NULL)
        Cant("First Name", NAME_PATTERN);
    *cp = '\0';
    first = strdup(Buf);
    *cp = ' ';

     /* Search back from end for a space */
    if ((cp = strrchr(Buf,' ')) == NULL)
        Cant("Last Name", NAME_PATTERN);
    last = strdup(++cp);

    /*
     * Expect number<space>street<comma><space>city<comma>
     */
    Prompt("Full Address");
     /* search forward for a comma */
    if ((cp = strchr(Buf,',')) == NULL)
        Cant("Street", ADDRESS_PATTERN);
    *cp = '\0';
    street = strdup(Buf);

     /* Search forward from last comma for next comma */
    if ((ocp = strchr(++cp,',')) == NULL)
        Cant("City", ADDRESS_PATTERN);
    *ocp = '\0';
    city = strdup(++cp);
    
    printf("\n\nYou Entered:\n");
    printf("\tFirst Name: \"%s\"\n", first);
    printf("\tLast Name:  \"%s\"\n", last);
    printf("\tStreet:     \"%s\"\n", street);
    printf("\tCity:       \"%s\"\n", city);

}

void Cant(char *what, char *pattern)
{
    printf("\n\n\bFormat Error!!!\n");
    printf("Can't parse your %s.\n", what );
    printf("Expected an entry of the form:\n\n");
    printf("%s\n\nAborted\n", pattern);
    exit(1);
}

void Prompt(char *str)
{
    while (1)
        {
        printf("\n%s: ", str );
        if (gets(Buf) == NULL || *Buf == '\0')
            {
            printf("Do you wish to quit? ");
            if (gets(Buf) == NULL || *Buf == 'y')
                exit (0);
            continue;
            }
        break;
        }
}

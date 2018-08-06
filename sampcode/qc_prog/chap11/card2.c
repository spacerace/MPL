/* card2.c --  demonstrates structure assignment and   */
/*             how to pass a structure to a function   */

#include <stdio.h>      /* for NULL  and stdin */
#include <string.h>     /* for strdup()        */

#define MAXN 79

struct cardstruct {                 /* global pattern */
    char *first, *last, *middle;
    long street_num;
    char *street, *city, *state;
    long zip;
    int  area_code;
    long phone;
};

main()
{
    int  i;
    char *Str_Input();
    long Lint_Input();
    struct cardstruct card1, card2;

    for (i = 0; i < 2; i++) /* do twice */
        {
        printf("\nCard %d:\n\n", i + 1);

        card1.first         = Str_Input("First Name");
        card1.last          = Str_Input("Last Name");
        card1.middle        = Str_Input("Middle Name");
        card1.street_num    = Lint_Input("Street Number");
        card1.street        = Str_Input("Street Name");
        card1.city          = Str_Input("City");
        card1.state         = Str_Input("State");
        card1.zip           = Lint_Input("Zip Code");
        card1.area_code     = (int)Lint_Input("Area Code");
        card1.phone         = Lint_Input("Phone Number");

        if (i == 0)
            card2 = card1;      /* structure assignment */
        }
    Showcard(card2);
    Showcard(card1);

}

Showcard(struct cardstruct card)
{
    printf("\n\n");

    printf("%s %s %s\n", card.first, card.middle, 
            card.last);
    printf("%ld %s, %s, %s %ld\n", card.street_num, 
            card.street, card.city, card.state,
            card.zip);
    printf("(%d) %ld\n", card.area_code, card.phone);
}

char *Str_Input(char *prompt)
{
    char buf[MAXN + 1], *ptr;

    printf("%s: ", prompt);
    if (fgets(buf, MAXN, stdin) == NULL)
        exit(0);
    buf[strlen(buf) - 1] = '\0'; /* strip '\n' */
    if (strlen(buf) == 0)
        exit(0);
    if ((ptr = strdup(buf)) == NULL)
        exit(0);
    return (ptr);
}

long Lint_Input(char *prompt)
{
    char buf[MAXN + 1];
    long num;

    printf("%s: ", prompt);
    if (fgets(buf, MAXN, stdin) == NULL)
        exit(0);
    if (sscanf(buf, "%ld", &num) != 1)
        exit(0);
    return (num);
}

/* rolo.c  --  demonstrates pointers to structures     */

#include <stdio.h>      /* for NULL  and stdin */
#include <string.h>     /* for strdup()        */

#define MAXN 79
#define MAXCARDS 3

struct cardstruct {                 /* global pattern */
    char first[MAXN],
         last[MAXN],
         middle[MAXN];
    unsigned long street_no;
    char street[MAXN],
         city[MAXN],
         state[MAXN];
    unsigned long zip;
    unsigned int area;
    unsigned long phone;
};

struct cardstruct cards[MAXCARDS];

main()
{
    int i;

    for (i = 0; i < MAXCARDS; ++i)
        {
        printf("\n<card %d of %d>\n", i + 1, MAXCARDS);
        Input(&cards[i]);
        }
    for (i = 0; i < MAXCARDS; ++i)
        {
        printf("\n<%d> ", i + 1);
        Showcard(&cards[i]);
        }
}

Input(struct cardstruct *cardp)
{
    char *Str_Input();
    long Lint_Input();

    strcpy(cardp->first,Str_Input("First Name"));
    strcpy(cardp->last,Str_Input("Last Name"));
    strcpy(cardp->middle,Str_Input("Middle Name"));
    cardp->street_no = Lint_Input("Street Number");
    strcpy(cardp->street,Str_Input("Street"));
    strcpy(cardp->city,Str_Input("City"));
    strcpy(cardp->state,Str_Input("State"));
    cardp->zip = Lint_Input("Zip Code");
    cardp->area = (int)Lint_Input("Area Code");
    cardp->phone = Lint_Input("Phone Number");
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
    long  num;

    printf("%s: ", prompt);
    if (fgets(buf, MAXN, stdin) == NULL)
        exit(0);
    if (sscanf(buf, "%ld", &num) != 1)
        exit(0);
    return (num);
}

Showcard(struct cardstruct *cardptr) 
{
    printf("\n\n");
    printf("%s %s %s\n", cardptr->first, cardptr->middle, 
            cardptr->last);
    printf("%ld %s, %s, %s %ld\n", cardptr->street_no, 
            cardptr->street, cardptr->city, cardptr->state,
            cardptr->zip);
    printf("(%d) %ld\n", cardptr->area, cardptr->phone);
}

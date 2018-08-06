/* rolo2.c --  demonstrates a linked list */

#include <stdio.h>      /* for NULL  and stdin */
#include <string.h>     /* for strdup()        */
#include <malloc.h>     /* for malloc()        */

#define MAXN 79

struct cardstruct {          /* global pattern */
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
    struct   cardstruct *nextcard;
};

main()
{
    int i;
    struct cardstruct card, *first, *current;

    first = (struct cardstruct *)malloc(sizeof(struct cardstruct));
    if(first == NULL)
        exit(1);
    if (Input(&card) != 0)
        exit(1);
    *first = card;
    current = first;

    while (Input(&card) == 0)
        {
        current->nextcard =
            (struct cardstruct *)malloc(sizeof(struct cardstruct));
        if(current->nextcard == NULL)
            exit(1);
        current = current->nextcard;
        *current = card;
        }
    current->nextcard = NULL;

    Dumplist(first);
}

Dumplist(struct cardstruct *head)
{
    do
        {
        Showcard(head);
        } while ((head = head->nextcard) != NULL);
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

Input(struct cardstruct *cardp)
{
    char *Str_Input();
    long Lint_Input();

    printf("\n<new card> (Empty first name Quits)\n");
    strcpy(cardp->first,Str_Input("First Name"));
    if (*(cardp->first) == '\0')
        return (1);
    strcpy(cardp->last,Str_Input("Last Name"));
    strcpy(cardp->middle,Str_Input("Middle Name"));
    cardp->street_no = Lint_Input("Street Number");
    strcpy(cardp->street,Str_Input("Street"));
    strcpy(cardp->city,Str_Input("City"));
    strcpy(cardp->state,Str_Input("State"));
    cardp->zip = Lint_Input("Zip Code");
    cardp->area = (int)Lint_Input("Area Code");
    cardp->phone = Lint_Input("Phone Number");
    return (0);
}

char *Str_Input(char *prompt)
{
    char buf[MAXN + 1], *ptr;

    printf("%s: ", prompt);
    if (fgets(buf, MAXN, stdin) == NULL)
        exit(0);
    buf[strlen(buf) - 1] = '\0'; /* strip '\n' */
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
        num = 0;
    return (num);
}

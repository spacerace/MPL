/* udemo.c  --  demonstrates a union at work */

#include <stdio.h>

char *Strings[6] = {
        "Quit",
        "line of text",
        "floating point double value",
        "long integer value",
        "floating point value",
        "integer value"
};

struct Unitstruct {
    union {
        char   wtype[BUFSIZ];
        double dtype;
        long   ltype;
        float  ftype;
        int    itype;
    } manyu;
    int type_in_union;
};

main()
{
    struct Unitstruct one_of_many;

    while ((one_of_many.type_in_union = Menu()) != 0)
        {
        Inputval(&one_of_many);
        Printval(&one_of_many);
        }
}

Inputval(struct Unitstruct *one_of_many)
    {
    printf("\nEnter a %s: ", Strings[one_of_many->type_in_union]);
    switch(one_of_many->type_in_union)
        {
        case 1: 
            fgets(one_of_many->manyu.wtype, BUFSIZ, stdin);
            break;
        case 2:
            scanf("%lf", &(one_of_many->manyu.dtype));
            while (getchar() != '\n');
            break;
        case 3:
            scanf("%ld", &(one_of_many->manyu.ltype));
            while (getchar() != '\n');
            break;
        case 4:
            scanf("%f", &(one_of_many->manyu.ftype));
            while (getchar() != '\n');
            break;
        case 5:
            scanf("%i", &(one_of_many->manyu.itype));
            while (getchar() != '\n');
            break;
        }
}

Printval(struct Unitstruct *one_of_many)
{
    printf("The %s you entered\nwas: ", Strings[one_of_many->type_in_union]);
    switch (one_of_many->type_in_union)
        {
        case 1: 
            fputs(one_of_many->manyu.wtype, stdout);
            break;
        case 2:
            printf("%lf", one_of_many->manyu.dtype);
            break;
        case 3:
            printf("%ld", one_of_many->manyu.ltype);
            break;
        case 4:
            printf("%f", one_of_many->manyu.ftype);
            break;
        case 5:
            printf("%i", one_of_many->manyu.itype);
            break;
        }
    printf("\n\n");
}

Menu()
{
    int i;
    char ch;

    for (i = 0; i < 6; ++i)
        {
        printf("%d) %s\n", i, Strings[i]);
        }
    printf("Which: ");
    do
        {
        ch = getch();
        } while (ch < '0' || ch > '5');
    printf("%c\n", ch);
    return (ch - '0');
}

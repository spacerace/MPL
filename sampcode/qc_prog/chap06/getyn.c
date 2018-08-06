/* getyn.c -- calls char function getyn()    */
/*               with error checking         */

#define TRUE 1

main()
{
    char ch;
    char getyn();

    printf("Do you want to continue? ");
    if ((ch = getyn()) == 'y')
        printf("Answer was y\n");
    else
        printf("Answer was n\n");
    printf("Value of ch was %c\n", ch);
}

char getyn()
{
    char ch;
    while (TRUE)
        {
        printf(" (y or n) ");
        ch = getche();
        printf("\n");
        if ((ch == 'y') || (ch == 'n'))
        /* valid response, break out of loop */
            break;
        /* give error message and loop again */
        printf("please enter ");
        }
    return(ch);
}

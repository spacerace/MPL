/* if.c -- simple IF statement */

char ch;
main()
{
    printf("Do you want to continue y/n? "); /* prompt */
    if (ch = getche() == 'y')
        printf("\nLet's continue ...\n");    /* if true */
    printf("\nAll done.\n");                 /* always executed */
}

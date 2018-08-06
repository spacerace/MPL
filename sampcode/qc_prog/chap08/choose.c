/* choose.c   --  an array of pointers to functions   */
/*                used to create a menu               */

void Choice1(), Choice2(), Choice3();

void (*Dochoice[3])() = {Choice1, Choice2, Choice3};

main()
    {
    int ch;

    printf("Select 1, 2 or 3: ");
    ch = getch(); putch(ch);
    ch -= '1';
    if (ch < 0 || ch > 2)
        printf("\nNo such choice.\n");
    else
        Dochoice[ch]();

}

void Choice1(void) 
{
        printf("\nThis is choice 1\n");
}

void Choice2(void) 
{
        printf("\nThis is choice 2\n");
}

void Choice3(void) 
{
        printf("\nThis is choice 3\n");
}

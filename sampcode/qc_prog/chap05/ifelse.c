/* ifelse.c -- IF with ELSE */

char ch;
int num;
main()
{
    printf("Are you a new user? y/n? ");
    if (ch = getche() == 'y')
        {
        /* executed if IF is true */
        printf("\n\nYou must register to use this\n");
        printf("bulletin board. Please read\n");
        printf("Bulletin #1 first. Thank You.\n");
        }
    else
        /* executed if IF is false */
        {
        printf("\n\nEnter your secret number: ");
        scanf("d", &num);
        }
}

/* change2.c -- modified to demonstrate passing       */
/*              an address to a function              */

#define NCOINS (4)
#define CENT (0x9b)  /* IBM PC cent character */

main()
{
    static int coins[NCOINS] = {25, 10, 5, 1};
    int pennies;
    float amount;

    printf("Enter an amount and I will ");
    printf(" give you change.\nAmount: ");
    if (scanf("%f", &amount) != 1)
        {
        printf("I don't know how to change that!\n");
        exit(1);
        }
    pennies = (int)(amount * 100.0);

    Show_change( coins, &coins[NCOINS], pennies);

}

Show_change( int amts[], int *end, int due)
{
    int count;

    while ( amts < end )    /* compare pointers */ 
        {
        count = 0;
	while ((due -= *amts) >= -1)
            {
            ++count;
            }
        if (count > 0)
            printf("%4d %2d%c\n", count, *amts, CENT);
        if (due == 0)
            break;
        due += *amts;

        ++amts;             /* increment a pointer */
        }
}

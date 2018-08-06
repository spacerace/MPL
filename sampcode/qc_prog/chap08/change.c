/* change.c  -- a changemaking program demonstrates   */
/*              how pointers advance the correct      */
/*              number of bytes based on type         */

#define NCOINS (4)
#define CENT (0x9b)  /* IBM PC cent character */
#define WAIT printf("(Press any key to continue)"); \
             getch(); printf("\n\n")  

main()
{
    static int coins[NCOINS] = {25, 10, 5, 1};
    int *coin_ptr, i = 0; 
    int pennies1, pennies2, count;
    float amount;

    printf("Enter an amount and I will ");
    printf(" give you change.\nAmount: ");
    if (scanf("%f", &amount) != 1)
        {
        printf("I don't know how to change that!\n");
        exit(1);
        }
    pennies2 = pennies1 = (int)(amount * 100.0);

    coin_ptr = coins;
    for (i = 0; i < NCOINS; ++i)
        {
        WAIT;
        count = 0;
	while ((pennies1 -= coins[i]) >= -1)
            ++count;
        if (count > 0)
            {
            printf("%4d %2d%c", count, coins[i], CENT);
            printf(" coins by array offset.\n");
            }
        if (pennies1 == 0)
            break;
        pennies1 += coins[i];

        count = 0;
	while ((pennies2 -= *coin_ptr) >= -1)
            ++count;
        if (count > 0)
            {
            printf("%4d %2d%c", count, *coin_ptr, CENT);
            printf(" coins by pointer indirection.\n");
            }
        if (pennies2 == 0)
            break;
        pennies2 += *coin_ptr;
        ++coin_ptr;
        }
}

/* expo.c -- uses exp() function to  */
/*            calculate powers       */

main()
{
    int expo(number, power);
    int number, power;

    printf("Enter a number: ");
    scanf("%d", &number);
    printf("Raise to what power? ");
    scanf("%d", &power);

    printf("Result: %d", expo(number, power));
}

int expo(number, power)
{
    int count, value;
    int total = 1;      /* store value of calculation */
    if (power < 0)      /* reject negative exponents  */
        {
        printf("Error in expo(): negative exponent\n");
        return(0);
        }

    if (power == 0) /* any number to 0 power is 1 */
        return(1);

    if (power == 1) /* any number to 1 power is itself */
        return(number);

    /* calculate for power > 1 */
    for (count = 1; count <= power; count++)
        total *= number;
    return(total);
}

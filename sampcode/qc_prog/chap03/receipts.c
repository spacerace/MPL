/* receipts.c -- calculates gross and net */
/*               receipts on sales        */

main()
{
    int units = 38;       /* number sold */
    float price = 149.99, /* price per item */
    rate = 0.06;          /* sales tax rate */

    /* variables to hold calculated totals */
    float gross, tax, net;

    /* perform calculations */
    net = units * price;
    tax = net * rate;
    gross = net + tax;

    /* print results */
    printf("\tSales Report\n");
    printf("Net sales: \t%6.2f\n", net);
    printf("Tax:\t\t %5.2f\n", tax);
    printf("Gross sales:\t%6.2f\n", gross);
}

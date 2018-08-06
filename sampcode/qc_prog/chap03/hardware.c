/* hardware.c -- shows a mixture of int, */
/*               float, and char types   */

main()
{
    int threads    = 8;       /* threads per inch */
    float length   = 1.25,    /* length in inches */
          diameter = 0.425,   /* diameter in inches */
          price    = 0.89;    /* price per hundred */
    char bin = 'A';           /* kept in bin A */
    long quantity = 42300;    /* number in bin */

    printf("Screws: %d threads/inch\n%f inches long\n",
            threads, length);
    printf("%f diameter\n\n", diameter);
    printf("Price per 100: %f\n", price);
    printf("Stored in bin: %c\nQuantity on hand: %ld",
            bin, quantity);
}

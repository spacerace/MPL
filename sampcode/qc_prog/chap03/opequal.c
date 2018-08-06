/* opequal.c -- shows combination math/assignment */
/*              operators and increment operators */

main()
{
    int m = 10, n = 5;
    printf("Starting values: m = %d n = %d\n",
            m, n);

    /* combination of arithmetic and assignment */
    printf("m += 2 makes m %d\n", m += 2);
    printf("m -= n makes m %d\n", m -= n);
    printf("m *= 2 makes m %d\n", m *= 2);

    /* two ways to increment m */
    printf("m = m + 1 makes m %d\n",
            m = m + 1);
    printf("m += 1 makes m %d\n",
            m += 1);
}

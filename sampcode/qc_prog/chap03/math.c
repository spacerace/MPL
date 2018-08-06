/* math.c -- shows arithmetic and       */
/*           precedence via expressions */

main()
{
    int a = 10, b = 4, c = 2;

    /* simple arithmetic expressions */
    printf("99 + 2 = %d\n", 99 + 2);  /* ints */
    printf("5 - 12 = %d\n", 5 - 12);
    printf("7.25 + 3.5 = %f\n", 7.25 + 3.5);
                                        /* floats */

    /* compare presedence on these */
    printf("20 * 20 + 40 = %d\n", 20 * 20 + 40);
    printf("20 * (20 + 40) = %d\n", 20 * (20 + 40));
    printf("a * a - c + b = %d\n", a * a - c + b);
    printf("a * (a - (c + b)) = %d\n",
            a * (a - (c + b)));

    /* compare integer and float division */

    printf("Integers: 5 / 2 = %d\n", 5 / 2);
    printf("Floats: 5.0 / 2.0 = %f\n", 5.0 / 2.0);
}

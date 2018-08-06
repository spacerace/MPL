/* relation.c -- shows effect of      */
/*               relational operators */

main()
{
    int a = 5, b = 3, c = 4;
    printf("a = %d\t b = %d\t c = %d\n", a, b, c);

    printf("Expression a > b has a value of %d\n",
            a > b);
    printf("Expression a == c has a value of %d\n",
            a == c);
    printf("Expression a > (b + c) has a value of %d\n",
            a > (b + c));
    printf("Expression a = b has a value of %d\n",
            a = b); /* what happened here? */
}

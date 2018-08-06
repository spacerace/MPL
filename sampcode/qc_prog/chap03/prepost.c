/* prepost.c -- shows effect of pre- */
/*              and post-increments  */
/*              and decrements       */

main()
{
    int b = 100;

    printf("b is %d\n", b);
    printf("b++ is still %d\n", b++);
    printf("but after it's used, ");
    printf("b is incremented to %d\n\n, b);

    printf("++b, on the other hand, ");
    printf("is immediately %d\n", ++b);
}

/* sadd.c -- a small adding machine that illustrates  */
/*           the need for array bounds checking.      */

main()
{
    int offset = 0, i, result = 0;
    int stack[3];

    while (scanf("%d", &stack[offset]) == 1)
        {
        ++offset;
        }
    for (i = 0; i < offset; ++i)
        {
        result += stack[i];
        }
    printf("-------------\n");
    printf("%d\n", result);

}

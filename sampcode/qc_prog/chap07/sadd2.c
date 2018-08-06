/* sadd2.c -- a small adding machine that includes */
/*            array bounds checking.               */

#define MAXSTAK 3

main()
{
    int offset = 0, i, result = 0;
    int stack[MAXSTAK];

    while (scanf("%d", &stack[offset]) == 1)
        {
        if (++offset >= MAXSTAK)
            {
            printf("Stack Full\n");
            break;
            }
        }
    for (i = 0; i < offset; ++i)
        {
        result += stack[i];
        }
    printf("-------------\n");
    printf("%d\n", result);

}

/* recurse.c -- demonstrates recursion */

int level = 1; /* recursion level */
main()
{
    int num, result;

    printf("Factorial of what number? ");
    scanf("%d", &num);
    result = factorial(num);
    printf("Result is: %d\n", result);
}

factorial(number)
{
    int result;
    printf("entering: ");
    printf("level %d. number = %d. &number = %d\n",
            level++, number, &number);

    if (number == 0)
        result = 1;
    else
        result = number * factorial(number - 1);

    printf("exiting : ");
    printf("level %d. number = %d. &number = %d. ",
            --level, number, &number);
    printf("result = %d\n", result);

    return(result);
}

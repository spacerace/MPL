/* mixtypes.c -- shows problem with calling           */
/*               a function with wrong type parameter */

main()
{
    /* didn't bother to declare int function */
    float n = 5.0;
    int i;

    printf("n in main() is %f\n", n);
    i = examine(n);    /* pass float to function */
    printf("examine() returned n as %d\n", i);
}

examine(num)  /* function didn't declare return type */
{
    printf("examine() says n is %d\n", num);
    return(num);
}


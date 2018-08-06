/* proto.c -- demonstrate function prototyping */
/*            and parameter checking           */

main()
{
    float n = 9995.997;
    int i;
    int examine(int num);  /* declare function */
                           /* with prototype   */

    printf("n in main() is %f\n", n);
    i = examine(n);    /* pass float to function */
    printf("examine() returned n as %d\n", i);
}

int examine(num)
{
    printf("examine() says n is %d\n", num);
    return(num);
}

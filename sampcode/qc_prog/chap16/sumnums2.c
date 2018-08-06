/* sumnums2.c -- type mismatch in function arguments  */
/*               Function prototyping              */

int sums(int, int);
main()
{
    float a = 10.0;
    float b = 20.0;
    int c;

    c = sums(a, b);
    printf("sum of %.1f and %.1f is %d\n", a, b, c);
}
int sums(x, y)
int x, y;
{
    return ( x + y);
}


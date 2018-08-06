/* specs.c -- shows printf() format  */
/*            specifiers for numbers */

main()
{
    int    i = 122;       /* ASCII code for 'z' */
    long   l = 93000000;  /* distance to Sun (miles) */
    float  f = 192450.88; /* someone's bottom line */
    double d = 2.0e+030;  /* mass of Sun (kg.) */

    printf("%d\n", i);  /* integer as decimal */
    printf("%x\n", i);  /* integer as hex */
    printf("%ld\n", l); /* long */
    printf("%f\n", f);  /* float as decimal */
    printf("%e\n", f);  /* float as exponential */
    printf("%f\n", d);  /* double as decimal */
    printf("%d\n", d);  /* double as exponential */
}

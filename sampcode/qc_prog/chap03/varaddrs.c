/* varaddrs.c -- use & operator to get     */
/*               addresses of variables    */

main()
{
    char c1, c2;
    int i;
    long l;
    float f;
    double d;

    printf("Address of c1 %d\n", &c1);
    printf("Address of c2 %d\n", &c2);
    printf("Address of i  %d\n", &i);
    printf("Address of l  %d\n", &l);
    printf("Address of f  %d\n", &f);
    printf("Address of d  %d\n", &d);
}

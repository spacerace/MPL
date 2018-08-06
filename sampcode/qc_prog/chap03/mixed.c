/* mixed.c -- shows the effects of mixing */
/*            data types in an expression */

main()
{
    int     i = 50, iresult;
    long    l = 1000000, lresult;
    float   f = 10.5, fresult;
    double  d = 1000.005, dresult;

    fresult = i + f;         /* int + float to float */
    printf("%f\n", fresult);

    fresult = i * f;         /* int * float to float */
    printf("%f\n", fresult);

    lresult = l + f;         /* long + int to long */
    printf("%f\n", lresult);

    printf("%f\n", d * f);   /* double * float */
                                 /* to double */
    fresult = d * f;         /* assigned to a float */
    printf("%f\n", fresult); /* loses some precision */

    /* debugging a division problem */

    iresult = i / l;          /* int / long to int */
    printf("%d\n", iresult);  /* whoops! loses result */
    printf("%ld\n", iresult); /* this won't fix it */
    fresult = i / l;          /* store in float result */
    printf("%f\n", fresult);  /* doesn't work */
    dresult = i / l;          /* try a double */
    printf("%f\n", dresult);  /* doesn't work */
    fresult = (float) i / l;  /* try type cast */
    printf("%f\n", fresult);  /* correct result */
}

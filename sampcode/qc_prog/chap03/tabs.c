/* tabs.c -- shows formatting with the \t */
/*           tab escape sequence          */

main()
{
    int    q1 = 338, q2 = 57, q3 = 1048, q4 = 778,
           /* quantity in bin */
           t1 = 6, t2 = 8, t3 = 12, t4 = 16;
           /* threads per inch */

    float  s1 = 0.250, s2 = 0.500, s3 = 0.750, s4 = 1.0;
           /* size in inches */

    /* print table header */
    printf("number\t\t size\t\t threads\n");
    printf("in bin\t\t (inches)\t per inch\n\n");

    /* print lines of table */
    printf("%d\t\t %f\t %d\n", q1, s1, t1);
    printf("%d\t\t %f\t %d\n", q2, s2, t2);
    printf("%d\t\t %f\t %d\n", q3, s3, t3);
    printf("%d\t\t %f\t %d\n", q4, s4, t4);
}

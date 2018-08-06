/* shortif.c -- shows 'shorthand' IF / ELSE   */
/*           -- gets absolute value of number */

main()
{
    int num, pos, abs;
    printf("Enter a whole number: ");
    scanf("%d", &num);

    pos = (num >= 0); /* is number positive? */

    abs = (pos) ? num : -num;  /* assigns negative of */
                      /* number if number is negative */
    if (pos)
        printf ("The number is positive.\n");
    else
        printf("The number is negative.\n");
    printf("Absolute value of number is: %d\n", abs);
}

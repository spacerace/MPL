/* avgtemp.c -- finds average temperature */
/*              for the week              */

main()
{
    int t1, t2, t3, t4, t5, t6, t7;
    float avg;

    printf("Enter the high temperature for:\n");
    printf("Monday: ");
    scanf("%d", &t1);
    printf("Tuesday: ");
    scanf("%d", &t2);
    printf("Wednesday: ");
    scanf("%d", &t3);
    printf("Thursday: ");
    scanf("%d", &t4);
    printf("Friday: ");
    scanf("%d", &t5);
    printf("Saturday: ");
    scanf("%d", &t6);
    printf("Sunday: ");
    scanf("%d", &t7);

    /* calculate and display average */
    avg = (t1 + t2 + t3 + t4 + t5 + t6 + t7) / 7.0;
          /* divide by 7.0 to ensure float result */
    printf("The average high temperature for");
    printf(" this week was %5.2f degrees.\n", avg);
}

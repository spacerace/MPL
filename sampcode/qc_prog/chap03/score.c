/* score.c -- defines and prints   */
/*            int and long vars    */

main()
{
    /* declare some int variables and assign values */
    /* to them in the same statement                */

    int home = 5, visitors = 2, inning =7, attendance = 31300;
    long total_attendance = 1135477;  /* long int */

    /* print out the values */

    printf("The score after %d innings is \n", inning);
    printf("Home team %d, Visitors %d.\n\n", home, visitors);
    printf("The attendance today is %d.\n", attendance);
    printf("Attendance this year to date is %ld.",
            total_attendance);
}

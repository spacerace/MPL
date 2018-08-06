/* alert.c -- sounds alarm by calling a        */
/*            beep() function with a parameter */

main()
{
    void beep(times);  /* function declaration */
    printf("*** Alert! Alert! ***\n");
    beep(3);     /* call beep() with parameter */
}

void beep(times)
    int times;   /* declare function parameter */
{
    int count;

    /* check that parameter is between 1 and 4 */
    if ((times < 1) || (times > 4))
        {
        printf("Error in beep(): %d beeps specified.\n",
                times);
        printf("Specify one to four beeps");
        }
    else /* sound the beeps */
        for (count = 1; count <= times; count++)
            printf("\a");  /* "alert" escape sequence */
}

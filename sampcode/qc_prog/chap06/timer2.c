/* timer2.c -- interval timer             */
/*             calls delay(), uses beep() */

main()
{
    /* function declarations */
    void beep (times);
    void delay (seconds);

    /* variable declarations */
    int seconds, interval, tick;

    printf("Set for how many seconds? ");
    scanf("%d", &seconds);
    printf("Interval to show in seconds? ");
    scanf("%d", &interval);
    printf("Press a key to start timing\n");
    getch();

    tick = 0;                /* run "clock" for */
    while (tick < seconds)   /* time specified  */
        {
        delay(interval); /* wait interval seconds */
        tick += interval;
        printf("%d\n", tick);
        beep(1);
        }
    beep(3);
}

void delay(seconds)
               /* wait for number of seconds specified */
               /* see TIMER.C in chapter 4 for details */
               /* on the library function time().      */

int seconds;   /* parameter declaration */
{
    /* variable declarations */
    long start, end, /* starting and ending times */
                     /* measured in seconds since */
                     /* Jan. 1, 1970 */
         ltime;      /* used to get val from time function */

    start = time(&ltime);  /* get system elapsed seconds */
                           /* since 1-1-70 */
    end = start + seconds; /* calculate alarm time */

    do
       ;                          /* null statement for loop body */
    while (time(&ltime) < end);   /* wait for end of time  */
}

void beep(times)
    /* parameter declaration */
int times;
{
    /* variable declaration  */
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

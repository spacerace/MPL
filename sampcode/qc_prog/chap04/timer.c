
/* TIMER.C -- uses do loop to    */
/*            check elapsed time */

#include <time.h>

main()
{
    long start, end, /* starting and ending times */
                     /* measured in seconds since */
                     /* Jan. 1, 1970 */
         ltime;      /* used to get val from time function */
    int seconds;     /* elapsed time to be set */

    printf("QuickC Egg Timer\n");
    printf("Enter time to set in seconds: ");
    scanf("%d", &seconds);
    start = time(&ltime);  /* get system elapsed seconds */
                           /* since 1-1-70 */
    end = start + seconds; /* calculate alarm time */

    do
       ;                       /* null statement for loop body */
    while (time(&ltime) < end);   /* wait for alarm time  */

    printf("Time's Up!\a\a\a\n");
}



/*
** BYTE Sieve Benchmark
** Version 1 for 8088/8086/80286/80386
** March 1988
** Written in BYTE Small-C
** Based on Small-C by J.E. Hendrix
**
** This program executes the infamous Eratosthenes Sieve Prime
** Number Program from BYTE, Jan. 1983.
**
** Operation:
** 1. Turn on stopwatch
** 2. Execute SIEVE for LOOP iterations
** 3. Turn off stopwatch
** 4. Report time and number of primes found
** 5. Exit
**
*/

#include stdio.h

#define size  8190
#define LOOP  100
#define TRUE  YES
#define FALSE NO

int tblock[4];		/* Timer holding array */
char flags [size + 1];

main()

{
    int i, prime, k, count, iter;

/* Announce yourself */
    printf("BYTE Sieve Benchmark\n");

    printf("%d iterations\n",LOOP);

/* Start timer and execute loop */
    gtime(tblock);
    for (iter = 1; iter <= LOOP; iter++)
          {
          count = 0;                              /* prime counter */
          for (i = 0; i <= size; i++)             /* set all flags true */
                flags [i] = TRUE;
          for (i = 0; i <= size; i++)
               {
                    if (flags [i])                /* found a prime */
                    {
                    prime = i + i + 3;            /* twice index + 3 */
/*                  printf ("\n%d", prime);  */
                    for (k = i + prime; k <= size; k+= prime)
                                 flags [k] = FALSE;  /* kill all multiple */
                    count++;                         /* primes found */
                    }
                }
      }
      calctim(tblock);

/* Report results */
     printf("Results:  (HH:MM:SS:1/100ths)\n");
     printf("Elapsed time: %d:%d:%d:%d\n\n",tblock[0],tblock[1],
             tblock[2],tblock[3]);

     printf ("%d primes.\n\n", count);     /* primes found on 100th pass */

/* Exit */
     printf("Press RETURN to exit:");
     fgetc(stdin);
     exit(0);
 }

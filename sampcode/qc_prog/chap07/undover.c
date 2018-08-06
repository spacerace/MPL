/* undover.c -- illustrates the effect of underinitializing and */
/*              overinitializing arrays.                        */

int Primes[6] = { 1, 2, 3, 5, 7, 11 };
#define NUMP (sizeof(Primes)/sizeof(int))

main()
{
    int i;

    printf("The first %d primes are: ", NUMP);
    for (i = 0; i < NUMP; ++i)
        {
        printf("%d ", Primes[i]);
        }
    printf("\n");
}

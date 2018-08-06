/* asimov.c -- illustrates how to initialize an      */
/*             array with starting values            */

#define MAXL 16
char Letters[MAXL] = { 
    'e', 'I', 'a', 'N', 'o', 'R', 'O', 'o',
    'u', 't', 'o', 'R', 'l', 'o', 'B', 'b', 
};

main()
{
    int num, i;

    printf("Guess my identity with numbers.\n");
    printf("(any non number quits)\n\n");

    while (scanf("%d", &num) == 1)
        {
        if (num <= 0)
            {
            printf("Guesses must be above zero\n");
            continue;
            }
        for (i = 1; i <= num; ++i)
            {
            printf("%c", Letters[(i * num) % MAXL]);
            }
        printf("\n");
        }
}

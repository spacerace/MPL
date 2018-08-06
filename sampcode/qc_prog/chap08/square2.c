            /* LISTING 8-2 */

/* square.c  --  A quiz to demonstrate passing        */
/*               pointers and addresses in functions. */

#include <stdio.h>

main()
    {
    int val, count, guess;

    for (count = 1; count < 255; ++count)
        {
        val = count;
        printf("\nWhat is the square of %d?\n", val);
        if (scanf(" %d", &guess) != 1)
            return(0);           /* non-number exits   */

        if(Square(&val) != 0)    /* pass val's address */
            {
            printf("Range Error\n");
            exit(1);
            }

        if (val != guess)
            printf("Wrong. It is %d.\n", val);
        else
            printf("Right!\n");


        printf("Continue? ");
        if (getche() != 'y')
            break;
        }
    return (0);
    }

int Square(int *where)
    {
    if (*where > 181 || *where < -181)
        return (-1);
    *where = (*where) * (*where);
    return (0);
    }

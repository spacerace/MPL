/* switch.c -- demonstrates switch statement */
/*             prints values according       */
/*             to user's choice              */

#include <math.h> /* for sqrt() */
#define TRUE 1
main()
{
    char choice;   /* routine wanted by user   */
    int number;    /* number entered by user   */

    while (TRUE)   /* endless loop */
    {
        printf("\nSelect value wanted:\n");
        printf("o = octal  h = hex   s = square\n");
        printf("r = square root  q = quit: ");
        choice = getche(); printf("\n");

        if (choice == 'q')
            break; /* exits WHILE loop; ends program */

        /* rest of program executed if base <> 'q' */
            printf("Enter a whole number: ");
            scanf("%d", &number);

        switch (choice) /* print according to */
                        /* choice requested   */
            {
            case 'o':   /* print octal */
                printf("Result: %o\n", number);
                break;  /* break here in each case    */
                        /* exits the switch statement */

            case 'h':   /* print hex */
                printf("Result: %x\n", number);
                break;

            case 's':   /* square */
                printf("Result: %d\n", number * number);
                break;

            case 'r':   /* square root */
                printf("Result: %f\n", sqrt(number) );
                break;

            default:
                printf("Choice must be o, h, s, r, or q\n");
            }
     }
}

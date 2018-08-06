/* bitout.c  -- compiles one way on an IBM-PC and      */
/*              another on a 68000 chip-based machine  */

#define CHIP_80286    /* don't define on a 68000 machine */
#include <stdio.h>

main()
{
    int num;

    printf("Enter an integer number and I will print"
           " it out in binary\nNumber: ");
    
    if (scanf("%d", &num) != 1)
        {
        fprintf(stderr, "Not an integer\n");
        exit(1);
        }
    Bitout(num);
}

Bitout(unsigned int num)
{
    int i, j;
    unsigned char *cp;

    cp = (char *)&num;

#if defined(CHIP_80286)    /* IBM-PC */
    for (i = 1; i >= 0; --i)
#endif
#if !defined(CHIP_80286)   /* otherwise 68000 machine */
    for (i = 0; i < 4; ++i)
#endif
        {
        for (j = 7; j >= 0; --j)
            putchar((cp[i] & (1 << j)) ? '1' : '0');
        }
    putchar('\n');
}

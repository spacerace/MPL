/* bitwise.c -- demonstrate the bitwise operators */

#include <stdio.h>

main()
{
    unsigned int val1, val2, result;
    int ch;
    extern void show();

    while(1)
        {
        printf("\nval1: ");
        if (scanf("%d", &val1) != 1)
            break;

        printf("val2: ");
        if (scanf("%d", &val2) != 1)
            break;

        printf("\tval1   = ");
        show(val1);
        printf("\tval2   = ");
        show(val2);

        printf("Bitwise Operator: ");
        while ((ch = getchar()) == '\n')
            {
            continue;
            }
        if (ch == EOF)
            break;
        switch (ch)
            {
            case '&':
                result = val1 & val2;
                printf("Executing: result = val1 & val2;\n");
                break;
            case '|':
                result = val1 |= val2;
                printf("Executing: result = val1 | val2;\n");
                break;
            case '^':
                result = val1 ^= val2;
                printf("Executing: result = val1 ^ val2;\n");
                break;
            case '~':
                result = ~val1;
                printf("Executing: result = ~val1;\n");
                printf("\tresult = ");
                show(result);
                result = ~val2;
                printf("Executing: result = ~val2;\n");
                break;
            case '<':
                result = val1 <<= val2;
                printf("Executing: result = val1 <<val2;\n");
                break;
            case '>':
                result = val1 >>= val2;
                printf("Executing: result = val1 >>val2;\n");
                break;
            case 'q':
            case 'Q':
                return(0);
              default:
                continue;
            }
        printf("\tresult = ");
        show(result);
        }
}

void bitout(unsigned char num[])
{
    int bytes = 2, i, j;

    /* IBM PC stores ints low/hi. */
    for (i = bytes-1; i >= 0; --i)
        {
        for (j = 7; j >= 0; --j)
            {
            putchar((num[i]&(1<<j))?'1':'0');
            }
        }
}

void show(unsigned int val)
{
    extern void bitout();

    printf("(%05u decimal)", val);
    bitout(&val);
    printf(" binary\n");
}

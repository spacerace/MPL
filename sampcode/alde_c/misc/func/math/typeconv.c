
/*
** Demonstration of Type Conversion
** across assignments.
*/
main()
        {
        char c1,c2,c3;
        int i1,i2,i3;
        float f1,f2,f3;

        c1 = 'x';       /* no conversion */
        c2 = 1000;      /* int constant demoted to char */
        c3 = 6.02e23;   /* float constant demoted to char */
        printf("%c  %c  %c\n",c1,c2,c3);

/* Note that the character value is printed as is; the integer
** with a value of 1000 is converted to its binary equivalent
** of 1111101000 and truncated to the first 8 data bits which
** gives 11101000 or decimal 232 or the Greek symbol "phi"
** when the ASCII symbol is printed; and the conversion from
** float to char is meaningless and does not occur.  */

        i1 = 'x';       /* char constant promoted to int */
        i2 = 1000;      /* no conversion */
        i3 = 6.02e23;   /* float constant demoted to int */
        printf("%d  %d  %d\n",i1,i2,i3);

/* Note that ASCII 'x' has an integer value of 120, and the
** character constant 'x' is promoted when we assign it to an
** integer.  The floating point constant is demoted to the
** largest integer 32767 that is possible in the Microsoft
** C compiler and that number is returned as an integer. */

        f1 = 'x';       /* x char constant promoted to float */
        f2 = 1000;      /* int constant promoted to float */
        f3 = 6.02e23;   /* no conversion */
        printf("%f %f %f\n",f1,f2,f3);

/* There are no demoted values, everything is represented as
** its double precision floating point equivalent!   */

        }



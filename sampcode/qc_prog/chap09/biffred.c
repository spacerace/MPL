/* biffred.c  --  strings in the string pool can be */
/*                manipulated via pointers          */

char Start[] = "start";

main()
{
    char *cp;
    int pass;

    for (pass = 0; pass < 2; ++pass)
        {
        printf("My name is FRED\n");

        cp = Start;        

        while (*cp != 'F')
            ++cp;

        *cp   = 'B';
        *++cp = 'I';
        *++cp = 'F';
        *++cp = 'F';
        }
}

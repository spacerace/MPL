/* phword.c   --  generate all the possible words     */
/*                in a phone number; demonstrates     */
/*                functions that return addresses     */

#define MAXD (7)    /* 7 digits max */

main()
{
    int digits[MAXD], ndigits = 0, line = 0;
    char *letters; 
    signed char digit;
    int a, b, c, d, e, f, g;
    extern char *Range();

    printf("Enter Phone Number (7 digits): ");
    do
        {
        digit = getch() - '0';
        if (digit == ('-' - '0'))
            continue;
        if (digit < 0 || digit > 9)
            {
            printf("\nAborted: Non Digit\n");
            return(1);
            }
        digits[ndigits++] = digit;
        printf("%d", digit);
        } while (ndigits < 7);
    printf("\n");

    for( a = 0; a < 3; ++a)
     for( b = 0; b < 3; ++b)
      for( c = 0; c < 3; ++c)
       for( d = 0; d < 3; ++d)
        for( e = 0; e < 3; ++e)
         for( f = 0; f < 3; ++f)
          for( g = 0; g < 3; ++g)
              {
              printf("%c", Range(digits[0])[a]);
              printf("%c", Range(digits[1])[b]);
              printf("%c", Range(digits[2])[c]);
              printf("%c", Range(digits[3])[d]);
              printf("%c", Range(digits[4])[e]);
              printf("%c", Range(digits[5])[f]);
              printf("%c", Range(digits[6])[g]);
              printf("\n");
              if (++line == 20)
                  {
                  printf("Press Any key for More");
                  printf(" (or q to quit): ");
                  if (getch() == 'q')
                      return (0);
                  printf("\n");
                  line = 0;
                  }
              }
}

char *Range(int key)
    {
    static char keys[10][3] = {
        {'0', '0', '0' },
        {'1', '1', '1' },
        {'a', 'b', 'c' },
        {'d', 'e', 'f' },
        {'g', 'h', 'i' },
        {'j', 'k', 'l' },
        {'m', 'n', 'o' },
        {'p', 'r', 's' },
        {'t', 'u', 'v' },
        {'w', 'x', 'y' }
    };

    return (keys[key]);
}

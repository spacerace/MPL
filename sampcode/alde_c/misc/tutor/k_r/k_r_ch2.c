pr 2.*


Jul 27 17:12 1984  2.34conv.c Page 1


/* type conversion */

main()  /* print conversion table from cm to inches
        /* 1 inch = 2.54 cm - from 1 through 12 cm */
{
        int cm;

        for (cm = 1; cm <= 12; ++cm)
          printf("cm = %3d, inch = %6.2f\n", cm, cm / 2.54);
}





















































Jul 27 17:12 1984  2.50comma.c Page 1


main()  /* calculate the sum of all integers between and including
        /* 2 input integers, compute their average and remainder */
{ int   i, n1, n2, ct, sm;
  n1 = getint(); n2 = getint();
  if (n1 <= n2)
  { for (ct = sm = 0, i = n1; i <= n2; ++i)
    {   sm += i; ++ct;
    }
    printf("n1 = %d, n2 = %d, sum = %d, ave = %.2f, remainder = %d\n",
                n1, n2, sm, (float) sm / ct, sm % ct);
  }
  else
  { printf("2nd number not big enough\n"); exit(1);
  }
}
getint()        /* function to convert ASCII
                /* input from terminal to an int */
{       int     c, sign = 1, num = 0;

        printf("input integer: ");
        if      ((c = getchar()) == '-')
                sign = -1;
        else if (c >= '0' && c <= '9')
                num = (c - '0');
        else
                return (0);
        while ((c = getchar()) >= '0' && c <= '9')
                num = num * 10 + (c - '0');
        return (num * sign);
}

































Jul 27 17:12 1984  2.58itob.c Page 1


#define SZ      8 * sizeof(int) /* for portabilty       */
main()  /* function to test itob(), integer to binary */
{       char asc[SZ+1];
        int i = getint();
        itob(i, asc);
        printf("dec = %d, octal = %o, binary = %s\n", i, i, asc);
}
itob(num, ar)   /* convert int to ASCII 0's and 1's */
int num;        /* received int */
char ar[];      /* array to store 0's and 1's */
{       int cnt, mask = 1;
        for (cnt = SZ - 1; cnt >= 0; --cnt)
        {       ar[cnt] = ((num & mask)? '1': '0'); mask <<= 1;
        }
        ar[SZ] = '\0';
}
getint()        /* function to convert ASCII
                /* input from terminal to an int */
{       int     c, sign = 1, num = 0;

        printf("input integer: ");
        if      ((c = getchar()) == '-')
                sign = -1;
        else if (c >= '0' && c <= '9')
                num = (c - '0');
        else
                return (0);
        while ((c = getchar()) >= '0' && c <= '9')
                num = num * 10 + (c - '0');
        return (num * sign);
}






























$ 

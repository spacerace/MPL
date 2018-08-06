pr 1.*


Jul 31 13:15 1984  1.00array.c Page 1


/* example of arrays */

#define EOF -1
#define SZ  25
main()  /* count digits 0 - 9 and store in elements 0 - 9 */
{
        int c, i, digit[SZ];            /* declaration */
        while (i = 1; i < SZ; ++i)      /* initialization */
              digit[i - 1] = i;
        printf("digits =");
        for (i = 0; i < SZ; ++i)
                printf("%4d", digit[i]);
        printf("\n");
}

















































Jul 27 17:12 1984  1.10first.c Page 1


/* This is a C program that prints a
/* message to standard output
/* (usually your terminal) */

main()
{
        printf("This is a C program.\n");
}























































Jul 27 17:12 1984  1.29while.c Page 1


/* print the numbers 1 - 10 with squares and cubes */

main()
{
        int     st, end, step, num, sq, cube;
        st = 1;
        end = 10;
        step = 1;
        num = st;
        while (num <= end)
        {       sq = num * num;
                cube = num * sq;
                printf("%6d %6d %6d\n", num, sq, cube);
                num = num + step;
        }
}















































Jul 27 17:12 1984  1.49for.c Page 1


/* also prints the numbers 1 - 10 with squares
   and cubes - this time with a "for" loop */

main()
{
        int n;

        for (n = 1; n <= 10; n = n + 1)
                printf("%6d %6d %6d\n", n, n * n, n * n * n);
}





















































Jul 27 17:12 1984  1.53inc.c Page 1


/* introduces the increment operator */

main()
{
        int n;

        for (n = 1; n <= 10; ++n)
                printf("%6d %6d %6d\n", n, n * n, n * n * n);
}






















































Jul 27 17:12 1984  1.55symbolic.c Page 1


/* introduces symbolic constants */

#define MAX     10
#define SQ      n * n
#define CU      n * n * n
main()
{
        int n;

        for (n = 1; n <= MAX; ++n)
                printf("%6d %6d %6d\n", n, SQ, CU);
}



















































Jul 27 17:12 1984  1.58copy.c Page 1


/* copy input to output */

#define EOF -1

main()
{
        int c;
        c = getchar();          /* see getc(3) */
        while (c != EOF) {
          putchar(c);           /* see putc(3) */
          c = getchar();
        }
}


















































Jul 27 17:12 1984  1.62copy2.c Page 1


/* assignments within tests */

#define EOF -1
main()  /* copy input to output; 2nd version */
{
        int c;

        while ((c = getchar()) != EOF)
                putchar(c);
}





















































Jul 27 17:12 1984  1.64ccount.c Page 1


/* character counting program */

#define EOF -1
main()  /* count number of characters received */
{
        double  nc;

        for (nc = 0; getchar() != EOF; ++nc)
                        ;       /* do nothing */
        printf("char count is %.0f\n", nc);
}




















































Jul 27 17:12 1984  1.67lcount.c Page 1


/* count lines */

#define EOF -1
main()  /* count number of lines in input */
{
        int c;
        long nl = 0; /* combined declaration and initialization */

        while ((c = getchar()) != EOF)
                if (c == '\n')
                        ++nl;
        printf("line count is %ld\n", nl);
}


















































Jul 27 17:12 1984  1.71uccount.c Page 1


/* count upper case and others */

#define EOF -1
main()  /* print count of upper case letters
        /* & all other characters received */
{
        int c, uc, oth;

        uc = oth = 0;   /* multiple assignment */
        while ((c = getchar()) != EOF)
                if      (c >= 'A' && c <= 'Z')
                        ++uc;
                else    ++oth;
        printf("uc is %d, oth is %d\n", uc, oth);
}
















































Jul 27 17:12 1984  1.73lwccount.c Page 1


/* line, word, and char count */
#define EOF -1
#define YES  1
#define NO   0
main()  /* count lines, words, chars in input */
{       int c, nl, nw, inword = NO;
        long nc;
        nl = nw = nc = 0;
        while ((c = getchar()) != EOF)
        {       ++nc;
                if (c == '\n')
                        ++nl;
                if      (c == ' ' || c == '\t' || c == '\n')
                        inword = NO;
                else if (inword == NO)
                {       inword = YES;
                        ++nw;
                }
        }
        printf("lines, words, chars: %d %d %ld\n", nl, nw, nc);
}










































Jul 27 17:12 1984  1.77power.c Page 1


/* example of function calls */

main()  /* test power() for range 1 - 9 */
{
        int i;
        printf("   Num   Sq\n");
        for (i = 1; i < 10; ++i)
                printf("%5d %5d\n", i, power(i, 2));
}
power(x, n)     /* raise x to the n-th power; n > 0 */
int x, n;       /* declaration of arguments */
{
        int i, p = 1;
        for (i = 1; i <= n; ++i)
                p = p * x;
        return (p);
}














































Jul 27 17:12 1984  1.85array.c Page 1


/* example of arrays */

#define EOF -1
#define SZ  10
main()  /* count digits 0 - 9 and store in elements 0 - 9 */
{
        int c, i, digit[SZ];            /* declaration */
        for (i = 0; i < SZ; ++i)        /* initialization */
                digit[i] = 0;
        while ((c = getchar()) != EOF)
                if (c >= '0' && c <= '9')
                        ++digit[c - '0'];
        printf("digits =");
        for (i = 0; i < SZ; ++i)
                printf("%4d", digit[i]);
        printf("\n");
}














































Jul 27 17:12 1984  1.91square.c Page 1


/* example of call by value/reference */

#define MAX 10
main()  /* test of square() */
{
        int answer[MAX], i;
        square(answer, MAX);
        for (i = 0; i < MAX; ++i)
                printf("element %d contains %2d\n", i, answer[i]);
}
square(ar, sz)  /* fills array ar of size sz with the
                /* square of the element number */
int ar[], sz;
{
        for (--sz; sz >= 0; --sz)
                ar[sz] = sz * sz;
}














































Jul 27 17:12 1984  1.94getline.c Page 1


/* example of character arrays */

#define EOF -1
#define MAX 30
main()  /* use getline() to get a name from terminal */
{
  char greet[6], name[MAX];

  printf("input name: ");
  if    (getline(name, MAX) > 1)
  {     greet[0] = 'H';
        greet[1] = 'E';
        greet[2] = 'L';
        greet[3] = 'L';
        greet[4] = 'O';
        greet[5] = '\0';
        printf("%s %s", greet, name);
  }
  else
        printf("no name received\n");
}
getline(line, lim)      /* function to read a line from std input;
                        /* insert NULL at end and return char count */
char    line[];         /* array where line will be stored */
int     lim;            /* max line size */
{
  int   c, i;

  for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
        line[i] = c;
  if (c == '\n')                /* retain  */
        line[i++] = c;          /* newline */
  line[i] = '\0'; return (i);
}





























Jul 27 17:12 1984  1.98external.c Page 1


/* example of external variables */
int max;        /* external variable */
main()
{       int i, j;

        i = getint();
        j = getint();
        if (findmax(i, j))
                printf("%d is largest\n", max);
        else    printf("%d and %d are equal\n", i, j);
}
findmax(x, y)
int x, y;
{
        if      (x == y)
                return (0);
        else if (x < y)
                max = y;
        else    max = x;
        return (1);
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
           num = num * 10 + (c - '0');
        return (num * 
pr 3.*


Jul 27 17:12 1984  3.10while.c Page 1


main()  /* example of nested while statements */
{
        int x = 10, y;

        while (x >= 0)
        {       y = x;
                while (y >= 0)
                {       printf("%2d%c", y, (y == 0)? '\n': '\040');
                        --y;
                }
                --x;
        }
}


















































Jul 27 17:12 1984  3.14for.c Page 1


main()  /* example of nested for statements */
{       int i, j;

        for (i = 0; i < 5; ++i)
        {
                for (j = 0; j < 5; ++j)
                        printf("%d,%d  ", i, j);
                putchar('\n');
        }
}





















































Jul 27 17:12 1984  3.20wc.c Page 1


#define EOF -1
main()  /* emulation of wc(1) without options */
{       int     c, inword;
        long    cc, lc, wc;
        inword = cc = lc = wc = 0;
        while ((c = getchar()) != EOF)
        {       ++cc;
                switch (c)
                { case '\n':    ++lc;
                  case '\t':
                  case '\040':  inword = 0; break;
                  default:      if (!inword)
                                        ++wc;
                                inword = 1;
                }
        }
        printf("%7ld %6ld %6ld\n", lc, wc, cc);
}













































Jul 27 17:12 1984  3.24strln.c Page 1


#define EOF -1
#define MAX 256
main()  /* test strln() */
{ char stor[MAX];
  getline(stor, MAX);
  printf("string length including null at end is %d\n", strln(stor));
}
strln(str)      /* return length of char string stored in array str;
                /* include null at end in count */
char str[];
{       int i = 0, length = 0;
        do
        {       ++length;
        } while (str[i++] != '\0');
        return (length);
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


































Jul 27 17:12 1984  3.28bc.c Page 1


main()  /* example of break and continue */
{
        int     x, y;
        for (x = 10; x; --x)
        {       if      (x == 3)
                        break;
                else    y = x - 1;
                while   (y)
                {       if (y == 3)
                        {       --y; continue;
                        }
                        printf("%3d,%d ", x, y--);
                }
                putchar('\n');
        }
}















































Jul 27 17:12 1984  3.32bcg.c Page 1


main()  /* example of break, continue, and goto */
{
        int     x, y;
        for (x = 10; x; --x)
        {       if      (x == 3)
                        break;
                else    y = x - 1;
                while   (y)
                {       if (y == 3)
                        {       --y; continue;
                        }
                        else if (x == 5 && y == 1)
                                goto done;
                        printf("%3d,%d ", x, y--);
                }
                putchar('\n');
        }
        done: printf(" DONE!\n");
}










































$ 

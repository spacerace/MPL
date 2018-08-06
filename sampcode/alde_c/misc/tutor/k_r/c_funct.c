pr *.f


Jul 27 17:12 1984  fact.f Page 1


double fact(num)        /* recursive function to compute and
                           return factorial (num!) for num > 0 */
int num;
{       if      (num > 1)
                return (num * fact(num - 1));
        else    return 1;
}
























































Jul 27 17:12 1984  getdbl.f Page 1


double  getdbl()        /* convert ASCII from terminal to dbl */
{       int     c, sign = 1;    /* c = char input, sign = + */
        float   d = 1.0;        /* d = decimal place */
        double  num = 0.0;      /* num = converted number */
        printf("input number: ");
        if      ((c = getchar()) == '-')
                sign = -1;                      /* sign = - */
        else if (c == '\n' || c == EOF)
                return (0);
        else if (c == '.')
                goto dot;
        else if (c >= '0' && c <= '9')
                num = (c - '0');
        while ((c = getchar()) != '\n' && c != '.' && c != EOF)
                if (c >= '0' && c <= '9')
                        num = num * 10 + (c - '0');
        if (c == '.')
dot:            while ((c = getchar()) != EOF && c != '\n')
                        if (c >= '0' && c <= '9')
                        { d /= 10.0; num += d * (c - '0');
                        }
        return (num * sign);
}








































Jul 27 17:12 1984  getdbl_a.f Page 1


double getdbl_a()       /* use (3S) & (3C) functions */
{       char stor[MAX];
        double atof();

        printf("input number: ");
        return (atof(fgets(stor, MAX, stdin)));
}
























































Jul 27 17:12 1984  getdbl_n.f Page 1


double  getdbl_n()      /* convert ASCII to dbl - 2nd version */
{       int     c, sign = 1;    /* c = char input, sign = + */
        float   d = 1.0;        /* d = decimal place */
        double  num = 0.0;      /* num = converted number */
        printf("input number: ");
        if      ((c = getc(stdin)) == '-')
                sign = -1;              /* sign = - */
        else if (c == '\n' || c == EOF)
                return (0);
        else if (c == '.' || (c >= '0' && c <= '9'))
                ungetc(c, stdin);       /* return c to buffer */
        while ((c = getc(stdin)) != '\n' && c != '.' && c != EOF)
                if (c >= '0' && c <= '9')
                        num = num * 10 + (c - '0');
        if (c == '.')
                while ((c = getc(stdin)) != EOF && c != '\n')
                        if (c >= '0' && c <= '9')
                        { d /= 10.0; num += d * (c - '0');
                        }
        return (num * sign);
}










































Jul 27 17:12 1984  getint.f Page 1


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
















































Jul 27 17:12 1984  getintp.f Page 1


int *getintp()  /* function to convert ASCII
                /* input from terminal to an int;
                /* return pointer to int */
{ int   c, sign = 1, num = 0;

  printf("input integer: ");
  if    ((c = getchar()) == '-')
        sign = -1;
  else if (c >= '0' && c <= '9')
        num = (c - '0');
  else
        return (&num);
  while ((c = getchar()) >= '0' && c <= '9')
        num = num * 10 + (c - '0');
  num *= sign;
  return (&num);
}














































Jul 27 17:12 1984  getline.f Page 1


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


















































Aug  3 14:21 1984  getlinep.f Page 1


getlinep(line, lim)     /* function to read a line from std input;
                        /* insert NULL at end and return char count */
char    *line;          /* array where line will be stored */
int     lim;            /* max line size */
{
  int   c, i;

  for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
        *line++ = c;
  if (c == '\n')                /* retain  */
        *line++ = c;            /* newline */
  *line = '\0'; return (i);
}


















































Jul 27 17:12 1984  getlinepn.f Page 1


getlinepn(pline, lim)   /* function to get a line from std input
                        /* (do not save newline); insert null at end
                        /* and return char count per PA ex. 04a */
char    *pline;         /* pointer to array where line will be stored */
int     lim;            /* max line size */
{
  register c, i;

  for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
        *pline++ = c;           /* newline is                   */
  *pline = '\0'; return (i);    /*      not retained or counted */
}



















































Jul 27 17:12 1984  inter.f Page 1


inter(pfi)      /* call function via pointer arg */
int (*pfi)();   /* pfi is pointer to function
                /* returning type int */
{
  return ((*pfi)());    /* (*pfi)() is a function call */
}

























































Jul 27 17:12 1984  itob.f Page 1


itob(num, ar)   /* convert int to ASCII 0's and 1's */
int num;        /* received int */
char ar[];      /* array to store 0's and 1's */
{       int cnt, mask = 1;
        for (cnt = (8 * sizeof(cnt)) - 1; cnt >= 0; cnt--)
        {       ar[cnt] = ((num & mask)? '1': '0'); mask <<= 1;
        }
        ar[(8 * sizeof(cnt))] = '\0';
}






















































Jul 27 17:12 1984  power.f Page 1


power(x, n)     /* raise x to the n-th power; n > 0 */
int x, n;       /* declaration of arguments */
{
        int i, p = 1;
        for (i = 1; i <= n; ++i)
                p = p * x;
        return (p);
}























































Jul 27 17:12 1984  square.f Page 1


square(ar, sz)  /* fills array ar of size sz with the
                /* square of the element number */
int ar[], sz;
{
        for (--sz; sz >= 0; --sz)
                ar[sz] = sz * sz;
}
























































Jul 27 17:12 1984  strcmp.f Page 1


strcmp(s1, s2)  /* compare 2 strings;
                /* if match return 0 (else char count) */
char    *s1, *s2;
{       register i;
        for (i = 1; *s1 == *s2; ++i)
                if (*s1++ == '\0')
                        return (0);
                else s2++;
        return (i);
}





















































Jul 27 17:12 1984  strcpy.f Page 1


strcpy(d, s)    /* copy string in s[] into d[] */
char d[], s[];
{       register i;
        for (i = 0; d[i] = s[i]; ++i)
                ;
}

























































Jul 27 17:12 1984  strcpy_p.f Page 1


strcpy_p(d, s)  /* copy string *s to *d */
char *d, *s;
{       
        while (*d++ = *s++)
                ;
}

























































Jul 27 17:12 1984  strln.f Page 1


strln(str)      /* return length of char string stored in array str;
                /* include null at end in count */
char str[];
{       int i = 0, length = 0;
        do
        {       ++length;
        } while (str[i++] != '\0');
        return (length);
}






















































Jul 27 17:12 1984  strln_p.f Page 1


strln_p(ptr)    /* return length of char string pointed to by ptr;
                /* include null at end in count */
char *ptr;
{       register length = 0;
        do
        {       ++length;
        } while (*ptr++ != '\0');       /* ++ increments ptr by 1 */
        return (length);
}






















































Jul 27 17:12 1984  swap.f Page 1


swap(px, py)            /* call by reference */
int *px, *py;           /* px, py are pointers to type int */
{       int tmp;
        tmp = *px;
        *px = *py;
        *py = tmp;
}
























































Jul 27 17:12 1984  vol.f Page 1


double vol(a, b, c)     /* given 3 dimensions compute volume */
double a, b, c;         /* arg declarations */
{
        return (a * b * c);
}
























































$ 

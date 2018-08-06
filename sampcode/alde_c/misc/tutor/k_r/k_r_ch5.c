pr 5.*


Aug  3 09:13 1984  5.12pdemo.c Page 1


main()          /* demonstration of pointers */
{
  int x, y;
  int *px;      /* declare px pointer to type int */

  x = 5;
  px = &x;      /* initialize px to address of x */
  y = *px;      /* initialize y to contents of address
                   contained in px */
  printf("addresses of x, y, px are...\n\t%8o, %8o, %8o\n",
                &x, &y, &px);
  printf("values of x, y, px, *px are...\n\t%8d, %8d, %8o, %8d\n",
                x, y, px, *px);
y = 12;
px = &y;
  printf("addresses of x, y, px are...\n\t%8o, %8o, %8o\n",
                &x, &y, &px);
  printf("values of x, y, px, *px are...\n\t%8d, %8d, %8o, %8d\n",
                x, y, px, *px);
}











































Jul 27 17:12 1984  5.14swap_ng.c Page 1


/* swapping values - wrong way */

#include "getint.f"     /* assumes getint() in
                           file getint.f in wd */
main()
{       int a = getint(), b = getint();

        printf("original a, b are\t%6d, %6d\n", a, b);
        swap_ng(a, b);
        printf("swapped a, b are\t%6d, %6d\n", a, b);
}
swap_ng(x, y)           /* call by value */
int x, y;
{       int tmp;
        tmp = x;
        x = y;
        y = tmp;
}













































Jul 27 17:12 1984  5.17swap.c Page 1


/* swapping values - correct way */

#include "getint.f"     /* assumes getint() in
                           file getint.f in wd */
main()
{       int a = getint(), b = getint();

        printf("original a, b are\t%6d, %6d\n", a, b);
        swap(&a, &b);
        printf("swapped a, b are\t%6d, %6d\n", a, b);
}
swap(px, py)            /* call by reference */
int *px, *py;           /* px, py are pointers to type int */
{       int tmp;
        tmp = *px;
        *px = *py;
        *py = tmp;
}













































Jul 27 17:12 1984  5.20ap.c Page 1


/* demonstration of similarity between arrays and pointers */

main()
{       static char ar[] = "test";
        char *par;
        int i;
        par = ar;               /* "&" not used, WHY? */

        printf("array is\t\"%s\"\n", ar);
        printf("pointer is\t\"%s\"\n", par);
        printf("array\tpointer\n");
        for (i = 0; i < (sizeof(ar) - 1); ++i)
                printf("  %c\t  %c\n", ar[i], *par++);
}

















































Jul 27 17:12 1984  5.26pcs.c Page 1


/* pointers to "character strings" */

main()
{       static char arr1[] = { 'I', ' ', 'u', 's', 'e', '\0' };
        char *ptr1 = arr1;
        char *ptr2 = "pointers.";

        printf("%s %s\n", ptr1, ptr2);
}






















































Jul 27 17:12 1984  5.28strln_p.c Page 1


/* pointer version of strln() */

#define MAX 256
#include <stdio.h>      /* getline() needs EOF */
#include "getline.f"    /* assumes getline() is in
                           file getline.f in wd */
main()  /* test strln_p() */
{       char stor[MAX];
        getline(stor, MAX);
        printf("string length is %d\n", strln_p(stor));
}
strln_p(ptr)    /* return length of char string pointed to by ptr;
                /* include null at end in count */
char *ptr;
{       register length = 0;
        do
        {       ++length;
        } while (*ptr++ != '\0');       /* ++ increments ptr by 1 */
        return (length);
}











































Jul 27 17:12 1984  5.30strcpy.c Page 1


/* copying strings - array version */

#define MAX 256
#include <stdio.h>      /* getline() needs EOF */
#include "getline.f"    /* assumes getline() is in
                           file getline.f in wd */
main() /* test strcpy() */
{       char src[MAX], dest[MAX];
        getline(src, MAX);
        strcpy(dest, src);
        printf("src, dest...\n%s%s", src, dest);
}
strcpy(d, s)    /* copy string in s[] into d[] */
char d[], s[];
{       register i;
        for (i = 0; d[i] = s[i]; ++i)
                ;
}













































Jul 27 17:12 1984  5.33strcpy_p.c Page 1


/* copying strings - pointer version */

#define MAX 256
#include <stdio.h>      /* getline() needs EOF */
#include "getline.f"    /* assumes getline() is in
                           file getline.f in wd */
main() /* test strcpy_p() */
{       char src[MAX], dest[MAX];
        getline(src, MAX);
        strcpy_p(dest, src);
        printf("src, dest...\n%s%s", src, dest);
}
strcpy_p(d, s)  /* copy string *s to *d */
char *d, *s;
{       
        while (*d++ = *s++)
                ;
}













































Jul 27 17:12 1984  5.34strcmp.c Page 1


/* comparing strings with pointers */

#define MAX 256
#include <stdio.h>
#include "getline.f"
main()  /* test strcmp() */
{       char str1[MAX], str2[MAX];
        register ret;
        printf("input 1st string: "); getline(str1, MAX);
        printf("input 2nd string: "); getline(str2, MAX);
        if (!(ret = strcmp(str1, str2)))
                printf("they are equal\n");
        else    printf("no match, char %d\n", ret);
}
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







































Jul 27 17:12 1984  5.38mda1.c Page 1


#define D1 3
#define D2 5
int a[D1][D2];  /* two-dimensional array */
main()          /* demonstration of multi-dimensional array */
{ register i, j;

  for (i = 0; i < D1; ++i)
    for (j = 0; j < D2; ++j)
    { a[i][j] = (i * D2) + j + 1;
      printf("a[%d][%d] adr, value is %o, %2d\n",
                i, j, &a[i][j], a[i][j]);
    }
}


















































Jul 27 17:12 1984  5.40mda2.c Page 1


#define D1 3
#define D2 5
int a[][D2] =   { {  1,  2,  3,  4,  5 },
                  {  6,  7,  8,  9, 10 },
                  { 11, 12, 13, 14, 15 }
                };      /* compiler initialized */
main()  /* two-dimensional array; another version */
{ register i, j;

  for (i = 0; i < D1; ++i)
    for (j = 0; j < D2; ++j)
      printf("a[%d][%d] adr, value is %o, %2d\n",
                i, j, &a[i][j], a[i][j]);
}

















































Jul 27 17:12 1984  5.44aop.c Page 1


main()  /* demonstration of an array of pointers */
{       register i;
        static char *arp[] =    { "I",
                                  "NEED",
                                  "HELP",
                                  ""
                                };
        for (i = 0; *arp[i]; ++i)
                printf("%s ", arp[i]);  
        putchar('\n');
}




















































Jul 27 17:12 1984  5.49argdemo.c Page 1


/* demonstration of arguments passed to main() */

main(argc, argv, envp)          /* three args to main() */
int     argc;                   /* argc is an int */
char    *argv[], *envp[];       /* argv & envp are arrays
                                /* of character pointers */
{       register i;

        printf("argc is %d\n", argc);
        for (i = 0; i < argc; ++i)
                printf("*argv[%d] is %s\n", i, argv[i]);
        for (i = 0; envp[i]; ++i)
                printf("*envp[%d] is %s\n", i, envp[i]);
}

















































Jul 27 17:12 1984  5.50echo1.c Page 1


main(argc, argv)        /* emulation of basic echo(1) command;
                        /* first version, array notation */
int     argc;
char    *argv[];
{
  register i;

  for (i = 1; i < argc; ++i)
    printf("%s%c", argv[i], (i < argc - 1)? ' ': '\n');
}





















































Jul 27 17:12 1984  5.53echo2.c Page 1


main(argc, argv)        /* emulation of basic echo(1) command;
                        /* second version, pointer notation */
int     argc;
char    **argv;         /* same as *argv[] */
{
  while (--argc > 0)
    printf("%s%c", *++argv, (argc > 1)? ' ': '\n');
}























































Jul 27 17:12 1984  5.60frp.c Page 1


/* demonstration of function returning pointer */

main()                  /* test new version of getint() */
{
  int *getintp();       /* getintp() returns pointer to type int */

  printf("%d\n", *getintp());
}
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






































Jul 27 17:12 1984  5.63pfi.c Page 1


/* demonstration of pointer to function */

main()          /* test inter() */
{
  int getint(); /* getint() must be declared */

  printf("%d\n", inter(getint));
}
inter(pfi)      /* call function via pointer arg */
int (*pfi)();   /* pfi is pointer to function
                /* returning type int */
{
  return ((*pfi)());    /* (*pfi)() is a function call */
}
#include "getint.f"     /* assumes getint() is in
                        /* file getint.f in wd */













































$ 

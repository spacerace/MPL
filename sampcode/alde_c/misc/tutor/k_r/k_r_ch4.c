pr 4.*


Jul 27 17:12 1984  4.08getdbl.c Page 1


#define EOF -1
int main()      /* obtain 3 dimensions and compute volume */
{       double l, w, h, getdbl(), vol();

        l = getdbl(); w = getdbl(); h = getdbl();
        printf("(dimensions %.2f x %.2f x %.2f) vol is %.2f\n",
                l, w, h, vol(l, w, h));
}
double vol(a, b, c)     /* given 3 dimensions compute volume */
double a, b, c;         /* arg declarations */
{
        return (a * b * c);
}
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



























Jul 27 17:12 1984  4.14vol_a.c Page 1


double l, w, h, volume; /* external variables */

int main()      /* obtain 3 dimensions and compute volume */
{
        double getdbl();
        l = getdbl(); w = getdbl(); h = getdbl();
        vol_ext();
        printf("(dimensions %.2f x %.2f x %.2f) vol is %.2f\n",
                l, w, h, volume);
}





















































Jul 27 17:12 1984  4.14vol_b.c Page 1


extern double l, w, h, volume;  /* extern required if sep src file */

vol_ext()       /* given 3 dimensions compute volume */
{
        volume = l * w * h;
}

























































Jul 27 17:12 1984  4.14vol_c.c Page 1


#define EOF -1          /* define moved to this file */
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







































Jul 27 17:12 1984  4.17perm.c Page 1


main()  /* test stat() */
{
        int i;
        for (i = 0; i <= 9; ++i)
                stat();
}
stat()  /* demonstration of internal static */
{
        int fgt = 0;
        static int rem;         /* count calls */

        printf("this is call %2d, fgt is %d\n", ++rem, ++fgt);
}


















































Jul 27 17:12 1984  4.18block.c Page 1


char ABC[] = { 'A', 'B', 'C', '\0' };                           /* 1st declaration */
main()  /* demonstration of static and scope rules */
{ printf("main() ABC[] is %s\n", ABC);
  sub(); sub();
}
sub()   /* declare, initialize, & access two ABC[]'s */
{ static int ABC[] = { 1, 2, 3 };                               /* 2nd declaration */
  { static long ABC[] = { 10000, 20000, 30000 };                /* 3rd declaration */
    printf("sub() sub-block ABC[1] is %ld\n", ABC[1]++);
  }
  printf("sub() ABC[1] is %d\n", ABC[1]++);
}



















































Jul 27 17:12 1984  4.26fact.c Page 1


/* example of recursive function */

main() /* test fact() for range 1 - 10 */
{       register int i;
        double fact();
        for (i = 1; i <= 10; ++i)
                printf("%2d! is %8.0f\n", i, fact(i));
}
double fact(num)        /* recursive function to compute and
                           return factorial (num!) for num > 0 */
int num;
{       if      (num > 1)
                return (num * fact(num - 1));
        else    return 1;
}
















































Jul 27 17:12 1984  4.29prepro.c Page 1


/* examples of #include & #define */

#include <stdio.h>      /* EOF - used by getdbl() -
                        /* is defined in <stdio.h> */
#include "getdbl.f"     /* assumes getdbl() is in
                        /* file getdbl.f in wd */
#define max(A, B) ((A) > (B)? (A): (B))

main()  /* test max macro */
{
        double getdbl();
        double i = getdbl(), j = getdbl();
        printf("max is %f\n", max(i, j));
}

















































Jul 27 17:12 1984  4.30sqrt.c Page 1


/* another #include and nested function calls
/* compile as "cc filename -lm"
/* see exp(3M) and start of section 3 (3M) */

#include <math.h>       /* declares sqrt() as type double */
#include <stdio.h>      /* provides EOF for getdbl() */
#include "getdbl.f"     /* assumes getdbl() is in
                        /* file getdbl.f in wd */

main() /* use sqrt() to compute square roots */
{
        double getdbl();
        printf("square root is %f\n", sqrt(getdbl()));
}















































$ 

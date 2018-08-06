pr 6.*


Jul 27 17:12 1984  6.14struct1.c Page 1


/* demonstration of a structure, 1st version */

struct data
{ char first[20];
  char  last[26];
  double     sal;
};                      /* external definition */
main()                  /* print structure contents */
{ static struct data emp = { "Linda",
                             "Sample",
                              39000.0
                           };   /* declare and init */
  printf("%s %s earns $%.2f per year\n",
                emp.first, emp.last, emp.sal);
}
















































Jul 27 17:12 1984  6.17struct2.c Page 1


/* demonstration of a structure, 2nd version */

#define FSZ 20
#define LSZ 26
#include <stdio.h>
#include "getlinepn.f"  /* does not retain newline */
#include "getdbl.f"
struct
{ char first[FSZ];
  char  last[LSZ];
  double     sal;
} emp;  /* external definition and declaration */
main()  /* populate then print structure */
{ double getdbl();
  printf("input first name: "); getlinepn(emp.first, FSZ);
  printf("input last name: ");  getlinepn(emp.last, LSZ);
  printf("(annual salary) ");   emp.sal = getdbl();
  printf("%s %s earns $%.2f per year\n",
                emp.first, emp.last, emp.sal);
}











































Jul 27 17:12 1984  6.20aos.c Page 1


/* demonstration of an array of structures */

struct data
{ char first[20];
  char  last[26];
  double     sal;
};              /* external definition */
main()          /* find and print last names M - Z */
{ register i;
  static struct data ar[] = { { "Linda", "Sample", 39000.0 },
                              { "Sam", "Next", 37500.0 },
                              { "Larry", "Last", 34900.0 },
                             }; /* declare and init array ar */
  printf("employees with last names M - Z...\n");
  for (i = 0; i < (sizeof(ar) / sizeof(ar[0])); ++i)
    if (ar[i].last[0] >= 'M' && ar[i].last[0] <= 'Z')
        printf("\t%s %s earns $%.2f per year\n",
          ar[i].first, ar[i].last, ar[i].sal);
}












































Jul 27 17:12 1984  6.24cbv.c Page 1


/* passing structures as arguments - by value */

struct data
{ char first[20];
  char  last[26];
  double     sal;
};              /* external definition */
main()          /* test check() */
{ register i;
  double pay = 36000.0;
  static struct data ar[] = { { "Linda", "Sample", 39000.0 },
                              { "Sam", "Next", 37500.0 },
                              { "Larry", "Last", 34900.0 },
                             }; /* declare and init array ar */
  printf("employees earning more than $%.2f per year...\n", pay);
  for (i = 0; i < (sizeof(ar) / sizeof(ar[0])); ++i)
    if (check(ar[i], pay))
        printf("\t%s %s earns $%.2f per year\n",
          ar[i].first, ar[i].last, ar[i].sal);
}
check(str, lim)         /* return 1 if sal >= lim */
struct data str;        /* struct declaration */
double lim;
{ if (str.sal >= lim)
        return (1);
  return (0);
}




































Jul 27 17:12 1984  6.26cbr.c Page 1


/* passing structures as arguments - by reference */

struct data
{ char first[20];
  char  last[26];
  double     sal;
};              /* external definition */
main()          /* test check_p() */
{ register i;
  double pay = 36000.0;
  static struct data ar[] = { { "Linda", "Sample", 39000.0 },
                              { "Sam", "Next", 37500.0 },
                              { "Larry", "Last", 34900.0 },
                             }; /* declare and init array ar */
  printf("employees earning more than $%.2f per year...\n", pay);
  for (i = 0; i < (sizeof(ar) / sizeof(ar[0])); ++i)
    if (check_p(&ar[i], pay))
        printf("\t%s %s earns $%.2f per year\n",
          ar[i].first, ar[i].last, ar[i].sal);
}
check_p(pstr, lim)      /* return 1 if sal >= lim */
struct data *pstr;      /* pointer to struct declaration */
double lim;
{ if (pstr->sal >= lim)
        return (1);
  return (0);
}




































Jul 27 17:12 1984  6.34fields.c Page 1


/* structure with fields */

#define YES 1
#define NO  0
struct
{ char first[20];
  char  last[26];
  double     sal;
  unsigned retire : 1;  /* field width 1 bit */
  unsigned school : 1;  /* field width 1 bit */
} ar[] = { { "Linda", "Sample", 39000.0, NO,  NO },
           { "Sam",   "Next",   37500.0, NO, YES },
           { "Larry", "Last",   34900.0, YES, NO },
         };     /* declare and init external array */
main()  /* find and print active employees;
        /* note if in school */
{ register i;
  printf("active employees...\n");
  for (i = 0; i < (sizeof(ar) / sizeof(ar[0])); ++i)
  { if (!ar[i].retire)
    {   printf("\t%s %s earns $%.2f per year\n",
           ar[i].first, ar[i].last, ar[i].sal);
        if (ar[i].school)
           printf("\t\t(in school)\n");
    }
  }
}




































Aug  7 15:11 1984  6.40unions.c Page 1


union utag
{ char  ca[256];        /* type 1 */
  int       num;        /* type 2 */
};                      /* external definition */
#include <stdio.h>
#include "getline.f"
#include "getint.f"
main(argc, argv)        /* load union, call chk_u() */
int argc;               /*      to test and print  */
char **argv;
{ union utag udemo;     /* union declaration */
  ++argv;               /* point to argv[1]  */
  if (*++*argv != '\0') /* test 2nd char of *argv[1] */
    chk_u(0, &udemo);   /*      if not '\0' fail    */
  switch (*--*argv)     /* test 1st char of *argv[1] */
  { case '1': printf("input line: "); getline(udemo.ca, 256);
              chk_u(**argv, &udemo);
    case '2': udemo.num = getint(); chk_u(**argv, &udemo);
    default:  chk_u(**argv, &udemo);
  }
}
chk_u(utype, pun)       /* test, then print union and exit */
char utype;
union utag *pun;        /* pun is pointer to union */
if(pun -> ca == pun ->num)
     printf("same size");
else printf("different size");
{ switch(utype)
  { case '1': printf("type 1, content...\n\t%s", pun->ca);
              break;
    case '2': printf("type 2, value is %d\n", pun->num);
              break;
    default:  printf("usage: filename { 1 2 }\n");
              exit(1);  /* exit unsuccessfully */
  }
  exit(0);              /* exit successfully */
}
























$ 

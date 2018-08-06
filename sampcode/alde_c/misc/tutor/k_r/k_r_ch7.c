pr 7.*


Jul 27 17:12 1984  7.14getdbl_n.c Page 1


#include <stdio.h>
main()                  /* test getdbl_n() */
{       double getdbl_n();
        printf("number is %f\n", getdbl_n());
}
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





































Jul 27 17:12 1984  7.20cp.c Page 1


#include <stdio.h>
main(argc, argv)        /* modified cp(1) command */
int argc;
char *argv[];
{ FILE *fpr, *fpw;      /* pointers to type FILE */
  if (argc != 3)
  { printf("usage: %s src_file dest_file\n", argv[0]);
    exit(1);
  }
  if ((fpr = fopen(argv[1], "r")) == NULL)
  { printf("%s: cannot access %s\n", argv[0], argv[1]);
    exit(2);
  }
  if ((fpw = fopen(argv[2], "w")) == NULL)
  { printf("%s: cannot create %s\n", argv[0], argv[2]);
    exit(2);
  }
  if (copy(fpr, fpw) == EOF)
        exit(0);        /* success */
  exit(3);              /* failure */
}
copy(fp1, fp2)          /* copy fp1 to fp2 */
FILE *fp1, *fp2;
{
        int c;

        while ((c = getc(fp1)) != EOF)
                putc(c, fp2);
        return (c);
}

































Jul 27 17:12 1984  7.27cat.c Page 1


#include <stdio.h>
main(argc, argv)        /* modified cat(1) command */
int argc;
char *argv[];
{
  register i, c;
  FILE *fp;             /* fp is pointer to type FILE */

  for (i = 1; i < argc; ++i)
  { if ((fp = fopen(argv[i], "r")) == NULL)
    {   fprintf(stderr, "%s: cannot open %s\n",
                argv[0], argv[i]);
        continue;
    }
    while ((c = getc(fp)) != EOF)
        putc(c, stdout);
    fclose(fp);
  }
}












































Jul 27 17:12 1984  7.34sscanf.c Page 1


#include <stdio.h>
char tst[] = "123456.78 592 89000 now is the time";
main()  /* demonstration of sscanf() */
{
  int   dec;
  float f;
  long  lg;
  char  s1[50], s2[50];
  int ret;

  ret = sscanf(tst, "%3d %f %*d %ld %s %[a-z ]",
                &dec, &f, &lg, s1, s2);
  printf("return from sscanf() was %d\n", ret);
  printf("int dec is %d\n", dec);
  printf("float f is %.2f\n", f);
  printf("long lg is %ld\n", lg);
  printf("array s1 is \"%s\"\n", s1);
  printf("array s2 is \"%s\"\n", s2);
}












































Jul 27 17:12 1984  7.46atof.c Page 1


/* another version of getdbl() */

#include <stdio.h>
#define MAX 256
main()  /* test getdbl_a() */
{       double getdbl_a();

        printf("num is %f\n", getdbl_a());
}
double getdbl_a()       /* use (3S) & (3C) functions */
{       char stor[MAX];
        double atof();

        printf("input number: ");
        return (atof(fgets(stor, MAX, stdin)));
}















































Jul 27 17:12 1984  7.50create.c Page 1


#include <stdio.h>
#define SCORE 8
struct record
{       char name[48];
        float score[SCORE];
};
main()  /* create DATAFILE with student names and scores */
{       struct record data;
        FILE *fp;
        if ((fp = fopen("DATAFILE", "w")) == NULL)
        {       fprintf(stderr, "cannot create DATAFILE\n");
                exit(2);
        }
        while (load(&data))
                fwrite(&data, sizeof (data), 1, fp);
        exit(0);
}
load(pstr)      /* get data from terminal; load structure */
struct record *pstr;
{
        register i;
        printf("\ninput name: ");
        gets(pstr->name);
        if (pstr->name[0] == '\0')
                return (0);
        printf("input up to %d scores ('q' to quit): ", SCORE);
        for (i = 0; i < SCORE ; ++i)
                if (scanf("%f", &pstr->score[i]) != 1)
                {       pstr->score[i] = EOF;
                        break;
                }
        while (getchar() != '\n')       /* buffer drain */
                ;
        return (1);
}




























Jul 27 17:12 1984  7.52getinfo.c Page 1


#include <stdio.h>
#define SCORE 8
struct record
{       char name[48];
        float score[SCORE];
};
main()  /* get information from DATAFILE */
{       struct record data;
        FILE *fp;
        if ((fp = fopen("DATAFILE", "r")) == NULL)
        {       fprintf(stderr, "cannot open DATAFILE\n");
                exit(2);
        }
        while (fread(&data, sizeof (data), 1, fp))
                print(&data);
        exit(0);
}
print(pstr)     /* compute student ave; provide printout */
struct record *pstr;
{
        register i, cnt = 0;
        double num = 0.0;
        for (i = 0; i < SCORE ; ++i)
                if      (pstr->score[i] != EOF)
                {       printf("%7.2f", pstr->score[i]);
                        num += pstr->score[i];
                        ++cnt;
                }
                else    for (   ; i < SCORE; ++i)
                                printf("   --- ");
        if      (cnt > 0)
                printf(" (%6.2f)", num / (double) cnt);
        else    printf(" (  --- )");
        printf(" %s\n", pstr->name);
}




























Jul 27 17:12 1984  7.56checknum.c Page 1


#include <stdio.h>
#define SCORE 8
struct record
{       char name[48];
        float score[SCORE];
};
main()  /* check number of entries in DATAFILE */
{       struct record data;
        FILE *fp;
        if ((fp = fopen("DATAFILE", "r")) == NULL)
        {       fprintf(stderr, "cannot open DATAFILE\n");
                exit(2);
        }
        fseek(fp, 0L, 2);       /* move pointer to end-of-file */
        printf("%5ld entries in DATAFILE\n", ftell(fp) / (long) sizeof (data));
        exit(0);
}














































Jul 27 17:12 1984  7.59system.c Page 1


main()  /* use sh(1) to get
        /* long listing of DATAFILE
        /* and use same exit status */
{
        unsigned ret;   /* machine independent */

        ret = system("ls -l DATAFILE");
        exit(ret >> 8); /* shift right 8 places
                        /* to get sh(1) exit status */
}





















































Jul 27 17:12 1984  7.70cp2.c Page 1


#include <fcntl.h>      /* included for open(2) */
main(argc, argv)        /* modified cp(1) command */
int argc;               /* 2nd version */
char *argv[];           /* using system calls */
{ int fdr, fdw;         /* file descriptors */
  if (argc != 3)
  { printf("usage: %s src_file dest_file\n", argv[0]);
    exit(1);
  }
  if ((fdr = open(argv[1], 0)) == -1)
  { printf("%s: cannot access %s\n", argv[0], argv[1]);
    exit(2);
  }
  if ((fdw = creat(argv[2], 0644)) == -1)
  { printf("%s: cannot create %s\n", argv[0], argv[2]);
    exit(2);
  }
  if (copy(fdr, fdw) == 0)
        exit(0);        /* success */
  exit(3);              /* failure */
}
copy(fd1, fd2)          /* copy fd1 to fd2 */
int fd1, fd2;
{ register n;
  char buf[512];        /* temp character storage */

  while ((n = read(fd1, buf, 512)) > 0)
    write(fd2, buf, n);
  return (n);
}































$ 

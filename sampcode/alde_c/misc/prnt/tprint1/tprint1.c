
/****************************************************************
*                                                                *
*                        P R I N T                               *
*                                                                *
*                ASCII file transfer to PRN device.              *
*                                                                *
*        T. Jennings 6 Nov. 82                                   *
*          created 23 Sept. 82                                   *
*                                                                *
*          revised 14 Nov.  85    Steve Ripley                   *
*                                                                *
****************************************************************/

#include <stdio.h>
#include <ctype.h>

#define  NUL     0x00
#define  CR      0x0d
#define  LF      0x0a
#define  HT      0x09
#define  FF      0x0c
#define  DEL     0x7f

#define  PAGELEN 66
#define  BOTTOM  58

struct srv {int ax,bx,cx,dx,si,di,ds,es;};

struct srv reg;

char *months[] = {
               "Januaary ",
               "February ",
               "March ",
               "April ",
               "May ",
               "June ",
               "July ",
               "August ",
               "September ",
               "October ",
               "November ",
               "December "
               };

FILE *printfile, *fopen();
FILE *printer;
char cin, temp[5];
int line,column;
int tabwidth;
int formfeed;
int number,lineno;
int brackets,brkt_level;
int pageno;
int printable;
int titles;
int margin;
int i;
char filename[12];
char  *date,
      hour,minute,second;

main(argc,argv)
int argc;
char **argv;
{
      getdate();
      gettime();
        fprintf(stderr,"\nFile Print Utility 6 Nov. 82\n");
        number =0;               /* dont number lines */
        titles= 0;               /* no top of page title */
        margin= 0;               /* no left margin tab */
        brackets =0;             /* dont count brackets */
        brkt_level =0;
        column =0;
        line =0; 
        tabwidth =8;
        formfeed =1;
        lineno =0;               /* current line number, */
        pageno =1;
        printable =0;

        if (--argc) {
                if ((printfile=fopen(*++argv,"r")) ==NULL) {
                        fprintf(stderr,"PRINT: Can't find file '%s'.\n",*argv);
                        exit();
                }
                strcpy(filename,*argv);          /* save filename for title */
                upper(filename);

                if ((printer= fopen("PRN","w")) ==NULL) {
                        fprintf(stderr,"PRINT: Can't open LST device\n");
                        exit();
                }
                while (--argc) {         /* look for options */
                        cin= **++argv;
                        switch(tolower(cin)) {
                        case 't':
                                fprintf(stderr,"\t-Page headers\n");
                                titles= 1;
                                break;
                        case 'n':                /* line numbers */
                                fprintf(stderr,"\t-Line numbers\n");
                                number =1;
                                break;
                        case 'c':                /* C source */
                                fprintf(stderr,"\t-'C' source\n");
                                brackets =1;
                                break;
                        case 'm':
                                fprintf(stderr,"\t-Left margin\n");
                                margin= 1;
                                break;
                        default:
                                fprintf(stderr,"Illegal option: '%s'\n",*argv);
                                break;
                        }
                }
                title();
                printchar(LF);
                while ((i=fgetc(printfile)) !=EOF) {
                        cin=i;                     /* type conversion, */
                        printchar(cin);            /* print character, */
                        if (cin =='{')             /* count bracket levels, */
                                ++brkt_level;
                        if (cin == '}') {          /* if closing bracket, and */
                                if ((--brkt_level ==0) && brackets) {
                                        printchar(FF); /* top level, formfeed */
                                }
                        }
                }
                fputc(LF,printer);
                fputc(FF,printer);               /* do a formfeed, */
                fprintf(printer,"\033J");        /* motor off, */
                exit();
        }
        fprintf(stderr,"\nYou must specify a file, followed by any options");
        fprintf(stderr,"\n\tT\tTop of page headers");
        fprintf(stderr,"\n\tN\tLine numbers");
        fprintf(stderr,"\n\tC\tC source");
        fprintf(stderr,"\n\tM\tLeft margin");
        fprintf(stderr,"\nExample:  PRINT <filename> N C T M");
        exit();
}
/* Print character <C> to the printer. */

printchar(c)
char c;
{
        switch (c) {
        case LF:
                ++lineno;
                fputc(CR,printer);
                fputc(c,printer);                /* then line feed, */
                if (++line >= BOTTOM)            /* if bottom margin, */
                        printchar(FF);           /* formfeed, */
                if (number) 
                        fprintf(printer,"%3d: \t",lineno);
                if (margin)
                        printchar(HT);           /* left margins */
                break;
        case FF:
                fputc(LF,printer);
                fputc(c,printer);
                line =0; ++pageno;
                title();
                break;
        default:
                fputc(c,printer);
                break;
        }
        return;
}
/* If the title flag is set, type the title across the top of the page. */

title() {
   if (titles) {
       fprintf(printer,
           "\015Date: %s   Time: %02d:%02d:%02d   File: %-12s   %sPage %d",
           date,hour,minute,second,filename," ",pageno);
       fprintf(printer,
           "\015Date: %s   Time: %02d:%02d:%02d   File: %-12s   %sPage %d\n",
           date,hour,minute,second,filename," ",pageno);
      ++line;
   }
   else
      if (pageno == 1) {
       fprintf(printer,"\015Date: %s   Time: %02d:%02d:%02d   File: %-12s",
           date,hour,minute,second,filename);
       fprintf(printer,"\015Date: %s   Time: %02d:%02d:%02d   File: %-12s\n",
           date,hour,minute,second,filename);
       ++line;
      }
   return;
}

getdate()
{
   date = calloc(1,25);
   reg.ax = 0x2a << 8;
   sysint21(&reg,&reg);
   strcpy(date,months[(reg.dx >> 8) - 1]);
   itoa(reg.dx & 0xff,temp);
   strcat(date,temp);
   strcat(date,", ");
   itoa(reg.cx,temp);
   strcat(date,temp);
   return;
}
gettime()
{
   reg.ax = 0x2c << 8;
   sysint21(&reg,&reg);
   hour = reg.cx >> 8;
   minute = reg.cx & 0xff;
   second = reg.dx >> 8;
   return;
}


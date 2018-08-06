/*
** HELP - Help driver for The Window Boss
**
** Copyright (c) 1985 - Philip A. Mongelluzzo
** All rights reserved.
**
** Comments:
**
**  Help files must be formatted properly, use GENINDEX to create the index
**  file.
**
*/

/* Header files */

#include "windows.h"
#include "math.h"                       /* needed for atol */

/* Equates & definitions */

#define MAXKEY 255                      /* ISAM file length (# of keys) */
#define FALSE 0                         /* lies */
#define TRUE ~FALSE                     /* Science */
#define RD "rb"                         /* file access mode */

#define K_PGUP 0x49                     /* a few scan codes */
#define ESC    0x01

#if CI86                                /* Compiler specific stuff */
#define atol atoi                       
#endif

/* Function declarations */

extern FILE *fopen();                   /* Stream I/O */
extern char *malloc();                  /* memory mgmt */
extern char *fgets();                   /* file line input */
extern char *strcpy();                  /* string copy */
extern char *strcat();                  /* concat function */
extern long atol();                     /* ascii to integer */
extern int fseek();                     /* DASD seek */
extern long ftell();                    /* where are we DASD */

static struct list {                    /* master list structure */
  char *key;                            /* pointer to %info string */
  long loc;                             /* file offset */
  int type;                             /* type <0=toc; 1=page; 2=menu> */
} master[MAXKEY];                       /* master ISAM structure */

static char *scrbuf[26];                /* screen buffer (array of ptrs) */
static char line[80];                   /* line buffer */
static int cpg, eoh;                    /* clear screen, end of help flags */
static FILE *fp;                        /* stream file pointer */

static WINDOWPTR wn1;                   /* window pointer */
static int row, col;                    /* original cursor location */

/*
*************
* help_init *
*************
*/

help_init(help_file_name)               /* init help function */
char *help_file_name;
{
  if(!help("@"))                        /* allocate the memory */
    return(FALSE);                      /* return if error */
  return(help(help_file_name));         /* setup help ISAM */
}

/*
********
* help *
********
*/

help(subject)                           /* help function */
char *subject;                          /* help subject request */
{
char tmpname[30];                       /* temporary filename */
char *p;                                /* scratch pointer */
int i,j,k,l,m,n;                        /* scratch integers */
int showpage();                         /* display page function */
int showmenu();                         /* display menu function */
static initflg = FALSE;                 /* init complete flag */

  switch(subject[0]) {                  /* on first char of subject ... */
    case '%':                           /* subject page */
      wn1 = wn_open(0, 0, 0, 78, 23, BLUE<<4 | WHITE, BLUE<<4 | WHITE);
      if(!wn1) return(FALSE);           /* error return */
      wn_wrap(wn1,FALSE);               /* no wrap thank you */
      v_rcpos(0, &row, &col);           /* remember current location */
      v_hidec();                        /* hide the physical cursor */
      wn_locate(wn1,0,0);               /* home virtual cursor */
      if(!initflg) return(FALSE);       /* illegal sequence of calls */
      i=0;                              /* start search index */
      while(i < MAXKEY) {               /* look for match in key table */
        if(!strcmp(subject,master[i].key)) {
          showpage(master[i].loc);      /* display all pages */
          return(TRUE);                 /* return .. all is well */
        }
        i++;
      }
      wn_printf(wn1," Sorry... No info on %s\n", subject);
      wn_printf(wn1," Press any key to continue...");
      v_getch();
      wn_close(wn1);
      return(FALSE);                    /* no match !! */
      break;
    case '$':                           /* menu page */
      break;                            /* sometime in the future */
    case '@':                           /* Initialize (allocate memory) */
      for(i=0; i<MAXKEY; i++) {
        master[i].key = malloc(25);     /* keys can be 25 chars long */
        if(!master[i].key) return(FALSE);
      }
      for(i=0; i<25; i++) {             /* allocate memory for screen */
        scrbuf[i] =  malloc(81);        /* buffer */
        if(!scrbuf[i]) return(FALSE);
      }
      initflg = TRUE;
      return(TRUE);
      break;
    default:                            /* assume help filename */
      if(!initflg) return(FALSE);       /* illegal sequence of calls */
      strcpy(tmpname,subject);          /* form filename from root */
      strcat(tmpname,".ndx");
      if(!(fp = fopen(tmpname,RD))) {   /* index file cant be found ! */
        printf("Can't find index file\n");
        printf("Press any key to continue...");
        v_getch();
        return(FALSE);
      }
      i = 0;                            /* read and process key file */
      while(fgets(line,80,fp)) {        /* build the master index table */
        strcpy(master[i].key, line);
        if(line[0] != '%')              /* ignore all but subects */
          continue;
        fgets(line,80,fp);
        master[i].loc = atol(line);
        i++;
      }
      fclose(fp);                       /* close index file */
      strcpy(tmpname, subject);         /* form help file name from root */
      strcat(tmpname, ".hlp");          /* open and leave open ! */
      if(!(fp = fopen(tmpname, RD))) return(FALSE);
      return(TRUE);
      break;
  }
}

/*
************
* showpage *
************
*/

static showpage(offset)                 /* display page function */
long offset;
{
int i,j;                                /* scratch integers */
long pages[MAXKEY];                     /* stack of pages */
int pndx;                               /* stack pointer */
int keyflg;                             /* vaild key flag */

  pndx = 0;                             /* init stack pointer */
  pages[0] = offset;                    /* initial page */

  while(TRUE) {                         /* till forever ... */
    i = filbuf(offset);                 /* fill the buffer */
    for(j=0; j<i; j++)                  /* display buffer */
      wn_puts(wn1,j,0,scrbuf[j]);
    wn1->style |= BOLD;                 /* toggle bold on */
    if(eoh) 
      wn_puts(wn1, 22, 0, " End of help, PgUp for previous screen, any other key to continue...");
    else
      wn_puts(wn1, 22, 0, " Esc to quit help, PgUp for previous screen, any other key to continue...");
    wn1->style ^= BOLD;                 /* toggle bold off */
    keyflg = FALSE;                     /* valid key flag */
    do {                                /* loop till vaild key */
      switch (v_getch() >> 8) {         /* based on user input */
        case K_PGUP:                    /* do all the right things */
          pndx--;
          if(pndx <0) {
            wn_close(wn1);
            v_locate(0, row, col);
            return;
          }
          offset = pages[pndx];
          keyflg = TRUE;
          eoh = FALSE;                  /* force clear screen */
          cpg = TRUE;                   /* and cancel end of help flag */
          break;
        case ESC:
          wn_close(wn1);
          v_locate(0, row, col);
          return;
        default:
          offset = pages[++pndx] = ftell(fp);
          keyflg = TRUE;
          break;
      }
    } while (!keyflg);                  /* loop till valid key */
    if(cpg) wn_clr(wn1);
    if(eoh) {
      wn_close(wn1);
      v_locate(0, row, col);
      return;
    }
  }
}

/*
**********
* filbuf *
**********
*/
static filbuf(offset)                   /* fill screen buffer */
long offset;
{
int i,j;
char *p, *p1;

  i=0;                                  /* initialize */
  cpg = eoh = FALSE;                    /* assume nothing */
  fseek(fp, offset, 0);                 /* position file */
  do {
    fgets(line, 80, fp);                /* fetch a line */
      cpg = !strcmp(".cp\r\n", line);   /* lite clear screen and */
      eoh = !strcmp("*END*\r\n",line);  /* end of help flags */
      if(cpg || eoh) break;
      p = &line[0];                     /* a long way around to */
      p1 = scrbuf[i];                   /* filter the carriage */
      while (*p) {                      /* returns !! */
        switch (*p) {
          case '\r':
          case '\n':
            p++;
            break;
          default:
            *p1++ = *p++;
            break;
        }
      }
      *p1 = NULL;
      i++;
  } while (TRUE);
  return(i);                            /* return the # of lines on page */
}

/* End */

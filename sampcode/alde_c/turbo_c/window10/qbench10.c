#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <w1.h>
#include <windprot.h>

/*==============================================================================*/
/*  Qbench.c - produces a 'Screens/second' table for Q screen utilities         */
/*                                                                              */
/*  VERSION: 1.0                                                                */
/*  AUTHOR:  (C) 1987 Michael G. Mlachak  (TURBO-C) version                     */
/*           (C) 86,87 James H. leMay (Turbo Pascal) version                    */
/*                                                                              */
/*  DATE: 6-2-1987                                                              */
/*  LIABILITY: Author assumes no reliabilty for misuse                          */
/*  COMMENTS:  Released under the teamware concept. (see other DOC FILES).      */
/*  I'm not trying to support this program, so don't expect it to be perfect.   */
/*  It will just give you a good feel for speed.  The time is adjusted for      */
/*  an average 8 second test for each condition - total of 6 minutes.  For more */
/*  accurate results, change TESTTIME = 16. I would appreciate anyone providing */
/*  me the results on their machine as a disk file including processor number,  */
/*  processor Mhz and Video card.                                               */
/*==============================================================================*/

typedef enum {  nowait,wait } WAITS;
typedef enum {  attr,noattr } ATTRS;
typedef enum {  start,stop } STARTSTOP;
typedef char str80[80];

  #define PROCS        11
  #define TESTTIME     8  /*  TestTime in seconds for each case.  8 gives +/- 1%  */
  #define HIDECURSOR   8192
  #define CAPS         83
  #define SMALLS       115
  #define CAPD         68
  #define SMALLD       100
  #define FALSE        0
  #define TRUE         1

  extern qinit(void);
  void timer(STARTSTOP);
  void checktime(void);
  void allprocs(int);
  void transproc(int);
  void exallproc(ATTRS,int,int);
  void extransproc(ATTRS,int,int);
  void readkb(int *);
  void getkey(void);
  int midword(void);
  int lobyte(void);

  int          attrib, count, screens, waitscreens, nowaitscreens, oldcursor;
  int          row, col, rows, cols, procnumber;
  double       scrpersec [PROCS][wait+1][noattr+1];
  double       t1, t2, elapsedtime;

  str80        strng;
  WAITS        w;
  ATTRS        a;
  char         scrarray[4000];
  FILE         *fcb;
  int          todisk;
  int          key;

   static str80 names[PROCS] = {
      " qwritelv    ",
      " qwritev     ",
      " qwrite      ",
      " qwritec     ",
      " qwritecv    ",
      " qfillc      ",
      " qfill       ",
      " qattr       ",
      " qattrc      ",
      " qstore      ",
      " qrestore    "};


void main(void)
{
  qinit();
  oldcursor = cursorchange(HIDECURSOR);
  gotorc(1,1);
  qfill(1, 1, 25, 80, 14, ' ');
  qwritec(1,1,80,14,"SCREENS / SECOND table for Q Screen Utilities (TURBO-C).");
  qwritec(2,1,80,14,"(C) COPYRIGHT 1987 Michael G. Mlachak");
  qwritec(3,1,80,14,"(Original TEAMWARE concept (C) 86,87 James H. LeMay");
  qwritec(8, 1, 80, 14, "Data to Screen or Disk[S/D]?");
  gotorc(1, 29);
  getkey();
  qwritec(12, 1, 80, 14, "Don't worry about snow if you have CGA.");
  qwritec(13, 1, 80, 14, "It's testing as if you had an EGA.");
  sleep(3);

  for (procnumber = 0; procnumber <= PROCS-1; procnumber++)
     for (w = nowait; w <= wait; w++)
        for (a = attr; a <= noattr; a++)
            scrpersec[procnumber][w][a] = (double)0.0;
  qwait = FALSE;
  w = nowait;
  screens = 600; /*  First guess for screens  */
  checktime();

  nowaitscreens = screens;
  exallproc(attr, nowaitscreens, 14);
  extransproc(attr, nowaitscreens, 14);
  qattr(1, 1, 25, 80, 7);
  exallproc(noattr, nowaitscreens, -1);

  if ((unsigned)qseg !=0xB000) {
    qwait = TRUE;
    w = wait;
    screens = 90; /*  First guess for screens  */
    checktime();
    waitscreens = screens;
    exallproc(attr, waitscreens, 14);
    extransproc(attr, waitscreens, 14);
    qattr(1, 1, 25, 80, 7);
    exallproc(noattr, waitscreens, -1);
  }

  qfill(1, 1, 25, 80, 14, ' ');
  if (todisk) {
     fcb = fopen("QBENCH.DTA","w+");
  }
  else
     fcb = stdout;
  gotorc(1, 1);
  fprintf(fcb,"S C R E E N S / S E C O N D\n");
  fprintf(fcb,"             Chng\n");
  fprintf(fcb,"Function     Attr EGA    CGA\n");
  fprintf(fcb,"--------     ---- -----------\n");
  for (procnumber = 0; procnumber <= 6; procnumber++)
     for (a = attr; a <= noattr; a++) {
         if (a == attr)
            fprintf(fcb,"%s",names[procnumber]);
         else
            fprintf(fcb,"             ");
         if (a == attr)
            fprintf(fcb,"Yes  ");
         else
            fprintf(fcb,"No   ");
         fprintf(fcb,"%5.1f",scrpersec[procnumber][nowait][a]);
         fprintf(fcb,"%6.1f\n",scrpersec[procnumber][wait][a]);
     }
     for (procnumber = 7; procnumber <= 10; procnumber++)  {
        fprintf(fcb,"%s",names[procnumber]);
        if (procnumber < 9)
           fprintf(fcb,"Yes  ");
        else
           fprintf(fcb,"N/A  ");
        fprintf(fcb,"%5.1f",scrpersec[procnumber][nowait][attr]);
        fprintf(fcb,"%6.1f\n",scrpersec[procnumber][wait][attr]);
     }
     gotorc(24, 1);
     fprintf(fcb,"waitscreens= %d\n",waitscreens);
     fprintf(fcb,"nowaitscreens= %d\n",nowaitscreens);
     if (todisk) {
        fclose(fcb);
        qwritec(3,1,80,14,"(Data has been written to QBENCH.DTA)");
     }
     gotorc(24, 1);
     oldcursor = cursorchange(oldcursor);
}  /* main */

void         timer(ss)
STARTSTOP    ss;
{
   switch (ss) {

      case start:
                 t1 = (midword() * 256.0) + lobyte();
                 elapsedtime = 0.0;
      break;

      case stop:
                t2 = (midword() * 256.0) + lobyte();
                if (t2 < t1)
                   t2 = t2 + 1573040.0;           /*  max 1,573,040 tics/day  */
                elapsedtime = (t2 - t1) / 18.206481934;  /* units of secs.. */
      break;
   } /* of switch */
} /* of timer */

void         checktime()
{
  strcpy(strng, "TimerTest ");
  for (col = 1; col <= 3; col++)
     strcpy(strng, strcat(strng, strng));
  qfill(1, 1, 25, 80, 14, ' ');
  timer(start);

  for (count = 1; count <= screens; count++)
     for (row = 1; row <= 25; row++)
        qwritev(row, 1, 14, strng);
  timer(stop);
  screens = (int)((screens * TESTTIME) / elapsedtime);
}  /* checktime */

void         allprocs(procnumber)
int          procnumber;
{
    switch (procnumber) {

       case 0:
          timer(start);
          for (count = 1; count <= screens; count++)
             for (row = 1; row <= 25; row++)
                qwritelv(row, 1, attrib, 80, strng);
          timer(stop);
       break;

       case 1:  
          timer(start); 
          for (count = 1; count <= screens; count++)
          for (row = 1; row <= 25; row++) 
             qwritev(row, 1, attrib, strng);
          timer(stop); 
       break;

       case 2:
           timer(start);
           for (count = 1; count <= screens; count++)
              for (row = 1; row <= 25; row++)
                 qwrite(row, 1, attrib, strng);
           timer(stop);
       break;

       case 3:
          timer(start);
          for (count = 1; count <= screens; count++)
             for (row = 1; row <= 25; row++)
                qwritec(row, 1, 80, attrib, strng);
          timer(stop);
       break;

       case 4: 
          timer(start); 
          for (count = 1; count <= screens; count++)
             for (row = 1; row <= 25; row++) 
                qwritecv(row, 1, 80, attrib, strng);
          timer(stop); 
       break; 

       case 5:
          timer(start);
          for (count = 1; count <= screens; count++)
             qfillc(1, 1, 80, 25, 80, attrib, 'C');
          timer(stop); 
       break; 
  
       case 6: 
          timer(start);
          for (count = 1; count <= screens; count++)
             qfill(1, 1, 25, 80, attrib, 'F'); 
          timer(stop);
       break;
    } /*  switch procnumber */
    if (attrib >= 0)  {
        switch (procnumber) {
  
           case 7: 
              qfill(1, 1, 25, 80, attrib, 'a'); 
              timer(start); 
              for (count = 1; count <= screens; count++)
                 qattr(1, 1, 25, 80, attrib); 
              timer(stop); 
           break;

           case 8:
              qfill(1, 1, 25, 80, attrib, 'c');
              timer(start);
              for (count = 1; count <= screens; count++)
              qattrc(1, 1, 80, 25, 80, attrib);
              timer(stop);
           break;
        } /*  switch procnumber  */
    } /* attrib >=0 */
    if (elapsedtime != (double)0.0) {
       scrpersec[procnumber][w][a] = (double)screens / elapsedtime;
    }
}  /* allprocs */

void         transproc(procnumber)
int         procnumber;
{
    for (row = 1; row <= 25; row++)
       qwritev(row, 1, attrib, strng);
    switch (procnumber) {

       case 9:
          timer(start);
          for (count = 1; count <= screens; count++)
             qstore(1, 1, 25, 80, scrarray);
          timer(stop);
       break;

       case 10:
          qstore(1, 1, 25, 80, scrarray);
          timer(start);
          for (count = 1; count <= screens; count++)
             qrestore(1, 1, 25, 80, scrarray);
          timer(stop);
       break;
    } /*  switch procnumber */
    if (elapsedtime != (double)0.0)  {
       scrpersec[procnumber][w][a] = (double)screens / elapsedtime;

    }
}  /* transproc */

void         exallproc(at, scrs, att)
ATTRS        at; 
int          scrs;
int          att;
{
  static char temp_array[60];

  a = at;
  screens = scrs;
  attrib = att; 
  for (procnumber = 0; procnumber <= 8; procnumber++) {
    strcpy(strng, names[procnumber]);
    if (w == wait) 
       strcpy(strng, strcat(strng, " Wait    ")); 
    else
       strcpy(strng, strcat(strng, " No Wait ")); 
    if (a == attr)
       strcpy(strng, strcat(strng, " w/Attr  "));
    else
       strcpy(strng, strcat(strng, " No Attr "));
    memset(temp_array,(char)procnumber + 49,59);
    strncat(strng,temp_array,(80-strlen(strng)));
    allprocs(procnumber); 
  } 
} /* exallproc */

void         extransproc(at, scrs, att) 
ATTRS        at; 
int          scrs;
int          att; 
{ 
  static char temp_array[60];

  a = at;
  screens = scrs; 
  attrib = att; 
  for (procnumber = 9; procnumber <= 10; procnumber++) {
    strcpy(strng, names[procnumber]); 
    if (w == wait)
       strcpy(strng, strcat(strng, " Wait    ")); 
    else
       strcpy(strng, strcat(strng, " No Wait ")); 
    strcpy(strng, strcat(strng, " w/Attr  ")); 
    memset(temp_array,(char)procnumber + 49,59);
    strncat(strng,temp_array,(80-strlen(strng)));
    transproc(procnumber);
  }
} /* extransproc */

int midword(void)
{
   return(peek(0x00,0x046D));
}

int lobyte(void)
{
  return(peek(0x00,0x046C));
}

void        readkb(key)
int         *key; 
{ 

  int t_key;

  t_key = getch();
  if (t_key ==0)     /* extended character hit */
     t_key = getch();
   *key = t_key;
} /* readkb() */

void         getkey(void) 
{ 
  do { 
     readkb(&key);
  }  while (!((key == CAPS) || (key == CAPD) || (key == SMALLD)
              || (key == SMALLS)));
  todisk = ((key == CAPD) || (key == SMALLD));
}  /* getkey() */


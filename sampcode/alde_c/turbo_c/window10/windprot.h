  extern void qfill(int, int, int, int, int, char);
  extern void qattr(int, int, int, int, int);
  extern void qfillc(int, int, int,int, int, int, char);
  extern void qattrc(int, int, int, int, int, int);

  extern void qstore(int, int, int, int, char *);
  extern void qrestore(int, int, int, int, char *);

  extern void qwritelv(int,int,int,int, char *);
  extern void qwritev(int,int,int, char *);
  extern void qwrite(int,int,int, char *);
  extern void qwritecv(int,int,int,int, char *);
  extern void qwritec(int,int,int,int, char *);

  extern void gotorc(int,int);
  extern int cursorchange(int);
  extern int wherex(void);
  extern int wherey(void);
  extern void window(int,int,int,int,int);
  extern int egacheck(int);
  extern void qpage(int);
  extern void qwritepage(int);

  extern void makewindow(int,int,int,int,int,int,BORDERS);
  extern void removewindow(void);
  extern void titlewindow(int, char *);
  extern void scrollwindow(int,int,int);
  extern void initwindow(int);
  extern int wiattr(int,int);
  extern void qbox(int,int,int,int,int,int,int);

  int page0seg,qseg,maxpage,cardwait,qwait;
  int zoomeffect = 1;
  int zoomdelay = 11;
  DIRTYPE shadoweffect;




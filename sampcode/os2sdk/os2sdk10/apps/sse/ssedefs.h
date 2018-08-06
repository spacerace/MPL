#define  FBUFFSIZE (1024 * 8)
#define  HEADERSIZE 8
#define  LINESIZE   80
#define  MAXLINES   4096
#define  MAXSEGS    100
#define  NOTSHARED  0
#define  SHORTPAGE  24
#define  LONGPAGE   42
#define  SEGSIZE   (1024 * 4)
#define  TRUE	    1
#define  FALSE	    0
#define  TABSIZE    8
#define  MAXTABS   (LINESIZE / TABSIZE)
#define  FOFLAG     0x0001  /* set openflag to open file or fail */
#define  CFFLAG     0x0010  /* set openflag to create file or fail */
#define  WCFLAG     0x0012  /* set openflag to replace or create file */

#define  inline(U, U1) ( ((unsigned char)U <  (unsigned char)LINESIZE) &&   \
			 ((unsigned char)U >= (unsigned char)U1) )


struct Line {
    unsigned short linenum;	      /* line nuber (only valid during
					 compaction) */
    unsigned char  deleted;	      /* flag to mark line as deleted */
    unsigned char  linelength;	      /* characters in line */
    char	   firstchar[1];      /* first character in line */
};

extern unsigned short	TotalLines;	     /* num of entries in line table */
extern struct Line far *LineTable[MAXLINES]; /* the line table */

struct SegEntry{
    unsigned short segment;	      /* seletor of segment */
    unsigned short free;	      /* num of free bytes in segment */
    unsigned short flags;	      /* various flags */
};
extern unsigned short	TotalSegs;	     /* num of entries in seg table */
extern struct  SegEntry SegTable[MAXSEGS];

extern char	      fbuffer[FBUFFSIZE];
extern unsigned short bufflength;
extern unsigned short bytesread;

extern unsigned char ScrBuff[LONGPAGE][LINESIZE];
extern unsigned char EditBuff[LINESIZE];
extern unsigned short EditBuffDirty;

extern unsigned short PageSize;
extern unsigned short Mode43Set;

extern unsigned short CurRow, CurCol;
extern unsigned short TopRow;

extern unsigned short LinesMarked, CharsMarked;
extern unsigned short MarkedLine[MAXLINES], MarkedChar[LINESIZE];

extern unsigned short ForeNorm, BackNorm, ForeHilite, BackHilite, Fore25, Back25;

extern char	      *fname;
extern unsigned short fhandle;

extern void  dispatch();
extern short readfile(unsigned short);
extern short openfile(char *, unsigned short *, unsigned short);
extern void  closefile(unsigned short);
extern short addline(unsigned short, unsigned char, unsigned char *);
extern void  freesegs();
extern void  clearscr();
extern void  drawscr(unsigned short);
extern short allocseg();
extern void  getline(unsigned short, unsigned char *);
extern void  del();

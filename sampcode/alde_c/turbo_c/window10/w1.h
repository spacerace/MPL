#define MAXWNDW 30
typedef int byte;
typedef enum {  nobrdr,blankbrdr,singlebrdr,doublebrdr,mixedbrdr,solidbrdr,
                evensolidbrdr,thinsolidbrdr,lhatchbrdr,mhatchbrdr,
                hhatchbrdr,userbrdr
             }  BORDERS;
typedef enum {  nodir,up,down,verytop,top,bottom,verybottom,farleft,left,right,
                farright,center
             }  DIRTYPE;

typedef struct { 
                 char tl[2], th, tr[2], lv, rv, bl[2], bh, br[2];
               }  BRDRREC;
typedef struct { 
                 byte     wsrow, wscol, wsrows, wscols, wswattr, wsbattr;
                 BORDERS  wsbrdr;
                 DIRTYPE  wsshadow;
                 byte     wslastx, wslasty;
               }  wndwstattype;

typedef char         *byteptr;
typedef char         str160[160];







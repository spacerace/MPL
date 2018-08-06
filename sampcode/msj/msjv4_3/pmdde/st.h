typedef struct _GDEDATA {
    USHORT               hwnd_idItem;        /* unique ID for data item     */
    LONG                 cBytes;             /* length of data in pGPI      */
    USHORT               rgfFormat;          /* data format stylebits       */
    char                 szItem[64];         /* string name in ctl          */
    SHORT                x;                  /* location                    */
    SHORT                y;
    unsigned char far   *pGpi;               /* pointer to bits or orders   */
    PBITMAPINFO          pbmapinfo;          /* pointer to bitmap table     */
} GDEDATA;

typedef GDEDATA FAR  *PGDEDATA;
typedef GDEDATA near *NPGDEDATA;

typedef struct _GDECTLDATA {
    USHORT                   hwnd_idItem;     /* unique ID for data item    */
    USHORT                   rgfFormat;       /* data format stylebits      */
    char                     szItem[64];      /* string name in ctl         */
    LONG                     segs[5];         /* seg id's for item          */
    HBITMAP                  hBitmap;         /* bitmap handle              */
    struct _GDECTLDATA FAR  *pNextGDECTLData; /* pointer to next GDECTLDATA */
} GDECTLDATA;

typedef GDECTLDATA FAR  *PGDECTLDATA;
typedef GDECTLDATA near *NPGDECTLDATA;

/* specialized messages */
#define APPM_CONV_CLOSE   0x03F0

/* routines from st.lib */
extern USHORT      st_Register_DDEFMT(PSZ);
extern void        st_Init_GDEData(PGDEDATA);
extern PDDESTRUCT  st_DDE_Alloc(int, char *);

/* specific control messages */

#define IC_FIRST                    WM_USER
#define IC_INSERTITEM               IC_FIRST
#define IC_DELETEITEM               IC_FIRST+1
#define IC_SETITEMSTRUCT            IC_FIRST+2

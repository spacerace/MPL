/***************************************************************************\
*
* Module Name: OS2DEF.H
*
* OS/2 Common Definitions file - Macintosh version
*
* Copyright (c) 1987  IBM Corporation
* Copyright (c) 1987  Microsoft Corporation
*
\***************************************************************************/

#define PASCAL
#define FAR
#define NEAR
#define VOID    void

#define far
#define near

typedef unsigned short SHANDLE;
typedef void far      *LHANDLE;

#define EXPENTRY PASCAL FAR
#define APIENTRY PASCAL FAR

#define CHAR    char            /* ch  */
#define SHORT   short           /* s   */
#define LONG    long            /* l   */
#define INT     int             /* i   */

#ifndef NULL
#ifdef M_I86L
#define NULL    0L
#else
#define NULL    0
#endif
#endif

typedef unsigned char UCHAR;    /* uch */
typedef unsigned short USHORT;  /* us  */
typedef unsigned long ULONG;    /* ul  */
typedef unsigned int  UINT;     /* ui  */

typedef unsigned char BYTE;     /* b   */

typedef char far  *PSZ;
typedef char near *NPSZ;

typedef char far  *PCH;
typedef char near *NPCH;

typedef int   (PASCAL far  *PFN)();
typedef int   (PASCAL near *NPFN)();
typedef PFN far *PPFN;

typedef BYTE   FAR  *PBYTE;
typedef BYTE   near *NPBYTE;

typedef CHAR   FAR *PCHAR;
typedef SHORT  FAR *PSHORT;
typedef LONG   FAR *PLONG;
typedef INT    FAR *PINT;

typedef UCHAR  FAR *PUCHAR;
typedef USHORT FAR *PUSHORT;
typedef ULONG  FAR *PULONG;
typedef UINT   FAR *PUINT;

typedef VOID   FAR *PVOID;

typedef unsigned short BOOL;    /* f   */
typedef BOOL FAR *PBOOL;

#define FALSE   0
#define TRUE    1

#ifndef INCL_SAADEFS
typedef unsigned short SEL;     /* sel */
typedef SEL FAR *PSEL;

/*** Useful Helper Macros */

/* Create untyped far pointer from selector and offset */
#define MAKEP(sel, off)     ((PVOID)MAKEULONG(off, sel))

/* Extract selector or offset from far pointer */
#define SELECTOROF(p)       (((PUSHORT)&(p))[1])
#define OFFSETOF(p)         (((PUSHORT)&(p))[0])
#endif  /* !INCL_SAADEFS */
 
/* Cast any variable to an instance of the specified type. */
#define MAKETYPE(v, type)   (*((type far *)&v))

/* Calculate the byte offset of a field in a structure of type type. */
#define FIELDOFFSET(type, field)    ((SHORT)&(((type *)0)->field))

/* Combine l & h to form a 32 bit quantity. */
#define MAKEULONG(l, h)  ((ULONG)(((USHORT)(l)) | ((ULONG)((USHORT)(h))) << 16))
#define MAKELONG(l, h)   ((LONG)MAKEULONG(l, h))

/* Combine l & h to form a 16 bit quantity. */
#define MAKEUSHORT(l, h) (((USHORT)(l)) | ((USHORT)(h)) << 8)
#define MAKESHORT(l, h)  ((SHORT)MAKEUSHORT(l, h))

/* Extract high and low order parts of 16 and 32 bit quantity */
#define LOBYTE(w)       LOUCHAR(w)
#define HIBYTE(w)       HIUCHAR(w)
#define LOUCHAR(w)      ((UCHAR)(w))
#define HIUCHAR(w)      ((UCHAR)(((USHORT)(w) >> 8) & 0xff))
#define LOUSHORT(l)     ((USHORT)(l))
#define HIUSHORT(l)     ((USHORT)(((ULONG)(l) >> 16) & 0xffff))

/*** Common Error definitions ****/

typedef ULONG ERRORID;  /* errid */
typedef ERRORID FAR *PERRORID;

/* Combine severity and error code to produce ERRORID */
#define MAKEERRORID(sev, error) (ERRORID)(MAKEULONG((error), (sev)))

/* Severity codes */
#define SEVERITY_NOERROR                    0x0000
#define SEVERITY_WARNING                    0x0004
#define SEVERITY_ERROR                      0x0008
#define SEVERITY_SEVERE                     0x000C
#define SEVERITY_UNRECOVERABLE              0x0010

/* Base component error values */

#define WINERR_BASE     0x1000  /* Window Manager                  */
#define GPIERR_BASE     0x2000  /* Graphics Presentation Interface */
#define DEVERR_BASE     0x3000  /* Device Manager                  */
#define SPLERR_BASE     0x4000  /* Spooler                         */

/*** Common types used across components */

/*** Common DOS types */

typedef USHORT    HMODULE;  /* hmod */
typedef HMODULE FAR *PHMODULE;

#ifndef INCL_SAADEFS
typedef USHORT    PID;      /* pid  */
typedef PID FAR *PPID;

typedef USHORT    TID;      /* tid  */
typedef TID FAR *PTID;

typedef VOID FAR *HSEM;     /* hsem */
typedef HSEM FAR *PHSEM;
#endif  /* !INCL_SAADEFS */

/*** Common SUP types */

typedef LHANDLE   HAB;      /* hab  */
typedef HAB FAR *PHAB;

/*** Common GPI/DEV types */

typedef LHANDLE   HPS;      /* hps  */
typedef HPS FAR *PHPS;

typedef LHANDLE   HDC;      /* hdc  */
typedef HDC FAR *PHDC;

typedef LHANDLE   HRGN;     /* hrgn */
typedef HRGN FAR *PHRGN;

#ifndef INCL_SAADEFS
typedef LHANDLE   HBITMAP;  /* hbm  */
typedef HBITMAP FAR *PHBITMAP;
#endif  /* !INCL_SAADEFS */

typedef LHANDLE   HMF;      /* hmf  */
typedef HMF FAR *PHMF;

typedef LONG     COLOR;    /* clr  */
typedef COLOR FAR *PCOLOR;

typedef struct _POINTL  {    /* ptl  */
    LONG  x;
    LONG  y;
} POINTL;
typedef POINTL  FAR  *PPOINTL;
typedef POINTL  near *NPPOINTL;
 
typedef struct _RECTL {      /* rcl */
    LONG  xLeft;
    LONG  yBottom;
    LONG  xRight;
    LONG  yTop;
} RECTL;
typedef RECTL FAR  *PRECTL;
typedef RECTL near *NPRECTL;

typedef CHAR STR8[8];      /* str8 */
typedef STR8 FAR *PSTR8;

#ifndef INCL_SAADEFS
/* common DOS/SHL types */

/* File time and date types */

typedef struct _FTIME {         /* ftime */
    unsigned twosecs : 5;
    unsigned minutes : 6;
    unsigned hours   : 5;
} FTIME;
typedef FTIME FAR *PFTIME;

typedef struct _FDATE {         /* fdate */
    unsigned day     : 5;
    unsigned month   : 4;
    unsigned year    : 7;
} FDATE;
typedef FDATE FAR *PFDATE;

typedef struct _FILEFINDBUF {   /* findbuf */
    FDATE  fdateCreation;
    FTIME  ftimeCreation;
    FDATE  fdateLastAccess;
    FTIME  ftimeLastAccess;
    FDATE  fdateLastWrite;
    FTIME  ftimeLastWrite;
    ULONG  cbFile;
    ULONG  cbFileAlloc;
    USHORT attrFile;
    UCHAR  cchName;
    CHAR   achName[13];
} FILEFINDBUF;
typedef FILEFINDBUF FAR *PFILEFINDBUF;
#endif  /* !INCL_SAADEFS */

/*** common DEV/SPL types */

/* structure for Device Driver data */

typedef struct _DRIVDATA {      /* driv */
    LONG    cb;
    LONG    lVersion;
    CHAR    szDeviceName[32];
    CHAR    abGeneralData[1];
} DRIVDATA;
typedef DRIVDATA far *PDRIVDATA;

/* array indices for array parameter for DevOpenDC, SplQmOpen or SplQpOpen */

#define ADDRESS          0
#ifndef INCL_SAADEFS
#define DRIVER_NAME      1
#define DRIVER_DATA      2
#define DATA_TYPE        3
#define COMMENT          4
#define PROC_NAME        5
#define PROC_PARAMS      6
#define SPL_PARAMS       7
#define NETWORK_PARAMS   8

/* structure definition as an alternative of the array parameter */

typedef struct _DEVOPENSTRUC {    /* dop */
    PSZ        pszLogAddress;
    PSZ        pszDriverName;
    PDRIVDATA  pdriv;
    PSZ        pszDataType;
    PSZ        pszComment;
    PSZ        pszQueueProcName;
    PSZ        pszQueueProcParams;
    PSZ        pszSpoolerParams;
    PSZ        pszNetworkParams;
} DEVOPENSTRUC;
typedef DEVOPENSTRUC FAR *PDEVOPENSTRUC;
#endif  /* !INCL_SAADEFS */

/*** common AVIO/GPI types */

/* values of fsSelection field of FATTRS structure */
#define FATTR_SEL_ITALIC        0x0001
#define FATTR_SEL_UNDERSCORE    0x0002
#define FATTR_SEL_STRIKEOUT     0x0010
#define FATTR_SEL_BOLD          0x0020
 
/* values of fsType field of FATTRS structure */
#define FATTR_TYPE_FIXED        0x0002
#define FATTR_TYPE_KERNING      0x0004
 
/* values of sQuality field of FATTRS structure */
#define FATTR_QUALITY_DEFAULT   0
#define FATTR_QUALITY_DRAFT     1
#define FATTR_QUALITY_PROOF     2
 
/* values of fsFontUse field of FATTRS structure */
#define FATTR_FONTUSE_NOMIX         0x0002
#define FATTR_FONTUSE_OUTLINE       0x0004
#define FATTR_FONTUSE_TRANSFORMABLE 0x0008
/* size for fields in the font structures */

#define FACESIZE 32

/* font struct for Vio/GpiCreateLogFont */

typedef struct _FATTRS {          /* fat */
    USHORT  usRecordLength;
    USHORT  fsSelection;
    LONG    lMatch;
    CHAR    szFaceName[FACESIZE];
    USHORT  idRegistry;
    USHORT  usCodePage;
    LONG    lMaxBaselineExt;
    LONG    lAveCharWidth;
    USHORT  fsType;
    SHORT   sQuality;
    USHORT  fsFontUse;
} FATTRS;
typedef FATTRS far *PFATTRS;

/* font metrics returned by GpiQueryFonts and others */

typedef struct _FONTMETRICS {   /* fm */
    CHAR    szFamilyname[FACESIZE];
    CHAR    szFacename[FACESIZE];
    SHORT   idRegistry;
    SHORT   usCodePage;
    LONG    lEmHeight;
    LONG    lXHeight;
    LONG    lMaxAscender;
    LONG    lMaxDescender;
    LONG    lLowerCaseAscent;
    LONG    lLowerCaseDescent;
    LONG    lInternalLeading;
    LONG    lExternalLeading;
    LONG    lAveCharWidth;
    LONG    lMaxCharInc;
    LONG    lEmInc;
    LONG    lMaxBaselineExt;
    SHORT   sCharSlope;
    SHORT   sInlineDir;
    SHORT   sCharRot;
    USHORT  usWeightClass;
    USHORT  usWidthClass;
    SHORT   sXDeviceRes;
    SHORT   sYDeviceRes;
    SHORT   sFirstChar;
    SHORT   sLastChar;
    SHORT   sDefaultChar;
    SHORT   sBreakChar;
    SHORT   sNominalPointSize;
    SHORT   sMinimumPointSize;
    SHORT   sMaximumPointSize;
    USHORT  fsType;
    USHORT  fsDefn;
    USHORT  fsSelection;
    USHORT  fsCapabilities;
    LONG    lSubscriptXSize;
    LONG    lSubscriptYSize;
    LONG    lSubscriptXOffset;
    LONG    lSubscriptYOffset;
    LONG    lSuperscriptXSize;
    LONG    lSuperscriptYSize;
    LONG    lSuperscriptXOffset;
    LONG    lSuperscriptYOffset;
    LONG    lUnderscoreSize;
    LONG    lUnderscorePosition;
    LONG    lStrikeoutSize;
    LONG    lStrikeoutPosition;
    SHORT   sKerningPairs;
    SHORT   sReserved;
    LONG    lMatch;
} FONTMETRICS;
typedef FONTMETRICS far *PFONTMETRICS;

/*** Common WIN types */

#ifdef MACPM_INTERNAL
  typedef struct _MYWND **HWND;
#else
  typedef LHANDLE HWND;      /* hwnd */
#endif

typedef HWND FAR *PHWND;

typedef struct _WRECT {    /* wrc */
    SHORT xLeft;
    SHORT dummy1;
    SHORT yBottom;
    SHORT dummy2;
    SHORT xRight;
    SHORT dummy3;
    SHORT yTop;
    SHORT dummy4;
} WRECT;
typedef WRECT FAR *PWRECT;
typedef WRECT near *NPWRECT;

typedef struct _WPOINT {   /* wpt */
    SHORT x;
    SHORT dummy1;
    SHORT y;
    SHORT dummy2;
} WPOINT;
typedef WPOINT FAR *PWPOINT;
typedef WPOINT near *NPWPOINT;

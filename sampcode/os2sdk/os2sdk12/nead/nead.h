/* This header file does all the necessary includes and defines all the
   structures and constants needed for the nead program                 */

#define INCL_WIN
#define INCL_WINHEAP
#define INCL_WINDIALOGS
#define INCL_GPIPRIMITIVES

#include <os2.h>
#include <dos.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

/* The HoldFEA is used to hold individual EAs.  The member names correspond
   directly to those of the FEA structure.  Note however, that both szName
   and aValue are pointers to the values.  An additional field, next, is
   used to link the HoldFEA's together to form a linked list. */

struct _HoldFEA
{
   BYTE       fEA;       /* Flag byte */
   BYTE       cbName;
   USHORT     cbValue;
   CHAR      *szName;
   CHAR      *aValue;
   struct _HoldFEA *next;
};
typedef struct _HoldFEA HOLDFEA;

/* The DeleteList is used to hold the names of EAs that need to be explicitly
   deleted before the current EAs are written.  This is necessary because
   there does not exist a way to automatically delete all existing EAs
   associated with a file.  The next field allows the structures to be
   linked. */

struct _DeleteList
{
   CHAR *EAName;
   struct _DeleteList *next;
};
typedef struct _DeleteList DELETELIST;

/* The PassData struct is used to pass data, especially as the user data
   parameter for dialog box calls.  The Point field points to several types
   of data throughout the program.  Usually it points to either an asciiz
   name or a HoldData structure.  The rest of the fields are used for m-m
   recursive calls.  cbMulti is the length of the m-m field currently being
   considered.  usMultiOffset is the offset from the beginning of the
   EA (->aValue) to the start of the current m-m field.  usIndex is a count
   of the number of sub-fields in the current m-m.   */

struct _PassData
{
   CHAR   *Point;
   USHORT cbMulti;
   USHORT usMultiOffset;
   USHORT usIndex;
   BYTE   fFlag;
};
typedef struct _PassData PASSDATA;

/* The ReEnter structure is used to keep track of the static data for
   MultiTypeProc.  This structure is necessary since the proc is recursive
   and the static data is only available during the initialize message.
   The structure holds the two static variables and has a next field to
   allow the list to be linked. */

struct _ReEnter
{
   HOLDFEA *pFEA;
   PASSDATA FAR *pPDat;
   struct _ReEnter *next;
};
typedef struct _ReEnter REENTER;

/* This struct holds the static data that allows translation between the
   EA type and descriptor string, etc.  Data is global. */

struct _EADATA
{
   USHORT usPrefix;
   CHAR   szFormat[36];
   USHORT usFldType;
};
typedef struct _EADATA EADATA;

BOOL OpenFile(HWND,USHORT);
BOOL AddEA(HWND);
BOOL QueryEAs(HWND,CHAR *);
BOOL EditEAValue(HWND, PASSDATA *);
BOOL EAExists(CHAR *);
BOOL EditEA(HWND);
BOOL CheckEAIntegrity(CHAR *,USHORT);
MRESULT EXPENTRY OpenFileProc  (HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY MainDlgProc   (HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY AddEAProc     (HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY AsciiEditProc (HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY IconDlgProc   (HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY MultiTypeProc (HWND, USHORT, MPARAM, MPARAM);
VOID  FillDirListBox  (HWND,CHAR *);
VOID  FillFileListBox (HWND);
VOID  Free_FEAList(HOLDFEA *,DELETELIST *);
VOID  ShowEAType(HWND);
VOID  DeleteCurEA(HWND);
VOID  WriteEAs(HWND);
VOID  ChangeName(HWND,HOLDFEA *,CHAR *);
VOID  MultiAdd(HWND, HOLDFEA *, PASSDATA FAR *);
SHORT ParseFileName (CHAR *, CHAR *);
USHORT LookupEAType(USHORT);
USHORT CurEAType(HOLDFEA *);
USHORT GetUSHORT(HOLDFEA *,USHORT);
HOLDFEA *GetCurFEA(HWND, HOLDFEA *);
CHAR *MultiTypeIndex(CHAR *, USHORT);
CHAR *EAValueString(HWND, CHAR *);

#define ARGFILE             1
#define MAX_GEA             500L /* Max size for a GEA List              */
#define Ref_ASCIIZ          1    /* Reference type for DosEnumAttribute  */
#define MAXEANAME           255  /* Maximum length an EA Name can be     */
#define MAXEAVALUE         2048  /* Arbitrary max edit fld len of EA Val */
#define EATABLESIZE          12  /* Number of entries in ConvTable       */
#define GROWSIZE            512  /* Minimum size to increase the heap by */
#define FILE_ALL         0x0007  /* Read-only, sys, hidden, & normal     */
#define MAXSHOWSIZE          34  /* Number of chars to place in desc lbox*/
#define LENUSHORTBUF          6  /* Buf size needed to for ascii USHORT  */

/* Return values from ParseFileName */
#define FILE_INVALID          0  /* The filename was invalid */
#define FILE_PATH             1  /* The filename was a path  */
#define FILE_VALID            2  /* The filename was valid   */

/* definition of level specifiers. required for File Info */

#define GetInfoLevel1   1                   /* Get info from SFT */
#define GetInfoLevel2   2                   /* Get size of FEAlist */
#define GetInfoLevel3   3                   /* Get FEAlist given the GEAlist */
#define GetInfoLevel4   4                   /* Get whole FEAlist */
#define GetInfoLevel5   5                   /* Get FSDname */

#define SetInfoLevel1   1                   /* Set info in SFT */
#define SetInfoLevel2   2                   /* Set FEAlist */

#define     OPENMODE    OM_DENY_NONE+OM_READ_WRITE
#define     OPENFLAG    OF_OPEN_FILE
#define     CREATFLAG   OF_CREATE_FILE+OF_FAIL
#define     REPLFLG     OF_REPLACE_FILE

#define EA_LPBINARY     0xfffe /* Length preceeded binary            */
#define EA_LPASCII      0xfffd /* Length preceeded ascii             */
#define EA_ASCIIZ       0xfffc /* Asciiz                             */
#define EA_LPBITMAP     0xfffb /* Length preceeded bitmap            */
#define EA_LPMETAFILE   0xfffa /* metafile                           */
#define EA_LPICON       0xfff9 /* Length preceeded icon              */
#define EA_ASCIIZFN     0xffef /* Asciiz file name of associated dat */
#define EA_ASCIIZEA     0xffee /* Asciiz EA of associated data       */
#define EA_MVMT         0xffdf /* Multi-value multi-typed field      */
#define EA_MVST         0xffde /* Multy value single type field      */
#define EA_ASN1         0xffdd /* ASN.1 field                        */

#define  HM_VALIDFLAGS  0x0003

/****** Resource IDs *****/

#define IDR_EAD             1
#define IDD_MULTILIST       2
#define IDD_BITMAP          3
#define IDD_ASCIIZ          4
#define ID_MENU_MAIN        5
#define IDD_OPENBOX         6
#define IDD_PATH            7
#define IDD_FILEEDIT        8
#define IDD_DIRLIST         9
#define IDD_FILELIST       10
#define IDD_MAIN           11
#define IDD_FNAME          12
#define IDD_LBOX           13
#define IDD_EATYPE         14
#define IDD_ADD            15
#define IDD_DELETE         16
#define IDD_COPY           17
#define IDD_EDIT           18
#define IDD_PASTE          19
#define IDD_NEWFILE        20
#define IDD_WRITE          21
#define IDD_QUIT           22
#define IDD_ADDEA          23
#define IDD_EANAME         24
#define IDD_NEW            25
#define IDD_EXISTING       26
#define IDD_ASCIIEDIT      27
#define IDD_EAVALUE        28
#define IDD_ICONWIN        29
#define BMP_EMPTY          30
#define IDD_ICON           31
#define IDD_MULTIBOX       32
#define DID_DONE           33
#define IDD_TITLE          34
#define IDD_LPDATA         35
#define IDD_NEEDBIT        36
#define IDD_CODEPAGE       37


/* Macros for allocating and deallocating memory from the heap.  They
   include a crude memory check and assume that the variable hwnd is
   defined in the environment the macro is executed in.               */

#define GetMem(x,y) x=(VOID *) WinAllocMem(hhp,y);       \
                    if(!x)                               \
                       WinMessageBox(HWND_DESKTOP, hwnd, \
                       "Sorry, Not enough memory",       \
                       NULL, 0, MB_OK);

#define ResizeMem(x,y,z) x=(VOID *) WinReallocMem(hhp,x,y,z); \
                         if(!x)                               \
                            WinMessageBox(HWND_DESKTOP, hwnd, \
                            "Sorry, Not enough memory",       \
                            NULL, 0, MB_OK);


#define FreeMem(x,y) WinFreeMem(hhp,(NPBYTE) x,(USHORT) y);


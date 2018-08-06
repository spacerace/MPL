/*
   DECLARE.H -- File Dialog Library Definitions
   Created by Microsoft Corporation, 1989
*/

/* from data.c */
extern HMODULE	vhModule;
extern HHEAP	vhheap;
extern PSTR	vrgsz[];

/* From tool.c */
PSZ  FAR PASCAL NextChar (PSZ);
PSZ  FAR PASCAL PrevChar (PSZ, PSZ);
PSZ  FAR PASCAL Upper (PSZ);

/* From tool1.c */
int   EXPENTRY AlertBox (HWND, int, PCH, PCH, USHORT, USHORT);
int   EXPENTRY DlgFile (HWND, PDLF);
void  EXPENTRY SetupDLF( PDLF, int, PHFILE, PSZ, PSZ, PSZ, PSZ);
BOOL  EXPENTRY MergeStrings(PSZ, PSZ, PSZ);

/* from init.c */
BOOL	 PASCAL InitLibrary (VOID);

/* from gpi.c */
ULONG	EXPENTRY GetTextExtent (HPS, PCH, int);

/* from file.c */
VOID      PASCAL DlgOpenName(HWND, PDLF);
VOID	  PASCAL DlgSaveAsName(HWND, PDLF);
VOID      PASCAL DlgInitOpen (HWND, ULONG);
VOID      PASCAL DlgInitSaveAs (HWND, ULONG);
VOID      PASCAL DlgAddSearchExt(PDLF, PSZ);
BOOL      PASCAL DlgSearchSpec(PSZ);
int       PASCAL DlgAlertBox(HWND, int, PDLF, USHORT);
VOID      PASCAL DlgRemoveHlpButton (HWND);
MRESULT EXPENTRY DlgOpenWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY DlgSaveAsWndProc(HWND, USHORT, MPARAM, MPARAM);
VOID	EXPENTRY AddExt(PSZ, PSZ);
PSZ	EXPENTRY FileInPath(PSZ);

/* from file1.c */
int    FAR PASCAL DlgDirList(HWND, PSZ, int, int, int, USHORT);
int    FAR PASCAL DlgDirSelect(HWND, PSZ, int);
BOOL	 EXPENTRY OpenFile(PSZ, PHANDLE, PSZ, USHORT);
PSZ        PASCAL DlgFitPathToBox(HWND, int, PSZ);
int        PASCAL DlgFillListBoxes(HWND, int, int, USHORT, PSZ);
USHORT     PASCAL DlgOpenFile(PDLF, HWND);
BOOL       PASCAL DlgParseFile(PSZ, PSZ, BOOL, BOOL);
BOOL       PASCAL DlgValidName(PSZ, BOOL);

/* from toola.asm */
extern char far szStarStar[];
extern char far szDot[];
int    EXPENTRY   lstrlen (PSZ);
VOID   EXPENTRY   lstrcpy (PSZ, PSZ);
VOID   EXPENTRY   lstrcat (PSZ, PSZ);
int    EXPENTRY   lstrcmp (PSZ, PSZ);
VOID   EXPENTRY   LCopyStruct (PCH, PCH, int);
VOID   EXPENTRY   LFillStruct (PCH, int, BYTE);
BOOL   FAR PASCAL LibInit (VOID);

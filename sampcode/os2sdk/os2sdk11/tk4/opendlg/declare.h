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
int   CALLBACK AlertBox (HWND, int, PCH, PCH, USHORT, USHORT);
int   CALLBACK DlgFile (HWND, PDLF);
void  CALLBACK SetupDLF( PDLF, int, PHFILE, PSZ, PSZ, PSZ, PSZ);
BOOL  CALLBACK MergeStrings(PSZ, PSZ, PSZ);

/* from init.c */
BOOL	 PASCAL InitLibrary (VOID);

/* from gpi.c */
ULONG	CALLBACK GetTextExtent (HPS, PCH, int);

/* from file.c */
VOID      PASCAL DlgOpenName(HWND, PDLF);
VOID	  PASCAL DlgSaveAsName(HWND, PDLF);
VOID      PASCAL DlgInitOpen (HWND, ULONG);
VOID      PASCAL DlgInitSaveAs (HWND, ULONG);
VOID      PASCAL DlgAddSearchExt(PDLF, PSZ);
BOOL      PASCAL DlgSearchSpec(PSZ);
int       PASCAL DlgAlertBox(HWND, int, PDLF, USHORT);
VOID      PASCAL DlgRemoveHlpButton (HWND);
MRESULT CALLBACK DlgOpenWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT CALLBACK DlgSaveAsWndProc(HWND, USHORT, MPARAM, MPARAM);
VOID	CALLBACK AddExt(PSZ, PSZ);
PSZ	CALLBACK FileInPath(PSZ);

/* from file1.c */
int    FAR PASCAL DlgDirList(HWND, PSZ, int, int, int, USHORT);
int    FAR PASCAL DlgDirSelect(HWND, PSZ, int);
BOOL	 CALLBACK OpenFile(PSZ, PHANDLE, PSZ, USHORT);
PSZ        PASCAL DlgFitPathToBox(HWND, int, PSZ);
int        PASCAL DlgFillListBoxes(HWND, int, int, USHORT, PSZ);
USHORT     PASCAL DlgOpenFile(PDLF, HWND);
BOOL       PASCAL DlgParseFile(PSZ, PSZ, BOOL, BOOL);
BOOL       PASCAL DlgValidName(PSZ, BOOL);

/* from toola.asm */
extern char far szStarStar[];
extern char far szDot[];
int    CALLBACK   lstrlen (PSZ);
VOID   CALLBACK   lstrcpy (PSZ, PSZ);
VOID   CALLBACK   lstrcat (PSZ, PSZ);
int    CALLBACK   lstrcmp (PSZ, PSZ);
VOID   CALLBACK   LCopyStruct (PCH, PCH, int);
VOID   CALLBACK   LFillStruct (PCH, int, BYTE);
BOOL   FAR PASCAL LibInit (VOID);

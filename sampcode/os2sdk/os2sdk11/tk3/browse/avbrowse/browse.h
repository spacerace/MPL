/* -----------------------------------------------------------------*\
/*	File Browser Demo Headers
/*	Version 1.0
/*	Created by Microsoft Corp. 1988
/* -----------------------------------------------------------------*/

/* ------------------------------------ */
/* forward declarations 		*/
/* ------------------------------------ */

/* -- top-level */
int cdecl main(int, char *[]);
void ReadFile(void);
void DosReadFile(void);

/* -- line-level    */
SHORT StoreLine(char *);
char * _loadds RetrieveLine(USHORT);

/* -- Window routines */
MRESULT CALLBACK BrowseWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT CALLBACK AboutDlgProc(HWND, USHORT, MPARAM, MPARAM);

/* ------------------------------------ */
/* constant definitions 		*/
/* ------------------------------------ */
#define NUM_DATA_LINES 4096

/**********************************************************************\
*  Resource Ids
\**********************************************************************/

#define ID_RESOURCE    1
#define IDD_ABOUT      2
#define IDD_SKEL       3


/**********************************************************************\
*  IDD - ID for Dialog item
*  IDM - ID for Menu commands
*  IDS - ID for String table
*  IDT - ID for Timers
\**********************************************************************/

#define ID_NULL         -1

#define IDM_FILE	10
#define	IDM_OPEN	0x0100
#define IDM_ABOUT       0x0101
#define	IDM_FONT	0x0102

#define IDS_CONTENT     10
#define IDS_CLOSE       11
#define IDS_OKCLOSE     12
#define IDS_SAVE        13
#define IDS_OKSAVE      14
#define IDS_ASYNC       15
#define IDS_OKASYNC     16
#define IDS_ABOUT       18
#define IDS_TITLE       19
#define IDS_DEFAULT     20

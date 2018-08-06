/*
   BIOrhythm header file
   Created by Microsoft Corp., 1988
*/
/* String table constants */
#define IDS_APPNAME      1
#define IDS_KIDNAME      2

/* Resource Ids */
#define ID_BIO          1
#define IDD_ABOUT       2
#define IDD_DATE        3

/* Menu constants */
#define IDM_DATES	0x100
#define IDM_LEGEND	0x101
#define	IDM_COPY	0x102
#define IDM_ABOUT	0x103

/* Bio Dialog Box constants */
#define ID_BDYEAR        301
#define ID_BDMONTH       302
#define ID_BDDAY         303
#define ID_YEAR          304
#define ID_MONTH         305
#define ID_DAY           306
#define ID_OS2INI        307

/* Function prototypes */
SHORT cdecl main	       ( void );
MRESULT CALLBACK BioWndProc    ( HWND, USHORT, MPARAM, MPARAM );
MRESULT CALLBACK KidWndProc    ( HWND, USHORT, MPARAM, MPARAM );
VOID	APIENTRY APPPaint      ( HWND );
MRESULT CALLBACK BioDlg        ( HWND, USHORT, MPARAM, MPARAM );
MRESULT CALLBACK About	       ( HWND, USHORT, MPARAM, MPARAM );
MRESULT CALLBACK FrameWndProc  ( HWND, USHORT, MPARAM, MPARAM );

double	    PASCAL julian      ( int, int, double );
void	    PASCAL calendar    ( double, int *, int *, double * );
long	    PASCAL trunc4      ( double );

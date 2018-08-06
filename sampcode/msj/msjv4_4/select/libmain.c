#include <windows.h>

HANDLE hInstLib;
HANDLE hBitmap;
#define XBITMAP    16
#define YBITMAP    16

static BYTE byBits[] = {
0xFE, 0xFF, 0xFE, 0x7F, 0xF8, 0x3F, 0xE6, 0x7F,
0xDE, 0xF7, 0xBF, 0xFB, 0x7F, 0xFD, 0x7F, 0xFD,
0x7F, 0xFD, 0x7F, 0xFD, 0xBF, 0xFB, 0xDE, 0xF7,
0xFC, 0xCF, 0xF8, 0x3F, 0xFC, 0xFF, 0xFE, 0xFF
};

/* some prototypes */
BOOL NEAR RegisterSelect (HANDLE hInstance);
long FAR PASCAL SelectWndProc( HWND hWnd, unsigned message,
    WORD wParam, LONG lParam);

/*
this function is called from the assembler
initialization in libentry.asm
*/
int FAR PASCAL LibMain(HANDLE hInstance, WORD wDataSeg, WORD wHeapSize,
    LPSTR lpCmdLine)
{
    int iret;
    if (wHeapSize == 0) return(0);
    iret = LocalInit(wDataSeg, NULL, (NPSTR)wHeapSize);
    UnlockData(0);
    RegisterSelect ( hInstLib = hInstance );
    return iret;
}

/*
this is called by LibMain to register the class
and initialize the bitmap
*/
static BOOL NEAR RegisterSelect (HANDLE hInstance)
{
    WNDCLASS wndclass;

    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = SelectWndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 2*sizeof(WORD);       /* space for control info */
    wndclass.hIcon         = NULL;
    wndclass.hInstance     = hInstance;
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = COLOR_WINDOW+1;
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = "select";

    hBitmap = CreateBitmap ( XBITMAP, YBITMAP, 1, 1, byBits );
    return RegisterClass(&wndclass);
}



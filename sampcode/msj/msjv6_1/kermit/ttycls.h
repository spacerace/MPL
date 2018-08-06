#ifdef WINDOWS
#define HPS HDC
#endif

/* 
 * this structure contains most of the information needed
 * to use the tty window.
 */

struct TTYWND {
    HWND hWnd;
#ifdef WINDOWS
    POINT Pos;
    HANDLE hVidBuf;
    HFONT hFont;
#else
    USHORT FontIndex;
    POINTL Pos;
#endif
    BYTE *pVidBuf;
    short CWidth, CHeight;
    short Top, Left;
    short Width, Height;
    short MaxLines,MaxCols;
    short MaxLineLength;
    short CurLineOffset;
    short oCurrentLine;
    short oVidLastLine;
    BOOL LFonCR;
    BOOL CRonLF;
    BOOL Wrap;
    BOOL LocalEcho;
    BYTE ebitmask;
};
typedef struct TTYWND *PTTYWND;

/* shared function declarations */
InitTTYWindow(PTTYWND,short,short,short,short,short,short,
			BOOL,BOOL,BOOL,unsigned short,BYTE);
int NEAR TTYDisplay(PTTYWND pTTYWnd, short len, BYTE *str);
void NEAR TTYWndPaint(PTTYWND pTTYWnd, HPS hPS, short top, short bottom);
void NEAR TTYClear(PTTYWND pTTYWnd);

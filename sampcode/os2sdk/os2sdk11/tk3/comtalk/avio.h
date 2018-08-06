/*
    avio.h -- AVIO module entry points
    Created by Microsoft Corporation, 1989
*/
void	AvioInit(HWND hWndFrame, HWND hWndClient);
void	AvioScroll(USHORT SB_Command, USHORT usPosition, BOOL fHorizontal);
MRESULT	AvioSize(HWND hWnd, USHORT msg, MPARAM mp1, MPARAM mp2);
void	AvioClose(void);
void	AvioPaint(HWND hWnd);
MRESULT	AvioMinMax(PSWP pSWP);
void	AvioClear(void);
void	AvioAdjustFrame(PSWP pSWP);
void	AvioTrackFrame(HWND hWnd, MPARAM mpTrackFlags);
void	AvioQueryTrackInfo(PTRACKINFO pTI);
BOOL	AvioUpdateLines(BOOL fPage, BOOL *fPaging);
void	AvioRedraw(void);
void	AvioStartup(HWND hWndClient);
void	AvioPageUp(void);

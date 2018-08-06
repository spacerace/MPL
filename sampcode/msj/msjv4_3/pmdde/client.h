/*  client.h */

#define ID_MASTER   1
#define ID_GRAPHICS1    100
#define ID_DDEANCHOR 700

#define IDM_EDIT        101

#define IDM_MOVE        300
#define IDM_COPY        301

#define WW_CONV_HWND    QWL_USER        /* dde    */
#define WW_CONV_FLAGS   QWL_USER+4      /* dde    */
#define WW_CLOSE        QWL_USER        /* client */
#define WW_CONVCOUNT    QWL_USER+4      /* client */
#define WW_MASTER_FLAGS QWL_USER+8      /* client */

#define WIN_MOVE_OBJECT  0x00000001

#define WIN_CLOSING_FLAG 0x00000001
#define WIN_TERM_FLAG    0x00000001

/* external variables */

extern HPS     hps1;
extern HAB     hab1;
extern HWND    DDEanchorHWND;

/* declarations from clinit.c */
extern  void far *ClientInit(void);

/* declarations from client.c */
extern MRESULT APIENTRY ClientWndProc(void far *hwnd, USHORT message, MPARAM lParam1, MPARAM lParam2);
extern MRESULT APIENTRY ClientDDEWndProc(void far *hwnd, USHORT message, MPARAM lParam1, MPARAM lParam2);
extern MRESULT APIENTRY ClientDDEParWndProc(void far *hwnd, USHORT message, MPARAM lParam1, MPARAM lParam2);

/* declarations from clientmn.c */
extern  int cdecl main(void);

/* ID's for the list view windows */
#define ID_GRAPHICS      200
#define ID_STD_LB        201

/* ID for the graphics segment */
#define IDSEG_PHONE      506

/* ID's for the menu options */
#define IDM_OPTION      1000
#define IDM_PHONE       1001
#define IDM_STD_LB      1002

#define IDM_TIMER       1100
#define IDM_TIMERON     1101
#define IDM_TIMEROFF    1102

/* Mode constants */
#define PHONE_MODE      2200
#define STD_LB_MODE     2201

/* Phone Colors Structure */
typedef struct {
               LONG   Body;
               LONG   BodyOutline;
               LONG   MsgBox;
               LONG   MsgBoxOutline;
               LONG   TextColor;
               LONG   FootColor;
               LONG   FootOutline;
               } PHCOLORS;

typedef PHCOLORS           *PPHCOLORS;

/* Window Word instance variables structure */
typedef struct {
               int         nMsgCnt;
               int         nMode;
               int         ConvCnt;
               BOOL        bInsert;
               BOOL        bClose ;
               HWND        hwndLink;
               BOOL        bAdvise;
               BOOL        bNoData;
               USHORT      usFormat;
               PHCOLORS    PhColors[2];
               } WWVARS;

typedef WWVARS             *PWWVARS;

/* Global Variables             */
extern HWND           hStdLB;
extern LONG           lPhFigCnt;
extern HAB            hab;
extern HWND           hwndDDE;

/* declarations from srvrini.c  */
HWND PhoneInit(void);
void InitPhoneSeg(HPS, int, PPHCOLORS);
void InitPhoneColors(PPHCOLORS, int);

/* declarations from server.c   */
MRESULT APIENTRY GraphicWndProc(void far *hwnd, USHORT message, MPARAM lParam1, MPARAM lParam2);
MRESULT APIENTRY ConversationWndProc(void far *hwnd, USHORT message, MPARAM lParam1, MPARAM lParam2);
PWWVARS InitWindowWordVars(void);

/* declarations from srvrmain.c */
int cdecl main(void);


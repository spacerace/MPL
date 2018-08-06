//----------------------------------------------
//-TRACER.H-------------------------------------
//----------------------------------------------
//----------------------------------------------
//-To use TRACER, follow these 5 steps.---------
//----------------------------------------------
//----1. Include this header in your program.---
//       Includes necessary macros.
//----------------------------------------------
//----2. Add macro TRACERVARIABLES above main().
//       Defines necessary variables / defines.-
//----------------------------------------------
//----3. Add macro TRACERHELLO in your WM_CREATE
//       Put between WM_CREATE and break;
//       Initiates handshaking with debugger.
//----------------------------------------------
//----4. Add macro TRACERATTACH in your WndProc.
//       Put as a seperate case in your WndProc.
//       Completes handshaking with debugger.
//----------------------------------------------
//----!!! When placing the TRACERATTACH in your-
//--------window procedure, don't forget the----
//--------break; statement after it. !!!--------
//----------------------------------------------
//----5. Use macro TRACER("string") freely.-----
//       This is the debug statement.
//----------------------------------------------
//----Note:-------------------------------------
//----You may need to include EXTERNTRACERVARS--
//----in secondary modules if you want to call--
//----TRACER from those modules.----------------
//----------------------------------------------

//----------------------------------------------
//-Note:----------------------------------------
//----------------------------------------------
//----In the TRACERHELLO macro, there is a param
//----called hWnd.  You need to pass your window
//----handle to me so that I can send mine back-
//----to you.  You may need to change "hWnd" to-
//----whatever your wndproc parameter name is---
//----for window handle ( e.g. MyhWnd ).--------
//----------------------------------------------

#define TRACER_SEGMENT     "\\SHAREMEM\\TRACER.SEG"
#define TRACER_SEMAPHORE   "\\SEM\\TRACER.SEM"

#define TRACERICON         1
#define ID_HELPBUTTON      2
#define ID_TRACERLB        3
#define IDMOPTIONS         4
#define IDMABOUT           5
#define ID_OK              6
#define ID_MENU            7
#define IDMCLEAR           8
#define IDMLOG             9
#define ID_CANCEL          10
#define ID_LOGFILEEDIT     11
#define ID_LOGFILESCROLL   12
#define ID_SCREENSCROLL    13
#define ID_REFRESH         14

// Step 1 - INCLUDE THIS FILE IN YOUR APP CODE AS FOLLOWS

//#include <tracer.h>

#define TRACER_REQUEST_HANDLE   WM_USER + 501
#define TRACER_RECIEVE_HANDLE   WM_USER + 502
#define TRACER_REQUEST_ACTION   WM_USER + 503
#define TRACER_GOODBYE          WM_USER + 504
#define TRACER_MYCLOSE          WM_USER + 505

// Step 2 - INCLUDE THE TRACERVARIABLES STATEMENT IN YOUR
//                       DECLARATION SECTION

#define TRACERVARIABLES                                            \
unsigned short      sTracerSelector;                               \
         HSYSSEM    hTracerSysSem;                                 \
         BOOL       bTracerConnected;                              \
         char far * szSelector_string;                             \
         HWND       hTracerWnd;


// Step 3 - INCLUDE THE TRACERHELLO STATEMENT IN YOUR
//                      WM_CREATE MESSAGE

#define TRACERHELLO                                                \
WinBroadcastMsg( hWnd, TRACER_REQUEST_HANDLE,                      \
                    MPFROMHWND( hWnd ), 0L,                        \
                        BMSG_FRAMEONLY | BMSG_POSTQUEUE );


// Step 4 - INCLUDE THE TRACERATTACH STATEMENT IN YOUR
//                        MESSAGE WNDPROC

#define TRACERATTACH                                               \
case TRACER_RECIEVE_HANDLE:                                        \
if ( bTracerConnected )                                            \
    break;                                                         \
hTracerWnd = HWNDFROMMP( mp1 );                                    \
bTracerConnected = 1;                                              \
if ( DosGetShrSeg( TRACER_SEGMENT, &sTracerSelector ) )            \
    bTracerConnected = 0;                                          \
if ( DosOpenSem( &hTracerSysSem, TRACER_SEMAPHORE ) )              \
    bTracerConnected = 0;

// Step 5 - THEN USE THE TRACER("HELLO, WORLD") MACRO FREELY

#define TRACER(s)                                                  \
 if ( bTracerConnected )                                           \
 {                                                                 \
 DosSemRequest( hTracerSysSem, -1L );                              \
 szSelector_string =                                               \
     ( char far * )( ( unsigned long )sTracerSelector << 16 );     \
 strncpy( ( char far * )szSelector_string, ( char far * )s, 80 );  \
 szSelector_string[80] = '\0';                                     \
 WinSendMsg( hTracerWnd, TRACER_REQUEST_ACTION, 0L, 0L );          \
 DosSemClear( hTracerSysSem );                                     \
 }

#define EXTERNTRACERVARS                                           \
extern unsigned short      sTracerSelector;                        \
extern          HSYSSEM    hTracerSysSem;                          \
extern          BOOL       bTracerConnected;                       \
extern          char far * szSelector_string;                      \
extern          HWND       hTracerWnd;

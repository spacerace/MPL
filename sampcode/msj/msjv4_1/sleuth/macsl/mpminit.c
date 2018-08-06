/*-----------------------------------------------------------------*/
/* MpmInit.c                                                       */
/* Initialization functions                                        */
/*-----------------------------------------------------------------*/

#include "MacPM.h"

/*-----------------------------------------------------------------*/

LOCAL BOOL      MpmInitSysValues( VOID );

/*-----------------------------------------------------------------*/
/* Create the application's message queue.  Well, not really!      */
/* We use the Mac's event queue for our messages, so just take     */
/* of all the other initialization (except memory management).     */
/*-----------------------------------------------------------------*/

HMQ APIENTRY WinCreateMsgQueue( hab, sSize )
    HAB         hab;
    SHORT       sSize;
{
    PMYWND      pwnd;

    /* Generic Macintosh initialization (already did memory stuff) */
    
    InitGraf( &thePort );
    InitFonts();
    FlushEvents( everyEvent, 0 );
    SetEventMask( everyEvent );
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs( 0L );
    InitCursor();

    /* Initialize the SV_ values and register the predefined
       window classes */

    MpmInitSysValues();

    if( ! WinRegisterClass( hab, WC_BUTTON, MpmFnwpButton,
                            CS_MOVENOTIFY | CS_PUBLIC, 0 ) )
      return NULL;

    if( ! WinRegisterClass( hab, WC_DESKTOP, MpmFnwpDesktop,
                            CS_PUBLIC, 0 ) )
      return NULL;

#ifdef FUTURE
    if( ! WinRegisterClass( hab, WC_DIALOG, MpmFnwpDialog,
                            CS_MOVENOTIFY | CS_PUBLIC, 0 ) )
      return NULL;
#endif

    if( ! WinRegisterClass( hab, WC_ENTRYFIELD, MpmFnwpEntryField,
                            CS_MOVENOTIFY | CS_PUBLIC, 0 ) )
      return NULL;

    if( ! WinRegisterClass( hab, WC_FRAME, MpmFnwpFrame,
                            CS_MOVENOTIFY | CS_PUBLIC, 0x20 /*??*/ ) )
      return NULL;

    if( ! WinRegisterClass( hab, WC_LISTBOX, MpmFnwpListBox,
                            CS_MOVENOTIFY | CS_PUBLIC, 0 ) )
      return NULL;

    if( ! WinRegisterClass( hab, WC_MENU, MpmFnwpMenu,
                            CS_MOVENOTIFY | CS_PUBLIC, 0 ) )
      return NULL;

    if( ! WinRegisterClass( hab, WC_SCROLLBAR, MpmFnwpScrollBar,
                            CS_MOVENOTIFY | CS_PUBLIC, 0 ) )
      return NULL;

    if( ! WinRegisterClass( hab, WC_SIZEBORDER, MpmFnwpSizeBorder,
                            CS_MOVENOTIFY | CS_PUBLIC, 0 ) )
      return NULL;

    if( ! WinRegisterClass( hab, WC_STATIC, MpmFnwpStatic,
                            CS_MOVENOTIFY | CS_PUBLIC, 0 ) )
      return NULL;

    if( ! WinRegisterClass( hab, WC_TITLEBAR, MpmFnwpTitleBar,
                            CS_MOVENOTIFY | CS_PUBLIC, 0 ) )
      return NULL;

    /* Create the object and desktop windows */

    _hwndObject =
      WinCreateWindow(
        NULL, WC_DESKTOP, _szNull,
        0, 0, 0, 0, 0,
        NULL, NULL, 0, NULL, NULL
      );
    if( ! _hwndObject )
      return NULL;

    pwnd = PMYWNDOF(_hwndObject);
    pwnd->ucKind = WK_OBJECT;
    pwnd->pfnwp  = MpmFnwpObject;

    _hwndDesktop =
      WinCreateWindow(
        NULL, WC_DESKTOP, _szNull,
        WS_DISABLED,
        0, 0, 0, 0,
        NULL, NULL, 0, NULL, NULL
      );
    if( ! _hwndDesktop )
      return NULL;

    pwnd = PMYWNDOF(_hwndDesktop);
    pwnd->cx = _alSysVal[SV_CXSCREEN];
    pwnd->cy = _alSysVal[SV_CYSCREEN];
    pwnd->flStyle |= WS_VISIBLE;

    return (HMQ)1;
}

/*-----------------------------------------------------------------*/
/* Destroy the message queue.  Since we use the Mac's event queue, */
/* there isn't anything to do here.                                */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinDestroyMsgQueue( hmq )
    HMQ         hmq;
{
    return TRUE;
}

/*-----------------------------------------------------------------*/
/* First phase of initialization - just do the memory management   */
/* stuff here.                                                     */
/*-----------------------------------------------------------------*/

HAB APIENTRY WinInitialize( fOptions )
    USHORT      fOptions;
{
    /* Static initialization - less hassle this way? */

    _pps1 = &_ps1;
    _hps1 = (HPS)&_pps1;

    /* Macintosh memory management initialization */

    MaxApplZone();
    MoreMasters();
    MoreMasters();
    MoreMasters();

    /* Open resource file - TEMP */

    OpenResFile( "\pMacPM.rsrc" );
    
    return (HAB)1;
}

/*-----------------------------------------------------------------*/
/* Last phase of application shutdown.  Nothing left to do!        */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinTerminate( hab )
    HAB         hab;
{
    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Initialize the SV_ system values.                               */
/* This could be made more compact with a static initializer!      */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmInitSysValues()
{
    memzero( &_alSysVal );

    _alSysVal[SV_DBLCLKTIME]      = GetDblTime() * 1000 / 60;
    _alSysVal[SV_CXDBLCLK]        = 5;
    _alSysVal[SV_CYDBLCLK]        = 5;
    _alSysVal[SV_ALARM]           = TRUE;
    _alSysVal[SV_CURSORRATE]      = GetCaretTime() * 1000 / 60;
    _alSysVal[SV_CXSCREEN]        = screenBits.bounds.right;
    _alSysVal[SV_CYSCREEN]        = screenBits.bounds.bottom;
    _alSysVal[SV_CXVSCROLL]       = 16;
    _alSysVal[SV_CYHSCROLL]       = 16;
    _alSysVal[SV_CYVSCROLLARROW]  = 16;
    _alSysVal[SV_CXHSCROLLARROW]  = 16;
    _alSysVal[SV_CXBORDER]        = 1;
    _alSysVal[SV_CYBORDER]        = 1;
    _alSysVal[SV_CXDLGFRAME]      = 6;
    _alSysVal[SV_CYDLGFRAME]      = 6;
    _alSysVal[SV_CYTITLEBAR]      = 18;
    _alSysVal[SV_CYVSLIDER]       = 16;
    _alSysVal[SV_CXHSLIDER]       = 16;
    _alSysVal[SV_CXMINMAXBUTTON]  = 10;
    _alSysVal[SV_CYMINMAXBUTTON]  = 10;
    _alSysVal[SV_CYMENU]          = 18;
    _alSysVal[SV_CXFULLSCREEN]    = _alSysVal[SV_CXSCREEN];
    _alSysVal[SV_CYFULLSCREEN]    = _alSysVal[SV_CYSCREEN];
    _alSysVal[SV_CXICON]          = 32;
    _alSysVal[SV_CYICON]          = 32;
    _alSysVal[SV_CXPOINTER]       = 16;
    _alSysVal[SV_CYPOINTER]       = 16;
    _alSysVal[SV_DEBUG]           = MPM_DEBUG;
    _alSysVal[SV_CMOUSEBUTTONS]   = 1;
    _alSysVal[SV_POINTERLEVEL]    = 0;
    _alSysVal[SV_CURSORLEVEL]     = 1;
    _alSysVal[SV_TRACKRECTLEVEL]  = 0;
    _alSysVal[SV_CTIMERS]         = 0;
    _alSysVal[SV_MOUSEPRESENT]    = TRUE;

    return TRUE;
}

/*-----------------------------------------------------------------*/

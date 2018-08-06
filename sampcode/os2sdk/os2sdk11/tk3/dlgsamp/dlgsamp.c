/*
    DLGSAMP -- Dialog Box Sample Application
    Created by Microsoft Corporation, 1989
*/
 
#define INCL_WINBUTTONS
#define INCL_WINDIALOGS
#define INCL_WINERRORS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINLISTBOXES
#define INCL_WINMENUS
#define INCL_WINMESSAGEMGR
#define INCL_WINRECTANGLES
#define INCL_WINSWITCHLIST
#define INCL_WINSYS
#define INCL_WINWINDOWMGR
#define M_I86L
#include <os2.h>
 
#include <string.h>
#include <stdlib.h>
 
#include "dlgsamp.h"
#include "dlgsamp1.h"
 
/*
    Function Prototypes
*/
 
VOID NEAR cdecl main(VOID);

/* Local Routines */ 
VOID cdecl CenterDlgBox(HWND);
VOID cdecl CheckColor(HWND, SHORT, COLOR *);
VOID cdecl EnableModality(HWND, BOOL);
BOOL cdecl IsIntInRange(HWND, SHORT, SHORT, SHORT, SHORT, SHORT);
VOID cdecl LoadDialog(HWND, HWND, SHORT, PFNWP, BOOL);
VOID cdecl MainWndCommand(HWND, USHORT, BOOL *);
VOID cdecl MainWndPaint(HWND);
VOID cdecl SetModality(HWND, BOOL);
VOID cdecl Trace(PSZ, PSZ);
 
/* Window Procedures */
MRESULT CALLBACK fnwpMainWnd(HWND, USHORT, MPARAM, MPARAM);
MRESULT CALLBACK fnwpEntryFieldDlg(HWND, USHORT, MPARAM, MPARAM);
MRESULT CALLBACK fnwpAutoRadioButtonDlg(HWND, USHORT, MPARAM, MPARAM);
MRESULT CALLBACK fnwpCheckBoxDlg(HWND, USHORT, MPARAM, MPARAM);
MRESULT CALLBACK fnwpListBoxDlg(HWND, USHORT, MPARAM, MPARAM);
MRESULT CALLBACK fnwpAboutBoxDlg(HWND, USHORT, MPARAM, MPARAM);
 
/*
    Global variables
*/ 
COLOR colorClient = CLR_RED | CLR_BLUE;    /* Color of client area    */
 
CHAR  szEntryField1[10] = "";              /* Used to pass back info  */
CHAR  szEntryField2[10] = "";              /* from entry fields       */
                                           /* in EntryFieldDlg        */
 
BOOL  bModality = TRUE;                    /* Does the user want modal*/
                                           /* or modeless dialogs?    */
COLOR colorSave;
CHAR  szSelection[LEN_LISTBOXENTRY] = "";  /* Used to pass back       */
                                           /* list box item selected  */
                                           /* in ListBoxDlg           */
 
HAB   hab;                                 /* Anchor block handle     */
HWND  hwndClient;                          /* Client Window handle    */
HWND  hwndFrame;                           /* Frame Window handle     */
HWND  hwndModelessDlg;                     /* Modeless Dialog handle  */
 
/**************************************************************************
*
*   FUNCTION: main
*
*   Typical PM main function which initializes PM, creates a message queue,
*   registers a window class, creates a window, gets and dispatches
*   messages to its winproc until its time to quit, and then tidies up
*   before terminating.
*
**************************************************************************/
 
VOID NEAR cdecl main(  )
{
  HMQ     hmq;                        /* Message Queue handle         */
  QMSG    qmsg;                       /* Message                      */
  ULONG   flCreate;
 
  hab   = WinInitialize( NULL );      /* Initialize PM                */
  hmq   = WinCreateMsgQueue( hab, 0 );/* Create application msg queue */
 
  WinRegisterClass(                   /* Register Window Class        */
      hab,                            /* Anchor block handle          */
      "DlgSamp Class",                /* Window Class name            */
      fnwpMainWnd,                    /* Address of Window Procedure  */
      (ULONG) NULL,                   /* No special class style       */
      0                               /* No extra window words        */
      );
 
  flCreate = FCF_STANDARD & ~FCF_ACCELTABLE;
 
  hwndFrame = WinCreateStdWindow(
        HWND_DESKTOP,                   /* Desktop Window is parent     */
        WS_VISIBLE,                     /* Window styles                */
        (PVOID)&flCreate,               /* Window creation parameters   */
        "DlgSamp Class",                /* Window Class name            */
        "",                             /* Window Text                  */
        0L,                             /* Client style                 */
        NULL,                           /* Module handle                */
        ID_MAINWND,                     /* Window ID                    */
        (HWND FAR *)&hwndClient         /* Client Window handle         */
        );
 
 
  /* Message Loop */
  while( WinGetMsg( hab, (PQMSG)&qmsg, (HWND)NULL, 0, 0 ) )
    WinDispatchMsg( hab, (PQMSG)&qmsg );
 
  /* Cleanup code */
  WinDestroyWindow( hwndFrame );
  WinDestroyMsgQueue( hmq );
  WinTerminate( hab );
}
 
/***********************************************************************
*
*   WinProc: fnwpMainWnd
*
*   Controls the state of the menu, and loads various dialogs.  The
*   dialogs will be modal or modeless depending on the setting of the
*   Modality menuitem.
*
***********************************************************************/
 
MRESULT  CALLBACK fnwpMainWnd( hwnd, message, mp1, mp2 )
HWND    hwnd;
USHORT  message;
MPARAM  mp1;
MPARAM  mp2;
{
  USHORT   Command;   /* Command passed by WM_COMMAND */
  SHORT    id;        /* ID of item selected from the list box */
 
  switch(message)
  {
    case WM_PAINT:
      MainWndPaint( hwnd );  /* Invoke window painting routine */
      break;
    case WM_HELP:
/*********************************************************************
*
*   This will be received when either:-
*
*   1. The user hits the F1 key
*   2. The user clicks on the action bar item F1=Help
*
*********************************************************************/
      WinMessageBox( HWND_DESKTOP,
                     hwndFrame,
                     (PSZ)"Dialog Sample Application: Help",
		     (PSZ)"Try out the pulldown menus, or Alt+selection",
                     ID_MB,
                     MB_OK );
      break;
    case WM_COMMAND:
      Command = SHORT1FROMMP( mp1 );
      MainWndCommand( hwnd, Command, &bModality );
      break;
    case DLGSAMP_EFCOMPLETE:
      WinQueryWindowText( WinWindowFromID( hwndModelessDlg, EF_1 ),
                          sizeof( szEntryField1 ), szEntryField1 );
      WinQueryWindowText( WinWindowFromID( hwndModelessDlg, EF_2 ),
                          sizeof( szEntryField2 ), szEntryField2 );
      WinInvalidateRect( hwnd, NULL, FALSE );/* Request whole window repaint */
      break;
    case DLGSAMP_LBCOMPLETE:
      id = SHORT1FROMMR( WinSendDlgItemMsg( hwndModelessDlg,
                                            LB_1,
                                            LM_QUERYSELECTION,
                                            0L,
                                            0L ) );
      if( id == LIT_NONE )
        strcpy( szSelection, "" );
      else
        WinSendDlgItemMsg( hwndModelessDlg,
                           LB_1,
                           LM_QUERYITEMTEXT,
                           MPFROM2SHORT( id, sizeof( szSelection ) ),
                           MPFROMP( szSelection ) );
      break;
    case DLGSAMP_RBCOMPLETE:
    case DLGSAMP_CBCOMPLETE:
      break;
    case DLGSAMP_DESTROYDLG:
      WinDestroyWindow( hwndModelessDlg );
      EnableModality( hwndFrame, TRUE );
      WinInvalidateRect( hwnd, NULL, FALSE );/* Request whole window repaint */
      break;
    case WM_CLOSE:
      WinPostMsg( hwnd, WM_QUIT, 0L, 0L );  /* Cause termination    */
      break;
    default:
      return WinDefWindowProc( hwnd, message, mp1, mp2 );
  }
  return FALSE;
}
 
/***********************************************************************
*
*  DlgProc:  fnwpEntryFieldDlg
*
*  A dialog proc which captures and validates the contents of two
*  entry fields.
*
***********************************************************************/
 
MRESULT CALLBACK fnwpEntryFieldDlg( hwndDlg, message, mp1, mp2 )
HWND    hwndDlg;
USHORT  message;
MPARAM  mp1;
MPARAM  mp2;
{
  switch (message)
  {
    case WM_INITDLG:
      CenterDlgBox( hwndDlg );
      break;
    case WM_COMMAND:
      switch( SHORT1FROMMP( mp1 ) )
      {
        case DID_OK: /* Enter key or pushbutton pressed/ selected */
 
/***************************************************************************
*
*   Validate the contents of the two entry fields
*
***************************************************************************/
 
          if( !IsIntInRange( hwndDlg, EF_1, 1, 100, ERR_EFINVALID, ID_MB ) )
            return FALSE;
          if( !IsIntInRange( hwndDlg, EF_2, 1, 100, ERR_EFINVALID, ID_MB ) )
            return FALSE;
 
/***************************************************************************
*
*   A modal dialog is destroyed before control is returned to the
*   invoking winproc, so it must pass the contents of its Entry Fields etc.
*   back to the invoking window before it returns.
*
*   When a modeless dialog box returns it still continues to exist. It
*   could pass the contents of its Entry Fields etc. back to the
*   invoking window in several ways.
*
*   Here a user message is posted to the invoking window to say that the
*   dialog has completed. The invoking window then has an opportunity
*   to extract the contents of the Entry Fields etc.
*
***************************************************************************/
 
          if( bModality )
          {
            WinQueryWindowText( WinWindowFromID( hwndDlg, EF_1 ),
                                sizeof( szEntryField1),
                                szEntryField1 );
            WinQueryWindowText( WinWindowFromID( hwndDlg, EF_2 ),
                                sizeof( szEntryField2),
                                szEntryField2 );
          }
          else
            WinPostMsg( hwndClient, DLGSAMP_EFCOMPLETE, 0L, 0L );
 
        case DID_CANCEL:/* Escape key or CANCEL pushbutton pressed/selected */
          if( bModality )
            WinDismissDlg( hwndDlg,TRUE );
          else
            WinPostMsg( hwndClient, DLGSAMP_DESTROYDLG, 0L, 0L );
          return FALSE;
        default:
          break;
      }
      break;
 
    default:  /* Pass all other messages to the default dialog proc */
      return WinDefDlgProc( hwndDlg, message, mp1, mp2 );
  }
  return FALSE;
}
 
/***********************************************************************
*
*  DlgProc:  fnwpAutoRadioButtonDlg
*
*  A dialog procedure which uses auto radio buttons to change the
*  color of the Client Area window.
*
***********************************************************************/
 
MRESULT CALLBACK fnwpAutoRadioButtonDlg( hwndDlg, message, mp1, mp2 )
HWND    hwndDlg;
USHORT  message;
MPARAM  mp1;
MPARAM  mp2;
{
  switch (message)
  {
    case WM_INITDLG:
      colorSave = colorClient;
      CenterDlgBox( hwndDlg );
      if ( colorClient == CLR_RED )
	WinPostMsg( WinWindowFromID( hwndDlg, RB_RED ),
		    BM_SETCHECK,
		    MPFROM2SHORT( TRUE, 0 ),
		    0L );

      if ( colorClient == CLR_GREEN )
	WinPostMsg( WinWindowFromID( hwndDlg, RB_GREEN ),
		    BM_SETCHECK,
		    MPFROM2SHORT( TRUE, 0 ),
		    0L );

      if ( colorClient == CLR_BLUE )
        WinPostMsg( WinWindowFromID( hwndDlg, RB_BLUE ),
		    BM_SETCHECK,
		    MPFROM2SHORT( TRUE, 0 ),
		    0L );

      break;
    case WM_CONTROL:
      if( SHORT2FROMMP( mp1 ) == BN_CLICKED )
        switch( SHORT1FROMMP( mp1 ) )
        {
          case RB_RED:
            colorClient = CLR_RED;
            break;
          case RB_GREEN:
            colorClient = CLR_GREEN;
            break;
          case RB_BLUE:
            colorClient = CLR_BLUE;
            break;
          default:
           return FALSE;
        }
        WinInvalidateRect( hwndClient, NULL, FALSE );
        break;
    case WM_COMMAND:
      switch( SHORT1FROMMP( mp1 ) )
      {
        case DID_OK:     /* Enter key or pushbutton pressed/ selected        */
          if( !bModality )
            WinPostMsg( hwndClient, DLGSAMP_RBCOMPLETE, 0L, 0L );
          break;
        case DID_CANCEL: /* Escape key or CANCEL pushbutton pressed/selected */
          colorClient = colorSave;
          break;
        default:
          return WinDefDlgProc( hwndDlg, message, mp1, mp2 );
      }
      if( bModality )
        WinDismissDlg( hwndDlg, TRUE );
      else
        WinPostMsg( hwndClient, DLGSAMP_DESTROYDLG, 0L, 0L );
      break;
 
    default:  /* Pass all other messages to the default dialog proc */
      return WinDefDlgProc( hwndDlg, message, mp1, mp2 );
  }
  return FALSE;
}
 
 
/***********************************************************************
*
*  DlgProc:  fnwpCheckBoxDlg
*
*  A dialog procedure to which use checkboxes to change the color
*  of the Client Area Window.
*
***********************************************************************/
 
MRESULT CALLBACK fnwpCheckBoxDlg( hwndDlg, message, mp1, mp2 )
HWND    hwndDlg;
USHORT  message;
MPARAM  mp1;
MPARAM  mp2;
{
  switch (message)
  {
    case WM_INITDLG:
      CenterDlgBox( hwndDlg );
      colorSave = colorClient;
      if( (colorClient & CLR_RED) == CLR_RED )
        WinPostMsg( WinWindowFromID( hwndDlg, CB_RED ),
                    BM_SETCHECK,
                    MPFROM2SHORT( TRUE,0 ),
                    0L );
      if( (colorClient & CLR_GREEN) == CLR_GREEN )
        WinPostMsg( WinWindowFromID( hwndDlg, CB_GREEN ),
                    BM_SETCHECK,
                    MPFROM2SHORT( TRUE,0 ),
                    0L );
      if( (colorClient & CLR_BLUE) == CLR_BLUE )
        WinPostMsg( WinWindowFromID( hwndDlg, CB_BLUE ),
                    BM_SETCHECK,
                    MPFROM2SHORT( TRUE,0 ),
                    0L );
      break;
    case WM_CONTROL:                  /* User has clicked on a checkbox */
      if( SHORT2FROMMP( mp1 ) == BN_CLICKED )
        CheckColor( hwndDlg, SHORT1FROMMP( mp1 ), &colorClient );
      WinInvalidateRect( hwndClient, NULL, FALSE );
      break;
    case WM_COMMAND:
      switch( SHORT1FROMMP( mp1 ) )
      {
        case DID_OK:     /* Enter key or pushbutton pressed/ selected */
          if( !bModality )
            WinPostMsg( hwndClient, DLGSAMP_CBCOMPLETE, 0L, 0L );
          break;
        case DID_CANCEL: /* Escape key or CANCEL pushbutton pressed/selected */
          colorClient = colorSave;
          break;
        default:
          return WinDefDlgProc( hwndDlg, message, mp1, mp2 );
      }
      if( bModality )
        WinDismissDlg( hwndDlg, TRUE );
      else
        WinPostMsg( hwndClient, DLGSAMP_DESTROYDLG, 0L, 0L );
      return FALSE;
 
    default:  /* Pass all other messages to the default dialog proc */
      return WinDefDlgProc( hwndDlg, message, mp1, mp2 );
  }
  return FALSE;
}
 
/***********************************************************************
*
*  DlgProc:  fnwpListBoxDlg
*
***********************************************************************/
 
MRESULT CALLBACK fnwpListBoxDlg( hwndDlg, message, mp1, mp2 )
HWND    hwndDlg;
USHORT  message;
MPARAM  mp1;
MPARAM  mp2;
{
  CHAR szBuffer[LEN_LISTBOXENTRY];
  SHORT  i;
  SHORT  id;
 
  switch (message)
  {
    case WM_INITDLG:
      CenterDlgBox( hwndDlg );
 
/*************************************************************************
*
*   Initialize the listbox with a set of strings loaded from a
*   resource file.
*
*************************************************************************/
 
      for ( i = 0; i < NUM_LISTBOXENTRIES; i++ )
      {
        WinLoadString( hab,
                       NULL,
                       LBI_1 + i,
                       LEN_LISTBOXENTRY,
                       (PSZ)szBuffer
                     );
        WinSendDlgItemMsg( hwndDlg,
                           LB_1,
                           LM_INSERTITEM,
                           MPFROM2SHORT( LIT_END, 0 ),
                           MPFROMP( szBuffer )
                         );
      }
      break;
    case WM_COMMAND:
      switch( SHORT1FROMMP( mp1 ) )
      {
        case DID_OK:     /* Enter key or pushbutton pressed/ selected */
          if( bModality )
          {
 
/***********************************************************************
*
*   Find out which item (if any) was selected and return the selected
*   item text.
*
***********************************************************************/
 
            id = SHORT1FROMMR( WinSendDlgItemMsg( hwndDlg,
                                                  LB_1,
                                                  LM_QUERYSELECTION,
                                                  0L,
                                                  0L ) );
            if( id == LIT_NONE )
              strcpy( szSelection, "" );
            else
              WinSendDlgItemMsg( hwndDlg,
                                 LB_1,
                                 LM_QUERYITEMTEXT,
                                 MPFROM2SHORT( id, LEN_LISTBOXENTRY ),
                                 MPFROMP( szSelection ) );
          }
          else
            WinPostMsg( hwndClient, DLGSAMP_LBCOMPLETE, 0L, 0L );
        case DID_CANCEL: /* Escape key or CANCEL pushbutton pressed/selected */
          if( bModality )
            WinDismissDlg( hwndDlg, TRUE );
          else
            WinPostMsg( hwndClient, DLGSAMP_DESTROYDLG, 0L, 0L );
          return FALSE;
        default:
          break;
      }
      break;
 
    default:  /* Pass all other messages to the default dialog proc */
      return WinDefDlgProc( hwndDlg, message, mp1, mp2 );
  }
  return FALSE;
}
 
/*************************************************************************
*
*   FUNCTION : CenterDlgBox
*
*   Positions the dialog box in the center of the screen
*
*************************************************************************/
 
VOID cdecl CenterDlgBox( hwnd )
HWND hwnd;
{
  SHORT ix, iy;
  SHORT iwidth, idepth;
  SWP   swp;
 
  iwidth = (SHORT)WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
  idepth = (SHORT)WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );
  WinQueryWindowPos( hwnd, (PSWP)&swp );
  ix = ( iwidth  - swp.cx ) / 2;
  iy = ( idepth  - swp.cy ) / 2;
  WinSetWindowPos( hwnd, HWND_TOP, ix, iy, 0, 0, SWP_MOVE );
}
 
/***************************************************************************
*
*  FUNCTION: CheckColor
*
*  Toggle the Checked/UnChecked state of a checkbox and add/remove
*  the corresponding CLR color component of the Client Area Color.
*
***************************************************************************/
 
VOID cdecl CheckColor( hwndDlg, iDlgItem, colorClient )
HWND   hwndDlg;
SHORT  iDlgItem;
COLOR *colorClient;
{
  BOOL  bChecked;
  COLOR color;
 
  switch( iDlgItem )
  {
    case CB_RED:
      color = CLR_RED;
      break;
    case CB_GREEN:
      color = CLR_GREEN;
      break;
    case CB_BLUE:
      color = CLR_BLUE;
      break;
    default:
      return;
  }
 
  bChecked = SHORT1FROMMR( WinSendMsg( WinWindowFromID( hwndDlg , iDlgItem ),
                                       BM_QUERYCHECK,
                                       0L,
                                       0L ) );
  WinPostMsg( WinWindowFromID( hwndDlg, iDlgItem ),
              BM_SETCHECK,
              MPFROM2SHORT( !bChecked, 0 ),
              0L );
  if( bChecked )                   /* If color previously checked */
    *colorClient -= color;         /* subtract it ... else        */
  else
    *colorClient += color;         /* ... add it.                 */
}
 
/**************************************************************************
*
*   FUNCTION: EnableModality
*
*   Enable or disable the Modality menuitems depending on the value
*   of modal. This is done to prevent the user from altering the
*   modality setting while a modeless dialog is active.
*
**************************************************************************/
 
VOID cdecl EnableModality( hwnd, bModal )
HWND hwnd;
BOOL bModal;
{
  if( bModal )
  {
    WinPostMsg( WinWindowFromID( hwnd, FID_MENU ),
                MM_SETITEMATTR,
                MPFROM2SHORT( MI_MODAL, TRUE ),
                MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED ) );
    WinPostMsg( WinWindowFromID( hwnd, FID_MENU ),
                MM_SETITEMATTR,
                MPFROM2SHORT( MI_MODELESS, TRUE ),
                MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED ) );
  }
  else
  {
    WinPostMsg( WinWindowFromID( hwnd, FID_MENU ),
                MM_SETITEMATTR,
                MPFROM2SHORT( MI_MODAL, TRUE ),
                MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED ) );
    WinPostMsg( WinWindowFromID( hwnd, FID_MENU ),
                MM_SETITEMATTR,
                MPFROM2SHORT( MI_MODELESS, TRUE ),
                MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED ) );
  }
}
 
/***************************************************************************
*
*  FUNCTION: IsIntInRange.
*
*  Checks whether the value of a dialog item is in an integer in
*  a given range.
*
***************************************************************************/
 
BOOL cdecl IsIntInRange( hwndDlg,  idEntryField,
                         iLoRange, iHiRange,
                         idErrMsg, idMessageBox )
HWND hwndDlg;
SHORT  idEntryField;
SHORT  iLoRange;
SHORT  iHiRange;
SHORT  idErrMsg;
SHORT  idMessageBox;
{
  SHORT ivalue;
  CHAR  szErrMsg[80];
 
/****************************************************************************
*
*   Validate an entry field.
*
*   If validation fails leave the dialog visible, issue an error message
*   using a messagebox, and when the user dismisses the messagebox,
*   set the input focus to the entry field containing the error. Leave
*   the contents of the entry field unchanged, and return FALSE.
*
*   If validation is successful return the value in ivalue and return
*   TRUE.
*
****************************************************************************/
 
  if( !WinQueryDlgItemShort( hwndDlg, idEntryField, &ivalue, TRUE ) ||
      ( ivalue < iLoRange ) ||
      ( ivalue > iHiRange ) )
  {
    WinLoadString( hab, NULL, idErrMsg, sizeof( szErrMsg ), szErrMsg );
    WinMessageBox( HWND_DESKTOP,
                   hwndFrame,
                   (PSZ)szErrMsg,
                   NULL,
                   idMessageBox,
                   MB_OK );
    WinSetFocus( HWND_DESKTOP, WinWindowFromID( hwndDlg, idEntryField ) );
    return FALSE;
  }
  else
    return TRUE;
}
 
/***********************************************************************
*
*  FUNCTION: LoadDialog
*
*  Use the appropriate functions to put up a modal or modeless
*  dialog box depending on the setting of the bModality parameter.
*
***********************************************************************/
 
VOID cdecl LoadDialog( hwndParent, hwndOwner, idDlg, fnwpDlgProc, bModality )
HWND  hwndParent;
HWND  hwndOwner;
SHORT idDlg;
PFNWP fnwpDlgProc;
BOOL  bModality;
{
  EnableModality( hwndOwner, FALSE ); /* Disable the Modality menu item */
 
  if( bModality )
  {
    WinDlgBox( hwndParent,        /* Parent                    */
               hwndOwner,         /* Owner                     */
               fnwpDlgProc,       /* Address of dialog proc    */
               NULL,              /* Module handle             */
               idDlg,             /* Id of dialog in resource  */
               NULL );            /* Initialisation data       */
    EnableModality( hwndOwner, TRUE ); /* Enable the Modality menu item */
  }
  else
  {
    /*******************************************************************
    *
    *   Check to see if a modeless dialog is already running: if
    *   so destroy it before for loading the requested dialog. Save
    *   the handle of the new dialog in a global variable so that in
    *   can be accessed by the WinProc that issued LoadDialog.
    *
    *******************************************************************/
 
    if( WinIsWindow( hab, hwndModelessDlg ) )
      WinDestroyWindow( hwndModelessDlg );
    hwndModelessDlg = WinLoadDlg( hwndParent,
                                  hwndOwner,
                                  fnwpDlgProc,
                                  NULL,
                                  idDlg,
                                  NULL );
  }
}
 
/*************************************************************************
*
*   FUNCTION: MainWndCommand
*
*   Take the appropriate action when a WM_COMMAND message is received by
*   MainWndProc.  Issues calls which load dialogs in the prevailing state
*   of modality.
*
*************************************************************************/
 
VOID cdecl MainWndCommand( hwnd, Command, bModality )
HWND   hwnd;
USHORT Command;
BOOL   *bModality;
{
  USHORT idDlg;
  PFNWP  pfnDlgProc;
 
  switch( Command )
  {
    case MI_MODAL:
    case MI_MODELESS:
      *bModality = ( Command == MI_MODAL ) ? TRUE
                                           : FALSE;
      SetModality( WinQueryWindow( hwnd, QW_PARENT, FALSE ), *bModality );
      WinInvalidateRect( hwnd, NULL, FALSE );
      return;
    case MI_ENTRYFIELDEXAMPLE:
      idDlg      = DLG_ENTRYFIELDEXAMPLE;
      pfnDlgProc = (PFNWP)fnwpEntryFieldDlg;
      break;
    case MI_AUTORADIOBUTTONEXAMPLE:
      idDlg      = DLG_AUTORADIOBUTTONEXAMPLE;
      pfnDlgProc = (PFNWP)fnwpAutoRadioButtonDlg;
      break;
    case MI_CHECKBOXEXAMPLE:
      idDlg      = DLG_CHECKBOXEXAMPLE;
      pfnDlgProc = (PFNWP)fnwpCheckBoxDlg;
      break;
    case MI_LISTBOXEXAMPLE:
      idDlg      = DLG_LISTBOXEXAMPLE;
      pfnDlgProc = (PFNWP)fnwpListBoxDlg;
      break;
    case MI_ABOUTBOX:
      WinDlgBox(HWND_DESKTOP, hwnd, fnwpAboutBoxDlg, NULL, DLG_ABOUT, NULL);
      return;
    default:
      return;
  }
  LoadDialog( HWND_DESKTOP,
              hwndFrame,
              idDlg,
              pfnDlgProc,
              *bModality );
  if( *bModality )
    WinInvalidateRect( hwnd, NULL, FALSE );  /* Request whole window repaint */
}
 
/************************************************************************
*
*   FUNCTION: MainWndPaint
*
*   An unsophisticated window painting routine which simply repaints the
*   entire window when a WM_PAINT message is received. In a real
*   application more sophisticated techniques could be used to determine
*   the minimum region needing repainting, and to paint only that
*   region
*
************************************************************************/
 
VOID cdecl MainWndPaint( hwnd )
HWND hwnd;
{
  POINTL   pointl;
  HPS      hps;                          /* Presentation space handle */
  RECTL    rcl;                          /* Window rectangle          */
  CHAR     string[50];
 
  hps = WinBeginPaint( hwnd, (HPS)NULL, (PRECTL)&rcl );
  /*
	Color in the background
  */
  switch ((int) colorClient) {
	case 0:		/* (r,g,b) = (0,0,0) */
	    WinFillRect( hps, (PRECTL)&rcl, CLR_BLACK );
	    break;
	case 7:		/* (r,g,b) = (1,1,1) */
	    WinFillRect( hps, (PRECTL)&rcl, CLR_WHITE );
	    break;
	default:
	    WinFillRect( hps, (PRECTL)&rcl, colorClient );
	    break;
  }
  /*
	Set the text character colors
  */
  GpiSetColor( hps, (colorClient == 0L) ? CLR_WHITE
                                        : CLR_BLACK );
  pointl.x = 10L; pointl.y = 70L;
  strcpy( string, "Dialog modality    = " );
  strcat( string, (bModality) ? "Modal"
                              : "Modeless" );
  GpiCharStringAt( hps, &pointl, (LONG)strlen( string ), (PSZ)string );
  pointl.y = 50L;
  strcpy( string, "Entry Field 1      = " );
  strcat( string, szEntryField1 );
  GpiCharStringAt( hps, &pointl, (LONG)strlen( string ), (PSZ)string );
  pointl.y = 30L;
  strcpy( string, "Entry Field 2      = " );
  strcat( string, szEntryField2 );
  GpiCharStringAt( hps, &pointl, (LONG)strlen( string ), (PSZ)string );
  pointl.y = 10L;
  strcpy( string, "List Box Selection = " );
  strcat( string, szSelection );
  GpiCharStringAt( hps, &pointl, (LONG)strlen( string ), (PSZ)string );
  WinEndPaint( hps );
 }
 
/**************************************************************************
*
*  FUNCTION: SetModality
*
*  Check or uncheck Modal and Modeless menu items as appropriate.
*
**************************************************************************/
 
VOID cdecl SetModality( hwnd, bModal )
HWND hwnd;
BOOL bModal;
{
  WinPostMsg( WinWindowFromID( hwnd, FID_MENU ),
              MM_SETITEMATTR,
              MPFROM2SHORT( MI_MODAL, TRUE ),
              MPFROM2SHORT( MIA_CHECKED, (bModal) ? ( MIA_CHECKED)
                                                  : (~MIA_CHECKED) ) );
 
  WinPostMsg( WinWindowFromID( hwnd, FID_MENU ),
              MM_SETITEMATTR,
              MPFROM2SHORT( MI_MODELESS, TRUE ),
              MPFROM2SHORT( MIA_CHECKED, (bModal) ? (~MIA_CHECKED)
                                                  : ( MIA_CHECKED) ) );
 
}

MRESULT CALLBACK fnwpAboutBoxDlg(hDlg, msg, mp1, mp2)
/*
    About... dialog procedure
*/
HWND	hDlg;
USHORT	msg;
MPARAM	mp1;
MPARAM	mp2;
{
    switch(msg) {
	case WM_COMMAND:
	    switch(COMMANDMSG(&msg)->cmd) {
		case DID_OK: WinDismissDlg(hDlg, TRUE); break;
		default: break;
	    }
	default: return WinDefDlgProc(hDlg, msg, mp1, mp2);
    }
    return FALSE;
}

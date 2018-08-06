/*
 * FORM LIBRARY - PRINT FORM MODULE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 3.0 SDK
 * STATUS        : operational
 *
 *    Eikon Systems, Inc.
 *    989 East Hillsdale Blvd, Suite 260
 *    Foster City, California 94404
 *    415-349-4664
 *
 * 07/12/90 1.00 - Kevin P. Welch - initial creation.
 *
 */

#define  NOCOMM
 
#include <windows.h>

#include "form.h"
#include "form.d"

/* local macro definitions */
#define  xRCtoDEV(x)          (((x)*TextMetric.tmAveCharWidth)/4)
#define  yRCtoDEV(x)          (((x)*TextMetric.tmHeight)/8)
#define  xDEVtoRC(x)          (((x)*4)/TextMetric.tmAveCharWidth)
#define  yDEVtoRC(x)          (((x)*8)/TextMetric.tmHeight)

/* local function definitions */
BOOL FAR PASCAL   PrintInit( HWND, WORD, LONG );
BOOL FAR PASCAL   PrintCommand( HWND, WORD, LONG );
BOOL FAR PASCAL   PrintOutput( HDC, HANDLE, LPSTR, WORD, WORD, WORD );
BOOL FAR PASCAL   PrintDestroy( HWND, WORD, LONG );

/**/

/*
 * FormPrint( hWndParent, hData ) : HANDLE
 *
 *    hWndParent     handle to parent window
 *    hData          handle to form data block
 *
 * This function displays a dialog box, enabling the user to print
 * the contents of the current form.  If the printing process was
 * successful the handle to the data block provided is returned.  If
 * the printing process was cancelled, a value of NULL is returned.
 *
 */

HANDLE FAR PASCAL FormPrint(
   HWND        hWndParent,
   HANDLE      hData )
{
   MSG         Msg;
   HDC         hPrnDC;
   WORD        wLine;
   WORD        wPage;
   WORD        wLinesPerPage;
   WORD        wPixelsPerLine;
   BOOL        bFirstLine;
   HWND        hWndFocus;
   HWND        hDlgPrint;
   HANDLE      hResult;
   HANDLE      hResInfo;
   HANDLE      hResData;
   HANDLE      hDlgData;
   LPDLGBOX    lpDlgData;
   LPSTR       lpszResData;
   TEXTMETRIC  TextMetric;

   char        szHeader[MAX_HEADER];
   char        szFooter[MAX_FOOTER];
   
   /* initialization */
   hResult = NULL;

   szHeader[0] = NULL;
   szFooter[0] = NULL;

   /* find report information */
   hResInfo = FindResource( FormInfo.hInstance, "REPORT", RT_RCDATA );
   if ( hResInfo ) {

      /* load resource information */
      hResData = LoadResource( FormInfo.hInstance, hResInfo );
      if ( hResData ) {

         /* lock resource data */
         lpszResData = LockResource( hResData );
         if ( lpszResData ) {
         
            /* create printer display context */
            hPrnDC = GetPrinterDC();
            if ( hPrnDC ) {

               /* retrieve page & text metrics */
               GetTextMetrics( hPrnDC, &TextMetric );
               
               wPixelsPerLine = TextMetric.tmHeight + TextMetric.tmExternalLeading;
               wLinesPerPage = GetDeviceCaps(hPrnDC,VERTRES) / wPixelsPerLine;

               /* allocate dialog template */
               hDlgData = GlobalAlloc( GHND, (DWORD)sizeof(DLGBOX) );
               if ( hDlgData ) {

                  /* define the contents */
                  lpDlgData = (LPDLGBOX)GlobalLock( hDlgData );
                  if ( lpDlgData ) {

                     lpDlgData->lStyle = WS_BORDER | WS_CAPTION | WS_DLGFRAME | WS_SYSMENU | WS_POPUP | WS_VISIBLE;
                     lpDlgData->wControls = 0;
                     lpDlgData->wX = 0;
                     lpDlgData->wY = 0;
                     lpDlgData->wCX = (lstrlen(FormInfo.szTitle)*4)+72;
                     lpDlgData->wCY = 46;

                     lpDlgData->szMenu[0] = 0;
                     lpDlgData->szClass[1] = 0;

                     lstrcpy( &lpDlgData->szCaption[0], "Printing" );

                     /* disable parent window */
                     hWndFocus = GetFocus();
                     EnableWindow( hWndParent, FALSE ); 
         
                     /* create dialog box */
                     hDlgPrint = CreateDialogIndirect( FormInfo.hInstance, (LPSTR)lpDlgData, hWndParent, FormPrintDlgFn );
                     if ( hDlgPrint ) {

                        /* unlock dialog template */
                        GlobalUnlock( hDlgData );

                        /* start printing */
                        if ( Escape(hPrnDC,STARTDOC,lstrlen(FormInfo.szTitle),FormInfo.szTitle,NULL) > 0 ) {
   
                           /* initialization for printing */
                           wLine = 1;
                           wPage = 1;

                           bFirstLine = TRUE;

                           /* print each line */
                           while ( *lpszResData && IsWindow(hDlgPrint) ) {
                           
                              /* allow other applications some time */
                              if ( PeekMessage(&Msg,NULL,0,0,PM_REMOVE) )
                                 if ( !IsDialogMessage(hDlgPrint,&Msg) ) {
                                    TranslateMessage( &Msg );
                                    DispatchMessage( &Msg );
                                 }

                              /* check if not header definition */
                              if ( StringMatch(lpszResData,"%HEADER") == FALSE ) {

                                 /* check if not footer definition */
                                 if ( StringMatch(lpszResData,"%FOOTER") == FALSE ) {
                              
                                    /* move to new frame (if necessary) */
                                    if ( ((szFooter[0])&&(wLine+2>=wLinesPerPage))||(wLine>=wLinesPerPage) ) {

                                       /* output footer (if necessary) */
                                       if ( szFooter[0] ) {
                                          PrintOutput(
                                             hPrnDC, 
                                             hData,
                                             szFooter, 
                                             wPage, 
                                             wLinesPerPage-1,
                                             wPixelsPerLine
                                          );
                                          wLine += 2;
                                       }

                                       /* move to a new page frame */
                                       wPage++;
                                       wLine = 1;
                                 
                                       Escape( hPrnDC, NEWFRAME, 0, NULL, NULL );

                                       /* output header (if necessary) */
                                       if ( szHeader[0] ) {
                                          PrintOutput(
                                             hPrnDC, 
                                             hData,
                                             szHeader, 
                                             wPage, 
                                             wLine,
                                             wPixelsPerLine
                                          );
                                          wLine += 2;
                                       }

                                    }
                              
                                    /* output header (if necessary) */
                                    if ( bFirstLine && szHeader[0] ) {
                                       bFirstLine = FALSE;
                                       PrintOutput(
                                          hPrnDC, 
                                          hData,
                                          szHeader, 
                                          wPage, 
                                          wLine,
                                          wPixelsPerLine
                                       );
                                       wLine += 2;
                                    }

                                    /* output current line */
                                    PrintOutput(
                                       hPrnDC, 
                                       hData,
                                       lpszResData, 
                                       wPage, 
                                       wLine++,
                                       wPixelsPerLine
                                    );

                                 } else 
                                    StringCopy( szFooter, lpszResData+7, sizeof(szFooter) );

                              } else
                                 StringCopy( szHeader, lpszResData+7, sizeof(szHeader) );

                              /* advance to next line in report */
                              while ( *lpszResData ) 
                                 lpszResData++;

                              lpszResData += 2;

                           }

                           /* end printing */
                           if ( IsWindow(hDlgPrint) ) {

                              /* define return value */
                              hResult = hData;

                              /* output footer (if necessary) */
                              if ( szFooter[0] ) 
                                 PrintOutput(
                                    hPrnDC, 
                                    hData,
                                    szFooter, 
                                    wPage, 
                                    wLinesPerPage-1,
                                    wPixelsPerLine
                                 );
                              
                              /* end printing */
                              Escape( hPrnDC, NEWFRAME, 0, NULL, NULL );
                              Escape( hPrnDC, ENDDOC, 0, NULL, NULL );

                           } else 
                              Escape( hPrnDC, ENDDOC, 0, NULL, NULL );

                        } else 
                           WARNING( hDlgPrint, "Unable to access printer!" );

                        /* destroy print dialog box */
                        if ( IsWindow(hDlgPrint) )
                           DestroyWindow( hDlgPrint );

                     } else {
                        GlobalUnlock( hDlgData );
                        WARNING( hWndParent, "Unable to create print dialog box!" );
                     }

                     /* enable parent window */
                     EnableWindow( hWndParent, TRUE ); 
                     SetFocus( hWndFocus );

                  }
         
                  /* free dialog template */
                  GlobalFree( hDlgData );

               }

               /* delete printer display context */
               DeleteDC( hPrnDC );
   
            } else
               WARNING( hWndParent, "Unable to access printer!" );
   
         
            /* unlock resource data */
            UnlockResource( hResData );

         } else
            WARNING( NULL, "Unable to lock report definition!" );

         /* free resource data */
         FreeResource( hResData );

      } else
         WARNING( NULL, "Unable to load report definition!" );

   } else
      WARNING( NULL, "Unable to find report definition!" );

   /* return final result */
   return( hResult );

}

/**/

/*
 * FormPrintDlgFn( hDlg, wMessage, wParam, lParam ) : LONG;
 *
 *    hDlg           dialog box window handle
 *    wMessage       dialog box message
 *    wParam         word parameter
 *    lParam         long parameter
 *
 * This function is responsible for processsing all the messages relating
 * to the print form dialog box.
 *
 */

BOOL FAR PASCAL FormPrintDlgFn(
   HWND     hDlg,
   WORD     wMessage,
   WORD     wParam,
   LONG     lParam )
{
   BOOL     bResult;

   /* initialization */
   bResult = TRUE;

   /* process message */
   switch ( wMessage )
      {
   case WM_INITDIALOG : /* initialize dialog box */
      bResult = PrintInit( hDlg, wParam, lParam );
      break;
   case WM_COMMAND:
      bResult = PrintCommand( hDlg, wParam, lParam );
      break;
   case WM_DESTROY :
      bResult = PrintDestroy( hDlg, wParam, lParam );
      break;
   default : /* some other message */
      bResult = FALSE;
      break;
   }

   /* return final result */
   return bResult;

}

/**/

/*
 * PrintInit( hDlg, wParam, lParam ) : BOOL;
 *
 *    hDlg           handle to dialog box
 *    wParam         word parameter
 *    lParam         long parameter
 *
 * The function defines each of the control pages and initializes the
 * print dialog box.  A value of TRUE is returned if the process
 * was successful.
 *
 */

BOOL FAR PASCAL PrintInit(
   HWND        hDlg,
   WORD        wParam,
   LONG        lParam )
{
   HDC         hDC;
   WORD        wWidth;
   WORD        wHeight;
   RECT        rcClient;
   TEXTMETRIC  TextMetric;

   char        szString[MAX_TITLE];

   /* warning level 3 compatibility */
   wParam;
   lParam;
   
   /* define property lists */
   SET_DATA( hDlg, FormInfo.hTempData );

   /* calculate text metrics */
   hDC = GetDC( hDlg );
   GetTextMetrics( hDC, &TextMetric );
   ReleaseDC( hDlg, hDC );

   /* calculate dialog box dimensions in RC coordinates */
   GetClientRect( hDlg, &rcClient );
   
   wWidth = xDEVtoRC( rcClient.right - rcClient.left );
   wHeight = yDEVtoRC( rcClient.bottom - rcClient.top );

   /* create static controls */
   StringCopy( szString, "The ", sizeof(szString) );
   StringCat( szString, FormInfo.szTitle, sizeof(szString) );
   StringCat( szString, " is", sizeof(szString) );
   
   CreateWindow(
      "static",
      szString,
      SS_CENTER | WS_CHILD | WS_VISIBLE,
      xRCtoDEV(4),
      yRCtoDEV(6),
      xRCtoDEV(wWidth-8),
      yRCtoDEV(8),
      hDlg,
      (WORD)-1,
      INSTANCE(PARENT(hDlg)),
      NULL
   );
     
   CreateWindow(
      "static",
      "being sent to the printer.",
      SS_CENTER | WS_CHILD | WS_VISIBLE,
      xRCtoDEV(4),
      yRCtoDEV(14),
      xRCtoDEV(wWidth-8),
      yRCtoDEV(8),
      hDlg,
      (WORD)-1,
      INSTANCE(PARENT(hDlg)),
      NULL
   );

   CreateWindow(
      "button",
      "Cancel",
      BS_PUSHBUTTON | WS_TABSTOP | WS_CHILD | WS_VISIBLE,
      xRCtoDEV((wWidth/2)-16),
      yRCtoDEV(28),
      xRCtoDEV(32),
      yRCtoDEV(14),
      hDlg,
      IDCANCEL,
      INSTANCE(PARENT(hDlg)),
      NULL
   );

   /* center within parent window */
   CenterPopup( hDlg, GetParent(hDlg) );

   /* return result */
   return( TRUE );

}

/**/

/*
 * PrintCommand( hDlg, wParam, lParam ) : BOOL;
 *
 *    hDlg           handle to dialog box
 *    wParam         word parameter
 *    lParam         long parameter
 *
 * The function is responsible for handling the WM_COMMAND messages
 * received by the print dialog box window function.  A value of TRUE
 * is returned if the process was successful.
 *
 */

BOOL FAR PASCAL PrintCommand(
   HWND        hDlg,
   WORD        wParam,
   LONG        lParam )
{

   /* warning level 3 compatibility */
   wParam;
   lParam;
   
   /* process commands */
   switch( wParam )
      {
   case IDCANCEL : /* cancel form */
      DestroyWindow( hDlg );
      break;
   default : /* something else - ignore */
      break;
   }

   /* return result */
   return( TRUE );

}

/**/

/*
 * PrintOutput( hPrnDC, hData, lpszPattern, wPage, wLine, wPixels ) : BOOL;
 *
 *    hPrnDC         printer display context
 *    hData          form data block
 *    lpszPattern    output specification string
 *    wPage          current page number
 *    wLine          current line number
 *    wPixels        number of pixels per output page
 *
 * This function outputs a report line based on the output specification
 * string and data block provided.  Any variable names present in the
 * output specification string are automatically expanded, including
 * those that reference the following predefined variables:
 *
 *    LINE     current line number
 *    PAGE     current page number
 *
 * A value of TRUE is returned if the current string was successfully
 * output to the printer.
 *
 */

BOOL FAR PASCAL PrintOutput(
   HDC         hPrnDC,
   HANDLE      hData,
   LPSTR       lpszPattern,
   WORD        wPage,
   WORD        wLine,
   WORD        wPixels )
{
   BOOL        bLeft;

   WORD        wWidth;
   WORD        wLength;
   WORD        wPrecision;

   LPSTR       lpszName;
   LPSTR       lpszData;
   LPSTR       lpszOutput;

   char        szName[MAX_NAME];
   char        szData[MAX_DATA];
   char        szOutput[MAX_DATA];

   /* define output string */
   lpszOutput = szOutput;
   
   while ( *lpszPattern ) {

      /* check for identifiers */
      if ( (*lpszPattern=='%')&&(*(lpszPattern+1)!='%')&&(*(lpszPattern+1)!=' ') ) {
            
         /* initialization */
         wWidth = 0;
         wPrecision = 0;

         bLeft = FALSE;
      
         /* extract identifier */
         lpszPattern++;
         lpszName = szName;
         
         /* extract allignment flag (optional) */
         if ( *lpszPattern == '-' ) {
            lpszPattern++;
            bLeft = TRUE;
         } 

         /* extract field formating parameters (optional) */
         if ( (*lpszPattern>='0')&&(*lpszPattern<='9') ) {

            /* extract field width */
            while ( (*lpszPattern>='0')&&(*lpszPattern<='9') ) {
               wWidth = (wWidth*10) + *lpszPattern - '0';
               lpszPattern++;
            }

            /* extract field width (optional) */
            if ( *lpszPattern == '.' ) {

               /* skip period */
               lpszPattern++;

               /* extract field width */
               while ( (*lpszPattern>='0')&&(*lpszPattern<='9') ) {
                  wPrecision = (wPrecision*10) + *lpszPattern - '0';
                  lpszPattern++;
               }

            }
         
         }

         /* extract field name */
         while ( (*lpszPattern)&&(*lpszPattern!=' ') )
            *lpszName++ = *lpszPattern++;

         /* append null character */
         *lpszName = 0;

         /* define field value */
         if ( StringMatch(szName,"LINENUMBER") == FALSE ) {
            if ( StringMatch(szName,"PAGENUMBER") == FALSE ) {
               if ( GetCtlData(hData,szName,szData,sizeof(szData)) == FALSE ) 
                  StringCopy( szData, "?", sizeof(szData) );
            } else
               wsprintf( szData, "%u", wPage );
         } else
            wsprintf( szData, "%u", wLine );
   
         /* copy field value to output string (if defined) */
         if ( *szData ) {

            lpszData = szData;

            /* check if field width specified */
            if ( wWidth ) {

               /* output data value */
               if ( bLeft ) {

                  /* skip leading blanks */
                  while ( (*lpszData)&&(*lpszData==' ') )
                     lpszData++;

                  /* check if precision specified */
                  if ( wPrecision ) {

                     /* copy value */
                     while ( *lpszData && wPrecision && wWidth ) {
                        wWidth--;
                        wPrecision--;
                        *lpszOutput++ = *lpszData++;
                     }

                     /* append blank characters */
                     while ( wWidth ) {
                        *lpszOutput++ = ' ';
                        wWidth--;
                     }

                     /* append null */
                     *lpszOutput = NULL;

                  } else {

                     /* copy value */
                     while ( *lpszData && wWidth ) {
                        wWidth--;
                        *lpszOutput++ = *lpszData++;
                     }

                     /* append null */
                     *lpszOutput = NULL;

                  }

               } else {

                  wLength = lstrlen( lpszData );
                  
                  /* strip trailing blanks */
                  while ( (wLength)&&(szData[wLength-1]==' ') ) 
                     szData[--wLength] = NULL;
                  
                  /* prefix blank characters */
                  while ( wLength < wWidth ) {
                     *lpszOutput++ = ' ';
                     wLength++;
                  }

                  /* check if precision specified */
                  if ( wPrecision ) {

                     /* copy value (checking precision) */
                     while ( *lpszData && wPrecision && wWidth ) {
                        wWidth--;
                        wPrecision--;
                        *lpszOutput++ = *lpszData++;
                     }

                     /* append null */
                     *lpszOutput = NULL;

                  } else {

                     /* copy value */
                     while ( *lpszData && wWidth ) {
                        wWidth--;
                        *lpszOutput++ = *lpszData++;
                     }

                     /* append null */
                     *lpszOutput = NULL;

                  }

               }
            
            } else {

               /* copy value without precision or allignment */
               while( *lpszData )
                  *lpszOutput++ = *lpszData++;
               *lpszOutput = 0;

            }

         }

      } else
         *lpszOutput++ = *lpszPattern++;

   }

   /* append final null */
   *lpszOutput = NULL;
   
   /* output text line */
   TextOut( hPrnDC, 0, wLine*wPixels, szOutput, lstrlen(szOutput) );

   /* return result */
   return( TRUE );

}
                                                               
/**/

/*
 * PrintDestroy( hDlg, wParam, lParam ) : BOOL;
 *
 *    hDlg           handle to dialog box
 *    wParam         word parameter
 *    lParam         long parameter
 *
 * The function is responsible for handling the case when the
 * WM_DESTROY message is received by the print dialog box window
 * function.  A value of TRUE is returned if the cleanup process
 * was successful.
 *
 */

BOOL FAR PASCAL PrintDestroy(
   HWND        hDlg,
   WORD        wParam,
   LONG        lParam )
{

   /* warning level 3 compatibility */
   hDlg;
   wParam;
   lParam;
   
   /* return result */
   return( TRUE );

}

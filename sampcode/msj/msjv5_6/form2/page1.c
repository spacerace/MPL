/*
 * Page Selection - INITIALIZATION MODULE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft Windows 3.0 SDK
 * STATUS        : operational
 *
 * This module defines the function that registers the Page
 * control window class.  When used within a dynamic-link library,
 * this function is called when the library is first loaded.
 *
 *    Eikon Systems, Inc.
 *    989 East Hillsdale Blvd, Suite 260
 *    Foster City, California 94404
 *    415-349-4664
 *
 * 11/30/89 1.00 - Kevin P. Welch - initial creation.
 *
 */

#define  NOCOMM
 
#include <windows.h>
#include <control.h>

#include "page.h"
#include "page.d"

/* global variables */
HANDLE      hPageInst;

/**/

/*
 * PageInit( hInst ) : BOOL;
 *
 *    hInst       handle to dll instance
 *
 * This function registers the control class and saves the dll
 * instance handle, if necessary.  Note that this function always
 * returns TRUE, regardless of teh success or failure of the
 * RegisterClass function.
 *
 */

BOOL FAR PASCAL PageInit(
   HANDLE      hInst )
{
   WNDCLASS    WndClass;

   /* register slider class if not already registered */
   if ( hPageInst == NULL ) {

      /* define slider class structure */
      WndClass.lpszClassName =   PAGE_NAME;
      WndClass.hCursor =         LoadCursor( NULL, IDC_ARROW );
      WndClass.lpszMenuName =    NULL;
      WndClass.style =           PAGE_CLASSSTYLE;
      WndClass.lpfnWndProc =     PageWndFn;
      WndClass.hInstance =       hInst;
      WndClass.hIcon =           NULL;
      WndClass.cbWndExtra =      PAGE_WNDEXTRA;
      WndClass.hbrBackground =   (HBRUSH)(PAGE_COLOR);
      WndClass.cbClsExtra =      PAGE_CLASSEXTRA;

      /* register slider window class */
      RegisterClass( &WndClass );

      /* save results */
      hPageInst = hInst;

   } 

   /* return final result */
   return( TRUE );

}

/**/

/*
 * WEP( wValue ) : WORD;
 *
 *		wValue		system exit value
 *
 * This function is called when the DLL is unloaded by the system.
 * It enables the library to perform any cleanup necessary.
 *
 */

WORD FAR PASCAL WEP(
	WORD		wValue )
{
	WORD		wResult;

	/* process exit value */
	switch( wValue )
		{
	case WEP_FREE_DLL : /* DLL is being released */
	case WEP_SYSTEM_EXIT : /* system is being shut down */
	default :
		wResult = 1;
		break;
	}

	/* return result */
	return( wResult );

}

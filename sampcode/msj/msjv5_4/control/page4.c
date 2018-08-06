/*
 * Page Selection - INFO MODULE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft Windows 3.0 SDK
 * STATUS        : operational
 *
 * This module defines the function responsible for returning a data
 * structure that describes the control library.  This data structure
 * can be used by other applications when working with the control.
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

/**/

/*
 * PageInfo( VOID ) : HANDLE;
 *
 * This function returns a handle to an information block
 * which describes the page selection control.  Included in the
 * information returned is the suggested size of the
 * control, default style flags, and other useful data.
 *
 */

HANDLE FAR PASCAL PageInfo( VOID )
{
   HANDLE      hCtlInfo;
   LPCTLINFO   lpCtlInfo;

   /* allocate memory for information structure */
   hCtlInfo = GlobalAlloc( GMEM_MOVEABLE, (DWORD)sizeof(CTLINFO) );
   if ( hCtlInfo ) {

      /* lock it down */
      lpCtlInfo = (LPCTLINFO)GlobalLock( hCtlInfo );
      if ( lpCtlInfo ) {

         /* define Information */
         lpCtlInfo->wCtlTypes = 1;
         lpCtlInfo->wVersion = PAGE_VERSION;
         lstrcpy( lpCtlInfo->szClass, PAGE_NAME );
         lstrcpy( lpCtlInfo->szTitle,
            "Page Selection Control\nby Kevin P. Welch" );

         lpCtlInfo->Type[0].wType = CT_STD;
         lpCtlInfo->Type[0].wWidth = 48;
         lpCtlInfo->Type[0].wHeight = 70;
         lpCtlInfo->Type[0].dwStyle = PAGE_WNDSTYLE;
         lstrcpy( lpCtlInfo->Type[0].szDescr, "Standard" );

         /* unlock data */
         GlobalUnlock( hCtlInfo );

      } else {
         GlobalFree( hCtlInfo );
         hCtlInfo = NULL;
      }

   }

   /* return final result */
   return( hCtlInfo );
}

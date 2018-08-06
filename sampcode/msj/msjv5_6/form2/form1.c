/*
 * FORM LIBRARY - INITIALIZATION MODULE
 *
 * LANGUAGE      : Microsoft C 5.1
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

/* global variables */
FORM        FormInfo;

/**/

/*
 * FormInit( hInstance ) : HANDLE
 *
 *    hInstance      handle to dll instance
 *
 * This function initializes the data entry form library.  This includes
 * saving the current library instance handle.  If the initialization
 * process was successful the dll instance handle is returned.
 *
 */

HANDLE FAR PASCAL FormInit(
   HANDLE      hInstance )
{
   HANDLE      hResInfo;
   HANDLE      hResData;
   LPSTR       lpResData;

   /* proceed if first time */
   if ( hInstance && !FormInfo.hInstance ) {

      /* initialization */
      FormInfo.wPages = 0;
      FormInfo.hTempData = NULL;
      FormInfo.fnOldMsgHook = NULL;
      FormInfo.hInstance = hInstance;

      StringCopy(
         FormInfo.szTitle, 
         "Form Library", 
         sizeof(FormInfo.szTitle)
      );

      /* find form information */
      hResInfo = FindResource( hInstance, "FORM", RT_RCDATA );
      if ( hResInfo ) {

         /* load resource information */
         hResData = LoadResource( hInstance, hResInfo );
         if ( hResData ) {

            /* lock resource data */
            lpResData = LockResource( hResData );
            if ( lpResData ) {

               /* extract number of pages & form name */
               FormInfo.wPages = *((WORD FAR *)lpResData);
               StringCopy(
                  FormInfo.szTitle, 
                  lpResData+2, 
                  sizeof(FormInfo.szTitle)
               );

               /* unlock resource data */
               UnlockResource( hResData );

            } else
               WARNING( NULL, "Unable to access form!" );

            /* free resource data */
            FreeResource( hResData );

         } else
            WARNING( NULL, "Unable to load form!" );

      } else
         WARNING( NULL, "Unable to find form!" );

   }

   /* return successfully */
   return( hInstance );

}

/**/

/*
 * WEP( wValue ) : WORD;
 *
 *    wValue       system exit code
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

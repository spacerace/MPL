/*
 * FORM LIBRARY - INITIALIZATION MODULE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 3.0 SDK
 * STATUS        : operational
 *
 * 07/12/90 1.00 - Kevin P. Welch - initial creation.
 *
 */

#define	NOCOMM

#include <windows.h>
#include "form.h"

/**/

/*
 * FormInit( hInstance ) : HANDLE
 *
 *    hInstance      handle to dll instance
 *
 * This function initializes the data entry form library.  This
 * includes saving the current library instance handle.  If the
 * initialization process was successful the dll instance handle
 * is returned.
 *
 */

HANDLE FAR PASCAL FormInit(
	HANDLE		hInstance )
{

	/* FORM INITIALIZATION CODE HERE */

	/* return successfully */
	return( hInstance );

}

/**/

/*
 * WEP( bExit ) : VOID;
 *
 *    bExit       flag indicating system exit
 *
 * This function is called when the DLL is unloaded by the system.
 * It enables the library to perform any cleanup necessary.
 *
 */

VOID FAR PASCAL WEP(
   BOOL     bExit )
{

	/* warning level 3 compatibility */
   bExit;

}

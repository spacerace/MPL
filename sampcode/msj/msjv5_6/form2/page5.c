/*
 * Page Selection Control - STYLE FLAGS MODULE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft Windows 3.0 SDK
 * STATUS        : operational
 *
 * This module defines the function responsible for converting
 * binary style flags into textual equivalents.  This function can
 * be used whenever a binary template that includes the page
 * selection control is being translated into text.
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
 * PageFlags( wStyle, lpszStyle, wMaxStr ) : WORD;
 *
 *    wStyle         control style (low word)
 *    lpszStyle      style flags string
 *    wMaxStr        maximum string length
 *
 * This function converts the control style flags into a text
 * string that corresponds to the constants defined in the
 * control header file.
 *
 * Note that in this example it is assumed that sufficient
 * space is available for the resulting string.
 *
 */

WORD FAR PASCAL PageFlags(
   WORD        wStyle,
   LPSTR       lpszStyle,
   WORD        wMaxStr )
{

   /* warning level 3 compatiblity */
   wStyle;
   wMaxStr;

   /* no style flags defined */
   lstrcpy( lpszStyle, "" );

   /* return string length */
   return( lstrlen(lpszStyle) );

}

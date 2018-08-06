/*
 * FORM LIBRARY - PRINT FORM MODULE
 *
 * 07/12/90 1.00 - Kevin P. Welch - initial creation.
 *
 */

#define NOCOMM

#include <windows.h>
#include "form.h"

/**/

/*
 * FormPrint( hWndParent, hData ) : HANDLE
 *
 *      hWndParent     handle to parent window
 *      hData          handle to form data block
 *
 * This function displays a dialog box, enabling the user to print
 * the contents of the current form.  If the printing process was
 * successful the handle to the data block provided is returned.  If
 * the printing process was cancelled, a value of NULL is returned.
 *
 */

HANDLE FAR PASCAL FormPrint(
  HWND    hWndParent,
  HANDLE  hData )
{

  /* warning level 3 compatibility */
  hWndParent;

  /* FORM PRINTING CODE HERE */
  MessageBox(
     hWndParent,
     "Form being printed!",
     "Form Library Stub",
     MB_OK|MB_ICONASTERISK
  );

  /* return handle to data block */
  return( hData );

}

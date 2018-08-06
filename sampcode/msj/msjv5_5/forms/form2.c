/*
 * FORM LIBRARY - EDIT FORM MODULE
 *
 * 07/12/90 1.00 - Kevin P. Welch - initial creation.
 *
 */

#define NOCOMM

#include <windows.h>
#include "form.h"

/**/

/*
 * FormEdit( hWndParent, hData ) : HANDLE
 *
 *      hWndParent     handle to parent window
 *      hData          handle to form data block
 *
 * This function displays a dialog box, enabling the user to enter
 * data using a series of linked dialog boxes.  The value returned by
 * this function is a global handle with the new data block.  If a
 * NULL handle is returned then it can be assumed that the data
 * entry was cancelled.
 *
 */

HANDLE FAR PASCAL FormEdit(
  HWND    hWndParent,
  HANDLE  hData )
{

  /* warning level 3 compatibility */
  hWndParent;

  /* FORM EDITING CODE HERE */
  MessageBox(
     hWndParent,
     "Form being edited!",
     "Form Library Stub",
     MB_OK|MB_ICONASTERISK
  );

  /* return handle to data */
  return( hData );

}

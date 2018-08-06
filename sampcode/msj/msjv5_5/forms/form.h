/*
 * FORM LIBRARY - HEADER FILE
 *
 * 07/12/90 1.00 - Kevin P. Welch - initial creation.
 *
 */

/* definitions */
#define ID(x)        GetWindowWord(x,GWW_ID)
#define PARENT(x)    GetWindowWord(x,GWW_HWNDPARENT)
#define INSTANCE(x)  GetWindowWord(x,GWW_HINSTANCE)

/* functions */
VOID FAR PASCAL    WEP( BOOL );
HANDLE FAR PASCAL  FormInit( HANDLE );
HANDLE FAR PASCAL  FormEdit( HWND, HANDLE );
HANDLE FAR PASCAL  FormPrint( HWND, HANDLE );

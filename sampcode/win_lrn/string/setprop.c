/*
 *  SetProp
 *  setprop.c
 *
 *  This program demonstrates the use of the function SetProp.
 *  It copies the character string and a data handle to the property
 *  list of the window. The string serves as a label for the data handle.
 *  The data handle can be retrieved from the property list at any time
 *  by using the GetProp function and specifying the string.
 *
 */

#include "windows.h"
#include "stdio.h"

/* This is the structure that is used to put into the property list.
 * It is just a sample structure.
 */
struct Data 
  {  short nRow, nSeat;
     char  szName[20];
  } strSeatings = { 2, 4, "John Smith" };



/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    WNDCLASS   wcClass;

    /* registering the parent window class */
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hIcon          = LoadIcon (hInstance, (LPSTR)"WindowIcon");
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"Setprop";
    wcClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wcClass.hInstance      = hInstance;
    wcClass.style          = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = DefWindowProc;
    wcClass.cbClsExtra     = 0;
    wcClass.cbWndExtra     = 0;

    RegisterClass( (LPWNDCLASS)&wcClass );
    return TRUE;        /* Initialization succeeded */
}


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HWND         hWnd;          /* Handle to the parent window    */
    BOOL         bSet;          /* Boolean flag                   */
    struct Data *hData;         /* Handle to the data in the      
                                 * property list
                                 */
    char         szbuffer[50];  /* String buffer for output       */

    WinInit (hInstance);

    hWnd = CreateWindow((LPSTR)"Setprop",
                        (LPSTR)"Setting the Property List",
                        WS_OVERLAPPEDWINDOW,
                        50,                /* x         */
                        50,                /* y         */
                        600,               /* width     */
                        250,               /* height    */
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                       );

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    
    /* Putting the structure into the property list of the window 
     * If it is successful, retrieve it to check if the correct structure
     * is put into the property list
     */
    bSet = SetProp (hWnd, (LPSTR)"Smith", (HANDLE) &strSeatings);
    if (bSet)
      {
        MessageBox (hWnd, (LPSTR)"Property Set", (LPSTR)"OK", MB_OK);
        hData = GetProp (hWnd, (LPSTR)"Smith");
        sprintf (szbuffer, "%s at row %d, seat# %d", 
                 hData->szName, hData->nRow, hData->nSeat);
        MessageBox (hWnd, (LPSTR)szbuffer, (LPSTR)"Retrieved", MB_OK);
      }
    else 
      MessageBox (hWnd, (LPSTR)"Property Not Set", (LPSTR)"FAIL", MB_OK);
  
    return 0;
}

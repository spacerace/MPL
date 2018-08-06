/* Listing 13-8 */


    HWND hWnd;                      /* window handle */
    HDC  hDC;                       /* device context handle */

    struct
    {
      HDC hdc;                      /* device context handle */
      BOOL fErase;
      RECT rcPaint;
      BOOL fRestore;
      BOOL fIncUpdate;
      BYTE rgbReserved[16];
    }                         ps;   /* "paint structure" */


    /* create a window */

    hWnd = CreateWindow( ... );
    .
    .
    .

    /* initialize device context for window */

    BeginPaint( hWnd, &ps );
    hDC = ps.hdc;
    .
    .
    .

    /* associate attributes with device context */

    hpen = CreatePen( PS_SOLID, 0, GetSysColor(COLOR_WINDOWTEXT) );
    SelectObject( hDC, (HANDLE)hpen );

    hbr = CreateSolidBrush( GetNearestColor(hDC,RectFillColor) );
    SelectObject( hDC, (HANDLE)hbr );
    .
    .
    .

    /* draw a filled rectangle */

    Rectangle( hDC, 0, 0, 100, 100 );

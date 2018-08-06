/*
**  lmem.h    header file for far memory functions
*/

VOID FAR PASCAL lmemmove( VOID FAR * lpDest,
                          VOID FAR * lpSrc,
                          WORD wCount );
VOID FAR PASCAL  lmemset( VOID FAR * lpDest,
                          int Char,
                          WORD wCount );


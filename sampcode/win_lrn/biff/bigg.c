/*
   This module was designed to work as a standalone
   program.  What it does is promts you for BIFF
   record information and then builds a biff buffer
   that is copied to the Clipboard and also creates
   a BIFF.XLS file.
*/

#include "windows.h"
#include "winexp.h"  /* needed for file i/o functions */
#include "stdlib.h"  /* needed for atof function */
#include "bigg.h"

#define BUFLENGTH  100

extern  HWND  hWnd;

int     iRow;               /*         INT value from edit control */
int     iColumn;            /*         INT value from edit control */
WORD    wInt;               /* BOOL or INT value from edit control */
int     iTextLength;        /*    length of text from edit control */
char    szText[40];         /*              text from edit control */
char    cBuffer[BUFLENGTH];
int     iLastButton;
int     iRadioButton;
int     PlaceBiffOnClipboard( HANDLE );
int     iLength;
LPSTR   lpData, lpDataInit;
FARPROC lpprocBiffDlgProc;

struct BIFFINTEGER FAR *lpBiffInt;
struct BIFFNUMBER  FAR *lpBiffNumber;
struct BIFFSTRING  FAR *lpBiffString;
struct BIFFFORMULA FAR *lpBiffFormula;

/* ------ formal declarations -------- */

BOOL FAR PASCAL BiffDlgProc( HWND, WORD, WORD, LONG );
HANDLE FillBiffBuffer( void );
void   CreateBiffBOF(void);
void   CreateBiffDIM( int, int, int, int );
void   SaveBiffData(  int, int, int);
void   CloseBiff(void);
int    DumpStuff(void);
void   lstrncpy(LPSTR, LPSTR, int);

/* -----------------------------------------
     Entry point of this module.
 ------------------------------------------- */
void FAR GetBiff(HWND hWnd, HANDLE hInst)
{
   lpprocBiffDlgProc = MakeProcInstance( (FARPROC)BiffDlgProc, hInst );
   DialogBox( hInst, MAKEINTRESOURCE(BIFFDLG), hWnd, lpprocBiffDlgProc );
   FreeProcInstance(lpprocBiffDlgProc);
}

/* -----------------------------------------
     Process Biff dialog box.
 ------------------------------------------- */
BOOL FAR PASCAL BiffDlgProc( HWND hDlg, WORD wMessage, WORD wParam, LONG lParam)
{
   BOOL bResult;  /* Translated flag */ 

   switch (wMessage) {

   case WM_INITDIALOG:                  

       SetDlgItemText(hDlg, IDTEXT, (LPSTR)szText);

       CheckRadioButton(hDlg, IDBOOL, IDFORMULA, iLastButton);
           iRadioButton = iLastButton;

       /* formula is not implemented so it is disabled */
       EnableWindow( GetDlgItem(hDlg,IDFORMULA), FALSE );

       break;

   case WM_COMMAND:

       switch (wParam) {

       case IDOK:

           iRow = GetDlgItemInt(hDlg, IDROW , (BOOL FAR *)&bResult, FALSE);
           if (bResult == FALSE) /* display an error. */
               MessageBox( hDlg,
                           (LPSTR)"Non-numeric character or exceeded max!",
                           (LPSTR)"Row Error!", MB_OK );
           if(iRow) iRow--;  // make ZERO based

           iColumn = GetDlgItemInt(hDlg, IDCOLUMN, (BOOL FAR *)&bResult, FALSE);
           if (bResult == FALSE) /* display an error. */
               MessageBox( hDlg,
                           (LPSTR)"Non-numeric character or exceeded max!",
                           (LPSTR)"Column Error!", MB_OK );

           if(iColumn)  iColumn--; // make ZERO based

           iLastButton = iRadioButton;
           switch (iRadioButton) {
              case IDBOOL:
                   wInt = GetDlgItemInt(hDlg, IDTEXT, (BOOL FAR *)&bResult, FALSE);
                   if (bResult == FALSE) /* display an error. */
                       MessageBox( hDlg,
                              (LPSTR)"Something is wrong with BOOL value!",
                              (LPSTR)"BOOL Error!", MB_OK );
                   break;

              case IDINT:
                   wInt = GetDlgItemInt(hDlg, IDTEXT, (BOOL FAR *)&bResult, FALSE);
                   if (bResult == FALSE) /* display an error. */
                   MessageBox( hDlg,
                              (LPSTR)"Must be 0 - 65535!",
                              (LPSTR)"Integer Error!", MB_OK );
                   break;

              case IDBLANK:
              case IDLABEL:
              case IDNUMBER:
              case IDFORMULA:

                   break;

              default:
                     MessageBox( hDlg,"Wrong data type", NULL, MB_OK);
           }
           iTextLength = GetDlgItemText(hDlg, IDTEXT, (LPSTR)szText, 40);
           szText[iTextLength+1] = '\0';

           PlaceBiffOnClipboard( FillBiffBuffer() );

           EndDialog(hDlg, TRUE);
           return(TRUE);

       case IDCANCEL:
               EndDialog(hDlg, FALSE);
               return(TRUE);

       case IDBLANK:
       case IDLABEL:
       case IDBOOL:
       case IDINT:
       case IDNUMBER:
       case IDFORMULA:
               CheckRadioButton(hDlg, IDBLANK, IDFORMULA,
                  (iRadioButton = wParam));
               return(TRUE);
               break;
       }
   default:
           return(FALSE);
   }
   return(TRUE);
}


/* ----------------------------------------------------------
   Place the biff buffer to the clipboard in BIFF format
 ------------------------------------------------------------ */
int  PlaceBiffOnClipboard(HANDLE hBiffBuffer)
{
  WORD     wCBFormat;
  int      iResult;
  unsigned long  ulLength;

  if (hBiffBuffer == 0)
    return FALSE;

  ulLength = (unsigned long)iLength;
  hBiffBuffer = GlobalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, ulLength );
  if (!hBiffBuffer) {
     MessageBox(GetFocus(), "Not enough memory.", NULL, MB_OK);
     return FALSE;
  }
  if ( lpData = GlobalLock(hBiffBuffer) ) {
    lstrncpy( lpData ,lpDataInit, iLength );
  }
  GlobalUnlock(hBiffBuffer);

  if ( OpenClipboard(hWnd) == 0) {
    MessageBox( GetFocus(), (LPSTR)"Opening clipboard", NULL, MB_OK);
    return FALSE;
  }
  EmptyClipboard();
  wCBFormat = RegisterClipboardFormat ( (LPSTR)"Biff" );
  iResult   = SetClipboardData(wCBFormat,hBiffBuffer);
  CloseClipboard();
  return TRUE;

}

/* ----------------------
   Fill the Biff buffer
 ------------------------ */
HANDLE FillBiffBuffer()
{

  lpDataInit = lpData = cBuffer;  /* set starting address to fill */
  CreateBiffBOF();
  CreateBiffDIM(iRow, iRow, iColumn, iColumn);
  SaveBiffData(iRadioButton, iRow, iColumn);
  CloseBiff();

  DumpStuff();
  return TRUE;

}


/* --------------------------------------------------
   Creates a BOF record,
 ---------------------------------------------------- */
void CreateBiffBOF()
{

   *lpData++ = 0x9 ; *lpData++ = 0x0 ; // rec type
   *lpData++ = 0x4 ; *lpData++ = 0x0 ; // rec length

   *lpData++ = 0x2 ; *lpData++ = 0x0 ; // version #
   *lpData++ = 0x10; *lpData++ = 0x0 ; // 10 = XLS, 20 = XLC, 40 = XLM

}

/* -----------------------------------------------------------
   Creates a Dimension record,
   Currently, this dialog does not:
      1] ask for more than ONE cell.
      2] limits range to two characters.
 ------------------------------------------------------------- */
void CreateBiffDIM(int iFirstRow, int iLastRow, int iFirstCol, int iLastCol)
{
        
   *lpData++ = 0x0;  *lpData++ = 0x0; // rec type
   *lpData++ = 0x8;  *lpData++ = 0x0; // rec length

   *lpData++ = LOBYTE(iFirstRow);  *lpData++ = 0x0; // HIBYTE(iFirstRow); 
   *lpData++ = LOBYTE(iLastRow+1); *lpData++ = 0x0; // HIBYTE(iLastRow+1);
   *lpData++ = LOBYTE(iFirstCol);  *lpData++ = 0x0; // HIBYTE(iFirstCol); 
   *lpData++ = LOBYTE(iLastCol+1); *lpData++ = 0x0; // HIBYTE(iLastCol+1);

}
        
/* --------------------------------------------------
   Creates a Data record,
 ---------------------------------------------------- */
void SaveBiffData(int iType, int iRow, int iCol)
{

  switch (iType) {

   case IDBLANK:   /* make a blank record */

         *lpData++ = 0x1;  *lpData++ = 0x0;// rec type
         *lpData++ = 0x7;  *lpData++ = 0x0;// rec length

         *lpData++ = LOBYTE(iRow); *lpData++ = 0x0; // HIBYTE(iRow); // row
         *lpData++ = LOBYTE(iCol); *lpData++ = 0x0; // HIBYTE(iCol); // col

         *lpData++ = 0x0;  // rec attribute
         *lpData++ = 0x0;
         *lpData++ = 0x0;

         break;


   case IDBOOL:    /* make a boolean record */

         *lpData++ = 0x5;  *lpData++ = 0x0;// rec type
         *lpData++ = 0x9;  *lpData++ = 0x0;// rec length

         *lpData++ = LOBYTE(iRow); *lpData++ = 0x0; // HIBYTE(iRow); // row
         *lpData++ = LOBYTE(iCol); *lpData++ = 0x0; // HIBYTE(iCol); // col

         *lpData++ = 0x0;  // rec attribute
         *lpData++ = 0x0;
         *lpData++ = 0x0;
         *lpData++ = (BYTE)(wInt?0x1:0x0); // data
         *lpData++ = 0x0;

         break;

   case IDINT:     /* make an integer record */
         lpBiffInt = lpData;

         lpBiffInt->wRecType   = 2;
         lpBiffInt->wRecLength = 9;
         lpBiffInt->wRow       = iRow;
         lpBiffInt->wColumn    = iCol;
         lpBiffInt->cAttrib[0] = 0;
         lpBiffInt->cAttrib[1] = 0;
         lpBiffInt->cAttrib[2] = 0;
         lpBiffInt->wInteger   = wInt;

         lpData = lpData + lpBiffInt->wRecLength + 4;

         break;

   case IDNUMBER:  /* make a number record */

         lpBiffNumber = lpData;

         lpBiffNumber->wRecType   =  3;
         lpBiffNumber->wRecLength = 15;
         lpBiffNumber->wRow       = iRow;
         lpBiffNumber->wColumn    = iCol;
         lpBiffNumber->cAttrib[0] = 0;
         lpBiffNumber->cAttrib[1] = 0;
         lpBiffNumber->cAttrib[2] = 0;
         lpBiffNumber->dNumber    = atof((PSTR)szText);

         lpData = lpData + lpBiffNumber->wRecLength + 4;

         break;

   case IDLABEL:   /* make a label (text) record */

         lpBiffString = lpData;

         lpBiffString->wRecType    = 4;
         lpBiffString->wRecLength  = iTextLength+1 + 8;
         lpBiffString->wRow        = iRow;
         lpBiffString->wColumn     = iCol;
         lpBiffString->cAttrib[0]  = 0;
         lpBiffString->cAttrib[1]  = 0;
         lpBiffString->cAttrib[2]  = 0;
         lpBiffString->bTextLength = (BYTE)LOWORD(iTextLength);
         lstrncpy( lpBiffString->cLabel, szText, iTextLength+1 );

         lpData = lpData + lpBiffString->wRecLength + 4 ;

         break;

   case IDFORMULA:  /* make a formula record */ // put here for future use

         lpBiffFormula = lpData;

         lpBiffFormula->wRecType     = 32;
         lpBiffFormula->wRecLength   = iTextLength+1 + 8;
         lpBiffFormula->wRow         = iRow;
         lpBiffFormula->wColumn      = iCol;
         lpBiffFormula->cAttrib[0]   = 0x0;
         lpBiffFormula->cCurValue[0] = 0x0;
         lpBiffFormula->bRecalc      = 0;
         lpBiffFormula->bTextLength  = (BYTE)LOWORD(iTextLength);
         lstrncpy( lpBiffFormula->cLabel, szText, iTextLength+1 );

         lpData = lpData + lpBiffFormula->wRecLength + 4 ;

         break;

   } /* end switch */

}

/* ---------------------
   Create an EOF record.
 ----------------------- */
void CloseBiff()
{
   *lpData++ = 0xa;  // rec type
   *lpData++ = 0x0;
   *lpData++ = 0x0;
   *lpData++ = 0x0;

}


/* --------------------------------------------------
   Dump buffer to a file.
 ---------------------------------------------------- */
int DumpStuff()
{
   OFSTRUCT OFFile;
   int      iFile;

   iFile = OpenFile((LPSTR)"dump.xls",
                    (LPOFSTRUCT)&OFFile,OF_CREATE|OF_WRITE);
   if (iFile == -1) {
     MessageBox (GetFocus(),(LPSTR)"OpenFile failed.", NULL, MB_OK);
     return(FALSE);
   }
   iLength = lpData-lpDataInit;
   if (_lwrite(iFile, lpDataInit, iLength) != iLength ) {
     MessageBox (GetFocus(),(LPSTR)"write",(LPSTR)"failed",MB_OK);
   }
   _lclose(iFile);

}

/* ---------------------
   Copy number of bytes.
 ----------------------- */

void lstrncpy(LPSTR lpDest, LPSTR lpSrc, int n)
{
    while (n--)
      *lpDest++ = *lpSrc++;
}


/*---------------------
   PMCAP.H header file
  ---------------------*/

VOID CheckMenuItem (HWND hwndMenu, SHORT idItem, BOOL fCheck) ;
VOID EnableMenuItem (HWND hwndMenu, SHORT idItem, BOOL fEnable) ;
VOID ErrorMessage (HWND hwnd, SHORT idError) ;
VOID AddItemToSysMenu (HWND hwndFrame) ;
VOID SaveColorSettings (BOOL fSave, BOOL fSetMono) ;
VOID IncrementFilename (CHAR *pchName) ;
HBITMAP CopyBitmap (HBITMAP hbmSrc) ;
VOID CopyPointerToScreen (HPS hpsScreen, LONG alBitmapFormats[], BOOL fCopy) ;
HBITMAP ScreenToBitmap (SHORT cxScreen, SHORT cyScreen, BOOL fIncludePtr,
                                                        BOOL fMonochrome) ;
BOOL IsBitmapMonoEGA (HBITMAP hbm) ;
SHORT SaveBitmap (HBITMAP hbm, CHAR *szFilename) ;
SHORT SavePaintFormat (HBITMAP hbm, CHAR *szFilename) ;

#define ID_RESOURCE      1

#define IDM_FILE         10
#define IDM_SAVE         11
#define IDM_SAVEPAINT    12
#define IDM_ABOUT        13

#define IDM_EDIT         20
#define IDM_COPY         21
#define IDM_PASTE        22

#define IDM_CAPTURE      30
#define IDM_BEGIN        31
#define IDM_ABORT        32
#define IDM_MONO         33
#define IDM_HIDE         34
#define IDM_AUTODLG      35
#define IDM_POINTER      36

#define IDM_DELAY        40
#define IDM_DELAY5       (IDM_DELAY + 5)
#define IDM_DELAY10      (IDM_DELAY + 10)
#define IDM_DELAY15      (IDM_DELAY + 15)
#define IDM_DELAY30      (IDM_DELAY + 30)
#define IDM_DELAY60      (IDM_DELAY + 60)

#define IDM_DISPLAY      110
#define IDM_ACTUAL       111
#define IDM_STRETCH      112

#define IDD_ABOUT        1
#define IDD_SAVE         2
#define IDD_SAVEPAINT    3
#define IDD_FILENAME     10

#define IDS_FILEOPEN     1
#define IDS_DISKFULL     2
#define IDS_BMPCREATE    3
#define IDS_MULTIPLANE   4
#define IDS_TIMER        5
#define IDS_MEMORY       6
#define IDS_MONOEGA      7

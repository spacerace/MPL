//---------------------------------------------------------------------------//
// cldemo.c
//
// Copyright (c) Microsoft Corporation, 1990-
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// include files 
//---------------------------------------------------------------------------

#include "windows.h"
#include "ecd.h"
#include "cldemo.h"
#include "client.h"

// ----- constants ---------------------------------------------------------
#define szLessonClass  "Lesson1Class"
#define szItemClass    "ItemClass"
#define szMenuName     "Menu"
#define szTitle        "Client - %s"
#define szDefOpen      "test.cl1"
#define szUntitled     "Untitled"
#define szSave         "Save Changes to %s ?"
#define szInvalidFn    "Invalid file name"
#define szNoFile       "No filename specified."
#define szFnError      "File Error"
#define dxCascade      20       
#define dyCascade      20
#define dxInflate      5
#define dyInflate      5
#define cbFnMax        255
#define szMsgMax       100
#define szMaxClassName 20
#define MaxChildWnd 		20


// ----- globals -----------------------------------------------------------



HANDLE  hAccTable;
HWND    hwndMain;
POINT   ptAnchor;                // top-left corner of a new window
char    rgchFilename[cbFnMax]; // file's name
char    rgchOpenName[cbFnMax]; 
BOOL    fDirty = FALSE;   // true if any modification has been made to the file
WORD    cfLink, cfOwnerLink, cfNative;
HWND	hInst;
HWND	hwndEdit = NULL;
HANDLE hEditBuffer = NULL;
FARPROC lpEditWndProc;
BOOL fInsUpdate = FALSE;
char szInsFileName[cbFnMax];
HWND hUndoitem = NULL;

HWND rgchChildWnd[MaxChildWnd];
int irgchChild = 0;
int nObjDestroy = 0;

// BOOL fObjectClose = FALSE;

extern HANDLE			hObjectUndo;
extern LPECDOBJECT     lpObjectUndo;
extern BOOL            fEcdrelease;
extern BOOL            fEcdReleaseError;
extern BOOL            fEcdReleaseStat;

// ----- Local functions ---------------------------------------------------
BOOL InitApplication (HANDLE hInstance);
BOOL InitInstance (HANDLE hInstance, int nCmdShow);
void TermInstance (void);
void GetFromClipboard (HWND, WORD);
void PutOnClipboard(HWND, WORD);
void ResetAnchor (PPOINT);
void MoveAnchor (PPOINT, PRECT, PPOINT);
void SetNullFilename (HWND);
void SetFilename (HWND, char*);
BOOL SaveAsNeeded (HWND);
BOOL New (HWND);
BOOL Open (HWND);
BOOL Save (HWND);
BOOL SaveAs (HWND);
BOOL WriteToFile (HWND);
BOOL ReadFromFile (HWND, int);
void CleanHwnd (HWND hwnd);
ECDSTATUS CreateObject(WORD wParam, PITEM pitem);
void CallPrint();
void RemoveObjUndo();
void PrintError(ECDSTATUS stat);
void PrintWindowTitle(PITEM pitem, HWND hwnd);

VOID TestEqual(); 

void PrepareUndoClear(PITEM pitem);
void GetUndo(PITEM pitem);
ECDSTATUS CopyLink(PITEM pCopy);
VOID WaitForRelease(PITEM pitem);
HWND GetTopChild();
HWND GetNextWnd(LPINT i);

long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);
long FAR PASCAL ItemWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL AboutDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL SaveAsDlgProc(HWND, unsigned, WORD, LONG);
int  FAR PASCAL OpenDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL InsFileProc (HWND, unsigned, WORD, LONG);

//---------------------------------------------------------------------------
// WinMain
// Purpose: Wins the Main
//---------------------------------------------------------------------------
int PASCAL
WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
	BOOL fMsg;
	 hInst = hInstance;

    // initialize the application
	fMsg = SetMessageQueue(32);

    if (!hPrevInstance)
        if (!InitApplication(hInstance))
            return (FALSE);

    // initialize the instance
    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    // initialize the client module
    if (!InitClient (hInstance))
        return FALSE;

    // Main message pump
    while (GetMessage(&msg, NULL, NULL, NULL))
        {
        if (!TranslateAccelerator(hwndMain, hAccTable, &msg))
            {
            TranslateMessage(&msg);
            DispatchMessage(&msg); 
            }
        }

    // clean up
    TermInstance ();
    TermClient ();

    return (msg.wParam);
}

//---------------------------------------------------------------------------
// CreateItemWindow
// Purpose: Create a window in which we'll put an object
// Returns: handle to the newly created window
//---------------------------------------------------------------------------
HWND CreateItemWindow (HWND hwndParent, BOOL fIconic, LPRECT lprect)
{
    RECT  rc;
    POINT ptSize;

    // Find a window position and size 
    // We do all the previous work even if lprect != NULL
    // create the window
    return (CreateWindow(
        szItemClass,
        "",                  // Window Title
        WS_DLGFRAME | WS_CHILD | WS_CLIPSIBLINGS | 
          WS_THICKFRAME |  WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX |
			(fIconic ? WS_MINIMIZE : 0),
//          WS_THICKFRAME | (fVisible ? WS_VISIBLE : 0),
        NULL,
        NULL,
        200,
        200,
        hwndParent,
        NULL,
        hInst,
        NULL));
}


//---------------------------------------------------------------------------
// InitApplication
// Purpose: Register windows' classes
// Return: TRUE if successful, FALSE otherwise.
//---------------------------------------------------------------------------
BOOL InitApplication(hInstance)
HANDLE hInstance;
{
    WNDCLASS  wc;

    wc.style = NULL;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH); 
    wc.lpszMenuName =  szMenuName;
    wc.lpszClassName = szLessonClass;

    if (!RegisterClass(&wc))
        return FALSE;

    wc.style   = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
    wc.hIcon   =  NULL;
    wc.cbWndExtra = sizeof(HANDLE); // used to store a handle to the window's
                                    //  content
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szItemClass;
    wc.lpfnWndProc   = ItemWndProc;

    if (!RegisterClass(&wc))
        return FALSE;
}



//---------------------------------------------------------------------------
// InitInstance
// Purpose: Create the main window
// Return: TRUE if successful, FALSE otherwise.
//---------------------------------------------------------------------------
BOOL InitInstance(HANDLE hInstance, int nCmdShow)
{

    hAccTable = LoadAccelerators(hInstance, "Accelerators");

    ResetAnchor(&ptAnchor);

    // register clipboard formats.
    cfLink      = RegisterClipboardFormat("ObjectLink");
    cfOwnerLink = RegisterClipboardFormat("OwnerLink");
    cfNative    = RegisterClipboardFormat("Native");

    // Create the main window
    hwndMain = CreateWindow(
        szLessonClass,               // class name
        "",                  // title
        WS_OVERLAPPEDWINDOW,         // style
        CW_USEDEFAULT,               // position: default
        CW_USEDEFAULT,
        CW_USEDEFAULT,               // size: default
        CW_USEDEFAULT,
        NULL,                        // parent: none
        NULL,                        // menu: use the class menu
        hInstance,
        NULL
    );

    if (!hwndMain)
        return (FALSE);

    // This document is Untitled
    SetNullFilename (hwndMain);
	 
    ShowWindow(hwndMain, nCmdShow);
    return (TRUE);
}

//---------------------------------------------------------------------------
// TermInstance
// Purpose: 
// Return: void
//---------------------------------------------------------------------------
void TermInstance (void)
{
}

//---------------------------------------------------------------------------
// MainWndProc
// Purpose: Handle the main window's events
//---------------------------------------------------------------------------
long FAR PASCAL
MainWndProc(HWND hwnd, unsigned message, WORD wParam, LONG lParam)
{
    PITEM           pitem;
    HANDLE          hitem;
    PAINTSTRUCT     ps;
    ECDCLIPFORMAT   cfFormat;
	 FARPROC lpInsProc;
	 RECT rc;
	HDC hDC;
	short mm;
	static BOOL object = 0;
   FARPROC lpProc;
	ECDSTATUS EcdStat;
	HWND hwndItem;


    switch (message)
        {
        case WM_PAINT:
				BeginPaint(hwnd, &ps);
//				CallWindowProc( lpEditWndProc, hwndEdit, message, ps.hdc, 0L);

            EndPaint (hwnd, &ps);
				break;

		  case WM_CREATE:	GetClientRect(hwnd, (LPRECT)&rc);
												break;


      case WM_COMMAND:
            switch (wParam)
                {
                case IDM_COPY:
                case IDM_CUT:
                    PutOnClipboard(hwnd, wParam);
                    if (wParam == IDM_COPY)
                        break;
							else{
								HWND hwndChild;
								PITEM pitem;
								HANDLE hitem;

								hwndChild = GetTopChild();

								hitem = GetWindowWord(hwndChild, 0);
								pitem = (PITEM)LocalLock(hitem);
								EcdDelete(pitem->lpecdobject);
								LocalUnlock(hwndChild);
								DestroyWindow(hwndChild);
								break;
								}

                case IDM_CLEAR:
                    {
                  HWND hwndChild;
						PITEM pitem, pObjectUndo;
						HANDLE hitem;
						hwndChild = GetTopChild();

						
                  hitem = GetWindowWord(hwndChild, 0);
                  pitem = (PITEM) LocalLock(hitem);
						PrepareUndoClear(pitem);
						LocalUnlock(hitem);
                  DestroyWindow(hwndChild);
                  fDirty = TRUE;
                  break;
                    }

                case IDM_PASTE:
                case IDM_LINK:
                    GetFromClipboard(hwnd, wParam);
						  RemoveObjUndo();
                    fDirty = TRUE;
                    break;
                    
                case IDM_PROPERTIES:
                    {
                    HWND    hwndTop;
                    HANDLE  hitem;
                    PITEM   pitem;

								hwndTop = GetTopChild();


                    hitem = GetWindowWord(hwndTop, 0);
                    pitem = (PITEM) LocalLock(hitem);

                    lpProc = MakeProcInstance(PropertiesDlgProc, hInst);
                    DialogBox(hInst,
                        (pitem->wType == IDM_LINK) ? "LinkProp" : "EmbProp",
                        hwnd, lpProc);
                    FreeProcInstance(lpProc);
                    LocalUnlock(hitem);
                    break;
                    }

                case IDM_ABOUT:
                    {
                    FARPROC lpProc;
                    HANDLE  hInst;

                    hInst = GetWindowWord(hwnd, GWW_HINSTANCE);
                    lpProc = MakeProcInstance(AboutDlgProc, hInst);
                    DialogBox(hInst, "AboutBox", hwnd, lpProc);
                    FreeProcInstance(lpProc);
                    break;
                    }

                case IDM_NEW:
                    New(hwnd);
						RemoveObjUndo();
                    break;
					 case IDM_INSERTFILE:
						{
						int fh;

            			fh = Insert(hwndMain);
            			if (fh < 0){
								MessageBox(hwndMain, 
                          "Error In Insert File",
                          szAppName, MB_ICONASTERISK | MB_OK);
								return FALSE;
								}
               		//Fall through
							}

					 case IDM_UNDO:
					 case IDM_COPYLINK:
					 case IDM_INSERT:
					 case IDM_SHAPES:
					 case IDM_PBRUSH:
					 case IDM_EXCEL:
					 case IDM_GRAPH:
					 case IDM_TORUS:

						{
						PITEM pitem;
						   // Allocate memory for a new item	 
						   hitem = LocalAlloc (LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof (ITEM));	 

						   if (hitem == NULL || ((pitem = (PITEM)LocalLock (hitem)) == NULL)){
						       goto errRtn;
								}

						   if (hwndItem = CreateItemWindow(hwndMain, FALSE, NULL))	 
						       {	 
						       SetWindowWord (hwndItem, 0, hitem);	 
						       BringWindowToTop (hwndItem);
						       }
							else
						       goto errRtn;

						rgchChildWnd[irgchChild++] = hwndItem;
        					SetWindowWord (hwndMain, 0, (GetWindowWord(hwndMain, 0)+1) );
							if ((wParam == IDM_UNDO) && hObjectUndo){
								GetUndo(pitem);
								goto Horrible; //dirtiest code 
							}
							else
								RemoveObjUndo(); // so nothing left of old object

							if (wParam == IDM_INSERTFILE)
							 EcdStat = EcdCreateFromTemplate(PROTOCOL, lpclient, (LPSTR)rgchOpenName, 0, NULL, &(pitem->lpecdobject),
							   ecdrender_draw, 0);
							else if (wParam == IDM_COPYLINK){
								EcdStat = CopyLink(pitem);
								}
							else
								EcdStat = CreateObject(wParam, pitem);
							if (EcdStat == ECD_WAIT_FOR_RELEASE){
								WaitForRelease(pitem);
        						}
							if (fTestEcdStat(EcdStat)){
								MessageBox(hwndMain, 
                          "Error In EcdCreate",
                          szAppName, MB_ICONASTERISK | MB_OK);
								goto errRtn;
								}
							else {
								    cfFormat = EcdEnumObjectFormats (pitem->lpecdobject, NULL);
									 if (cfFormat == cfNative)
    								     pitem->wType = IDM_PASTE;  
    								 else if (cfFormat == cfLink)
    								     pitem->wType = IDM_LINK;  
    
									 pitem->wUpdate = IDD_AUTO;
Horrible:
									pitem->fOpen = FALSE;
									pitem->fClose = FALSE;
		
									 hDC = GetDC(hwndItem);
    						  //		 EcdSetHostNames(pitem->lpecdobject, szHostname, szHostobjectname);
									mm = SetMapMode(hDC, MM_HIMETRIC);
									EcdQueryBounds(pitem->lpecdobject, &rc);
									LPtoDP(hDC, (LPPOINT)&rc, 2);
									SetMapMode(hDC, mm);
									fInsUpdate = TRUE;
									if ((rc.right - rc.left == 0) || (rc.bottom - rc.top == 0)){
										rc.right = 200;
										rc.bottom = 200;
										rc.left = 0;
										rc.top = 0;
										}
									MoveWindow(hwndItem, rc.left, rc.top, rc.right - rc.left,
										rc.bottom - rc.top, TRUE);
								   SendMessage (hwndItem, WM_USER, 0, 0L);
									 ShowWindow(hwndItem, SW_SHOW);
   								 InvalidateRect(hwndItem, NULL, TRUE);
									 ReleaseDC(hwndItem, hDC);
	
									}
                		LocalUnlock(hitem);
							break;
							}

                case IDM_OPEN:
                    Open(hwnd);
							RemoveObjUndo(); // so nothing left of old
                    break;

                case IDM_SAVE:
                    Save(hwnd);
                    break;

                case IDM_SAVEAS:
                    SaveAs(hwnd);
                    break;

					case IDM_EQUAL:
							TestEqual();
							break;

                case IDM_EXIT:
                    SendMessage (hwnd, WM_CLOSE, 0, 0L);
                    return(0L);
                    break;
                case IDM_CLOSE:
						{
						HWND hwndTop;
						/*
						hwndTop = GetTopWindow(hwndMain);
						if (hwndTop == hwndEdit)
							hwndTop = GetWindow(hwndTop, GW_HWNDNEXT);
						*/
						hwndTop = GetTopChild();

						SendMessage (hwndTop, WM_USER+1, 0, 0L);
                  return(0L);
						}
					case IDM_RECONNECT:
						{
						HWND hwndTop;
						hwndTop = GetTopChild();

						SendMessage (hwndTop, WM_USER+2, 0, 0L);
                  break;
						}

					case IDM_PRINT:
						CallPrint();
						break;
            } 
            break;

        case WM_CLOSE:
            if (!SaveAsNeeded(hwnd))
                return(0L);
            break;

        case WM_DESTROY:
			{
			int nObjects;
			HWND hwndT;
			int i = 0;
			MSG msg;

				nObjDestroy = GetWindowWord(hwndMain, 0);
				nObjects = nObjDestroy;
				while (nObjects--){
					hwndT = GetNextWnd(&i);
					DestroyWindow(hwndT);
					}
				while (nObjDestroy){
					GetMessage(&msg, NULL, NULL, NULL);
            	TranslateMessage(&msg);
            	DispatchMessage(&msg); 
            	}
            PostQuitMessage (0);
            return(0L);
				}

        case WM_INITMENU:
            ClientUpdateMenu(hwnd, (HMENU) wParam);
            break;

    }
   return (DefWindowProc(hwnd, message, wParam, lParam));

	errRtn:
    if (pitem)
        LocalUnlock (hitem);

    if (hitem)
        LocalFree (hitem);

    if (hwndItem)
        {
        SetWindowWord(hwndItem, 0, NULL);
        DestroyWindow(hwndItem);
        }

}


//---------------------------------------------------------------------------
// ItemWndProc
// Purpose: Handle Item window's messages
//---------------------------------------------------------------------------
long FAR PASCAL
ItemWndProc (HWND hwnd, unsigned message, WORD wParam, LONG lParam)
{
    PITEM           pitem;
    HANDLE          hitem;
	 HANDLE				hOldwnd = hwnd;
	 static POINT ptOld, ptNew;
	 static BOOL bCapture = FALSE;
	 static RECT rc;
	 int xChange, yChange;
	 HWND hNext;
	 static int xWnd, yWnd;


    switch (message)
        {
		  case WM_USER:
			{
            HANDLE hitem;
            PITEM pitem;
				ECDSTATUS EcdStat;
				HANDLE hWinTitle;
				LPSTR pTitle, pTitle1;

            hitem = GetWindowWord (hwnd, 0);
            pitem = (PITEM) LocalLock (hitem);
				PrintWIndowTitle(pitem ,hwnd);

            LocalUnlock(hitem);
            break;
            }
			case WM_SIZE:
				{
				RECT rc;
				HWND hdc;
				short mm;
				ECDSTATUS EcdStat;
	
            hitem = GetWindowWord (hwnd, 0);
				if (hitem && (pitem = (PITEM) LocalLock (hitem))){
					if (pitem->fOpen){
						GetClientRect(hwnd, &rc);
            		hdc = GetDC(hwnd);
						mm = SetMapMode(hdc, MM_HIMETRIC);
						DPtoLP(hdc,(LPPOINT)&rc,2);
						EcdSetBounds(pitem->lpecdobject, (LPRECT)&rc);
   					SetMapMode(hdc, mm);
   					ReleaseDC(hwnd, hdc);
						}
						LocalUnlock(hitem);
					}
    			return (DefWindowProc(hwnd, message, wParam, lParam));
				}
	

        case WM_DESTROY:
			{
				int i = 0;
            hitem = GetWindowWord (hwnd, 0);
            if (hitem)
                {
                pitem = (PITEM) LocalLock (hitem);
                ClientDelete(pitem);
                LocalUnlock(hitem);
                LocalFree(hitem);
                }
					while (hwnd != rgchChildWnd[i])
						i += 1;
					if (i > MaxChildWnd){
						MessageBox(hwndMain, "Wrong Child Id",
                         szAppName, MB_ICONASTERISK | MB_OK);
						return;
						}
					rgchChildWnd[i] = 0;
					nObjDestroy -= 1;
        			SetWindowWord (hwndMain, 0, (GetWindowWord(hwndMain, 0) - 1) );
            break;
				}

        case WM_PAINT:
            {
            HDC             hdc;
            PAINTSTRUCT     ps;

            BeginPaint (hwnd, (LPPAINTSTRUCT)&ps);
            hitem = GetWindowWord (hwnd, 0);
            pitem = (PITEM) LocalLock (hitem);

            if (pitem)
                ClientPaint(hwnd, ps.hdc, pitem);
            LocalUnlock(hitem);
				hNext = GetWindow(hwnd, GW_HWNDNEXT);
				if (hNext && (hNext != hwndEdit))
					InvalidateRect(hNext, NULL, FALSE);

            EndPaint (hwnd, (LPPAINTSTRUCT)&ps);
            break;
            }


        case WM_LBUTTONDBLCLK:
            {
            HANDLE hitem;
            PITEM pitem;
            
            // edit the object
            hitem = GetWindowWord (hwnd, 0);
            pitem = (PITEM) LocalLock (hitem);
            ClientEdit(hwnd, pitem);
            LocalUnlock(hitem);
            break;
            }
        case WM_LBUTTONDOWN:
            BringWindowToTop (hwnd);
				SetCapture(hwnd);
				GetCursorPos((LPPOINT)&ptOld);
				bCapture = TRUE;
				break;

		  case WM_LBUTTONUP:
				if (bCapture){
					bCapture = FALSE;
					ReleaseCapture();
					GetCursorPos((LPPOINT)&ptNew);
					xChange = ptNew.x - ptOld.x;
					yChange = ptNew.y - ptOld.y;
					GetWindowRect(hwnd, (LPRECT)&rc);
        			ScreenToClient(hwndMain, (LPPOINT) &rc);
        			ScreenToClient(hwndMain, ((LPPOINT) &rc)+1);
					MoveWindow( hwnd, rc.left + xChange, rc.top + yChange,
						rc.right - rc.left, rc.bottom - rc.top, TRUE);
					}
				break;
			case WM_USER+1: // Asked by user to close the document
            {
            HANDLE hitem;
				ECDSTATUS EcdStat;
            PITEM pitem;
            
            // edit the object
            hitem = GetWindowWord (hwnd, 0);
            pitem = (PITEM) LocalLock (hitem);
				pitem->fOpen = FALSE;
				pitem->fClose = TRUE;
				if ((EcdStat = EcdClose(pitem->lpecdobject)) == ECD_WAIT_FOR_RELEASE){
					WaitForRelease(pitem);
					}
					if (fTestEcdStat(EcdStat)){
						MessageBox(hwnd, "Error In Closing Object",
						   szAppName, MB_ICONASTERISK | MB_OK);
						}
            LocalUnlock(hitem);
    			return (DefWindowProc(hwnd, message, wParam, lParam));
            }
			case WM_USER+2: // Asked by user to Reconnect
            {
            HANDLE hitem;
				ECDSTATUS EcdStat;
            PITEM pitem;
            
            // edit the object
            hitem = GetWindowWord (hwnd, 0);
            pitem = (PITEM) LocalLock (hitem);
				pitem->fOpen = TRUE;
				pitem->fClose = FALSE;
				if ((EcdStat = EcdReconnect(pitem->lpecdobject)) == ECD_WAIT_FOR_RELEASE){
					WaitForRelease(pitem);
					}
					if (fTestEcdStat(EcdStat)){
						MessageBox(hwnd, "Error In Reconnecting Object",
						   szAppName, MB_ICONASTERISK | MB_OK);
						}
            LocalUnlock(hitem);
    			return (DefWindowProc(hwnd, message, wParam, lParam));
            }


    }
    return (DefWindowProc(hwnd, message, wParam, lParam));
}


//---------------------------------------------------------------------------
// AboutDlgProc
// Purpose: Handle the About... dialog box events
//---------------------------------------------------------------------------
BOOL FAR PASCAL
AboutDlgProc(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    switch (message)
        {
        case WM_COMMAND:
            if (wParam == IDOK
              || wParam == IDCANCEL)
                {
                EndDialog(hDlg, TRUE);
                return (TRUE);
                }
            break;
        }
    return (FALSE);
}


//---------------------------------------------------------------------------
// SaveAsDlgProc
// Purpose: Handle the File Save As... dialog box events
//---------------------------------------------------------------------------
BOOL FAR PASCAL
SaveAsDlgProc (HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    switch (message)
        {
        case WM_COMMAND:
            switch (wParam)
                {
                case IDOK:
                    GetDlgItemText(hDlg, IDC_EDIT, (LPSTR) rgchFilename, cbFnMax);
                    EndDialog(hDlg, TRUE);
                    return (TRUE);

                case IDCANCEL:
                    EndDialog(hDlg, NULL);
                    return (FALSE);
                }
            break;

        case WM_INITDIALOG:
            SetDlgItemText(hDlg, IDC_EDIT, (LPSTR)rgchFilename);
            SendDlgItemMessage(hDlg, IDC_EDIT, EM_SETSEL, 0,
                MAKELONG(0, 32767));
            DlgDirList(hDlg, "", 0, IDC_PATH, 0);
            SetFocus(GetDlgItem(hDlg, IDC_EDIT));
            return (FALSE);
    }
    return FALSE;
}

//---------------------------------------------------------------------------
// OpenDlgProc
// Purpose: Handle the File Open... dialog box events
//---------------------------------------------------------------------------
int FAR PASCAL OpenDlgProc(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    int fh; 

    switch (message)
        {
        case WM_COMMAND:
            switch (wParam)
                {
                case IDC_LISTBOX:
                    switch (HIWORD(lParam))
                        {
                        case LBN_SELCHANGE:
                            DlgDirSelect(hDlg, rgchOpenName, IDC_LISTBOX);
                            SetDlgItemText(hDlg, IDC_EDIT, rgchOpenName);
                            SendDlgItemMessage(hDlg, IDC_EDIT, EM_SETSEL,
                                NULL, MAKELONG(0, 32767));
                            break;

                        case LBN_DBLCLK:
                            goto openfile;
                        }
                    return (TRUE);

                case IDOK:
openfile:
                    GetDlgItemText(hDlg, IDC_EDIT, rgchOpenName, cbFnMax);

                    if (!rgchOpenName[0])
                        {
                        MessageBox(hDlg, szNoFile,
                            NULL, MB_OK | MB_ICONHAND);
                        return (TRUE);
                        }

                    if ((fh = _lopen(rgchOpenName, OF_READ)) == -1)
                        {
                        MessageBox(hDlg, szInvalidFn,
                            NULL, MB_OK | MB_ICONHAND);
                        return (TRUE);
                        }
                    EndDialog(hDlg, fh);
                    return (TRUE);

                case IDCANCEL:
                    EndDialog(hDlg, -1);
                    return (FALSE);
            }
            break;

        case WM_INITDIALOG:                      
            DlgDirList(hDlg, "", IDC_LISTBOX, IDC_PATH, 0); 
            SetDlgItemText(hDlg, IDC_EDIT, szDefOpen);
            SendDlgItemMessage(hDlg, IDC_EDIT, EM_SETSEL, NULL, 
                MAKELONG(0, 32767));              
            SetFocus(GetDlgItem(hDlg, IDC_EDIT));
            return (FALSE); // The focus is already set to a control
    }
    return FALSE;
}



//---------------------------------------------------------------------------
// GetFromClipboard
// Purpose: Paste or Link from the Clipboard
// Returns: void
//---------------------------------------------------------------------------
void GetFromClipboard (HWND hwndMain, WORD w)
{   
    ClientPasteLink(hwndMain, w);
}

//---------------------------------------------------------------------------
// PutOnClipboard
// Purpose:  Cut or Copy onto the Clipboard
// Returns: void
//---------------------------------------------------------------------------
void PutOnClipboard(HWND hwndMain, WORD w)
{   
    ClientCutCopy(hwndMain, w);
}

//---------------------------------------------------------------------------
// ResetAnchor
// Purpose: Resets the anchor so that new child windows are created at the 
//          top left of the main window
// Returns: void
//---------------------------------------------------------------------------
void ResetAnchor(PPOINT pptAnchor)
{
    pptAnchor->x = dxInflate - dxCascade ;
    pptAnchor->y = dyInflate - dyCascade ;
}

//---------------------------------------------------------------------------
// MoveAnchor
// Purpose: Moves the anchor so that the new child window 
//          1) appear in cascade;
//          2) doesn't get created past the edge
// Returns: void
//---------------------------------------------------------------------------
void MoveAnchor(PPOINT pptAnchor, PRECT pclientRect, PPOINT pptSize)
{
    ptAnchor.y += dyCascade;
    if (pptAnchor->y + pptSize->y >= pclientRect->bottom)
        pptAnchor->y = pclientRect->top ;

    ptAnchor.x += dxCascade;
    if (pptAnchor->x + pptSize->x >= pclientRect->right)
        pptAnchor->x  = pclientRect->left ;
}

//---------------------------------------------------------------------------
// SetNullFilename
// Purpose: This document is untitled...
// Returns: void
//---------------------------------------------------------------------------
void SetNullFilename (HWND hwndMain)
{
    SetFilename (hwndMain, szUntitled);
}


//---------------------------------------------------------------------------
// SetFilename
// Purpose: Update window title 
//          Keep a copy of the filename
// Returns: FALSE if need to cancel the operation, TRUE otherwise
//---------------------------------------------------------------------------
void SetFilename (HWND hwnd, char* rgch)
{
    char rgchTitle[szMsgMax];

    // Keep a copy of the filename handy
    lstrcpy (rgchFilename, rgch);

    // Set up the window title
    wsprintf ((LPSTR) rgchTitle, (LPSTR) szTitle, (LPSTR) rgch);
    SetWindowText(hwnd, rgchTitle);
}

//---------------------------------------------------------------------------
// SaveAsNeeded
// Purpose: If necessary, ask the user if s/he wants to save the file
// Returns: FALSE if need to cancel the operation, TRUE otherwise
//---------------------------------------------------------------------------
BOOL SaveAsNeeded(HWND hwnd)
{
    int retval;
    char rgch[szMsgMax];

    if (!fDirty)
        return TRUE;
    wsprintf ((LPSTR) rgch, (LPSTR) szSave, (LPSTR) rgchFilename);
    retval = MessageBox (hwnd, rgch, szAppName,
        MB_YESNOCANCEL | MB_ICONINFORMATION);
    switch (retval)
        {
        case IDNO:
            return TRUE;
        case IDYES:
            return (Save(hwnd));
        default:
            return FALSE;
        }
}

//---------------------------------------------------------------------------
// New
// Purpose: Delete old Data
// Returns: FALSE if the operation was canceled, TRUE otherwise
//---------------------------------------------------------------------------
BOOL New (HWND hwnd)
{
    // Save the current file if necessary
    if (!SaveAsNeeded(hwnd))
        return (FALSE);

    // delete current objects
    CleanHwnd (hwnd);

    fDirty = FALSE;

    // reset filename
    SetNullFilename(hwnd);
}

//---------------------------------------------------------------------------
// Open
// Purpose: Ask for a file name
//          Open the file
// Returns: FALSE if the operation was canceled, TRUE otherwise
//---------------------------------------------------------------------------
BOOL Open (HWND hwnd)
{
    FARPROC lpProc;
    int     fh;

    // Save the current file if necessary
    if (!SaveAsNeeded(hwnd))
        return (FALSE);

    // Ask for a file name
    lpProc = MakeProcInstance(OpenDlgProc, hInst);
    fh = DialogBox(hInst, "Open", hwnd, lpProc);
    FreeProcInstance(lpProc);
    if (fh < 0)
        return (FALSE); 
    SetFile(fh);
    lstrcpy(rgchFilename, rgchOpenName);
    SetFilename(hwnd, rgchFilename);

    // delete current objects
    CleanHwnd (hwnd);

    // read new objects
    return(ReadFromFile(hwnd, fh));
}


//---------------------------------------------------------------------------
// Save
// Purpose: Save the current data to a file 
//          Ask for a new name if necessary
// Returns: FALSE if the operation was canceled, TRUE otherwise
//---------------------------------------------------------------------------
BOOL Save(HWND hwnd)
{
    if (!fDirty)
        {
        // The file is already saved.  We are done.
        return (TRUE);
        }
    if (!lstrcmp((LPSTR) rgchFilename, (LPSTR) szUntitled))
        return (SaveAs(hwnd));
    return (WriteToFile(hwnd));
}


//---------------------------------------------------------------------------
// SaveAs
// Purpose: Save the current data to a file 
//          Ask for a new name
// Returns: FALSE if the operation was canceled, TRUE otherwise
//---------------------------------------------------------------------------
BOOL SaveAs(HWND hwnd)
{
    FARPROC lpProc;
    int     dlg;

    lpProc = MakeProcInstance(SaveAsDlgProc, hInst);
    dlg = DialogBox(hInst, "SaveAs", hwnd, lpProc);
    FreeProcInstance(lpProc);
    if (!dlg)
        return (FALSE);
    SetFilename(hwnd, rgchFilename);
    return(WriteToFile(hwnd));
}


//---------------------------------------------------------------------------
// WriteToFile
// Purpose: Write the current data into a file 
// Returns: FALSE if the operation was aborted, TRUE otherwise
//---------------------------------------------------------------------------
BOOL    WriteToFile (HWND hwndMain)
{
    int     fh, nObjects;
    HANDLE  hitem;
    PITEM   pitem;
    HWND    hwnd;
	int i =0;
    RECT    rc;

	WORD  wFile, wSize = 0;
	LPSTR lpEditBuffer;
				ECDSTATUS EcdStat;



		    if ((fh =_lcreat (rgchFilename, 0)) <= 0)
		        {
		        MessageBox (
		                GetFocus(),
		                szInvalidFn,
		                szAppName,
		                MB_ICONEXCLAMATION | MB_OK);
		        return FALSE;
		        }
		    SetFile(fh);

			nObjects = GetWindowWord(hwndMain, 0);
			if (_lwrite (fh, (LPSTR)&nObjects, 2) <= 0){
				MessageBox (
                    GetFocus(),
                    szFnError,
                    szAppName,
                    MB_ICONEXCLAMATION | MB_OK);
            _lclose(fh);
            return(FALSE);
            }

									
        hwnd = GetNextWnd(&i);

		while (hwnd && nObjects--){
			if (hwnd != hwndEdit){
				if (IsIconic(hwnd)){
					rc.left = 0;
					rc.right = 0;
					rc.top = 0;
					rc.bottom = 0;
					}
				else { // if normal size window
					GetWindowRect(hwnd, &rc);
					// Screen coordinates => Parent coordinates
        			ScreenToClient(hwndMain, (LPPOINT) &rc);
        			ScreenToClient(hwndMain, ((LPPOINT) &rc)+1);
        			}
        		hitem = GetWindowWord (hwnd, 0);
        		pitem = (PITEM) LocalLock(hitem);
        		if ( (_lwrite (fh, (LPSTR) &rc, sizeof(RECT)) <= 0)
        		    || (_lwrite (fh, (LPSTR) &(pitem->wType), sizeof(WORD)) <= 0)
        		    || (_lwrite (fh, (LPSTR) &(pitem->wUpdate), sizeof(WORD)) <= 0)
        		    || ((EcdStat = EcdSaveToStream(((PITEM) pitem)->lpecdobject,
        		            (LPECDSTREAM) lpstream)) != ECD_OK))
        		   {
					
					if (EcdStat == ECD_WAIT_FOR_RELEASE)
						WaitForRelease(pitem);
					if (fTestEcdStat(EcdStat)){
        		    	MessageBox (
        		            GetFocus(),
        		            szFnError,
        		            szAppName,
        		            MB_ICONEXCLAMATION | MB_OK);
        		    	LocalUnlock(hitem);
        		    	_lclose(fh);
        		    	return(FALSE);
						}
        		   }
        		LocalUnlock(hitem);
        		}
		  
        hwnd = GetNextWnd(&i);
		}
    _lclose (fh);
    fDirty = FALSE;
    return TRUE;
}

//---------------------------------------------------------------------------
// ReadFromFile
// Purpose: Read new data from a file 
// Returns: FALSE if the operation was aborted, TRUE otherwise
//---------------------------------------------------------------------------
BOOL ReadFromFile (HWND hwnd, int fh)
{
    char ch;
    RECT rc;
    BOOL fRet = TRUE;
    LPECDOBJECT lpobject;
    WORD wType, wUpdate;
	 WORD wSize;
	 LPSTR lpEditBuffer;
	 int nObjects;
				ECDSTATUS EcdStat;

        if (_lread (fh, (LPSTR)&nObjects, 2) <= 0)
           return FALSE; // end of file
			SetWindowWord(hwndMain, 0, nObjects);


			while(nObjects--){
				if ((_lread (fh, (LPSTR)&rc, sizeof(RECT)) <= 0)
				    || (_lread (fh, (LPSTR) &wType, sizeof(WORD)) <= 0)
        		    || (_lread (fh, (LPSTR) &wUpdate, sizeof(WORD)) <= 0)
        		    || ((EcdStat = EcdLoadFromStream((LPECDSTREAM) lpstream, PROTOCOL,
        		        lpclient, 0, NULL, &lpobject)) != ECD_OK))
        		    {
        		    // file problem while reading an object description
						while(EcdQueryReleaseStatus(lpobject) == ECD_BUSY)
							{
							MSG msg;
							GetMessage(&msg, NULL, NULL, NULL);			
   	   				TranslateMessage(&msg);     					
   	   				DispatchMessage(&msg);      					
   						}
						if (fEcdReleaseError){
        		    			MessageBox (
        		            GetFocus(),
        		            szFnError,
        		            szAppName,
        		            MB_ICONEXCLAMATION | MB_OK);
								PrintError(fEcdReleaseStat);
        		    			fRet = FALSE;
        		    			break;
								}
        		    }
        
        		ClientCreateNewItem (hwnd, TRUE, &rc, lpobject, wType, wUpdate);
				}
        
    _lclose (fh);
    return (fRet);
}


//---------------------------------------------------------------------------
// CleanHwnd
// Purpose: Delete all the children of the main window
// Returns: void
//---------------------------------------------------------------------------
void CleanHwnd (HWND hwndMain)
{
    HWND hwnd, hwndT;
    HANDLE hitem;
	int nObjects = GetWindowWord(hwndMain, 0);
	int i = 0;

    hwnd = GetWindow (hwndMain, GW_CHILD);

    while (nObjects--){
        hwndT = GetNextWnd(&i);
		if (hwndT != hwndEdit)
        {
        DestroyWindow(hwndT);
        }
		}
    ResetAnchor(&ptAnchor);
}


long far PASCAL EditWndProc(HWND hwnd, unsigned message, WORD wParam, LONG lParam)
{
	PAINTSTRUCT ps;
	PITEM           pitem;
   HANDLE          hitem;
	POINT pt;
	HWND hNext;


	switch (message) {

	case WM_PAINT:

				BeginPaint(hwnd, &ps);
				CallWindowProc( lpEditWndProc, hwndEdit, message, ps.hdc, 0L);
				hNext = GetWindow(hwnd, GW_HWNDNEXT);
				if (hNext)
					InvalidateRect(hNext, NULL, FALSE);

            EndPaint (hwnd, &ps);
				break;

	default:
	    return CallWindowProc(lpEditWndProc, hwndEdit, message, wParam, lParam);
    	    break;
	}
    return(0L);
}


//*------------------------------------------------------------------------
//| InsFileProc
//|     Parameters:
//|         hWnd    - Handle to Window which message is delivered to.
//|         msgID   - ID number of message
//|         wParam  - 16-bit parameter
//|         lParam  - 32-bit parameter
//|
//*------------------------------------------------------------------------
BOOL FAR PASCAL InsFileProc (HWND     hWnd,
                            unsigned wMsgID,
                            WORD     wParam,
                            LONG     lParam)
{
	int nRet;

    switch(wMsgID)
    {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch (wParam)
        {
        case IDOK:
            nRet = GetDlgItemText(hWnd, 101, (LPSTR)szInsFileName, cbFnMax);
            EndDialog(hWnd,TRUE);
            return TRUE;

        case IDCANCEL:
            EndDialog(hWnd,FALSE);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

ECDSTATUS CreateObject(WORD wParam, PITEM pitem)
		{
		char szClassName[szMaxClassName];
		switch(wParam){
			case IDM_SHAPES:
				lstrcpy ((LPSTR)szClassName, "Shapes");
				break;
			case IDM_PBRUSH:
 				lstrcpy ((LPSTR)szClassName, "PBrush");
				break;
			case IDM_EXCEL :
				lstrcpy ((LPSTR)szClassName, "ExcelWorksheet");
				break;
			case IDM_GRAPH :
				lstrcpy ((LPSTR)szClassName, "Graph");
				break;
			case IDM_TORUS :
				lstrcpy ((LPSTR)szClassName, "Torus");
				break;
				}
		 return(EcdCreate(PROTOCOL, lpclient, (LPSTR)szClassName, 0, NULL, &(pitem->lpecdobject),
		 		 ecdrender_draw,  0));

		}



	void CallPrint(){
		static char szPrinter[80];
		char *szDevice, *szDriver, *szOutput;
		HWND hPrnDC, hDC;
		int nObjects;
	   HANDLE  hitem;
	   PITEM   pitem;
	   HWND    hwnd;
	   RECT    rc;
		short mm;
		int nRecSize, nRet;
    	HSTR hstr;
    	LPSTR lpstrInfo;
		int i =0;



						
		nObjects = GetWindowWord(hwndMain, 0);
		GetProfileString("windows", "device", ",,", szPrinter, 80);				
		if ((szDevice = strtok(szPrinter, ",")) &&
			(szDriver = strtok(NULL, ",")) &&				
			(szOutput = strtok(NULL, ",")))				
			hPrnDC = CreateIC(szDriver, szDevice, szOutput, NULL);
		else {
			   MessageBox (GetFocus(), "Not Printed",
        		            szAppName, MB_ICONEXCLAMATION | MB_OK);
        		    return;
        		    }

		nObjects = GetWindowWord(hwndMain, 0);				
		hwnd = GetWindow (hwndMain, GW_CHILD);
		while (hwnd && nObjects--){
			if (hwnd != hwndEdit){
				GetWindowRect(hwnd, &rc);
        		// Screen coordinates => Parent coordinates
        		ScreenToClient(hwnd, (LPPOINT) &rc);
        		ScreenToClient(hwnd, ((LPPOINT) &rc)+1);
				mm = SetMapMode(hPrnDC, MM_ISOTROPIC);
				LPtoDP(hPrnDC, (LPPOINT)&rc, 2);

        		hitem = GetWindowWord (hwnd, 0);
        		pitem = (PITEM) LocalLock(hitem);
				if (EcdDraw( pitem->lpecdobject, hPrnDC, (LPRECT)&rc, NULL) != ECD_OK)
        		    {
        		    MessageBox (
        		            GetFocus(),
        		            szFnError,
        		            szAppName,
        		            MB_ICONEXCLAMATION | MB_OK);
        		    LocalUnlock(hitem);
        		    return;
        		    }
				else {
        			LocalUnlock(hitem);
					nRecSize = (rc.right-rc.left)*(rc.top - rc.bottom);
					if (nRecSize < 0)
						nRecSize = - nRecSize;
						nRet = Escape(hPrnDC, STARTDOC, 5, "HELLO",   NULL);
						nRet = Escape(hPrnDC, NEWFRAME, 0, NULL,  NULL);
						nRet = Escape(hPrnDC, ENDDOC, 0, NULL,  NULL);
				SetMapMode(hPrnDC, mm);

        		}
			}
        hwnd = GetNextWnd(&i);
			}
		DeleteDC(hPrnDC);
		}
				
						
void RemoveObjUndo(){

			PITEM pitem;
			if (hObjectUndo){
				pitem = (PITEM)LocalLock(hObjectUndo);	
				EcdDelete(pitem->lpecdobject);
				LocalUnlock(hObjectUndo);
			  	LocalFree(hObjectUndo);  				  
      		hObjectUndo = NULL;				  
			}
		}
						  
void PrintError(ECDSTATUS stat){
	char szError[szMsgMax];

	switch(stat){
		case ECD_ERROR_MEMORY:
			lstrcpy(szError, "Error ECD_ERROR_MEMORY");
			break;
		case ECD_ERROR_FATAL:
			lstrcpy(szError, "Error ECD_ERROR_FATAL");
			break;
		case ECD_ERROR_STREAM:
			lstrcpy(szError, "Error ECD_ERROR_STREAM");
			break;
		case ECD_ERROR_STATIC:
			lstrcpy(szError, "Error ECD_ERROR_STATIC");
			break;
		case ECD_ERROR_BLANK:
			lstrcpy(szError, "Error ECD_ERROR_BLANK");
			break;
		case ECD_ERROR_LAUNCH:
			lstrcpy(szError, "Error ECD_ERROR_LAUNCH");
			break;
		case ECD_ERROR_COMM:
			lstrcpy(szError, "Error ECD_ERROR_COMM");
			break;
		case ECD_ERROR_DRAW:
			lstrcpy(szError, "Error ECD_ERROR_DRAW");
			break;
		case ECD_ERROR_CLIP:
			lstrcpy(szError, "Error ECD_ERROR_CLIP");
			break;
		case ECD_ERROR_FORMAT:
			lstrcpy(szError, "Error ECD_ERROR_FORMAT");
			break;
		case ECD_ERROR_ILLEGALOBJECT:
			lstrcpy(szError, "Error ECD_ERROR_ILLEGALOBJECT");
			break;
		case ECD_ERROR_OPTION:
			lstrcpy(szError, "Error ECD_ERROR_OPTION");
			break;
		case ECD_ERROR_PROTOCOL:
			lstrcpy(szError, "Error ECD_ERROR_PROTOC");
			break;
		case ECD_ERROR_ADDRESS:
			lstrcpy(szError, "Error ECD_ERROR_ADDRESS");
			break;

		case ECD_ERROR_NOT_EQUAL:
			lstrcpy(szError, "Error ECD_ERROR_NOT_EQUAL");
			break;

		case ECD_ERROR_HANDLE:
			lstrcpy(szError, "Error ECD_ERROR_HANDLE");
			break;
		case ECD_ERROR_GENERIC:
			lstrcpy(szError, "Error ECD_ERROR_GENERIC");
			break;
		case ECD_WAIT_FOR_RELEASE:
			lstrcpy(szError, "Error ECD_WAIT_FOR_RELEASE");
			break;
		case ECD_ERROR_MAPPING_MODE:
			lstrcpy(szError, "Error ECD_ERROR_MAPPING_MODE");
			break;

		case ECD_ERROR_INVALIDCLASS:
			lstrcpy(szError, "Error ECD_ERROR_INVALIDCLASS");
			break;
		case ECD_ERROR_SYNTAX:
			lstrcpy(szError, "Error ECD_ERROR_SYNTAX");
			break;
		case ECD_ERROR_NOT_OPEN:
			lstrcpy(szError, "Error ECD_ERROR_NOTOPEN");
			break;
		case ECD_ERROR_PROTECT_ONLY:
			lstrcpy(szError, "Error ECD_ERROR_PROTECT_ONLY");
			break;
		case ECD_ERROR_POKENATIVE:
			lstrcpy(szError, "Error ECD_ERROR_POKENATIVE");
			break;
		case ECD_ERROR_ADVISE_PICT:
			lstrcpy(szError, "Error ECD_ERROR_ADVISE_PICT");
			break;
		case ECD_ERROR_DATATYPE:
			lstrcpy(szError, "Error ECD_ERROR_DATATYPE");
			break;
		case ECD_ERROR_ALREADY_BLOCKED:
			lstrcpy(szError, "Error ECD_ERROR_ALREADYBLOCKED");
			break;
		default:
			lstrcpy(szError, "Error Other");
			break;
			}

			MessageBox(hwndMain,szError, szAppName, MB_ICONASTERISK | MB_OK);
			}




void PrintWindowTitle(PITEM pitem, HWND hwnd){

	ECDSTATUS EcdStat;
	HANDLE hWinTitle;
  	LPSTR pTitle, pTitle1;
   LPSTR lpstrInfo, lpstr1, lpstr2;
	HSTR hstr;

            if (((EcdStat = EcdGetData(pitem->lpecdobject,
                    (pitem->wType == IDM_PASTE) ? cfOwnerLink : cfLink,
                    &hstr))
                    == ECD_OK)
                && hstr)
                {
                // Both link formats are:  "szClass szDocument szItem 0"
                lpstrInfo = GlobalLock(hstr);
					 lpstr1 = lpstrInfo;
					 hWinTitle = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, 512);
					 pTitle = pTitle1 = (LPSTR)LocalLock(hWinTitle);
					if (pitem->wType == IDM_PASTE){
						lstrcpy(pTitle, "Embed!");
						pTitle += 6;
						}
					else{
						lstrcat(pTitle, "Link!");
						pTitle += 5;
						}
					 while (*lpstr1){
						*(pTitle++) = *(lpstr1++);
						}
					 *(pTitle++) = '!';
					 lpstr1 += 1;
					 while (*lpstr1)
						*(pTitle++) = *(lpstr1++);
					 *(pTitle++) = '!';
					 lpstr1 += 1;
					 while (*lpstr1)
						*(pTitle++) = *(lpstr1++);
					 *pTitle = '!';
					 lpstr1 += 1;
					

                SetWindowText(hwnd, (LPSTR)pTitle1);
					 LocalUnlock(hWinTitle);
					LocalFree(hWinTitle);
            	 GlobalUnlock(hstr);
                }
					}

void GetUndo(PITEM pitem){
	PITEM pObjectUndo;

	pObjectUndo = (PITEM)LocalLock(hObjectUndo);
	pitem->wType = pObjectUndo->wType ;						
	pitem->wUpdate = pObjectUndo->wUpdate ;						
	pitem->lpecdobject = pObjectUndo->lpecdobject;						
	LocalUnlock(hObjectUndo);						
	LocalFree(hObjectUndo);						
	hObjectUndo = 0;						
	}
							
ECDSTATUS CopyLink(PITEM pCopy){
	 HWND hwnd;
    HANDLE hitem;
    PITEM pitem;
	ECDSTATUS EcdStat;
	int i = 0;

	hwnd = GetNextWnd(&i);
   hitem = GetWindowWord(hwnd, 0);
   pitem = (PITEM) LocalLock(hitem);
	EcdStat = EcdCopyFromLink(pitem->lpecdobject, PROTOCOL, lpclient,NULL, NULL, (LPECDOBJECT FAR *) &(pCopy->lpecdobject));
	LocalUnlock(hitem);
	return EcdStat;
}

VOID WaitForRelease(PITEM pitem){
   MSG msg;
 
	fEcdrelease = FALSE;
	do  {								
	GetMessage(&msg, NULL, NULL, NULL);								
   TranslateMessage(&msg);     						   
   DispatchMessage(&msg);      						   
   }
while (!fEcdrelease);
}


HWND GetTopChild(){
	HWND  hwnd;
	int i = 0;
	
	hwnd = GetTopWindow(hwndMain);

	while ((hwnd != rgchChildWnd[i]) && i++ < irgchChild)
		;
	if (i <= irgchChild) // if it was found
		return hwnd;
	// else find the first non null guy
	i = 0;
	while (!(hwnd = rgchChildWnd[i]) && i++ < irgchChild)
		;
		return hwnd;
}

HWND GetNextWnd(LPINT i){
	HWND hwnd;
	while (!(hwnd = rgchChildWnd[*i]) && *i < irgchChild)
	  *i += 1;
	*i += 1;
	return hwnd;
}


BOOL fTestEcdStat(ECDSTATUS EcdStat){
	// use it for Async conditions

	switch(EcdStat){
		case ECD_WAIT_FOR_RELEASE:
			if (fEcdReleaseError){
				PrintError(fEcdReleaseStat);
				return TRUE;
				}
			else
				return FALSE;
			break;
		case ECD_OK:
			return FALSE;
		default:
			PrintError(EcdStat);
			return TRUE;
			}
		}


BOOL Insert(HWND hwnd)
{
    FARPROC lpProc;
    int     fh;


    // Ask for a file name
    lpProc = MakeProcInstance(OpenDlgProc, hInst);
    fh = DialogBox(hInst, "Open", hwnd, lpProc);
    FreeProcInstance(lpProc);
	if (fh > 0)
		_lclose(fh); // because the file is opened in Open
    return (fh);

}


VOID TestEqual(){ // test the top and next object are equal
	HWND hTop, hNext;
	HANDLE hitem1, hitem2;
	PITEM pitem1, pitem2;
	ECDSTATUS EcdStat;
	int i = 0;

	hTop = GetTopChild();
	hitem1 = GetWindowWord(hTop, 0);
	pitem1 = (PITEM)LocalLock(hitem1);

	hNext = GetNextWnd(&i);
	hitem2 = GetWindowWord(hNext, 0);
	pitem2 = (PITEM)LocalLock(hitem2);

	EcdStat = EcdEqual(pitem1->lpecdobject, pitem2->lpecdobject);
	if (EcdStat == ECD_WAIT_FOR_RELEASE)
		WaitForRelease(pitem1);
	if (fTestEcdStat(EcdStat))
		PrintError(EcdStat);
}





//---------------------------------------------------------------------------
// cldemo.h
//
// Copyright (c) Microsoft Corporation, 1990-
//---------------------------------------------------------------------------

extern HWND     hwndMain;
extern WORD     cfLink, cfOwnerLink, cfNative;
extern BOOL     fDirty;

#define szAppName      "Client"
#define szHostname     szAppName
#define szHostobjectname ""

#define IDM_NEW        0x0100
#define IDM_OPEN       0x0101
#define IDM_SAVE       0x0102
#define IDM_SAVEAS     0x0103
#define IDM_EXIT       0x0104
#define IDM_INSERT     0x0105
#define IDM_INSERTFILE 0x0106
#define IDM_CLOSE      0x0107
#define IDM_PRINT      0x0108

#define IDM_SHAPES	  0x0201
#define IDM_PBRUSH	  0x0202
#define IDM_EXCEL		  0x0203
#define IDM_GRAPH		  0x0204
#define IDM_TORUS		  0x0205



#define IDM_UNDO       0x0110 // try something else
#define IDM_CUT        0x0111
#define IDM_COPY       0x0112
#define IDM_PASTE      0x0113
#define IDM_LINK       0x0114
#define IDM_STATIC     0x0115
#define IDM_CLEAR      0x0116
#define IDM_PROPERTIES 0x0117
#define IDM_RECONNECT  0x0118
#define IDM_COPYLINK  0x0119
#define IDM_EQUAL		  0x011a

#define IDM_ABOUT      0x0120

#define IDD_MANUAL     0x0130
#define IDD_AUTO       0x0131
#define IDD_FREEZE     0x0132
#define IDD_UPDATE     0x0133
#define IDD_EDIT       0x0134
#define IDD_WHAT       0x0135
#define IDD_WHICH      0x0136
#define IDD_LINKINFO   0x0137
#define IDD_DOCNM		   0x0138
#define IDD_ITEMNM   0x0139

#define IDC_FILENAME   0x0100
#define IDC_EDIT       0x0101    
#define IDC_PATH       0x0102
#define IDC_FILES      0x0103
#define IDC_LISTBOX    0x0104

HWND CreateItemWindow (HWND, BOOL, LPRECT);

long far PASCAL EditWndProc(HWND hwnd, unsigned message, WORD wParam, LONG lParam);

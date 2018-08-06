//---------------------------------------------------------------------------
// client.h
//
// Copyright (c) Microsoft Corporation, 1990-
//---------------------------------------------------------------------------

#define PROTOCOL 	((LPSTR)"StdFileEditing")
#define	SPROTOCOL	((LPSTR)"Static")

typedef struct _APPSTREAM {
    LPECDSTREAMVTBL    lpstbl;
    HANDLE          hobjStream;         // stream handle
    int             fh;                 // file handle
} APPSTREAM, FAR *LPAPPSTREAM;

typedef struct _ITEM {
    LPECDOBJECT lpecdobject;
    WORD        wType;
    WORD        wUpdate;  
    HANDLE      hitem;
	 BOOL			fOpen; // is object editor open
	 BOOL			fClose; //is object editor connection closed so Reconnect
} ITEM, *PITEM;

extern LPECDCLIENT lpclient;
extern LPAPPSTREAM lpstream;

BOOL InitClient (HANDLE);
void TermClient (void);
void ClientUpdateMenu (HWND, HMENU);
void SetFile (int);
void ClientPasteLink (HWND, WORD);
void ClientCutCopy (HWND, WORD);
void ClientPaint (HWND, HDC, PITEM);
void ClientEdit (HWND, PITEM);
void ClientDelete(PITEM);
void ClientCreateNewItem (HWND, BOOL, LPRECT, LPECDOBJECT, WORD, WORD);

BOOL FAR PASCAL PropertiesDlgProc(HWND, unsigned, WORD, LONG);

int FAR PASCAL CallBack(LPECDCLIENT, int, LPECDOBJECT);
LONG FAR PASCAL ReadStream(LPAPPSTREAM, LPSTR, LONG);
LONG FAR PASCAL WriteStream(LPAPPSTREAM, LPSTR, LONG);
LONG FAR PASCAL PosStream(LPAPPSTREAM, LONG);
WORD FAR PASCAL StatusStream(LPAPPSTREAM);


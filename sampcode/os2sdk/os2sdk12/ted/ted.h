/***************************************************************************

   TED.H

   Created by Microsoft Corporation, IBM Corporation 1989

****************************************************************************/









#define ID_RESOURCE    0x100     /* Resource identifier */

/* Submenu IDs */

#define IDM_FILE 0x100
#define IDM_EDIT 0x200
#define IDM_OPT  0x300
#define IDM_HELP 0x400

/* Message IDs */

#define IDM_NEWFILE     0x101
#define IDM_OPENFILE    0x105
#define IDM_SAVE        0x110
#define IDM_SAVEAS	0x115

#define IDM_UNDO        0x201
#define IDM_CUT 	0x205
#define IDM_COPY	0x210
#define IDM_PASTE	0x215
#define IDM_FIND        0x220
#define IDM_SELECT_ALL  0x225


#define IDM_SETWRAP	0x301
#define IDM_QUERYLINES	0x305


#define IDM_DISPLAY_HELP 0x401
#define IDM_EXT_HELP	 0x405
#define IDM_KEYS_HELP	 0x410
#define IDM_INDEX_HELP	 0x415
#define IDM_ABOUT        0x420





/***************************************
   Function prototypes
 ***************************************/

MRESULT EXPENTRY ClientWndProc	(HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY FrameWndProc	(HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY FindDlgProc	(HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY AboutDlgProc   (HWND, USHORT, MPARAM, MPARAM) ;

VOID MLESaveFile( HWND, PSZ, PSZ );
VOID MLEOpenFile( HWND, PSZ, PSZ );
VOID MLEInit( HWND );
VOID MLESetWrap( HWND, HWND, BOOL );
VOID  MLEEnable ( HWND );
VOID  MLEDisable( HWND );
BOOL  MLEDeleteContents( HWND );
BOOL  MLESaveToFile ( HFILE );
VOID  TEDInitDlgSysMenu( HWND );
BOOL  TEDDisplayErrorID( USHORT );
VOID  TEDInitFileName ( VOID );
VOID  TEDCleanupExit  ( VOID );
VOID  TEDUpdateTitleBar( HWND, PSZ, PSZ );


/*  Some macros */


/**************************************************************************
    Note: "TED Notification" should be loaded from a resource string, but
          since this is only a sample it was left this way for clarity.
**************************************************************************/

#define MESSAGE(x) WinMessageBox(HWND_DESKTOP, \
			 WinQueryActiveWindow( HWND_DESKTOP, FALSE), \
			 x, \
                         "TED Notification", \
                         0, \
			 MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL | MB_MOVEABLE)




/**********************
      Buffer Allocation
***********************/


/*************************************************************************
    Define the maximum size of the MLE, and the buffer to allocate when
    we EXPORT/IMPORT data to it.
*************************************************************************/


#define TXT_LIMIT 30000   /* NOTE: this should never exceed 64000 without */
                          /* rewriting key parts of the application       */
                          /* DosRead / DosWrite etc. !!!!!!!!!!!!!!!!!!!! */


#define MAX_STRING_SIZE  256 /* Max size of strings */

/*
** String ID's
*/

#define SID_HERR_NO_INIT                 0x200
#define SID_UNABLE_TO_CREATE_FRAME       0x201
#define SID_UNABLE_TO_SUB_FRAME          0x202
#define SID_UNABLE_TO_SUB_CLIENT         0x203
#define SID_UNABLE_TO_SET_FRAME_OWNER    0x204
#define SID_MLEMSG_TEXTOVERFLOW          0x205
#define SID_MLEMSG_MLN_OVERFLOW          0x206
#define SID_DLG_CANT_LOAD_FIND           0x207
#define SID_DLG_CANT_LOAD_ABOUT          0x208
#define SID_DLG_FIND_CANT_DISMISS        0x209
#define SID_TED_UNTITLED                 0x210
#define SID_UNABLE_TO_EXPORT             0x211
#define SID_CANT_ALLOCATE_MEMORY         0x212



#define SID_UNABLE_TO_SET_NEWSIZE        0x213
#define SID_DOSWRITE_FAILED              0x214
#define SID_WRONG_BYTE_COUNT             0x215
#define SID_DOSQFILEINFO_FAILED          0x216
#define SID_FILE_TOO_LARGE               0x217
#define SID_DOS_READ_FAILED              0x219
#define SID_CANT_SET_WRAP                0x220
#define SID_CANT_OPEN_EX_FILE            0x221
#define SID_CANT_FIND                    0x222

#define SID_HELP_CANT_CREATE_INST        0x500
#define SID_HELP_CANT_ASSOC_INST         0x501
#define SID_HELP_HLP_ERROR               0x502



#define SID_HERR_NO_LIBRARY              0x600
#define SID_HERR_TITLE                   0x601
#define SID_HERR_UNKNOWN                 0x602
#define SID_HERR_MEMORY                  0x603
#define SID_HERR_LIB_ERROR               0x604

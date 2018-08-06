/****** Resource IDs *****/

#define IDR_MDI             1
#define IDR_MDIDOC          2
#define	IDD_ABOUT	    3

/****** Menu command IDs *****/

#define ID_DUMMY    -126

/* File menu */
#define IDM_FILE            -1
#define CMD_NEW         0x0100
#define CMD_OPEN        0x0101
#define CMD_CLOSE       0x0102
#define CMD_SAVE        0x0103
#define CMD_SAVEAS      0x0104
#define CMD_SAVEENV     0x0105
#define CMD_PAGESETUP   0x0106
#define CMD_PRINT       0x0107
#define CMD_PRINTSETUP  0x0108
#define CMD_EXIT        0x0109
#define	CMD_ABOUT	0x010a

/* Edit menu */
#define IDM_EDIT            -2
#define CMD_UNDO        0x0200
#define CMD_REDO        0x0201
#define CMD_CUT         0x0202
#define CMD_COPY        0x0203
#define CMD_PASTE       0x0204
#define CMD_CLEAR       0x0205
#define CMD_SELECT      0x0206
#define CMD_SELECTALL   0x0207

/* Window menu */
#define IDM_WINDOW              -9
#define CMD_ARRANGETILED    0x0900
#define CMD_ARRANGECASCADED 0x0901
#define CMD_WINDOWITEMS     0x0902

/* Help menu */
#define IDM_HELP           -10


#define AWP_TILED     1
#define AWP_CASCADED  2

/*
 * Style flags for the DOC structure.
 */
#define DS_HORZSPLITBAR     0x0001  /* Document has horizontal splitbar.   */
#define DS_VERTSPLITBAR     0x0002  /* Document has vertical splitbar.     */

/*
 * State flags for the DOC structure.
 */
#define DF_SPLITHORZ        0x0001  /* Document is split horzontally.      */
#define DF_SPLITVERT        0x0002  /* Document is split vertically.       */
#define DF_HSPLITOVERFLOW   0x0004  /* Window is smaller than split area.  */
#define DF_VSPLITOVERFLOW   0x0008  /* Window is smaller than split area.  */

typedef struct _DOC {   /* doc */
    USHORT  idMI;           /* The ID of the menu item on the Window menu. */
    ULONG   clrBackground;  /* The background color of the document.       */
    USHORT  fsStyle;        /* Style flags for the document.               */
    USHORT  fs;             /* State flags for the document.               */
    HWND    hwndFrame;      /* The frame window for this document.         */
    SHORT   cxVertSplitPos; /* x position of the vertical splitbar.        */
    SHORT   cyHorzSplitPos; /* x position of the horizontal splitbar.      */
    HWND    hwndSysMenu;
    HWND    hwndMinmax;
    struct _DOC NEAR *npdocNext;    /* Next document in the application.   */
    struct _VIEW NEAR *npviewFirst; /* The first in the linked list of     */
} DOC;                              /*   views for this document.          */
typedef DOC NEAR *NPDOC;

typedef struct _VIEW {  /* view */
    SHORT   xOrigin;        /* The amount to offset by in the x direction. */
    SHORT   yOrigin;        /* The amount to offset by in the y direction. */
    USHORT  fs;             /* Flags for this view.                        */
    HWND    hwndClient;     /* The window handle for this view.            */
    NPDOC   npdoc;          /* The document this view is representing.     */
    struct _VIEW NEAR *npviewNext;  /* The next view for the document.     */
} VIEW;
typedef VIEW NEAR *NPVIEW;

/*
 * Macros for DOC/VIEW structures.
 */
#define NPDOCFROMCLIENT(hwndClient)  (((NPVIEW)WinQueryWindowUShort(\
        hwndClient, QWS_USER))->npdoc)

#define NPVIEWFROMCLIENT(hwndClient)  ((NPVIEW)WinQueryWindowUShort(\
        hwndClient, QWS_USER))

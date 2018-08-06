/*****************************************************************

	dispatch.h

*******************************************************************/

#ifdef MAC /*----------------------------------------------------*/

#define R_ID_WINDOW 	128

#define ErrorAlert		256
#define	AdviseAlert		257

/* resource IDs of menu categories */
#define R_ID_SYS_MENU		128
#define R_ID_FILE_MENU		129
#define R_ID_EDIT_MENU		130
#define R_ID_VIEW_MENU		131
#define R_ID_PALETTE_MENU	132

#define NUM_MENU_CATEGORIES	5

/* Index values for menu categories */

#define SYS_MENU		0
#define FILE_MENU		1
#define EDIT_MENU		2
#define VIEW_MENU		3
#define PALETTE_MENU		4

/* values for menu items for FILE MENU*/
#define FILE_CMD_NEW		1
#define FILE_CMD_OPEN		2

#d­fine FILE_CMD_CLOSE		4
#define FILE_CMD_SAVE		5
#define FILE_CMD_SAVE_AS	6
#define FILE_CMD_REVERT		7

#define FILE_CMD_PAGE_SETUP	9
#define FILE_CMD_PRINT		10

#define FILE_CMD_QUIT		12

/*values for menu items for EDIT MENU */
#define EDIT_CMD_UNDO 	1
#define EDIT_CMD_CUT	3
#define EDIT_CMD_COPY	4
#define EDIT_CMD_PASTE	5
#define EDIT_CMD_CLEAR	6

/* values for menu items for VIEW MENU */
#define VIEW_CMD_GRAPHICS_VIEW	1
#define VIEW_CMD_TEXT_VIEW		2

/* values for menu items for PALETTE MENU */
#define PALETTE_SELECTION_MODE		1
#define PALETTE_CREATE_RECT_MODE	2

#else /*----------------------------------------------------*/

/* resource IDs of menu categories */
#define R_ID_SYS_MENU		100
#define R_ID_FILE_MENU		200
#define R_ID_EDIT_MENU		300
#define R_ID_VIEW_MENU		400
#define R_ID_PALETTE_MENU	500

#define SYS_MENU		100
#define FILE_MENU		200
#define EDIT_MENU		300
#define VIEW_MENU		400
#define PALETTE_MENU		500

/* values for menu items for FILE MENU*/
#define FILE_CMD_NEW		1
#define FILE_CMD_OPEN		2
#define FILE_CMD_CLOSE		3
#define FILE_CMD_SAVE		4
#define FILE_CMD_SAVE_AS	5
#define FILE_CMD_REVERT		6
#define FILE_CMD_PAGE_SETUP	7
#define FILE_CMD_PRINT		8
#define FILE_CMD_QUIT		9

/*values for menu items for EDIT MENU */
#define EDIT_CMD_UNDO 	1
#define EDIT_CMD_CUT	2
#define EDIT_CMD_COPY	3
#define EDIT_CMD_PASTE	4
#define EDIT_CMD_CLEAR	5

/* values for menu items for VIEW MENU */
#define VIEW_CMD_GRAPHICS_VIEW		1
#define VIEW_CMD_TEXT_VIEW		2

/* values for menu items for PALETTE MENU */
#define PALETTE_SELECTION_MODE		1
#define PALETTE_CREATE_RECT_MODE	2

#endif /*----------------------------------------------------*/

/*>>>>>
#define aaSave		1
#define aaDiscard	2
#define aaCancel	3

#define SBarWidth	15
<<<<<<*/



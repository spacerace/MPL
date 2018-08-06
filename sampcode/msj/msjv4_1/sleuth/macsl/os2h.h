/*-----------------------------------------------------------------*/
/* Os2h.h                                                          */
/* OS/2 header file for MacPM.  This plus MyOs2.c are used to      */
/* create the precompiled header Os2.h.                            */
/*-----------------------------------------------------------------*/

#include "WhichSys.h"

#include "Os2def.h"

#define INCL_DOSPROCESS
#define INCL_GPICONTROL
#define INCL_GPILCIDS
#define INCL_GPIPRIMITIVES
#define INCL_WINBUTTONS
#define INCL_WINCOMMON
#define INCL_WINENTRYFIELDS
#define INCL_WINFRAMECTLS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINLISTBOXES
#define INCL_WINMENUS
#define INCL_WINMESSAGEMGR
#define INCL_WINRECTANGLES
#define INCL_WINSCROLLBARS
#define INCL_WINSTATICS
#define INCL_WINSYS
#define INCL_WINWINDOWMGR

#include "BseDos.h"
#include "PmWin.h"
#include "PmGpi.h"

/*-----------------------------------------------------------------*/

/*************************************************************************\
* DDESPY.H
\*************************************************************************/

#define INCL_WIN
#define INCL_DOSNLS
#include "os2.h"
#include "ddeml.h"
#include "dialog.h"

#define IW_FIRST        0
#define IW_APPSTITLE    0
#define IW_TOPICSTITLE  1
#define IW_ITEMSTITLE   2
#define IW_DATATITLE    3
#define IW_APPSLBOX     4
#define IW_TOPICSLBOX   5
#define IW_ITEMSLBOX    6
#define IW_DATATEXT     7
#define IW_LAST         7

#define IDR_MAIN        1

#define IDM_REFRESH     10
#define IDM_SETTIMEOUT  20
#define IDM_GETTIMEOUT  30
#define IDM_EXEC        40
#define IDM_OPTIONS     50

#define ENHAN_ENTER     WM_USER

#define SZINDETERMINATE "Unknown"

#define MAX_QSTRING     100L

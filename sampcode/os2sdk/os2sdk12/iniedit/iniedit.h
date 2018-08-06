/* include res file definitions */
#include "inires.h"

/****************************** Global Defines ******************************/

#define MAX_STRING_LEN	 132
#define MAX_APP_NAMES	 256
#define APP_FORM	   0			/* Display Formats */
#define ALL_FORM	   1

#define SZALL	 "Show All Entries"
#define SZAPP	 "Show Application Names"


/*********************** Common Function Declarations ***********************/

void ErrMessage( PCHAR );
VOID FindNext( VOID );
VOID PrintThread( VOID );

MRESULT _loadds EXPENTRY SearchWndProc( HWND, USHORT, MPARAM, MPARAM );
MRESULT _loadds EXPENTRY AddKeyWndProc( HWND, USHORT, MPARAM, MPARAM );
MRESULT _loadds EXPENTRY DelAppWndProc( HWND, USHORT, MPARAM, MPARAM );
MRESULT _loadds EXPENTRY DelKeyWndProc( HWND, USHORT, MPARAM, MPARAM );
MRESULT _loadds EXPENTRY ChangeKeyWndProc( HWND, USHORT, MPARAM, MPARAM );


/****************************** Type Definitions ****************************/

typedef struct
    {
    CHAR    szKey[MAX_STRING_LEN];
    CHAR    szValue[MAX_STRING_LEN];
    }	PAIRSTRUCT;
typedef PAIRSTRUCT* PPAIRSTRUCT;


typedef struct
    {
    CHAR	 szAppName[MAX_STRING_LEN];
    USHORT	 cKeys;
    PPAIRSTRUCT  pPairs;
    }	GROUPSTRUCT;
typedef GROUPSTRUCT* PGROUPSTRUCT;

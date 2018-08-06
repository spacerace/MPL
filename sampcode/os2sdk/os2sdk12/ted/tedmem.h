/****************************************************************************

    Tedmem.h

    Created by Microsoft Corporation, IBM Corporation 1989

------------------------------------------------------------------------------

    Function Prototypes for tedmem.c functions.  This file is designed to
    be a virtual memory allocate / Free mechanism which is independent
    of the Selector based memory allocation currently in use

*****************************************************************************/



/* Typedef's */

typedef ULONG APIRET;




/* Function prototypes */

APIRET APIENTRY MyAllocMem( PVOID * , ULONG , ULONG ) ;
APIRET APIENTRY MyFreeMem ( PVOID );

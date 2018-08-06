/***************************************************************************

    Tedmem.c

    Created by Microsoft Corporation, IBM Corporation 1989

----------------------------------------------------------------------------

        This file contains the functions nessesary to allocate and free
        memory blocks.  It is implemented to return pointers and thus
        does not tie the application to Selector/Offsets etc.

***************************************************************************/


#define INCL_DOS

#include <os2.h>
#include "tedmem.h"




/***************************************************************************

    MyAllocMem - This procedure allocates a buffer of a requested length.

        Return - FALSE if succesful otherwise TRUE

***************************************************************************/


APIRET APIENTRY MyAllocMem( PVOID *ppch,    /* Pointer to memory        */
                            ULONG usBytes,  /* Number of bytes to alloc */
                            ULONG ulDum1  ) /* Dummy                    */
{

    APIRET apiRetVal;                       /* Return Value (FALSE = success*/
    PCHAR pchTemp;                          /* Temp ptr used to alloc mem   */


    if ( (apiRetVal = (APIRET) DosAllocSeg( (USHORT) usBytes,
                                            &SELECTOROF( pchTemp ),
			                    0 ) ) == (APIRET) 0 )

    {

    /* Success!! */

        OFFSETOF( pchTemp ) = 0;            /* Set offset of selector to 0  */
        *ppch = pchTemp;                    /* Set passed in ptr to new mem */
    }


    return ( apiRetVal );

}



/***************************************************************************

    MyFreeMem - This procedure frees a buffer that was previously allocated
                with MyAllocMem.

       Return - FALSE if succesful otherwise TRUE

***************************************************************************/


APIRET APIENTRY MyFreeMem( PVOID pvTemp )
{

    return ( APIRET) DosFreeSeg( SELECTOROF( pvTemp ) );

}

/******************************* Module Header ******************************\
* Module Name: Print.c
*
*
* Created by Microsoft Corporation, 1989
*
*
* PM OS2.ini Editor
*
* Printing Functions
*
\***************************************************************************/


#define LINT_ARGS
#define INCL_WINSHELLDATA
#define INCL_GPIBITMAPS
#define INCL_GPIREGIONS
#define INCL_GPILCIDS
#define INCL_GPIPRIMITIVES
#define INCL_DEV

#include <string.h>
#include <stdio.h>

#include <os2.h>

#include "noncomm.h"
#include "IniEdit.h"


/******************************* Constants *********************************/

#define LEFT_MARGIN      3
#define TOP_MARGIN       7
#define BOTTOM_MARGIN   15
#define	MAX_PRINT_LEN	1024


/******************************* Externals *********************************/

extern USHORT        cAppNames;
extern PGROUPSTRUCT  pGroups;
extern HAB           habIniEdit;
extern USHORT        usLineHeight;
extern USHORT        usPrintFormat;

/******************************* Internals ********************************/
CHAR	szBuf[MAX_STRING_LEN];			// Output character buffer
CHAR	szPrinter[MAX_PRINT_LEN];		// Logical Printer Name
CHAR	szDetails[MAX_PRINT_LEN];		// Logical Printer Details


/***************************** Function Header *****************************\
*
* PrintThread
*
*
* Prints the ini info in specified format
*
\***************************************************************************/

VOID PrintThread()
{
    HAB      habPrint;                          // HAB of Print Thread
    HDC      hdcPrint;                          // DC of Printer
    HPS      hpsPrint;                          // PS of Printer
    SIZEL    sizel;                             // Size of PS to Create
    POINTL   ptlOutput;                         // Output location
    BYTE     abOut[50];                         // DevEscape Output
    ULONG    ulOut;                             // DevEscape Output Count
    USHORT   usHeight;                          // Height of Printer Page
    USHORT   cch;                               // Count of Characters in buffer
    USHORT   i,j;                               // Loop Counters
    DEVOPENSTRUC  dop;                          // DevOpenDC Info
    CHAR     szMessage[80];

    /* initialization of thread */
    if (!(habPrint = WinInitialize(0))) DosExit(EXIT_THREAD, -1);


    /*** Select Port ***/
    /* get name of default logical printer */
    cch = WinQueryProfileString(habPrint, "PM_SPOOLER", "PRINTER",
                           "",                /* default  */
                           szPrinter,
                           MAX_PRINT_LEN);    /* max chars */

    szPrinter[cch-2] = 0;                     /* remove terminating ';' */

    /* get specifics of default logical printer */
    cch = WinQueryProfileString(habPrint, "PM_SPOOLER_PRINTER", szPrinter,
                           "",                /* default  */
                           szDetails,
                           MAX_PRINT_LEN);    /* max chars */


    /* info in form of "port; driver; logical address" */
    /* grab driver name; one past semicolon */
    dop.pszDriverName = strchr( szDetails, ';' )+1;

    /* Grab logical address */
    dop.pszLogAddress = strchr( dop.pszDriverName, ';' )+1;

    /* Make driver and Logical address Null terminated */
    dop.pszLogAddress = strtok( dop.pszLogAddress, ",;" );
    dop.pszDriverName = strtok( dop.pszDriverName, ",.;" );  /* Driver.Model */

    dop.pdriv = NULL;
    dop.pszDataType = NULL;

    /* Create Printer DC */
    hdcPrint = DevOpenDC( habPrint, OD_QUEUED, "*", 3L, (PDEVOPENDATA)&dop,
            (HDC)NULL);

    if( hdcPrint == (HDC)NULL )
        {
	DosExit(EXIT_THREAD, -1);
        }

    strcpy( szBuf, (usPrintFormat == APP_FORM) ? SZAPP : SZALL );
    DevEscape( hdcPrint, DEVESC_STARTDOC, (LONG)strlen( szBuf ), (PBYTE)szBuf,
            &ulOut, (PBYTE)NULL );

    /* Determine size of page */
    DevQueryCaps( hdcPrint, CAPS_HEIGHT, 1L, (PLONG)&usHeight );
    ptlOutput.y = usHeight - TOP_MARGIN - usLineHeight;
    ptlOutput.x = LEFT_MARGIN;

    /* Create Printer PS */
    sizel.cx = sizel.cy = 0L;
    hpsPrint = GpiCreatePS( habPrint, hdcPrint, &sizel,
            PU_PELS | GPIF_DEFAULT | GPIT_NORMAL | GPIA_ASSOC );

    if( hpsPrint == (HPS)NULL )
        {
	DosExit(EXIT_THREAD, -1);
        }


    /* print items */
    if( usPrintFormat == APP_FORM )
        {
        for( i=0; i<cAppNames; i++ )
            {
            sprintf(szMessage,"CharString: X = %ld,  Y = %ld",
            					ptlOutput.x, ptlOutput.y);
            GpiCharStringAt( hpsPrint, &ptlOutput, (LONG)strlen( pGroups[i].szAppName ),
                    pGroups[i].szAppName );
            ptlOutput.y -= usLineHeight;

            if( ptlOutput.y <= BOTTOM_MARGIN )
                {
                DevEscape( hdcPrint, DEVESC_NEWFRAME, 0L, (PBYTE)NULL , &ulOut, abOut );
                ptlOutput.y = usHeight - TOP_MARGIN - usLineHeight;
                }

            }
        }  /* if */
    else
        {
        /* Print App Names */
        for( i=0; i<cAppNames; i++ )
            {
            GpiCharStringAt( hpsPrint, &ptlOutput, (LONG)strlen( pGroups[i].szAppName ),
                    pGroups[i].szAppName );
            ptlOutput.y -= usLineHeight;
            if( ptlOutput.y <= BOTTOM_MARGIN )
                {
                DevEscape( hdcPrint, DEVESC_NEWFRAME, 0L, (PBYTE)NULL , &ulOut, abOut );
                ptlOutput.y = usHeight - TOP_MARGIN - usLineHeight;
                }

            /* Print Key Value Pairs for current App Name */
            for( j=0; j<pGroups[i].cKeys; j++ )
                {
                sprintf( szBuf, "   %s: %s", pGroups[i].pPairs[j].szKey,
                        pGroups[i].pPairs[j].szValue );

                if( GpiCharStringAt( hpsPrint, &ptlOutput, (LONG)strlen( szBuf ), szBuf ) == GPI_ERROR )
			;

                ptlOutput.y -= usLineHeight;
                if( ptlOutput.y <= BOTTOM_MARGIN )
                    {
                    DevEscape( hdcPrint, DEVESC_NEWFRAME, 0L, (PBYTE)NULL , &ulOut, abOut );
                    ptlOutput.y = usHeight - TOP_MARGIN - usLineHeight;
                    }
                }  /* for */
            }  /* for */

        }

    /* Cleanup */
    DevEscape( hdcPrint, DEVESC_ENDDOC, 0L, (PBYTE)NULL , &ulOut, abOut );
    GpiAssociate( hpsPrint, (HDC)NULL );
    GpiDestroyPS( hpsPrint );
    DevCloseDC( hdcPrint );
    DosExit(EXIT_THREAD, 0);
}  /* PrintThread */

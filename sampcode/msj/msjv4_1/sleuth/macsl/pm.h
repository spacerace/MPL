/****************************** Module Header ******************************\
*
* Module Name: PM.H
*
* This is the top level include file that includes all the files necessary
* for writing a Presentation Manager application.
*
* History:
*  24-Sep-87 MPerks    Created
*  14-Nov-87 MPerks    DCR23505
*  19-Nov-87 MPerks    taken out conditions
*                      1) AVIO needs FONTMETRICS/FATTRS from GPI
*                      2) HMF shared between DEV and GPI
*                      3) DRIVDATA shared between DEV and SPL
*   3-Dec-87 MPerks    implemented name change
*
* Copyright (c) 1987  Microsoft Corporation
* Copyright (c) 1987  IBM Corporation
*
* ===========================================================================
*
* The following symbols are used in this file for conditional sections.
*
*   INCL_PM     -  ALL of OS/2 Presentation Manager
*   INCL_WIN    -  OS/2 Window Manager
*   INCL_GPI    -  OS/2 GPI
*   INCL_DEV    -  OS/2 Device Support
*   INCL_AVIO   -  OS/2 Advanced VIO
*   INCL_SPL    -  OS/2 Spooler
*   INCL_ERRORS -  OS/2 Errors
*
\***************************************************************************/
 
/* if INCL_PM defined then define all the symbols */
#ifdef INCL_PM
    #define INCL_WIN
    #define INCL_GPI
    #define INCL_DEV
    #define INCL_AVIO
    #define INCL_SPL
    #define INCL_ERRORS
#endif /* INCL_PM */
 
#include <pmwin.h>     /* OS/2 Window Manager definitions */
#include <pmgpi.h>     /* OS/2 GPI definitions */
#include <pmdev.h>     /* OS/2 Device Context definitions */
#ifdef INCL_AVIO
#include <pmavio.h>    /* OS/2 AVIO definitions */
#endif
#ifdef INCL_SPL
#include <pmspl.h>     /* OS/2 Spooler definitions */
#endif

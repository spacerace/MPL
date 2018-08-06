/***************************************************************************\
* DATA.C -- This file contains per process global variables
* Created by Microsoft Corporation, 1989
\***************************************************************************/

#define NO_DOS
#define NO_GPI
#include "tool.h"

/*
   This library uses a NON SHARED DATA selector.  This means each
   process using the library gets its own selector, and also that
   values cannot be shared and must be recreated for each process.
*/

HMODULE vhModule;            /* Library module handle */
HHEAP  vhheap;               /* Library heap */

PSTR   vrgsz[CSTRINGS];      /* Array of pointer to our strings (indexed
                                by IDS_... */

/*-------------------------------------------------------------------
  PERROR.C -- PBX error message routines

  Description:

  This file contains the function used by PBXSRV.EXE to report
  errors into the LAN Manager error log.

  Author:  Brendan Dixon
           Microsoft, inc.
           LAN Manager Developer Support

  This code example is provided for demonstration purposes only.
  Microsoft makes no warranty, either express or implied,
  as to its usability in any given situation.
-------------------------------------------------------------------*/

// Includes ---------------------------------------------------------
#define   INCL_DOS
#define   INCL_DOSERRORS
#include  <os2.h>

#define   INCL_NETERRORLOG
#define   INCL_NETSERVICE
#define   INCL_NETERRORS
#include  <lan.h>

#include  <stdio.h>
#include  <string.h>

#include  "pbxsrv.h"

// Function prototypes ----------------------------------------------
void AddStr(CHAR **ppbBuf, CHAR *pszStr);


/* AddStr -----------------------------------------------------------
  Description:  Add a string into a buffer of strings.
  Input      :  ppbBuf -- Pointer to buffer pointer
                pszStr -- Pointer to string to add
  Output     :  None
-------------------------------------------------------------------*/
void AddStr(CHAR **ppbBuf, CHAR *pszStr)
{
  while (*(*ppbBuf)++ = *pszStr++);
  return;
}


/* ErrorRpt ---------------------------------------------------------
  Description:  Write an error to the LAN Manager error log.
  Input      :  pszMsg ---- Error message
                pszFile --- File name (e.g., PBXSRV.C, ROUTER.C)
                usLine ---- Line number of error report call
                usRC   ---- Optional return code (0 is ignored)
                esSev  ---- Error message severity
  Output     :  None
-------------------------------------------------------------------*/
void ErrorRpt(PSZ    pszMsg,
              PSZ    pszFile,
              USHORT usLine,
              USHORT usRC,
              ERRSEV esSev)
{
  CHAR    cTmp[80];                       // Temporary buffer
  CHAR    cArgs[255];                     // Error strings
  CHAR    *pcBuf;                         // Pointer to buffer

  // Build the error message
  pcBuf = cArgs;
  AddStr(&pcBuf, "MSJ ");                 // System name
  AddStr(&pcBuf, "PBX Service ");         // Service name

  switch (esSev) {                        // Severity
    case Error        : AddStr(&pcBuf, "Error ");         break;
    case Warning      : AddStr(&pcBuf, "Warning ");       break;
    case Informational: AddStr(&pcBuf, "Informational "); break;
  }

  sprintf(cTmp, "File: %s ",    pszFile); // OEM Sub-id and data
  AddStr(&pcBuf, cTmp);
  sprintf(cTmp, "Line: %u ",    usLine);
  AddStr(&pcBuf, cTmp);
  sprintf(cTmp, "Message: %s ", pszMsg);
  AddStr(&pcBuf, cTmp);

  // Include any non-zero return code
  if (usRC != NO_ERROR) {
    sprintf(cTmp, "Return Code: %u ", usRC);
    AddStr(&pcBuf, cTmp);
  }
  else
    AddStr(&pcBuf, " ");

  AddStr(&pcBuf, " ");
  AddStr(&pcBuf, " ");

  // Write error to log file
  NetErrorLogWrite(NULL,                  // Always NULL
                   NELOG_OEM_Code,        // Internal error
                   "PBX Service",         // Component
                   NULL,                  // No Raw data
                   0,
                   cArgs,                 // Error messages
                   9,                     // Nine strings
                   NULL);                 // Always NULL

  return;
}

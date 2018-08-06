/*-------------------------------------------------------------------
  MAKEPIPE.C -- PBX pipe creation thread

  Description:

  This routine dynamically creates the named-pipes managed by PBX.
  It will attempt to always keep available a minimum number of
  unowned pipes for clients.  After creating a pipe, control of the
  pipe is tranferred to the PipeMgr thread by associating the pipe
  with the PipeMgr thread's pipe semaphore (via DosSetNmPipeSem).
  When a pipe is closed or connected to by a client, this thread
  needs to be notified by clearing the semMakePipe semaphore so
  an additional pipe can be created.

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

#include  <lan.h>

#include  <errno.h>
#include  <process.h>
#include  <stddef.h>
#include  <stdio.h>
#include  <string.h>

#include  "pbxsrv.h"


/* MakePipe ---------------------------------------------------------
  Description:  See file header
  Input      :  None
  Output     :  None
-------------------------------------------------------------------*/
void _cdecl MakePipe(void)
{
  USHORT  usLinesNeeded;                  // Lines to create
  USHORT  usLineNum;                      // Current line number
  USHORT  usRetCode;                      // Return code

  for (;;) {
    // Wait until more pipes need to be created
    DosSemRequest(&(pbPBXMem->semMakePipe), SEM_INDEFINITE_WAIT);

    // Check for PBX shutdown
    usRetCode = DosSemWait(&(pbPBXMem->semExit),
                           SEM_IMMEDIATE_RETURN);
    if (usRetCode)
      break;

    // Check for PBX pause
    DosSemWait(&(pbPBXMem->semPause), SEM_INDEFINITE_WAIT);

    // If all pipes have not been created and if the number
    // currently not owned by a client is less then the minimum
    // goal, create more lines
    if (pbPBXMem->usLinesCreated < pbPBXMem->usLines       &&
        (pbPBXMem->usLinesCreated-pbPBXMem->usLinesOwned)
                                   < pbPBXMem->usOpenLines  ) {

      // Determine the number of lines to create
      // It should be either the number needed such that all lines
      // are created or the number needed such that the minimum goal
      // is met, whichever is the lessor value
      usLinesNeeded =
           MIN(pbPBXMem->usLines-pbPBXMem->usLinesCreated,
               pbPBXMem->usOpenLines-
                (pbPBXMem->usLinesCreated-pbPBXMem->usLinesOwned));

      // Loop through the routing table filling available entries
      // until the number of needed lines has been created
      for (usLineNum=0;
           usLinesNeeded && usLineNum < pbPBXMem->usLines;
           usLineNum++) {

        // Is the table slot open for the allocation of a new line?
        if (pbPBXMem->abRTable[usLineNum].fState == Available) {
          // Create a new pipe instance
          // Pipe is initially created as non-blocking so that the
          // DosConnectNmPipe call will not block this thread
          usRetCode = DosMakeNmPipe(
                         PIPELINE,                // Pipe name
                         &(pbPBXMem->abRTable[usLineNum].hLine),
                         NP_ACCESS_DUPLEX  |      // Full duplex
                         NP_NOINHERIT      |      // Private
                         NP_WRITEBEHIND,          // Speedy writes
                         NP_NOWAIT         |      // Non-blocking
                         NP_READMODE_BYTE  |      // Byte mode
                         NP_TYPE_BYTE      |
                         (pbPBXMem->usLines > 254 ?
                         NP_UNLIMITED_INSTANCES   :
                         pbPBXMem->usLines),      // Number of lines
                         pbPBXMem->usLineBufSize, // Line
                         pbPBXMem->usLineBufSize, //   Buffers
                         LINEWAITTIMEOUT);        // Timeout value
          if (usRetCode) {
            ERRRPT("DosMakeNmPipe", usRetCode, Warning);
            break;
          }

          // Pipe was created successfully, finish initialization
          else {
            // Set number of read/write bytes available
            pbPBXMem->abRTable[usLineNum].usRData = 0;
            pbPBXMem->abRTable[usLineNum].usWSpace=
                                           pbPBXMem->usLineBufSize;

            // Mark the entry as open for assignment
            pbPBXMem->abRTable[usLineNum].fState  = Open;

            // Associate the line with the PipeMgr thread semaphore
            // (set the key for the line to its routing table offset)
            DosSetNmPipeSem(pbPBXMem->abRTable[usLineNum].hLine,
                            pbPBXMem->hsemPipeMgr,
                            usLineNum);

            // Place the pipe into a connect state
            usRetCode = DosConnectNmPipe(
                              pbPBXMem->abRTable[usLineNum].hLine);
            if (usRetCode                            &&
                usRetCode != ERROR_PIPE_NOT_CONNECTED ) {
              ERRRPT("DosConnectNmPipe", usRetCode, Warning);
              break;
            }

            // Set pipe to blocking mode
            // This is necessary because DosQNmPipeSemState is only
            // valid for blocking pipes
            usRetCode = DosSetNmPHandState(
                              pbPBXMem->abRTable[usLineNum].hLine,
                              NP_READMODE_BYTE | NP_WAIT);
            if (usRetCode                            &&
                usRetCode != ERROR_PIPE_NOT_CONNECTED ) {
              ERRRPT("DosSetNmPHandState", usRetCode, Warning);
              break;
            }

            // Line was successfully created and initialized,
            // adjust line counts
            usLinesNeeded--;
            DosSemRequest(&(pbPBXMem->semPBXMem),
                          SEM_INDEFINITE_WAIT);
            pbPBXMem->usLinesCreated++;
            DosSemClear(&(pbPBXMem->semPBXMem));
          }
        }
      }
    }
  }

  // PBX is in a shutdown process, exit this thread
  _endthread();
}



/* Handle to isrequest struct, previously allocated from the
nonbanked portion  of the  global heap. An appropriate place
to allocate the request block is in isconnect. */

GLOBALHANDLE hRequestBlock;

/* Table  maintained by  the C  API that  holds  the  record
length of all open ISAM files. Indexed by isfd of open file.
The table  record length  is filled in the table when a file
is opened or created. */

int iRecLenTbl[MAX_FILE_DESCRIPTORS];

/* Function to read a record from an ISAM file. */

int isread(
    int    isfd,         /* fd  of  previously  opened  ISAM
file */
    char   *record,      /* buf to copy read record into */
    int    mode)         /* which record, next, prev, equal,
etc */
{
    struct isquery far *lpRB;
    LPSTR              lpRec;
    int                iRecLen;
    int                iRetVal;

    lpRB = (struct isquery far *)GlobalLock(hRequestBlock);
    if (lpRB == NULL) {
        /* Error, not able to lock query block. */
        iserrno = ISNULLPTR;
        return -1;
    }
    lpRB->iOpcode = OPREAD;
    lpRB->iIsfd = isfd;
    lpRB->iMode = mode;

/* WinTrieve allows the user to search for a record based on
index key fields. It is expected the  values of interest are
filled in  the record  bufferto be passed to the server. For
the sake  of  simplicity assume that the global memory block
specified by hRecord is large enough to hold the record. */

    lpRec = GlobalLock(lpRB->hRecord);
    if (lpRec == NULL) {
        /* Error, not able to lock the record block. */
        GlobalUnLock(hRequestBlock);
        iserrno = ISNULLPTR;
        return -1;
    }
    /* Copy the record. */
    iRecLen = iRecLenTbl[isfd];
    lmemcpy(lpRec, (LPSTR)record, iRecLen);

      /* All fields filled, now unlock and make the request.
*/
    GlobalUnlock(lpRB->hRecord);
    GlobalUnlock(hRequestBlock);
          iRetVal  =   SendMessage(hWndServer,   wmSendISAM,
hWndClient, MAKELONG(hRequestBlock, 0));
    if (iRetVal != ISOK) {
        /* Protocol error. */
        if (iRetVal == 0L)
            iserrno = ISNOSERVER;
        else
            iserrno = iRetVal;
        return -1;
    }

    /* Lock the request block to get the response values. */
    lpRB = (struct isquery far *)GlobalLock(hRequestBlock);
    if (lpRB == NULL) {
        /* Error, not able to lock query block. */
        iserrno = ISNULLPTR;
        return -1;
    }
    /* This introduces the second level of error handling.
     * These are errors related to the processing of the
     * request which are differentiated from the processing
     * of the protocol.
     */
    iRetVal = lpRB->iRetVal;
    if (iRetVal == -1) {
        iserrno = lpRB->iserrno;
        return iRetVal;
    }

    lpRec = GlobalLock(lpRB->hRecord);
    if (lpRec == NULL) {
        /* Error, not able to lock the record block. */
        GlobalUnLock(hRequestBlock);
        iserrno = ISNULLPTR;
        return -1;
    }
    /* Copy the record that was read. Assume lmemcpy is
     * long pointer version of C Runtime memcpy.
     */
    lmemcpy((void far *)record, lpRec, iRecLen);
    GlobalUnlock(lpRB->hRecord);
    GlobalUnlock(hRequestBlock);

    return iRetVal;
}


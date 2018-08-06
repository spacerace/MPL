        ;************************************************************;
        ;                                                            ;
        ;       Function 13H: Delete File(s), FCB-based              ;
        ;                                                            ;
        ;       int FCB_delete(uXFCB)                                ;
        ;           char *uXFCB;                                     ;
        ;                                                            ;
        ;       Returns 0 if file(s) were deleted OK, otherwise      ;
        ;       returns -1.                                          ;
        ;                                                            ;
        ;       Note: uXFCB must have the drive and                  ;
        ;       filename fields (bytes 07H through 12H) and          ;
        ;       the extension flag (byte 00H) set before             ;
        ;       the call to FCB_delete (see Function 29H).           ;
        ;                                                            ;
        ;************************************************************;

cProc   FCB_delete,PUBLIC,ds
parmDP  puXFCB
cBegin
        loadDP  ds,dx,puXFCB    ; Pointer to unopened extended FCB.
        mov     ah,13h          ; Ask MS-DOS to delete file(s).
        int     21h
        cbw                     ; Return value of 0 or -1.
cEnd
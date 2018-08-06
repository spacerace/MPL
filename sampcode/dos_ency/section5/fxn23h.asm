        ;************************************************************;
        ;                                                            ;
        ;      Function 23H: Get File Size, FCB-based                ;
        ;                                                            ;
        ;      long FCB_nrecs(uXFCB,recsize)                         ;
        ;           char *uXFCB;                                     ;
        ;           int recsize;                                     ;
        ;                                                            ;
        ;      Returns a long -1 if file not found, otherwise        ;
        ;      returns the number of records of size recsize.        ;
        ;                                                            ;
        ;      Note: uXFCB must have the drive and                   ;
        ;      filename fields (bytes 07H through 12H) and           ;
        ;      the extension flag (byte 00H) set before              ;
        ;      the call to FCB_nrecs (see Function 29H).             ;
        ;                                                            ;
        ;************************************************************;

cProc   FCB_nrecs,PUBLIC,ds
parmDP  puXFCB
parmW   recsize
cBegin
        loadDP  ds,dx,puXFCB    ; Pointer to unopened extended FCB.
        mov     bx,dx           ; Copy FCB pointer into BX.
        mov     ax,recsize      ; Get record size
        mov     [bx+15h],ax     ; and store it in FCB.
        mov     ah,23h          ; Ask MS-DOS for file size (in
                                ; records).
        int     21h
        cbw                     ; If AL = 0FFH, set AX to -1.
        cwd                     ; Extend to long.
        or      dx,dx           ; Is DX negative?
        js      nr_exit         ; If so, exit with error flag.
        mov     [bx+2bh],al     ; Only low 24 bits of the relative-
                                ; record field are used, so clear the
                                ; top 8 bits.
        mov     ax,[bx+28h]     ; Return file length in DX:AX.
        mov     dx,[bx+2ah]
nr_exit:
cEnd

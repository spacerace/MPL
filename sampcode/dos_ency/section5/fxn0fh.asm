        ;************************************************************;
        ;                                                            ;
        ;        Function 0FH: Open File, FCB-based                  ;
        ;                                                            ;
        ;        int FCB_open(uXFCB,recsize)                         ;
        ;            char *uXFCB;                                    ;
        ;            int recsize;                                    ;
        ;                                                            ;
        ;        Returns 0 if file opened OK, otherwise returns -1.  ;
        ;                                                            ;
        ;        Note: uXFCB must have the drive and filename        ;
        ;        fields (bytes 07H through 12H) and the extension    ;
        ;        flag (byte 00H) set before the call to FCB_open     ;
        ;        (see Function 29H).                                 ;
        ;                                                            ;
        ;************************************************************;

cProc   FCB_open,PUBLIC,ds
parmDP  puXFCB
parmW   recsize
cBegin
        loadDP  ds,dx,puXFCB    ; Pointer to unopened extended FCB.
        mov     ah,0fh          ; Ask MS-DOS to open an existing file.
        int     21h
        add     dx,7            ; Advance pointer to start of regular
                                ; FCB.
        mov     bx,dx           ; BX = FCB pointer.
        mov     dx,recsize      ; Get record size parameter.
        mov     [bx+0eh],dx     ; Store record size in FCB.
        xor     dx,dx
        mov     [bx+20h],dl     ; Set current-record
        mov     [bx+21h],dx     ; and relative-record
        mov     [bx+23h],dx     ; fields to 0.
        cbw                     ; Set return value to 0 or -1.
cEnd

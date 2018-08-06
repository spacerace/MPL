        ;************************************************************;
        ;                                                            ;
        ;           Function 42H: Move File Pointer                  ;
        ;                                                            ;
        ;           long seek(handle,distance,mode)                  ;
        ;                int handle,mode;                            ;
        ;                long distance;                              ;
        ;                                                            ;
        ;           Modes:                                           ;
        ;                   0: from beginning of file                ;
        ;                   1: from the current position             ;
        ;                   2: from the end of the file              ;
        ;                                                            ;
        ;           Returns -1 if there was a seek error,            ;
        ;           otherwise returns long pointer position.         ;
        ;                                                            ;
        ;************************************************************;

cProc   seek,PUBLIC
parmW   handle
parmD   distance
parmB   mode
cBegin
        mov     bx,handle       ; Get handle.
        les     dx,distance     ; Get distance into ES:DX.
        mov     cx,es           ; Put high word of distance into CX.
        mov     al,mode         ; Get move method code.
        mov     ah,42h          ; Set function code.
        int     21h             ; Ask MS-DOS to move file pointer.
        jnb     sk_ok           ; Branch if seek successful.
        mov     ax,-1           ; Else return -1.
        cwd
sk_ok:
cEnd

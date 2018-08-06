        ;************************************************************;
        ;                                                            ;
        ;        Function 0AH: Buffered Keyboard Input               ;
        ;                                                            ;
        ;        int read_str(pbuf,len)                              ;
        ;            char *pbuf;                                     ;
        ;            int len;                                        ;
        ;                                                            ;
        ;        Returns number of bytes read into buffer.           ;
        ;                                                            ;
        ;        Note: pbuf must be at least len+3 bytes long.       ;
        ;                                                            ;
        ;************************************************************;

cProc   read_str,PUBLIC,<ds,di>
parmDP  pbuf
parmB   len
cBegin
        loadDP  ds,dx,pbuf      ; DS:DX = pointer to buffer.
        mov     al,len          ; AL = len.
        inc     al              ; Add 1 to allow for CR in buf.
        mov     di,dx
        mov     [di],al         ; Store max length into buffer.
        mov     ah,0ah          ; Set function code.
        int     21h             ; Ask MS-DOS to read string.
        mov     al,[di+1]       ; Return number of characters read.
        mov     ah,0
        mov     bx,ax
        mov     [bx+di+2],ah    ; Store 0 at end of buffer.
cEnd

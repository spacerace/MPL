        ;************************************************************;
        ;                                                            ;
        ;            Function 47H: Get Current Directory             ;
        ;                                                            ;
        ;            int get_dir(drive_ltr,pbuffer)                  ;
        ;                int drive_ltr;                              ;
        ;                char *pbuffer;                              ;
        ;                                                            ;
        ;            Returns -1 for bad drive,                       ;
        ;            otherwise returns pointer to pbuffer.           ;
        ;                                                            ;
        ;************************************************************;

cProc   get_dir,PUBLIC,<ds,si>
parmB   drive_ltr
parmDP  pbuffer
cBegin
        loadDP  ds,si,pbuffer   ; Get pointer to buffer.
        mov     dl,drive_ltr    ; Get drive number.
        or      dl,dl           ; Leave 0 alone.
        jz      gdir
        and     dl,not 20h      ; Convert letter to uppercase
        sub     dl,'A'-1        ; Convert to drive number: 'A' = 1,
                                ; 'B' = 2, etc.
gdir:
        mov     ah,47h          ; Set function code.
        int     21h             ; Call MS-DOS.
        mov     ax,si           ; Return pointer to buffer ...
        jnb     gd_ok
        mov     ax,-1           ; ... unless an error occurred.
gd_ok:
cEnd

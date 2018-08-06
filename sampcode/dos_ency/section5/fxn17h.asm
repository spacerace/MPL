        ;************************************************************;
        ;                                                            ;
        ;         Function 17H: Rename File(s), FCB-based            ;
        ;                                                            ;
        ;         int FCB_rename(uXFCBold,uXFCBnew)                  ;
        ;             char *uXFCBold,*uXFCBnew;                      ;
        ;                                                            ;
        ;         Returns 0 if file(s) renamed OK, otherwise         ;
        ;         returns -1.                                        ;
        ;                                                            ;
        ;         Note: Both uXFCB's must have the drive and         ;
        ;         filename fields (bytes 07H through 12H) and        ;
        ;         the extension flag (byte 00H) set before           ;
        ;         the call to FCB_rename (see Function 29H).         ;
        ;                                                            ;
        ;************************************************************;

cProc   FCB_rename,PUBLIC,<ds,si,di>
parmDP  puXFCBold
parmDP  puXFCBnew
cBegin
        loadDP  es,di,puXFCBold ; ES:DI = Pointer to uXFCBold.
        mov     dx,di           ; Save offset in DX.
        add     di,7            ; Advance pointer to start of regular
                                ; FCBold.
        loadDP  ds,si,puXFCBnew ; DS:SI = Pointer to uXFCBnew.
        add     si,8            ; Advance pointer to filename field
                                ; FCBnew.
                                ; Copy name from FCBnew into FCBold
                                ; at offset 11H:
        add     di,11h          ; DI points 11H bytes into old FCB.
        mov     cx,0bh          ; Copy 0BH bytes, moving new
        rep     movsb           ; name into old FCB.
        push    es              ; Set DS to segment of FCBold.
        pop     ds
        mov     ah,17h          ; Ask MS-DOS to rename old
        int     21h             ; file(s) to new name(s).
        cbw                     ; Set return flag to 0 or -1.
cEnd

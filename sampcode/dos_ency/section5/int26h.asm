;****************************************************************;
;                                                                ;
;       Interrupt 26H: Absolute Disk Write                       ;
;                                                                ;
;       Write the contents of the memory area named buff         ;
;       into logical sector 3 of drive C.                        ;
;                                                                ;
;       WARNING: Verbatim use of this code could damage          ;
;       the file structure of the fixed disk. It is meant        ;
;       only as a general guide. There is, unfortunately,        ;
;       no way to give a really safe example of this interrupt.  ;
;                                                                ;
;****************************************************************;

        mov     al,2            ; Drive C.
        mov     cx,1            ; Number of sectors.
        mov     dx,3            ; Beginning sector number.
        mov     bx,seg buff     ; Address of buffer.
        mov     ds,bx
        mov     bx,offset buff
        int     26h             ; Request disk write.
        jc      error           ; Jump if write failed.
        add     sp,2            ; Clear stack.
        .
        .
        .
error:                          ; Error routine goes here.
        .
        .
        .
buff    db      512 dup (?)     ; Data to be written to disk.

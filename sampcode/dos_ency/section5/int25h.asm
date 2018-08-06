;***************************************************************;
;                                                               ;
;      Interrupt 25H: Absolute Disk Read                        ;
;                                                               ;
;      Read logical sector 1 of drive A into the memory area    ;
;      named buff. (On most MS-DOS floppy disks, this sector    ;
;      contains the beginning of the file allocation table.)    ;
;                                                               ;
;***************************************************************;

        mov     al,0            ; Drive A.
        mov     cx,1            ; Number of sectors.
        mov     dx,1            ; Beginning sector number.
        mov     bx,seg buff     ; Address of buffer.
        mov     ds,bx
        mov     bx,offset buff
        int     25h             ; Request disk read.
        jc      error           ; Jump if read failed.
        add     sp, 2           ; Clear stack.
        .
        .
        .
error:                          ; Error routine goes here.
        .
        .
        .
buff    db      512 dup (?)

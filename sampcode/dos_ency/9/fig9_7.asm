emm_int equ     67h             ; EMM software interrupt

        .
        .
        .
                                ; first fetch contents of
                                ; EMM interrupt vector...
        mov     al,emm_int      ; AL = EMM int number
        mov     ah,35h          ; Fxn 35H = get vector
        int     21h             ; transfer to MS-DOS
                                ; now ES:BX = handler address

                                ; assume ES:0000 points
                                ; to base of the EMM...
        mov     di,10           ; ES:DI = address of name
                                ; field in device header
        mov     si,seg emm_name ; DS:SI = address of
        mov     ds,si           ; expected EMM driver name
        mov     si,offset emm_name
        mov     cx,8            ; length of name field
        cld
        repz cmpsb              ; compare names...
        jnz     error           ; jump if driver absent
        .
        .
        .


emm_name db     'EMMXXXX0'      ; guaranteed device name for EMM

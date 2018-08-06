cmdtail equ     80h             ; PSP offset of command tail
fname   db      64 dup (?)
fhandle dw      ?

        .
        .
        .
                                ; assume that DS already 
                                ; contains segment of PSP

                                ; prepare to copy filename...
        mov     si,cmdtail      ; DS:SI = command tail
        mov     di,seg fname    ; ES:DI = buffer to receive
        mov     es,di           ; filename from command tail
        mov     di,offset fname
        cld                     ; safety first!

        lodsb                   ; check length of command tail
        or      al,al
        jz      error           ; jump, command tail empty

label1:                         ; scan off leading spaces
        lodsb                   ; get next character
        cmp     al,20h          ; is it a space?
        jz      label1          ; yes, skip it

label2:
        cmp     al,0dh          ; look for terminator
        jz      label3          ; quit if return found
        cmp     al,20h
        jz      label3          ; quit if space found
        stosb                   ; else copy this character
        lodsb                   ; get next character
        jmp     label2

label3:
        xor     al,al           ; store final NULL to
        stosb                   ; create ASCIIZ string

                                ; now open the file...
        mov     dx,seg fname    ; DS:DX = address of
        mov     ds,dx           ; pathname for file
        mov     dx,offset fname
        mov     ax,3d02h        ; Function 3DH = open r/w
        int     21h             ; transfer to MS-DOS
        jnc     label4          ; jump if file found

        cmp     ax,2            ; error 2 = file not found
        jnz     error           ; jump if other error
                                ; else make the file...
        xor     cx,cx           ; CX = normal attribute
        mov     ah,3ch          ; Function 3CH = create
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if create failed

label4:
        mov     fhandle,ax      ; save handle for file
        .
        .
        .

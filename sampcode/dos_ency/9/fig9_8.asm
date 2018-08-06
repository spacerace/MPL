        .
        .
        .
        mov     ah,40h          ; test EMM status
        int     67h
        or      ah,ah
        jnz     error           ; jump if bad status from EMM

        mov     ah,46h          ; check EMM version
        int     67h
        or      ah,ah
        jnz     error           ; jump if couldn't get version
        cmp     al,30h          ; make sure at least ver. 3.0
        jb      error           ; jump if wrong EMM version

        mov     ah,41h          ; get page frame segment
        int     67h
        or      ah,ah
        jnz     error           ; jump if failed to get frame
        mov     page_frame,bx   ; save segment of page frame

        mov     ah,42h          ; get no. of available pages
        int     67h
        or      ah,ah
        jnz     error           ; jump if get pages error
        mov     total_pages,dx  ; save total EMM pages
        mov     avail_pages,bx  ; save available EMM pages
        or      bx,bx
        jz      error           ; abort if no pages available

        mov     ah,43h          ; try to allocate EMM pages
        mov     bx,needed_pages
        int     67h             ; if allocation is successful
        or      ah,ah
        jnz     error           ; jump if allocation failed

        mov     emm_handle,dx   ; save handle for allocated pages

        .
        .                       ; now we are ready for other
        .                       ; processing using EMM pages
        .
                                ; map in EMM memory page...
        mov     bx,log_page     ; BX <- EMM logical page number
        mov     al,phys_page    ; AL <- EMM physical page (0-3)
        mov     dx,emm_handle   ; EMM handle for our pages
        mov     ah,44h          ; Fxn 44H = map EMM page
        int     67h
        or      ah,ah
        jnz     error           ; jump if mapping error

        .
        .
        .
                                ; program ready to terminate,
                                ; give up allocated EMM pages...
        mov     dx,emm_handle   ; handle for our pages
        mov     ah,45h          ; EMM Fxn 45H = release pages
        int     67h
        or      ah,ah
        jnz     error           ; jump if release failed
        .
        .
        .

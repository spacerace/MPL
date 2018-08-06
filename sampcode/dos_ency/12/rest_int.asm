rest_int proc   near
        push    ds              ; save our data segment
        mov     dx,word ptr oldint23
        mov     ds,word ptr oldint23+2
        mov     ax,2523h        ; restore original contents
        int     21h             ; of Int 23H vector
        pop     ds              ; restore our data segment
        push    ds              ; then save it again
        mov     dx,word ptr oldint1B
        mov     ds,word ptr oldint1B+2
        mov     ax,251Bh        ; restore original contents
        int     21h             ; of Int 1BH vector
        pop     ds              ; get back our data segment
        ret                     ; return to caller
rest_int endp

_DATA   segment para public 'DATA'
oldint1b dd     0               ; original INT 1BH vector
oldint23 dd     0               ; original INT 23H vector
_DATA   ends
_TEXT   segment byte public 'CODE'
        assume cs:_TEXT,ds:_DATA,es:NOTHING
myint1b:                        ; handler for Ctrl-Break
myint23:                        ; handler for Ctrl-C
        iret

set_int proc    near
        mov     ax,351bh        ; get current contents of
        int     21h             ; Int 1BH vector and save it
        mov     word ptr oldint1b,bx
        mov     word ptr oldint1b+2,es
        mov     ax,3523h        ; get current contents of
        int     21h             ; Int 23H vector and save it
        mov     word ptr oldint23,bx
        mov     word ptr oldint23+2,es
        push    ds              ; save our data segment
        push    cs              ; let DS point to our
        pop     ds              ; code segment
        mov     dx,offset myint1b
        mov     ax,251bh        ; set interrupt vector 1BH
        int     21h             ; to point to new handler
        mov     dx,offset myint23
        mov     ax,2523h        ; set interrupt vector 23H
        int     21h             ; to point to new handler
        pop     ds              ; restore our data segment
        ret                     ; back to caller
set_int endp
_TEXT   ends

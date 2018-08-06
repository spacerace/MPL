;               Video Display Library (part 2)

_TEXT   segment byte public 'CODE'      ; Place the code in the code segment
        assume  CS:_TEXT                ; Assume the CS register points to it

; _scrClr()
;
; Function: Clear the screen.
;
; Algorithm: Call the ROM BIOS scroll up function to do the actual clearing.

        public  _scrClr         ; This routine is public (available to
                                ; other modules)

_scrClr proc near               ; This is a NEAR type subroutine
        push    bp              ; Save the BP resgister
        push    si              ; And SI
        push    di              ; And DI
        xor     al,al           ; Set the number of lines to scroll to zero
        mov     ch,al           ; Set the upper left corner row to 0
        mov     cl,al           ; Set the upper left corner column to 0
        mov     dh,24           ; Set the lower right corner row to 24
        mov     dl,79           ; Set the lower right corner column to 79
        mov     bh,7            ; Set the attribute to white-on-black
        mov     ah,6            ; Video function 6: scroll up
        int     10H             ; Call the ROM BIOS video interrupt
        mov     ah,15           ; Do a get state interrupt to get the 
                                ; page # into bh
        int     10H             ; Video BIOS interrupt
        mov     dx,0            ; Set the row and column to 0
        mov     ah,2            ; Do a set cursor position interrupt
        int     10H             ; Video interrupt
        pop     di              ; Restore the DI register
        pop     si              ; And SI
        pop     bp              ; And BP
        ret                     ; Return to the calling program
_scrClr endp                    ; End of subroutine

; _scrPuts(s)
;
; Function: Do a TTY style write of the null-terminated string pointed
; to by s.
;
; Algorithm: Repeatedly call the ROM BIOS TTY style write character
; function for each character in the string.

        public  _scrPuts        ; Routine is available to other modules

        s = 4                   ; The offset to the s parameter

_scrPuts proc near              ; NEAR type subroutine
        push    bp              ; Save the BP register
        mov     bp,sp           ; Set BP to SP, so we can access the parameters
        push    si              ; Save the SI register
        push    di              ; And DI
        mov     si,[bp+s]       ; Set SI to the string pointer

sLoop:  lods    byte ptr [si]   ; AL = *SI++ (get the next byte in the string)
        or      al,al           ; Is it zero (end-of-string)?
        jz      sExit           ; If it is, exit the loop
        mov     bl,3            ; Set foreground color to white
        mov     ah,14           ; Do a TTY style write using the BIOS
        int     10H             ; ROM BIOS video interrupt
        jmp     sLoop           ; Go get the next character

sExit:  pop     di              ; Restore the DI register
        pop     si              ; Restore the SI register
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_scrPuts endp                   ; End of subroutine

_TEXT   ends                    ; End of code segment
        end                     ; End of assembly code


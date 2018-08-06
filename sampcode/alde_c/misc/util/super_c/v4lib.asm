;               Text Video Library (part 4)

_TEXT   segment byte public 'CODE'      ; Place the code in the code segment
        assume  CS:_TEXT                ; Assume the CS register points to it

; _putDot(page,row,col,color)
;
; Function: Write a dot to the screen on the page, row, and column specified.
; Write it in color color. Note that page is only used with the EGA, and
; the range of valid colors depends on the adapter and mode being used.
;
; Algorithm: Call the ROM BIOS put dot function; int 10H, AH = 12.

        public  _putDot         ; Routine is available to other modules

        page = 4                ; Offset from BP to parameter page
        row = 6                 ; Offset to parameter row
        col = 8                 ; Offset to col
        color = 10              ; Offset to color

_putDot proc near               ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        push    si              ; Save SI
        push    di              ; Save DI
        mov     bh,[bp+page]    ; Set BH to the page
        mov     dx,[bp+row]     ; Set DX to the row
        mov     cx,[bp+col]     ; Set CX to the column
        mov     al,[bp+color]   ; Set AL to the color
        mov     ah,12           ; Set AH to 12 (put dot function)
        int     10H             ; Call ROM BIOS video interrupt
        pop     di              ; Restore the DI register
        pop     si              ; Restore the SI register
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_putDot endp                    ; End of subroutine

; _getDot(page,row,col)
;
; Function: Return the color of the dot at the page, row, and column given.
; Note: The page specification is only meaningfull when using the EGA, and
; the color returned will be constrained to a range that depends on the
; display adapter and mode being used.
;
; Algorithm: Call the ROM BIOS get dot function; int 10H, AH = 13.

        public  _getDot         ; Routine is available to other modules

        page = 4                ; Offset from BP to parameter page
        row = 6                 ; Offset to parameter row
        col = 8                 ; Offset to col

_getDot proc near               ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        push    si              ; Save SI
        push    di              ; Save DI
        mov     bh,[bp+page]    ; Set BH to the page
        mov     dx,[bp+row]     ; Set DX to the row
        mov     cx,[bp+col]     ; Set CX to the column
        mov     ah,13           ; Set AH to 13 (get dot function)
        int     10H             ; Call ROM BIOS video interrupt
        xor     ah,ah           ; Zero the top byte of the return value
        pop     di              ; Restore the DI register
        pop     si              ; Restore the SI register
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_getDot endp                    ; End of subroutine

_TEXT   ends                    ; End of code segment

        end                     ; End of assembly code


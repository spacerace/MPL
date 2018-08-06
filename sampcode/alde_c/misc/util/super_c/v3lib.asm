;               Equipment Configuration Library

_TEXT   segment byte public 'CODE'      ; Place the code in the code segment
        assume  CS:_TEXT                ; Assume the CS register points to it

; _setPal(colorID,color)
;
; Function: Set the palette color ID to the color given.
;
; Algorithm: Call ROM BIOS set palette video function.

        public  _setPal         ; Routine is available to other modules

        colorID = 4             ; The offset to the color ID parameter
        color = 6               ; The offset to the color parameter

_setPal proc near               ; NEAR type subroutine
        push    bp              ; Save the BP register
        mov     bp,sp           ; Set BP to SP, so we can access parameters
        push    si              ; Save the SI register
        push    di              ; Save the DI register
        mov     bh,[bp+colorID] ; Set BH to the color ID
        mov     bl,[bp+color]   ; Set BL to the color
        mov     ah,11           ; Function 11: set palette
        int     10H             ; Call the ROM BIOS with interrupt 10 (video)
        pop     di              ; Restore the DI register
        pop     si              ; Restore the SI register
        pop     bp              ; Restore BP
        ret                     ; Return to calling program
_setPal endp                    ; End of subroutine

_TEXT   ends                    ; End of code segment
        end                     ; End of assembly code


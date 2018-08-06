;               Example Function to Add Two Numbers

_TEXT   segment byte public 'CODE'      ; Place the code in the code segment

        assume  CS:_TEXT                ; Assume the CS register points to it

; _add(a,b)
;
; Function: Add together the two input parameters and return the result.
;
; Algorithm: Save BP and set it to SP to access the parameters. Add the two
; parameters together in the AX register. Restore the BP register. And
; return.

        public  _add            ; Routine is available to other modules

        a = 4                   ; Offset from BP to the parameter a
        b = 6                   ; Offset to parameter b

_add    proc near               ; NEAR type subroutine
        push    bp              ; Save the BP register
        mov     bp,sp           ; Set BP to SP; easier to access parameters
        mov     ax,[bp+a]       ; AX = a (get the a parameter)
        add     ax,[bp+b]       ; AX += b (add in the b parameter)
        pop     bp              ; Restore the BP register
        ret                     ; Return to caller
_add    endp                    ; End of subroutine

_TEXT   ends                    ; End of code segment

        end                     ; End of assembly code


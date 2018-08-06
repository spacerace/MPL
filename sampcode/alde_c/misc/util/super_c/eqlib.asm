;               Equipment Configuration Library

_TEXT   segment byte public 'CODE'      ; Place the code in the code segment
        assume  CS:_TEXT                ; Assume the CS register points to it

; _memSz()
;
; Function: Return the number of 1K blocks of memory in the system.
;
; Algorithm: Just call the ROM BIOS via interrupt 10H.

        public  _memSz          ; This routine is public (available to
                                ; other modules)

_memSz  proc near               ; _memSz is a NEAR type subroutine
        int     12H             ; Call the ROM BIOS with interrupt 12 (hex)
        ret                     ; Return to the calling program
_memSz  endp                    ; End of subroutine

; _getEq()
;
; Function: Return the current equipment configuration.
;
; Algorithm: Call ROM BIOS interrupt 11H to get the configuration.

        public  _getEq          ; Routine is available to other modules

_getEq  proc near               ; NEAR type subroutine
        int     11H             ; Call the ROM BIOS with interrupt 11 (hex)
        ret                     ; Return to calling program
_getEq  endp                    ; End of subroutine

_TEXT   ends                    ; End of code segment
        end                     ; End of assembly code


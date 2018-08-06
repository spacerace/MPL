;               Printer Port ROM BIOS Interface Library

_TEXT   segment byte public 'CODE'      ; Place the code in the code segment
        assume  CS:_TEXT                ; Assume the CS register points to it

; _prtInit(port)
;
; Function: Initialize the printer port specified. Returns the port status.
;
; Algorithm: Call the ROM BIOS printer init function; int 17H, AH = 0.

        public  _prtInit        ; Routine is available to other modules

        port = 4                ; Offset from BP to parameter port

_prtInit proc near              ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     dx,[bp+port]    ; Set DX to the port number
        mov     ah,1            ; Set AH to 1 (init port function)
        int     17H             ; Call ROM BIOS printer port interrupt
        mov     al,ah           ; Return status in AX
        xor     ah,ah           ; With high byte zero
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_prtInit endp                   ; End of subroutine

; _prtPrint(port,char)
;
; Function: Print the character char on port port. Return the port status.
;
; Algorithm: Call the ROM BIOS print char function; int 17H, AH = 0.

        public  _prtPrint       ; Routine is available to other modules

        port = 4                ; Offset from BP to parameter port
        char = 6                ; Offset to parameter char

_prtPrint proc near             ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     dx,[bp+port]    ; Set DX to the port number
        mov     al,[bp+char]    ; Set AL to the character
        mov     ah,0            ; Set AH to 0 (print char function)
        int     17H             ; Call ROM BIOS printer port interrupt
        mov     al,ah           ; Return the status in AX
        xor     ah,ah           ; With the top byte zero
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_prtPrint endp                  ; End of subroutine

; _prtStat(port)
;
; Function: Return the status of the printer port specified.
;
; Algorithm: Call the ROM BIOS printer status function; int 17H, AH = 2.

        public  _prtStat        ; Routine is available to other modules

        port = 4                ; Offset from BP to parameter port

_prtStat proc near              ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     dx,[bp+port]    ; Set BH to the port number
        mov     ah,2            ; Set AH to 2 (printer status function)
        int     17H             ; Call ROM BIOS printer port interrupt
        mov     al,ah           ; Return status in AX
        xor     ah,ah           ; With the top byte zero
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_prtStat endp                   ; End of subroutine

_TEXT   ends                    ; End of code segment

        end                     ; End of assembly code


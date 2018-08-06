;               Serial Port ROM BIOS Interface Library

_TEXT   segment byte public 'CODE'      ; Place the code in the code segment
        assume  CS:_TEXT                ; Assume the CS register points to it

; _serInit(port,config)
;
; Function: Initialize and configure serial port port (0 for COM1, 1 for
; COM2). See the main text of the chapter for details on the format of the
; config parameter.
;
; Algorithm: Call the ROM BIOS config function; int 14H, AH = 0.

        public  _serInit        ; Routine is available to other modules

        port = 4                ; Offset from BP to parameter port
        config = 6              ; Offset to parameter config

_serInit proc near              ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     dx,[bp+port]    ; Set DX to the port number
        mov     al,[bp+config]  ; Set AL to the configuration
        mov     ah,0            ; Set AH to 0 (configure port function)
        int     14H             ; Call ROM BIOS serial port interrupt
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_serInit endp                   ; End of subroutine

; _serSend(port,char)
;
; Function: Send the character char out over serial port port. Return the
; completion code/status.
;
; Algorithm: Call the ROM BIOS put char function; int 14H, AH = 1.

        public  _serSend        ; Routine is available to other modules

        port = 4                ; Offset from BP to parameter port
        char = 6                ; Offset to parameter char

_serSend proc near              ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     dx,[bp+port]    ; Set DX to the port number
        mov     al,[bp+char]    ; Set AL to the character
        mov     ah,1            ; Set AH to 1 (send char function)
        int     14H             ; Call ROM BIOS serial port interrupt
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_serSend endp                   ; End of subroutine

; _serRecv(port)
;
; Function: Wait for and receive a character from the serial port.
; Return the character in the low byte of the return value, and the
; status in the high byte.
;
; Algorithm: Call the ROM BIOS receive char function; int 14H, AH = 2.

        public  _serRecv        ; Routine is available to other modules

        port = 4                ; Offset from BP to parameter port

_serRecv proc near              ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     dx,[bp+port]    ; Set BH to the port number
        mov     ah,2            ; Set AH to 2 (receive char function)
        int     14H             ; Call ROM BIOS serial port interrupt
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_serRecv endp                   ; End of subroutine

; _serStat(port)
;
; Function: Return the status of the serial port specified.
;
; Algorithm: Call the ROM BIOS serial status function; int 14H, AH = 3.

        public  _serStat        ; Routine is available to other modules

        port = 4                ; Offset from BP to parameter port

_serStat proc near              ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     dx,[bp+port]    ; Set BH to the port number
        mov     ah,3            ; Set AH to 3 (serial status function)
        int     14H             ; Call ROM BIOS serial port interrupt
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_serStat endp                   ; End of subroutine

_TEXT   ends                    ; End of code segment

        end                     ; End of assembly code


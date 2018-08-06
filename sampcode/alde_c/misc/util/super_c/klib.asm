;               Equipment Configuration Library

_TEXT   segment byte public 'CODE'      ; Place the code in the code segment
        assume  CS:_TEXT                ; Assume the CS register points to it

; _keyRd()
;
; Function: Return the next key pressed. The ASCII value (if any) is in the
; lower byte; the scan code is in the upper byte. If the key doesn't have
; an ASCII value, the lower byte is zero.
;
; Algorithm: Call ROM BIOS read key function of the keyboard interrupt.

        public  _keyRd          ; Routine is available to other modules

_keyRd  proc near               ; NEAR type subroutine
        mov     ah,0            ; Function 0: read key
        int     16H             ; Call the ROM BIOS with interrupt 16 (keybd)
        ret                     ; Return to calling program
_keyRd  endp                    ; End of subroutine

; _keyChk(&keyPtr)
;
; Function: Return TRUE if there is a key press waiting, and if so, return
; the key in the location pointed to by keyPtr; if there is no key waiting,
; return FALSE. *keyPtr is set to the same value that would be returned
; from keyRd, but the key press is not actually read; keyRd must be called
; to remove it from the queue.
;
; Algorithm: Call ROM BIOS key check function of the keyboard interrupt.

        public  _keyChk         ; Routine is available to other modules

        keyPtr = 4              ; The offset to the keyPtr parameter

_keyChk proc near               ; NEAR type subroutine
        push    bp              ; Save the BP register
        mov     bp,sp           ; Set BP to SP, to get at the parameters
        mov     ah,1            ; Function 1: check for key press
        int     16H             ; Call the ROM BIOS with interrupt 16 (keybd)
        mov     bx,[bp+keyPtr]  ; Save the key, if there is any
        mov     [bx],ax
        mov     ax,0            ; Assume not key (i.e., return FALSE)
        jz      noKey           ; Branch if no key
        mov     ax,1            ; Otherwise return TRUE
noKey:  pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_keyChk endp                    ; End of subroutine

; _keyShf()
;
; Function: Return the current state of the shift keys and toggled modes.
; See the main text of the chapter for details on the format of the word
; returned.
;
; Algorithm: Call ROM BIOS get keyboard state function of the keyboard 
; interrupt.

        public  _keyShf         ; Routine is available to other modules

_keyShf proc near               ; NEAR type subroutine
        mov     ah,2            ; Function 2: get keyboard state
        int     16H             ; Call the ROM BIOS with interrupt 16 (keybd)
        ret                     ; Return to calling program
_keyShf endp                    ; End of subroutine

_TEXT   ends                    ; End of code segment
        end                     ; End of assembly code


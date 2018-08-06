;               Noise Library

_TEXT   segment byte public 'CODE'      ; Place the code in the code segment
        assume  CS:_TEXT                ; Assume the CS register points to it

; _sound(count) 
;
; Function: Turn on the speaker and have it play the note specified by
; count.
;
; Algorithm: Write the count to the portion of the timer that drives the
; speaker, and then enable the speaker output.

        public _sound           ; Routine is available to other modules

        count = 4               ; Offset from BP to the parameter count

_sound  proc near               ; NEAR type subroutine
        push    bp              ; Save the BP register
        mov     bp,sp           ; Set BP to SP; easier to access parameters
        mov     al,0B6H         ; Tell timer to prepare for a new count
        out     43H,al
        mov     ax,[bp+count]   ; Write new count to the timer, low byte 1st
        out     42H,al
        mov     al,ah           ; Then high byte
        out     42H,al
        in      al,61H          ; Enable speaker output from timer
        or      al,3
        out     61H,al
        pop     bp              ; Restore the BP register
        ret                     ; Return to caller
_sound  endp                    ; End of subroutine

; _sndoff()
;
; Function: Turn off speaker.
;
; Algorithm: Disable speaker output from timer.

        public _sndOff          ; Routine is available to other modules

_sndOff proc near               ; NEAR type subroutine
        in      al,61H          ; Disable speaker output from timer
        and     al,0FCH
        out     61H,al
        ret                     ; Return to caller
_sndOff endp                    ; End of subroutine

_TEXT   ends                    ; End of code segment
        end                     ; End of assembly code


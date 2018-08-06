;       Video_Display Library (part 5)

_TEXT   segment byte public 'CODE'      ; Place the code in the code segment
        assume CS:_TEXT                 ; Assume the CS register points to it

        public _scrClr

_scrClr   proc near
        push    bp
        mov     bp,sp
        push    di
        push    si
        mov     al,0              ; Set number of lines to scroll to 0
        mov     ch,0              ; Set upper-left row
        mov     cl,0              ; Set upper-left column
        mov     dh,24             ; Set lower-right row
        mov     dl,79             ; Set lower-right Column
        mov     bh,7              ; Set blank lines attribute
        mov     ah,6              ; Scroll page up interrupt
        int     10H
        mov     ah,15             ; Get page number
        int     10H
        mov     dx,0              ; Set position to (0,0)
        mov     ah,2
        int     10H
        pop     si
        pop     di
        pop     bp
        ret
_scrClr   endp

; _scrPuts(s)
;
; Function: Do a TTY style write of the null-terminated string pointed
; to by s.
;
; Algorithm: Directly write the characters to the display, then update
; the cursor to the end of the string written.
;
; Comments: This function does not implement full TTY style output; most
; importantly, it will not scroll the display when the end is reached.

        public  _scrPuts        ; Routine is available to other modules

        s = 4                   ; The offset to the s parameter

_scrPuts proc near              ; NEAR type subroutine
        push    bp              ; Save the BP register
        mov     bp,sp           ; Set BP to SP, so we can access the parameters
        push    di              ; Save the DI register
        push    si              ; Save the SI register
        push    es              ; Save the ES register
        mov     ah,15           ; Get mode & active page number by calling
        int     10H             ;   the ROM BIOS
        mov     cx,0B000H       ; Assume monochrome adapter base address
        cmp     al,7            ; Check it by checking the display mode
        je      setma           ; If it is monochrome, branch
        mov     cx,0B800H       ; Otherwise, set the base address to CGA
setma:  mov     es,cx           ; ES = adapter base address
        mov     si,[bp+s]       ; SI = start of string to write
        mov     ah,3            ; Get cursor position into DH/DL by calling
        int     10H             ;   the ROM BIOS
;
; Calculate the character position offset:
;
calcCh: push    es              ; Save display memory ES
        xor     ax,ax           ; Point ES to low memory (i.e., zero it)
        mov     es,ax
        mov     al,dh           ; AL = row of cursor
        mov     bl,es:44AH      ; BL = screen width in characters
        mul     bl              ; AX = row # times columns per row
        mov     cl,dl           ; CX = column of cursor
        xor     ch,ch
        add     ax,cx           ; Add the column number to AX
        shl     ax,1            ; Adjust for char/attribute (multiply by 2)
        add     ax,es:44EH      ; Add in the page start address (from low mem)
        mov     di,ax           ; Point DI at next display byte
        pop     es              ; Restore ES to the screen memory base
;
; Main character copying loop:
;
loop:   lods    byte ptr [si]   ; Get the next character in the string
        or      al,al           ; Check for end-of-string
        jz      exit            ; If end-of-string, exit
        cmp     al,0DH          ; Check for carriage return
        jne     notCR           ; If not carriage return, branch
        xor     dl,dl           ; Go to the first column, this line
        jmp     calcCh          ; Go recalc display pointer
notCR:  cmp     al,0AH          ; Check for line feed
        jne     notLF           ; If not line feed, branch
        inc     dh              ; Go to the next line, this column
        jmp     calcCh          ; Go recalc display pointer
notLF:  cmp     al,' '          ; Check for other control characters
        jl      loop            ; If it is a control character, skip it
        cmp     dl,bl           ; Are we off the right edge of the screen?
        je      loop            ; If yes, don't display this one
        stos    byte ptr es:[di]; Store the character into display memory
        inc     di              ; Skip over display attribute
        inc     dl              ; Next column
        jmp     loop            ; Go do the next one

exit:   mov     ah,2            ; Set the new cursor position by calling
        int     10H             ;   the ROM BIOS
        pop     es              ; Restore the ES register
        pop     si              ; Restore the SI register
        pop     di              ; Restore the DI register
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_scrPuts endp                   ; End of subroutine

_TEXT   ends                    ; End of code segment
        end                     ; End of assembly code


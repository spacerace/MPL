                        TITLE   String Commands for Lattice Library
                        NAME    STRING
                        INCLUDE DOS.MAC
 
COMMENT #
                AUTHOR          Jon Wesener
                DATE            7 / 8 / 1985
                PURPOSE           These are support routines for use
                                with C and Assembler.  They are quick
                                string functions I need a lot.
        #
 
                PSEG
                PUBLIC  STRLEN, STRCPY, STRCAT, STRCMP, STRCOM
 
; Strlen gives the length of a string minus the terminator, 0.
;  strlen(string)
; RETURN: length of string.
 
STRLEN          PROC    NEAR
                push    bp
                mov     bp, sp
                xor     ax, ax          ; search for terminator 0
                mov     di, [bp+4]      ; get string
                mov	cx, 0ffffh	; don't get stopped before we start
                repne   scasb
                mov     ax, [bp+4]      ; get start position
                xchg    ax, di          ; switch them around
                sub     ax, di
                dec     ax              ; subtract terminator
                pop     bp
                ret
STRLEN          ENDP
 
; Strcpy copies the second string into the first
;  strcpy(to,from)
; RETURN: nothing
 
STRCPY          PROC    NEAR
                push    bp
                mov     bp, sp
                mov     si, [bp+6]      ; get from string
                mov     di, [bp+4]      ; get to string
scl1:           lodsb                   ; read a byte
                stosb                   ; copy it
                and     al, al          ; is it the end ?
                jnz     scl1            ; nope, continue
                pop     bp
                ret
STRCPY          ENDP
 
; Strcat concatenates a string to the end of another.
;  strcat(to,from);
; RETURN: nothing
 
STRCAT          PROC    NEAR
                push    bp
                mov     bp, sp
                push    [bp+4]          ; push the to string
                call    strlen          ; find its length
                add     ax, [bp+4]      ; find the end of it
                push    ax              ; push new to string
                push    [bp+6]          ; push the from string
                call    strcpy
                mov     sp, bp          ; clear the stack
                pop     bp
                ret
STRCAT          ENDP
 
; Strcmp compares two string to see if they are the same.
;  strcmp(string1, string2)
; RETURN: 1= equal      0= not equal
 
STRCMP          PROC    NEAR
                push    bp
                mov     bp, sp
                mov     si, [bp+4]
                mov     di, [bp+6]
stcml1:         cmpsb                   ; compare two bytes
                jnz     stcmne          ; they aren't equal
                test    byte ptr [si-1], 0ffh
                jnz     stcml1          ; not done yet
                mov     ax, 1           ; signal equal
stcmex:         pop     bp
                ret
stcmne:         xor     ax, ax          ; signal not equal
                jmp     stcmex
STRCMP          ENDP
 
; Strcom will return the 128 byte buffer containing the command line.
;  strcom(buf);
; RETURN:       nothing
; NOTE: THIS WILL ONLY WORK FOR .COM PROGRAMS
 
STRCOM          PROC    NEAR
                push    bp
                mov     bp, sp
                mov     di, [bp+4]      ; pt to buffer
                mov     si, 80h
                push    cs
                pop     ds              ; pt to source
                mov     cx, 80h         ; get 128 bytes
                rep     movsb           ; copy it over
                push    es
                pop     ds              ; restore data segment
                pop     bp
                ret
STRCOM          ENDP
                ENDPS
                END
 

                        TITLE   Keyboard Subroutines for Lattice
                        NAME    KEYBOARD
                        INCLUDE DOS.MAC
 
COMMENT #
                AUTHOR          Jon Wesener
                DATE            7 / 16 / 85
                PURPOSE         These routines will give quick access
                                to the keyboard
        #
                DSEG
TABLESIZE       EQU     19
 
convert         db      75, 200         ; left character
                db      77, 201         ; right character
                db      82, 202         ; insert
                db      83, 203         ; delete
                db      59, 204         ; F1
                db      60, 205         ; F2
                db      61, 206         ; F3
                db      62, 207         ; F4
                db      63, 208         ; F5
                db      64, 209         ; F6
                db      65, 210         ; F7
                db      66, 211         ; F8
                db      67, 212         ; F9
                db      68, 213         ; F10
                db      71, 214         ; BOL
                db      79, 215         ; EOL
                db      73, 216         ; Page Up
                db      81, 217         ; Page Down
                db      115, 218        ; Previous Word
                db      116, 219        ; Next Word
                db       0, 0           ; invalid character, ignore
                ENDDS
 
                PSEG
                PUBLIC  GETC, GETCW, GETCRAW
 
; Getc returns a character if available and converts it via the table above.
;  ch= getc();
; RETURNS:      0= No character         ?= Character
 
GETC            PROC    NEAR
                push    bp
                mov     ax, 100h        ; get keyboard buffer status
                int     16h
                jz      nochar          ; no character available
getc1:          xor     ax, ax
                int     16h             ; get character from buffer
                or      al, al          ; is is special ?
                jnz     not_spec        ; nope
                mov     bx, offset convert
                mov     cx, TABLESIZE
gl1:            cmp     ah, [bx]        ; find it in table ?
                je      found           ; yes
                add     bx, 2           ; pt to next table entry
                loop    gl1             ; are we done with the table ?
found:          inc     bx              ; pt to conversion character
                mov     al, [bx]        ; and get it
not_spec:       xor     ah, ah          ; clean it up
getex:          pop     bp
                ret                     ; and send it back !
nochar:         xor     ax, ax          ; return nothing
                jmp     getex
GETC            ENDP
 
; Getcw waits for a key to be pressed and then converts it.
;  ch= getcw();
; RETURNS       converted character
 
GETCW           PROC    NEAR
                push    bp
                jmp     getc1
GETCW           ENDP
 
; Getcraw waits for a key to be pressed but doesn't convert it.
;  ch= getcraw()
; RETURNS:      al= 0   ah= special key
;               al= ?   ah= Scan code
 
GETCRAW         PROC    NEAR
                push    bp
                xor     ax, ax
                int     16h             ; wait for keyboard input
                pop     bp
                ret
GETCRAW         ENDP
                ENDPS
                END
 

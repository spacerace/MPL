                        TITLE   SOUND ROUTINE FOR LATTICE
                        NAME    SOUNDER
                        INCLUDE DOS.MAC
 
COMMENT #
                AUTHOR          Jon Wesener
                DATE            7 / 19 / 85
                PURPOSE         This routine will let you specify a
                                frequency and duration in .01 seconds.
        #
 
                PSEG
                PUBLIC  SOUND
; Sound lets you generate a frequency with a given duration in hundredths
; of seconds.
;  sound(frequency, duration);
; RETURN:       sound
 
SOUND           PROC    NEAR
                push    bp
                mov     bp, sp
                mov     di, [bp+4]
                mov     bx, [bp+6]
                mov     al, 0b6h        ; write timer mode register
                out     43h, al
                mov     dx, 14h         ; timer divisor=
                mov     ax, 4f38h       ; 1331000/ frequency
                div     di
                out     42h, al         ; write timer 2 count low byte
                mov     al, ah
                out     42h, al         ; write timer 2 count high byte
                in      al, 61h         ; get current port B setting
                mov     ah, al
                or      al, 3           ; turn speaker on
                out     61h, al
wait:           mov     cx, 2801        ; wait 10 milliseconds
spkr_on:        loop    spkr_on
                dec     bx              ; speaker on count expired
                jnz     wait            ; no. keep it on!
                mov     al, ah          ; otherwise restore it.
                out     61h, al
                pop     bp
                ret
SOUND           ENDP
                ENDPS
                END

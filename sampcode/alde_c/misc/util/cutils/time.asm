                        TITLE   Time Routines for Lattice
                        NAME    TIME
                        INCLUDE DOS.MAC
 
COMMENT $
                AUTHOR          Jon Wesener
                DATE            7 / 22 /85
                PURPOSE           These routines will provide access to
                                the onboard clock routines.
        $
 
                PSEG
                PUBLIC  GETTIME, SETTIME
 
; Gettime returns the current time as stated by the PC
;  time(&hour, &min, &sec, &hun);
; RETURN:       nothing
 
GETTIME         PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, 2c00h       ; get the time
                int     21h
                xor     ax, ax
                mov     di, [bp+4]      ; point to hours
                mov     al, ch
                stosw                   ; send hours home
                mov     di, [bp+6]      ; pt to minutes
                mov     al, cl
                stosw                   ; send back minutes
                mov     di, [bp+8]
                mov     al, dh
                stosw                   ; same for seconds
                mov     di, [bp+10]
                mov     al, dl
                stosw                   ; and for hundredths
                pop     bp
                ret
GETTIME         ENDP
 
; Settime allows you to set the time
;  err= settime(hour, min, sec, hun);
; RETURN:       0= Success      ?= Error
 
SETTIME         PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, 2d00h
                mov     cx, [bp+6]      ; get minutes
                mov     ch, [bp+4]      ; get hours
                mov     dx, [bp+10]     ; get hundredths
                mov     dh, [bp+8]      ; get seconds
                int     21h
                pop     bp
                ret
SETTIME         ENDP
                ENDPS
                END

                        TITLE   Move commands for 11:11
                        NAME    MOVED
                        INCLUDE DOS.MAC
COMMENT $
                AUTHOR          Jon Wesener
                DATE            7 / 26 / 85
                PURPOSE            This bit of code will allow me to
                                fill a buffer with data from any segment.
        $
 
 
                PSEG
                PUBLIC  MOVE
; Move will move count number of bytes from segment:offset to buffer.
;  move(buffer, seg, offs, count);
; return        nothing
; NOTE:         If an invalid segment is used, I have no idea what
;               will be returned
 
MOVE            PROC    NEAR
                push    bp
                mov     bp, sp
                mov     di, [bp+4]      ; get destination buffer
                mov     ds, [bp+6]      ; get segment to move
                mov     si, [bp+8]      ; get offset for move
                mov     cx, [bp+10]     ; get count
                rep     movsb           ; move it
                pop     bp
                ret
MOVE            ENDP
                ENDPS
                END

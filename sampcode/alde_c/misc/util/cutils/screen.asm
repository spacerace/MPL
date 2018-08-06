                                TITLE   Screen Subroutines for Lattice
                                NAME    SCREEN
                                INCLUDE DOS.MAC
 
COMMENT *
                AUTHOR          Jon Wesener
                DATE            7 / 9 / 85
                PURPOSE            These routines are to be used for
                                controlling the display.
        *
 
                DSEG
scr_map         dw      0b800h          ; page 1
                dw      0b800h + 250    ; page 2
                dw      0b800h + 500    ; page 3
                dw      0b800h + 750    ; page 4
                ENDDS
 
                PSEG
                PUBLIC  ATPUTS, CLS, CUR_ON, CUR_OFF, SET_CUR, ATPUTC
                PUBLIC  SCRL_UP, SCRL_DWN, SET_PAGE, SET_MODE
 
; atputs will put a string with an attribute at the specified cursor position
;  atputs(string, page, row, col, attr)
; RETURN: nothing
 
ATPUTS          PROC    NEAR
                push    bp
                mov     bp, sp
                mov     bx, [bp+6]      ; get page ptr
                shl     bx, 1           ; convert to table arg
                mov     es, scr_map[bx] ; get screen segment
                mov     ax, [bp+8]      ; get row
                mov     cl, 80          ; 80 characters per column
                mul     cl              ; figure row offset into page
                add     ax, [bp+10]     ; add the column offset
                shl     ax, 1           ; adjust for attribute bytes
                mov     di, ax          ; destination
                mov     ah, [bp+12]     ; get attribute
                mov     si, [bp+4]      ; get string
atl1:           lodsb
                and     al, al          ; is it the end ?
                jz      atex1           ; yes
                stosw                   ; write it to the screen
                jmp     atl1
atex1:          push    ds
                pop     es              ; reset the extra segment
                pop     bp
                ret
ATPUTS          ENDP
 
; atputc puts a character with the given attribute on the screen
;  atputc(character, page, row, col, attr)
; RETURN: nothing
 
ATPUTC          PROC    NEAR
                push    bp
                mov     bp, sp
                mov     bx, [bp+6]      ; get page ptr
                shl     bx, 1           ; convert to table arg
                mov     es, scr_map[bx] ; get screen segment
                mov     ax, [bp+8]      ; get row
                mov     cl, 80          ; 80 characters per column
                mul     cl              ; figure row offset into page
                add     ax, [bp+10]     ; add the column offset
                shl     ax, 1           ; adjust for attribute bytes
                mov     di, ax          ; destination
                mov     ax, [bp+4]      ; get character
                mov     ah, [bp+12]     ; get attribute
                stosw                   ; write it to the screen
atex:           push    ds
                pop     es              ; restore extra segment
                pop     bp
                ret
ATPUTC          ENDP
 
; CLS clears the requested page in 80X25 mode.
;  cls(page);
; RETURN: nothing- if it does return.
 
 
; SET_CUR sets the cursor of the requested page.
;  set_cur(row, column, page)
; RETURN: nothing
 
SET_CUR         PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, 200h        ; set cursor
                mov     bx, [bp+8]      ; get page
                mov     dx, [bp+6]      ; get column
                mov     dh, [bp+4]      ; get row
                int     10h             ; change it
                pop     bp
                ret
SET_CUR         ENDP
 
; CUR_ON turns the cursor on.
;  cur_on()
; RETURN: nothing
 
CUR_ON          PROC    NEAR
                push    bp
                mov     ax, 100h        ; set cur type
                mov     cx, 607h        ; last two rows
                int     10h             ; change it
                pop     bp
                ret
CUR_ON          ENDP
 
; Cur_off turns the cursor off
;  cur_off()
; RETURN: nothing
 
CUR_OFF         PROC    NEAR
                push    bp
                mov     ax, 100h        ; set cur type
                mov     cx, 0f00h       ; this should turn it off
                int     10h
                pop     bp
                ret
CUR_OFF         ENDP
 
; Set_mode sets the video controller to the desired mode.
;  set_mode(mode)
; RETURNS: nothing
 
; valid modes are 0 - 7
;               0=      40X25 BW
;               1=      40X25 Color
;               2=      80X25 BW
;               3=      80X25 Color
;               4=      320X200 Color           GRAPHICS **
;               5=      320X200 BW              GRAPHICS **
;               6=      640X200 BW              GRAPHICS **
;               7=      80X25 BW (internal card)
 
SET_MODE        PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, [bp+4]      ; set the graphics mode
                int     10h             ; now !
                pop     bp
                ret
SET_MODE        ENDP
 
CLS             PROC    NEAR
                push    bp              ;save from video call
; get screen info
                mov     ax, 0f00h
                int     10h
; clear screen
                dec     ah              ; columns -1
                mov     dl, ah
                mov     dh, 24          ; rows - 1
                mov     ax, 600h        ;ask for a clear window
                xor     cx, cx          ;start at 0,0
                mov     bh, 7           ;attributes for new blanks
                int     10h             ;do the clear
; home cursor
                xor     dx, dx          ; column & row = (0,0)
                xor     bx, bx          ; force page zero
                mov     ax, 200h        ; set cursor location
                int     10h             ; call bios
                pop     bp
                ret
CLS             ENDP
 
; scrl_up will scroll the active screen page up
;  scrl_up(no_lines, urow, ucol, drow, dcol, battr);
; RETURN: nothing
 
SCRL_UP         PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, [bp+4]      ; get the number of lines
                mov     ah, 6           ; screen scroll function
                mov     cx, [bp+8]      ; get upper left column
                mov     ch, [bp+6]      ; get upper left row
                mov     dx, [bp+12]     ; get lower right column
                mov     dh, [bp+10]     ; get lower right row
                mov     bx, [bp+14]     ; get blank line attribute
                int     10h             ; do it
                pop     bp
                ret
SCRL_UP         ENDP
 
; scrl_dwn will scroll the active screen page down
;  scrl_dwn(no_lines, urow, ucol, drow, dcol, battr);
; RETURN: nothing
 
SCRL_DWN        PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, [bp+4]      ; get the number of lines
                mov     ah, 7           ; screen scroll function
                mov     cx, [bp+8]      ; get upper left column
                mov     ch, [bp+6]      ; get upper left row
                mov     dx, [bp+12]     ; get lower right column
                mov     dh, [bp+10]     ; get lower right row
                mov     bx, [bp+14]     ; get blank line attribute
                int     10h             ; do it
                pop     bp
                ret
SCRL_DWN        ENDP
 
; set_page sets the current displayed page
;  set_page(page)
; RETURN: nothing
 
SET_PAGE        PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, [bp+4]      ; get the page
                mov     ah, 5           ; screen function to set page
                int     10h
                pop     bp
                ret
SET_PAGE        ENDP
                ENDPS
                END

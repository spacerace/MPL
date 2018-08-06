                        TITLE   Windowing Character Device
                        NAME    WINDOW
 
COMMENT %
                AUTHOR          Jon Wesener
                DATE            7 / 22 / 85
                PURPOSE           This will allow me to control the
                                scrolling regions on the screen when
                                everyone else is writing to stdout.
                                (C) Copyright 1985, BEYOND CONTROL Software.
        %
 
 
CODE            SEGMENT BYTE
                ASSUME  CS:CODE, DS:NOTHING, ES:NOTHING
 
windev:                                 ; Header for Window Device
                dd      -1
                dw      1000000000010000B       ; Con in and Con out & special
                dw      STRATEGY
                dw      ENTRY
                db      'WINDOW  '
 
; ---------------------------------------------------------
;       Device Data Structures
ESC             EQU     27
CMD             EQU     2       ; function to perform
STATUS          EQU     3       ; function return status
COUNT           EQU     18      ; ptr to read / write count
TRANS           EQU     14      ; more information for each function
MEDIA           EQU     13      ; Media descriptor
rh_ptr          dd      ?       ; pointer to request header
altah           db      0       ; holds special character
row             db      0       ; current row
col             db      0       ; current col
urow            db      0       ; upper left row of window
ucol            db      0       ; upper left col of window
lrow            db      24      ; lower right row of window
lcol            db      79      ; lower right col of window
rowcol          dw      ?       ; current row and column
state           dw      s1      ; offset to escape processing
attpag          label   word
attr            db      7       ; attribute of character
paged           db      0       ; starting on page zero
 
; ---------------------------------------------------------
;       Command Jump Tables
cmdtbl:
                dw      CON$INIT
                dw      EXIT
                dw      EXIT
                dw      CMDERR
                dw      CMDERR
                dw      CMDERR
                dw      EXIT
                dw      CMDERR
                dw      CON$WRIT
                dw      CON$WRIT
                dw      EXIT
                dw      EXIT
 
STRATEGY        PROC    FAR
                mov     word ptr cs:rh_ptr, bx
                mov     word ptr cs:rh_ptr+2, es
                ret
STRATEGY        ENDP
 
ENTRY           PROC    FAR
                push    ax
                push    bx
                push    cx
                push    dx              ; Save all fucking regs
                push    di
                push    si
                push    bp
                push    ds
                push    es
 
                lds     bx, cs:rh_ptr
                xor     ax, ax          ; use command as a pointer
                mov     al, ds:[bx].CMD
                cmp     al, 11          ; we eleven commands
                ja      cmderr
 
                mov     cx, ds:[bx].COUNT
                les     di, dword ptr ds:[bx].TRANS
 
                shl     ax, 1           ; word addresses
 
                mov     si, offset cmdtbl
                add     si, ax
 
; ---------------------------------------------------------
;       Set Data Segment For Our Routines
                push    cs
                pop     ds
 
                ASSUME  DS:CODE
; ---------------------------------------------------------
;       Call Specific Routine
                jmp     word ptr [si]
 
; ---------------------------------------------------------
;       Specific routine done, Return to DOS
 
bus$exit:       mov     ah, 11b         ; Device done & busy
                jmp     err1
 
cmderr:         mov     al, 3           ; Unknown Command Error
 
err$exit:       mov     ah, 10000001b   ; Device done and error occurred
                jmp     err1
 
exit:           mov     ah, 1b          ; Device done and SUCCESSFUL
err1:           lds     bx,cs:rh_ptr    ; get ptr to request header
                mov     ds:[bx].STATUS, ax
                                        ; Mark operation complete
                pop     es
                pop     ds
                pop     bp
                pop     si
                pop     di
                pop     dx              ; restore same regs
                pop     cx
                pop     bx
                pop     ax
                ret
ENTRY           ENDP
 
; ---------------------------------------------------------
;       Console Write Routine
COMMENT $
       Note:    The following <ESC>APE sequences have the following effects
 
        <ESC> A#        - Sets the Attribute of the written characters.
        <ESC> C####     - Creates a window of following bytes size
                          clears the window and homes the cursor inside it.
        <ESC> E         - Erases the current window.
        <ESC> P#        - Sets the Page of the displayed screen.
        $
 
CON$WRIT        PROC    NEAR
                jcxz    exit            ; don't want anything
                push    cx              ; save count
                mov     ah, 3           ; get current cursor position
                mov     bh, paged       ; get for our page
                int     10h             ; get it
                cmp     dh, urow
                jb      resetcurs       ; its below the window
                cmp     dh, lrow
                ja      resetcurs       ; its above the window
                cmp     dl, ucol
                jb      resetcurs       ; its to the left of window
                cmp     dl, lcol
                ja      resetcurs       ; its to the right of window
writc1:         mov     rowcol, dx      ; save row
                pop     cx              ; get count
con$lp:         mov     al, es:[di]     ; get character
                inc     di              ; point to the next one
                push    cx
                push    di
                push    es
                call    video           ; output character
                pop     es
                pop     di
                pop     cx
                loop    con$lp          ; until all are done
                jmp     exit
 
resetcurs:      mov     dh, urow
                mov     dl, ucol        ; home cursor
                mov     bh, paged       ; get current page
                mov     ax, 200h
                int     10h             ; change it
                jmp     writc1
CON$WRIT        ENDP
 
VIDEO           PROC    NEAR
                mov     si, offset state
                jmp     word ptr [si]   ; go to proper routine
 
s1:             cmp     al, ESC
                jnz     s1b
                mov     word ptr [si], offset s2
                ret
 
s1b:            call    chrout
s1a:            mov     state, offset s1       ; put us to a low state
                ret
 
; ---------------------------------------------------------
;       Process Escape Sequence
s2:             cmp     al, 'A'         ; change attribute ?
                jnz     cretwin         ; try create window
                mov     state, offset chgatt
                ret
chgatt:         mov     attr, al        ; save new attribute
                jmp     s1a             ; were out of the sequence
 
; ---------------------------------------------------------
;       Create new window
 
cretwin:        cmp     al, 'C'         ; create window ?
                jnz     erasewin        ; try change page
                mov     state, offset cret1
                ret
 
cret1:          mov     urow, al        ; set upper row
                mov     state, offset cret2
                ret
cret2:          mov     ucol, al        ; set upper col
                mov     state, offset cret3
                ret
cret3:          mov     lrow, al        ; set lower row
                mov     state, offset cret4
                ret
cret4:          mov     lcol, al        ; set lower col
                jmp     s1a             ; we're done.
 
; ---------------------------------------------------------
;       Erase window
erasewin:       mov     al, ucol
                mov     col, al
                mov     al, urow
                mov     row, al         ; home the cursor
                mov     ax, 600h
                mov     state, offset s1a
                jmp     cls
 
; ---------------------------------------------------------
;       Set new page
chgpage:        cmp     al, 'P'
                jnz     s1avec          ; invalid sequence, ignore it
                mov     state, offset newpage
                ret
newpage:        cmp     al, 4
                ja      s1a             ; invalid page number
                mov     paged, al       ; store new page
                mov     ah, 5
                int     10h             ; set active page!
s1avec:         jmp     s1a             ; reset escape state
VIDEO           ENDP
 
CHROUT          PROC    NEAR
                cmp     al, 13          ; Carriage return
                jnz     trytab
                mov     al, ucol        ; get left margin
                mov     col, al         ; for set it
                jmp     setit
 
trytab:         cmp     al, 9           ; tab
                jnz     trylf
                mov     al, col
                add     al, 8
                and     al, 0f8h        ; compute new col
                mov     col, al
                jmp     bndchk          ; do a bounds check
 
trylf:          cmp     al, 10          ; line feed
                jz      lf              ; yes
                cmp     al, 7           ; bell ?
                jnz     outchr
 
torom:          mov     bx, attpag      ; get attribute and page
                mov     ah, 14
                int     10h             ; send it to rom
                ret
 
outchr:         mov     bx, attpag      ; get attribute and page
                mov     cx, 1
                mov     ah, 9
                int     10h
                inc     col
bndchk:         mov     al, col
                cmp     al, lcol        ; hit max col ?
                jbe     setit
 
                mov     al, ucol
                mov     col, al         ; we're wrapping
 
lf:             inc     row             ; let's see how we do
                mov     al, row
                cmp     al, lrow
                jbe     setit
                mov     al, lrow
                mov     row, al         ; we form feeded
scrollup:       mov     ax, 601h        ; scroll up 1 line
cls:            mov     ch, urow
                mov     cl, ucol
                mov     dh, lrow
                mov     dl, lcol
                mov     bh, attr
                int     10h             ; scroll it up
setit:          mov     dh, row
                mov     dl, col
                mov     bh, paged
                mov     ah, 2
                int     10h             ; set cursor position
                ret
CHROUT          ENDP
 
CON$INIT        PROC    NEAR
                lds     bx, cs:rh_ptr
                mov     word ptr [bx].TRANS, offset CON$INIT
                mov     [bx].TRANS+2, cs
                jmp     exit
CON$INIT        ENDP
CODE            ENDS
                END

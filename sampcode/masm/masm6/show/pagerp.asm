;* PAGERP.ASM - Module containing routines for paging through a file and
;* writing text to the screen buffer. Works with main module SHOWP.ASM.

        TITLE   Pager
        .MODEL  small, pascal, os_os2
        .286

INCL_NOCOMMON   EQU 1         ; Enable call groups
INCL_VIO        EQU 1

        INCLUDE os2.inc
        INCLUDE show.inc

        .CODE

;* Pager - Displays status line and all the text lines for a screen.
;*
;* Params: cLines - lines to scroll (negative up, positive down)
;*
;* Uses:   Global variables: segBuf, offBuf, yCur
;*
;* Return: None

Pager   PROC,
        cLines:SWORD

        mov     es, segBuf              ; Initialize buffer position
        mov     di, offBuf

        mov     cx, cLines              ; Get line count
        mov     ax, 10                  ; Search for linefeed

        or      cx, cx                  ; Argument 0?
        jl      backward                ; If below, backward
        jg      foreward                ; If above, forward
        jmp     showit                  ; If equal, done

backward:
        call    GoBack                  ; Adjust backward
        jmp     showit                  ; Show screen

foreward:
        call    GoForeward              ; Adjust forward

; Write line number to status line

showit:
        cld                             ; Forward
        push    di                      ; Save
        push    ds                      ; ES = DS
        pop     es

        INVOKE  BinToStr,               ; Write line number as string
                yCur,
                ADDR stLine[LINE_POS]

; Fill in status line

        mov     cx, 6                   ; Six spaces to fill
        sub     cx, ax                  ; Subtract those already done
        mov     al, ' '                 ; Fill with space
        rep     stosb

        INVOKE  VioWrtCharStrAtt,       ; Write to screen
                ADDR stLine,
                X_MAX,
                0,
                0,
                ADDR atSta,
                0

        pop     di                      ; Update position
        mov     si, di
        mov     cx, yMax                ; Lines per screen

        .REPEAT
        mov     bx, yMax                ; Lines per screen
        inc     bx                      ; Adjust for 0
        sub     bx, cx                  ; Calculate current row
        push    cx                      ; Save line number
        mov     es, segBuf              ; Reload

        INVOKE  ShowLine,               ; Write line to screen
                es::si,                 ; Pointer to current position
                bx,                     ; Line number
                cbBuf,                  ; File length (for bounds check)
                ADDR atScr              ; Attribute

        pop     cx                      ; Restore line number
        mov     si, ax                  ; Get returned position

        dec     cx                      ; Count the line
        .UNTIL  (ax >= cbBuf) || !cx    ; Continue if more lines and not
        jcxz    exit                    ; Done if more lines,
                                        ;   else fill screen with spaces
        mov     ax, X_MAX               ; Columns times remaining lines
        mul     cl
        mov     dx, ax                  ; INVOKE uses AX, so use DX
        sub     cx, yMax                ; Calculate starting line
        neg     cx
        inc     cx

        INVOKE  VioWrtNCell,            ; Write space cells
                ADDR celScr,            ; Cell of space and attribute
                dx,                     ; Number of cells to fill
                cx,                     ; Line to start fill
                0,                      ; Column 0
                0                       ; Console handle
exit:
        ret

Pager   ENDP


;* ShowLine - Writes a line of text to the screen.
;*
;* Params: pchIn - Far pointer to input text
;*         y - Line number
;*         cbMax - Maximum number of characters (file length)
;*         pcelAtrib - Far pointer to attribute
;*
;* Return: None

ShowLine PROC USES si di,
        pchIn:PBYTE,
        y:WORD,
        cbMax:WORD,
        pcelAtrib:PBYTE

        LOCAL   achOut[X_MAX]:BYTE

        push    ds                      ; Save
        push    ss                      ; ES = SS
        pop     es
        lea     di, achOut              ; Destination line
        lds     si, pchIn               ; Source line
        mov     cx, X_MAX               ; Cells per row
        mov     bx, di                  ; Save copy of start for tab calc
loop1:
        lodsb                           ; Get character
        cmp     al, 9                   ; Tab?
        je      filltab                 ; Space out tab
        cmp     al, 13                  ; CR?
        je      filleol                 ; Fill rest of line with spaces
        stosb                           ; Copy out
        cmp     si, cbMax               ; Check for end of file
        ja      filleol
        loop    loop1
loop2:
        lodsb                           ; Throw away rest of line to truncate
        cmp     si, cbMax               ; Check for end of file
        ja      exit
        cmp     al, 13                  ; Check for end of line
        jne     loop2
        inc     si                      ; Throw away line feed

        jmp     exit                    ; Done
filltab:
        push    bx                      ; Fill tab with spaces
        push    cx

        sub     bx, di                  ; Get current position in line
        neg     bx

        mov     cx, 8                   ; Default count 8
        and     bx, 7                   ; Get modulus
        sub     cx, bx                  ; Subtract
        mov     bx, cx                  ; Save modulus

        mov     al, ' '                 ; Write spaces
        rep     stosb

        pop     cx
        sub     cx, bx                  ; Adjust count
        .IF     sign?
        sub     cx, cx                  ; Make negative count 0
        .ENDIF

        pop     bx
        jcxz    loop2                   ; If beyond limit done
        jmp     loop1
filleol:
        inc     si                      ; After CR, throw away LF
        mov     al, ' '                 ; Fill rest of line
        rep     stosb
exit:
        pop     ds
        INVOKE  VioWrtCharStrAtt,
                ADDR achOut,
                X_MAX,
                y,
                0,
                pcelAtrib,
                0

        mov     ax, si                  ; Return position
        ret

ShowLine ENDP


        END

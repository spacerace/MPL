          TITLE   Pager
          .MODEL  small, pascal

INCL_VIO  EQU 1

          INCLUDE os2.inc
          .DATA
          EXTRN   stAtrib:BYTE, scAtrib:BYTE, Cell:WORD, stLine:BYTE
          EXTRN   sBuffer:WORD, oBuffer:WORD, Buffer:DWORD, lBuffer:WORD
          EXTRN   nLines:WORD, curLine:WORD 

          .CODE
          PUBLIC  Pager

; Procedure Pager
; Purpose   Displays status and text lines
; Input     Stack variable: lines to scroll (negative up, positive down)
;           Global variables: "sbuffer", "oBuffer", "curLine"
; Output    To screen

Pager     PROC    count

          mov     es, sBuffer           ; Initialize buffer position
          mov     di, oBuffer

          mov     cx, count             ; Get count argument
          mov     ax, 10                ; Search for linefeed

          or      cx, cx                ; Argument 0?
          jl      skip1                 ; If below, backward
          jg      skip2                 ; If above, forward
          jmp     SHORT skip3           ; If equal, done

skip1:    call    GoBack                ; Adjust backward
          jmp     SHORT skip3           ; Show screen
skip2:    call    GoForwd               ; Adjust forward

; Write line number to status line

skip3:    cld                           ; Go forward
          push    di                    ; Save
          push    ds                    ; ES = DS
          pop     es

; BinToStr (curLine, OFFSET stLine[6])

          push    curLine               ; Arg 1
          @Pushc  <OFFSET stLine[6]>    ; Arg 2
          call    BinToStr              ; Convert to string

; Fill in status line

          mov     cx, 6                 ; Six spaces to fill
          sub     cx, ax                ; Subtract those already done
          mov     al, " "               ; Fill with space
          rep     stosb

          @VioWrtCharStrAtt stLine, 80, 0, 0, stAtrib, 0 ; Write to screen

          pop     di                    ; Update position
          mov     si, di
          mov     cx, nLines            ; Lines per screen

loop1:    mov     bx, nLines            ; Lines of text
          inc     bx                    ; Adjust for 0
          sub     bx, cx                ; Calculate current row
          push    cx                    ; Save line number

; ShowLine (position, line, maxlength, &scAtrib)

          push    sBuffer               ; Arg 1
          push    si
          push    bx                    ; Arg 2
          push    lBuffer               ; Arg 3
          push    ds                    ; Arg r4
          @Pushc  <OFFSET scAtrib>
          call    ShowLine              ; Write line

          pop     cx                    ; Restore line number
          mov     si, ax                ; Get returned position

          cmp     ax, lBuffer           ; If beyond end of file,
          jae     skip4                 ;   fill screen with spaces
          loop    loop1                 ; else next line
          jmp     SHORT exit            ; Exit if done

; Fill the rest with spaces

skip4:    dec     cx
          jcxz    exit
          mov     ax, 80                ; Columns times remaining lines
          mul     cl
          mov     dx, ax                ; Macros use AX, so use DX
          sub     cx, nLines            ; Calculate starting line
          neg     cx
          inc     cx

          @VioWrtNCell Cell, dx, cx, 0, 0 ; Write space cells

exit:     ret
Pager     ENDP

; Procedure ShowLine (inLine, sRow, &pAtrib)
; Purpose   Writes a line to screen
; Input     Stack variables: 1 - FAR PTR to input line
;                            2 - line number
;                            3 - maximum number of characters (file length)
;                            4 - FAR PTR to attribute
; Output    Line to screen

ShowLine  PROC    USES si di, inLine:FAR PTR BYTE, srow, max, pAtrib:FAR PTR BYTE
          LOCAL   outLine[80]:BYTE

          push    ds                    ; Save
          push    ss                    ; ES = SS
          pop     es
          lea     di, outLine           ; Destination line
          lds     si, inLine            ; Source line
          mov     cx, 80                ; Cells per row
          mov     bx, di                ; Save copy of start for tab calc
loop1:    lodsb                         ; Get character
          cmp     al, 9                 ; Tab?
          je      skip1                 ; Space out tab
          cmp     al, 13                ; CR?
          je      skip3                 ; Fill rest of line with spaces
          stosb                         ; Copy out
          cmp     si, max               ; Check for end of file
          ja      skip3
          loop    loop1

loop2:    lodsb                         ; Throw away rest of line to truncate
          cmp     si, max               ; Check for end of file
          ja      exit
          cmp     al, 13                ; Check for end of line
          jne     loop2
          inc     si                    ; Throw away line feed

          jmp     SHORT exit            ; Done

skip1:    push    bx                    ; Fill tab with spaces
          push    cx

          sub     bx, di                ; Get current position in line
          neg     bx

          mov     cx, 8                 ; Default count 8
          and     bx, 7                 ; Get modulus
          sub     cx, bx                ; Subtract
          mov     bx, cx                ; Save modulus

          mov     al, " "               ; Write spaces
          rep     stosb

          pop     cx
          sub     cx, bx                ; Adjust count
          jns     skip2                 ; Make negative count 0
          sub     cx, cx
skip2:    pop     bx
          jcxz    loop2                 ; If beyond limit done
          jmp     SHORT loop1

skip3:    inc     si                    ; After CR, throw away LF
          mov     al, ' '               ; Fill rest of line
          rep     stosb 

exit:     pop     ds
          @VioWrtCharStrAtt outLine, 80, [srow], 0, [pAtrib], 0
          
          mov     ax, si                ; Return position
          ret
ShowLine  ENDP

; Procedure GoBack
; Purpose   Searches backward through buffer
; Input     CX has number of lines; ES:DI has buffer position
; Output    Updates "curLine" and "oBuffer"

GoBack    PROC
          std                           ; Go backward
          neg     cx                    ; Make count positive
          mov     dx, cx                ; Save a copy
          inc     cx                    ; One extra to go up one
          or      di, di                ; Start of file?
          je      exit                  ; If so, ignore
loop1:    push    cx                    ;   else save count
          mov     cx, 0FFh              ; Load maximum character count
          cmp     cx, di                ; Near start of buffer?
          jl      skip1                 ; No? Continue
          mov     cx, di                ;   else search only to start
skip1:    repne   scasb                 ; Find last previous LF
          jcxz    skip4                 ; If not found, must be at start
          pop     cx
          loop    loop1
          cmp     curLine, -1           ; End of file flag?
          jne     skip2                 ; No? Continue
          add     di, 2                 ; Adjust for cr/lf
          mov     oBuffer, di           ; Save position
          call    EndCount              ; Count back to get line number
          ret

skip2:    sub     curLine, dx           ; Calculate line number
          jg      skip3
          mov     curLine, 1            ; Set to 1 if negative
skip3:    add     di, 2                 ; Adjust for cr/lf
          mov     oBuffer, di           ; Save position
          ret

skip4:    pop     cx
          sub     di, di                ; Load start of file
          mov     curLine, 1            ; Line 1
          mov     oBuffer, di           ; Save position
exit:     ret
GoBack    ENDP

; Procedure GoForwd
; Purpose   Searches forward through a buffer
; Input     CX has number of lines; ES:DI has buffer position
; Output    Updates "curLine" and "oBuffer"

GoForwd   PROC
          cld                           ; Go forward
          mov     dx, cx                ; Copy count
loop1:    push    cx                    ; Save count
          mov     cx, 0FFh              ; Load maximum character count
          mov     bx, lBuffer           ; Get end of file

          sub     bx, di                ; Characters to end of file
          cmp     cx, bx                ; Less than maximum per line?
          jb      skip1
          mov     cx, bx
skip1:    repne   scasb                 ; Find next LF
          jcxz    exit                  ; If not found, must be at end
          cmp     di, lBuffer           ; Beyond end?
          jae     exit
          pop     cx
          loop    loop1
          add     curLine, dx           ; Calulate line number
          mov     oBuffer, di           ; Save position
          ret

exit:     pop     cx
          mov     di, oBuffer           ; Restore position
          ret
GoForwd   ENDP

; Procedure EndCount
; Purpose   Counts backward to count lines in file
; Input     ES:DI has buffer position
; Output    Modifies "curLine"

EndCount  PROC
          push    di

          mov     al, 13                ; Search for CR
          mov     curLine, 0            ; Initialize

loop1:    inc     curLine               ; Adjust count
          mov     cx, 0FFh              ; Load maximum character count
          cmp     cx, di                ; Near start of buffer?
          jl      skip1                 ; No? Continue
          mov     cx, di                ;   else search only to start
skip1:    repne   scasb                 ; Find last previous cr
          jcxz    exit                  ; If not found, must be at start
          jmp     SHORT loop1

exit:     pop     di
          ret
EndCount  ENDP

; Procedure BinToStr (number, string)
; Purpose   Converts integer to string
; Input     Stack arguments: 1 - Number to convert; 2 - Near address for write
; Output    AX has characters written

BinToStr  PROC    number, string:PTR BYTE

          mov     ax,number
          mov     di,string

          sub     cx, cx                ; Clear counter
          mov     bx, 10                ; Divide by 10

; Convert and save on stack backwards

loop1:    sub     dx, dx                ; Clear top
          div     bx                    ; Divide to get last digit as remainder
          add     dl, "0"               ; Convert to ASCII
          push    dx                    ; Save on stack
          or      ax, ax                ; Quotient 0?
          loopnz  loop1                 ; No? Get another

; Take off the stack and store forward

          neg     cx                    ; Negate and save count
          mov     dx, cx
loop2:    pop     ax                    ; Get character
          stosb                         ; Store it
          loop    loop2   
          mov     ax, dx                ; Return digit count

          ret     
BinToStr  ENDP

          END

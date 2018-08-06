;               Text Video Library

_TEXT   segment byte public 'CODE'      ; Place the code in the code segment
        assume  CS:_TEXT                ; Assume the CS register points to it

; _setMod(mode)
;
; Function: Set the display mode to that specified by mode.
;
; Algorithm: Call the ROM BIOS set mode function; int 10H, AH = 0.

        public  _setMod         ; Routine is available to other modules

        mode = 4                ; Offset from BP to parameter mode

_setMod proc near               ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        push    si
        push    di
        mov     al,[bp+mode]    ; Set AL to mode
        mov     ah,0            ; Set AH to 0 (set mode function)
        int     10H             ; Call ROM BIOS video interrupt
        pop     di
        pop     si
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_setMod endp                    ; End of subroutine

; _setCTyp(start,end)
;
; Function: Set a new cursor, which starts on scan line start and ends on
; scan line end.
;
; Algorithm: Call the ROM BIOS set cursor type function; int 10H, AH = 1.

        public  _setCTyp        ; Routine is available to other modules

        start = 4               ; Offset from BP to parameter start
        end = 6                 ; Offset to parameter end

_setCTyp proc near              ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     ch,[bp+start]   ; Set CH to start
        mov     cl,[bp+end]     ; Set CL to end
        mov     ah,1            ; Set AH to 1 (set cursor type function)
        int     10H             ; Call ROM BIOS video interrupt
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_setCTyp endp                   ; End of subroutine

; _setPos(page,row,col)
;
; Function: Set the cursor position on the page specified to the row and
; column specified.
;
; Algorithm: Call the ROM BIOS set cursor position function; int 10H, AH = 2.

        public  _setPos         ; Routine is available to other modules

        page = 4                ; Offset from BP to parameter page
        row = 6                 ; Offset to parameter row
        col = 8                 ; Offset to col

_setPos proc near               ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        push    si
        push    di
        mov     bh,[bp+page]    ; Set BH to the page
        mov     dh,[bp+row]     ; Set DH to the row
        mov     dl,[bp+col]     ; Set DL to the column
        mov     ah,2            ; Set AH to 2 (set cursor position function)
        int     10H             ; Call ROM BIOS video interrupt
        pop     di
        pop     si
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_setPos endp                    ; End of subroutine

; _getPos(page,&rowPtr,&colPtr,&startPtr,&endPtr)
;
; Function: Get the current cursor. This includes getting the row and
; column, as well as the start and end scan lines of the cursor. The
; page parameter tells which page's cursor to get.
;
; Algorithm: Call the ROM BIOS get cursor function; int 10H, AH = 3.

        public  _getPos         ; Routine is available to other modules

        page = 4                ; Offset from BP to parameter page
        rowPtr = 6              ; Offset to parameter rowPtr
        colPtr = 8              ; Offset to colPtr
        startPtr = 10           ; Offset to startPtr
        endPtr = 12             ; Offset to endPtr

_getPos proc near               ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     bh,[bp+page]    ; Set BH to the page
        mov     ah,3            ; Set AH to 3 (get cursor function)
        int     10H             ; Call ROM BIOS video interrupt
        xor     al,al           ; Zero AL
        mov     bx,[bp+rowPtr]  ; *rowPtr = row number (with top byte zeroed)
        mov     [bx],dh
        mov     [bx+1],al
        mov     bx,[bp+colPtr]  ; *colPtr = column number (with zero top byte)
        mov     [bx],dl
        mov     [bx+1],al
        mov     bx,[bp+startPtr]; *startPtr = cursor scan line start
        mov     [bx],ch
        mov     [bx+1],al
        mov     bx,[bp+endPtr]  ; *endPtr = cursor scan line end
        mov     [bx],cl
        mov     [bx+1],al
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_getPos endp                    ; End of subroutine

; _getLite(&rowPtr,&colPtr,&rasterPtr,&pixColPtr)
;
; Function: If the light pen button is not depressed, return FALSE.
; Otherwise, return TRUE and the location of the press in the locations
; pointed to by rowPtr, colPtr, rasterPtr, and pixColPtr.
;
; Algorithm: Call the ROM BIOS get light pen function; int 10H, AH = 4.

        public  _getLite        ; Routine is available to other modules

        rowPtr = 4              ; Offset from BP to parameter rowPtr
        colPtr = 6              ; Offset to parameter colPtr
        rasterPtr = 8           ; Offset to rasterPtr
        pixColPtr = 10          ; Offset to pixColPtr

_getLite proc near              ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     ah,4            ; Set AH to 4 (get light pen function)
        int     10H             ; Call ROM BIOS video interrupt
        or      ah,ah           ; Is the button down?
        mov     ax,0            ; Assume not (i.e., return FALSE)
        jz      glDone          ; Branch if it indeed wasn't down
        push    bx              ; Save the BX register (the pixel column)
        mov     bx,[bp+rowPtr]  ; *rowPtr = row number, with top byte zeroed
        mov     [bx],dh
        mov     [bx+1],al
        mov     bx,[bp+colPtr]  ; *colPtr = column number
        mov     [bx],dl
        mov     [bx+1],al
        mov     bx,[bp+rasterPtr] ; *rasterPtr = raster line
        mov     [bx],ch
        mov     [bx+1],al
        pop     ax              ; Restore the pixel column into AX
        mov     bx,[bp+pixColPtr] ; *pixColPtr = pixel column
        mov     [bx],ax
        mov     ax,1            ; Return TRUE

glDone: pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_getLite endp                   ; End of subroutine

; _setPage(page)
;
; Function: Set the active display page.
;
; Algorithm: Call the ROM BIOS set page function; int 10H, AH = 5.

        public  _setPage        ; Routine is available to other modules

        page = 4                ; Offset from BP to parameter page

_setPage proc near              ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     al,[bp+page]    ; Set AL to the page number
        mov     ah,5            ; Set AH to 5 (set active page function)
        int     10H             ; Call ROM BIOS video interrupt
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_setPage endp                   ; End of subroutine

; _scrUp(n,ulRow,ulCol,lrRow,lrCol,attr)
;
; Function: Scroll a rectangular block of the display up by n lines. The
; block to scroll has an upper left corner of row ulRow and column ulCol,
; and a lower right corner of row lrRow and column lrCol. New lines are
; filled with blanks with attribute attr.
;
; Algorithm: Call the ROM BIOS scroll up function; int 10H, AH = 6.

        public  _scrUp          ; Routine is available to other modules

        n = 4                   ; Offset from BP to parameter n
        ulRow = 6               ; Offset to parameter ulRow
        ulCol = 8               ; Offset to ulCol
        lrRow = 10              ; Offset to lrRow
        lrCol = 12              ; Offset to lrCol
        attr = 14               ; Offset to attr

_scrUp  proc near               ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     al,[bp+n]       ; Set AL to the number of lines to scroll
        mov     ch,[bp+ulRow]   ; Set CH to the upper left row
        mov     cl,[bp+ulCol]   ; Set CH to the upper left column
        mov     dh,[bp+lrRow]   ; Set DH to the lower right row
        mov     dl,[bp+lrCol]   ; Set DL to the lower right column
        mov     bh,[bp+attr]    ; Set BH to the attribute to fill with
        mov     ah,6            ; Set AH to 6 (scroll up function)
        int     10H             ; Call ROM BIOS video interrupt
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_scrUp  endp                    ; End of subroutine

; _scrDn(n,ulRow,ulCol,lrRow,lrCol,attr)
;
; Function: Scroll a rectangular block of the display down by n lines. The
; block to scroll has an upper left corner of row ulRow and column ulCol,
; and a lower right corner of row lrRow and column lrCol. New lines are
; filled with blanks with attribute attr.
;
; Algorithm: Call the ROM BIOS scroll up function; int 10H, AH = 6.

        public  _scrDn          ; Routine is available to other modules

        n = 4                   ; Offset from BP to parameter n
        ulRow = 6               ; Offset to parameter ulRow
        ulCol = 8               ; Offset to ulCol
        lrRow = 10              ; Offset to lrRow
        lrCol = 12              ; Offset to lrCol
        attr = 14               ; Offset to attr

_scrDn  proc near               ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     al,[bp+n]       ; Set AL to the number of lines to scroll
        mov     ch,[bp+ulRow]   ; Set CH to the upper left row
        mov     cl,[bp+ulCol]   ; Set CH to the upper left column
        mov     dh,[bp+lrRow]   ; Set DH to the lower right row
        mov     dl,[bp+lrCol]   ; Set DL to the lower right column
        mov     bh,[bp+attr]    ; Set BH to the attribute to fill with
        mov     ah,7            ; Set AH to 7 (scroll down function)
        int     10H             ; Call ROM BIOS video interrupt
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_scrDn  endp                    ; End of subroutine

; _getACh(page,&chPtr,&attrPtr)
;
; Function: Get the character and the attributes of the character at the
; current cursor position in the specified page.
;
; Algorithm: Call the ROM BIOS get character/attribute function; int 10H,
; AH = 8.

        public  _getACh         ; Routine is available to other modules

        page = 4                ; Offset from BP to parameter page
        chPtr = 6               ; Offset to parameter chPtr
        attrPtr = 8             ; Offset to attrPtr

_getACh proc near               ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     bh,[bp+page]    ; Set BH to the page number
        mov     ah,8            ; Set AH to 8 (get char/attribute function)
        int     10H             ; Call ROM BIOS video interrupt
        mov     bx,[bp+chPtr]   ; *chPtr = character at cursor
        mov     [bx],al
        mov     bx,[bp+attrPtr] ; *attrPtr = attribute at cursor
        mov     [bx],ah
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_getACh endp                    ; End of subroutine

; _putACh(page,char,attr,n)
;
; Function: Put n copies of the character and attribute specified at the
; current cursor position of the specified page.
;
; Algorithm: Call the ROM BIOS put character/attribute function; int 10H, 
; AH = 9.

        public  _putACh         ; Routine is available to other modules

        page = 4                ; Offset from BP to parameter page
        char = 6                ; Offset to parameter ch
        attr = 8                ; Offset to attr
        n = 10                  ; Offset to n

_putACh proc near               ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     bh,[bp+page]    ; Set BH to the page number
        mov     cx,[bp+n]       ; Set CX to the number of chars to write
        mov     al,[bp+char]    ; Set AL to the character
        mov     bl,[bp+attr]    ; Set BL to the attribute
        mov     ah,9            ; Set AH to 9 (put char/attribute function)
        int     10H             ; Call ROM BIOS video interrupt
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_putACh endp                    ; End of subroutine

; _putOCh(page,char,n)
;
; Function: Write n copies of the character char to the current cursor
; position on the specified page. Leave the attribute as it was.
;
; Algorithm: Call the ROM BIOS write character function; int 10H, AH = 10.

        public  _putOCh         ; Routine is available to other modules

        page = 4                ; Offset from BP to parameter page
        char = 6                ; Offset to parameter ch
        n = 8                   ; Offset to n

_putOCh proc near               ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     bh,[bp+page]    ; Set BH to the page number
        mov     cx,[bp+n]       ; Set CX to the number of chars to write
        mov     al,[bp+char]    ; Set AL to the character to write
        mov     ah,10           ; Set AH to 10 (write character function)
        int     10H             ; Call ROM BIOS video interrupt
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_putOCh endp                    ; End of subroutine

; _putTty(page,char,color)
;
; Function: Write the character char to the current cursor position of the
; page specified; if in graphics mode, write it in the color given. Unlike,
; the putACh and putOCh functions, this function advances the cursor after
; writing, and interprets some control codes such as carriage return, line
; feed, and backspace.
;
; Algorithm: Call the ROM BIOS TTY write character function; int 10H, AH = 13.

        public  _putTty         ; Routine is available to other modules

        page = 4                ; Offset from BP to parameter page
        char = 6                ; Offset to parameter ch
        color = 8               ; Offset to color

_putTty proc near               ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        push    si
        push    di
        mov     bh,[bp+page]    ; Set BH to the page number
        mov     al,[bp+char]    ; Set AL to the character to write
        mov     bl,[bp+color]   ; Set BL to the foreground color to use
        mov     ah,14           ; Set AH to 14 (TTY write char function)
        int     10H             ; Call ROM BIOS video interrupt
        pop     di
        pop     si
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_putTty endp                    ; End of subroutine

; _getSt(&modePtr,&widthPtr,&pagePtr)
;
; Function: Get the current display mode, display width (in characters),
; and the active page number.
;
; Algorithm: Call the ROM BIOS get video state function; int 10H, AH = 15.

        public  _getSt          ; Routine is available to other modules

        modePtr = 4             ; Offset from BP to parameter modePtr
        widthPtr = 6            ; Offset to parameter widthPtr
        pagePtr = 8             ; Offset to pagePtr

_getSt  proc near               ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        mov     ah,15           ; Set AH to 15 (get video state function)
        int     10H             ; Call ROM BIOS video interrupt
        mov     dh,bh           ; Save BH in DH
        mov     bx,[bp+modePtr] ; *modePtr = the current video mode
        mov     [bx],al
        xor     al,al           ; Set AL to zero
        mov     [bx+1],al       ; Zero high byte of *modePtr
        mov     bx,[bp+widthPtr]; *widthPtr = ah (with top byte zero)
        mov     [bx],ah
        mov     [bx+1],al
        mov     bx,[bp+pagePtr] ; *pagePtr = the active page number
        mov     [bx],dh
        mov     [bx+1],al
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_getSt  endp                    ; End of subroutine

_TEXT   ends                    ; End of code segment

        end                     ; End of assembly code


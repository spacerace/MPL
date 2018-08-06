;               Rectangle Drawing Routine

_TEXT   segment byte public 'CODE'
        assume CS:_TEXT

; rect(left,top,right,bottom,pattern,globalPatt,mode) 
;
; Function: Draw a rectangle consisting of the bits (left,top) up to but 
; not including (right,bottom), filling the rectangle with the pattern 
; pointed to by pattern; if globalPatt is true, lock the pattern to a
; global reference otherwise start it at (left,top). mode specifies which 
; type of operation to use to combine the new and old dots: 0 for copy, 
; 1 for exclusive-or, 2 for bit-set (or), and 3 for bit-clear.
;
; Algorithm: First, clip the rectangle to the display. Second, figure out
; the start and end words and word masks. Third, rotate the pattern as
; needed. Fourth, branch to and perform the appropriate type of copying.
; Finally, restore the saved registers and exit.

        public _rect            ; Routine is available to other modules

; Parameters:

        left = 4                ; Offset from BP to parameter left
        top = 6                 ; Offset to parameter top
        right = 8               ; Offset to right
        bottom = 10             ; Offset to bottom
        pattern = 12            ; Offset to pattern
        globPatt = 14           ; Offset to globPatt
        mode = 16               ; Offset to mode

; Local variables:

        lStrt = 2               ; Offset from BP to lStrt local variable
        lEnd = 4                ; Offset to lEnd local variable
        lSMask = 6              ; Offset to lSMask
        lEMask = 8              ; Offset to lEMask
        lCnt = 10               ; Offset to lCnt
        pattCnt = 12            ; Offset to pattCnt
        clipLeft = 14           ; Offset to clipLeft
        clipTop = 16            ; Offset to clipTop
        clipRight = 18          ; Offset to clipRight
        clipBottom = 20         ; Offset to clipBottom
        pattRot = 52            ; Offset to pattRot

_rect   proc near               ; NEAR type subroutine
        push    bp              ; Save BP register
        mov     bp,sp           ; Set BP to SP; to allow access to parameters
                                ;   and local variables
        sub     sp,52           ; Allocate space on the stack for loca vars
        push    di              ; Save the DI register
        push    si              ; Save SI register
        push    es              ; Save ES register
        mov     ax,0B800H       ; Set ES to the base of the graphics display
        mov     es,ax
;
; Clip left to produce clipLeft:
;
        mov     ax,[bp+left]    ; AX = left
        cmp     ax,640          ; Is AX >= 640 (i.e., off screen right)?
        jge     getOut1         ; If yes, there's nothing to do -- exit
        or      ax,ax           ; Is AX < 0 (i.e., off screen left)?
        jns     clip1           ; If no, leave it alone
        xor     ax,ax           ; If yes, set it to zero
clip1:  mov     [bp-clipLeft],ax ; Set clipLeft
;
; Clip top to produce clipTop:
;
        mov     ax,[bp+top]     ; AX = top
        cmp     ax,200          ; Is AX >= 200 (i.e., off screen bottom)?
        jge     getOut1         ; If yes, there's nothing to do, so exit
        or      ax,ax           ; Is AX < 0 (i.e., off screen top)?
        jns     clip2           ; If no, leave it alone
        xor     ax,ax           ; If yes, set it to zero
clip2:  mov     [bp-clipTop],ax ; Set clipTop
;
; Clip right to produce clipRight:
;
        mov     ax,[bp+right]   ; AX = right
        or      ax,ax           ; Is AX < 0 (i.e., off screen left)?
        js      getOut1         ; If yes, there's nothing to do, so just exit
        cmp     ax,640          ; Is AX > 640 (i.e., off screen right)?
        jle     clip3           ; If no, leave it alone
        mov     ax,640          ; If yes, set it to 640
clip3:  mov     [bp-clipRight],ax ; Set clipRight
;
; Clip bottom to produce clipBottom:
;
        mov     ax,[bp+bottom]  ; AX = bottom
        or      ax,ax           ; Is AX < 0 (i.e., off screen top)?
        jns     clip4           ; If no, continue clipping
getOut1: jmp    getOut          ; If yes, there's nothing to do -- exit
clip4:  cmp     ax,200          ; Is AX > 200 (i.e., off screen bottom)?
        jle     clip5           ; If no, leave it alone
        mov     ax,200          ; If yes, set it to 200
clip5:  mov     [bp-clipBottom],ax ; Set clipBottom
;
; Compute lCnt:
;
        sub     ax,[bp-clipTop] ; AX = clipBottom - clipTop
        jz      getOut1         ; If lCnt is zero, exit (nothing to do)
        js      getOut1         ; If lCnt is negative, also exit
        mov     [bp-lCnt],ax    ; Set lCnt
;
; Compute lSMask:
;
        mov     cl,[bp-clipLeft]; CL = clipLeft & 15 (i.e., bottom 4 bits)
        and     cl,15
        xor     ax,ax           ; AX = FFFF
        not     ax
        shr     ax,cl           ; Shift AX right according to CL
        mov     bl,ah           ; BX = AX byte-reversed
        mov     bh,al
        mov     [bp-lSMask],bx  ; lSMask = BX
;
; Compute lEMask:
;
        mov     cl,[bp-clipRight] ; CL = clipRight & 15 (bottom 4 bits)
        and     cl,15
        xor     ax,ax           ; AX = FFFF
        not     ax
        shr     ax,cl           ; Shift AX right according to CL
        not     ax
        mov     bl,ah           ; BX = AX byte-reversed
        mov     bh,al
        mov     [bp-lEMask],bx  ; lEMask = BX
;
; Compute pattCnt (assuming globalPatt is FALSE):
;
        mov     ax,[bp-clipTop] ; AX = clipTop - top
        sub     ax,[bp+top]
        shl     ax,1            ; AX = 2 * AX
        mov     [bp-pattCnt],ax ; pattCnt = AX
;
; Transfer pattern to pattRot, rotated as specified by globalPatt (and, if
; globalPatt is TRUE, replace pattCnt with a recomputed value):
;
        mov     ch,16           ; CH = 16 (number of words to transfer)
        mov     bl,[bp+left]    ; BL = left & 15 (bottom four bits)
        and     bl,15
        mov     ax,[bp+globPatt]; Is globalPatt TRUE?
        or      ax,ax
        jz      locPat          ; If not, branch
        mov     ax,[bp-clipTop] ; If yes, recompute pattCnt = 2 * clipTop
        shl     ax,1
        mov     [bp-pattCnt],ax
        xor     bl,bl           ; And set BL = 0
locPat: mov     si,[bp+pattern] ; SI = pattern
        lea     di,[bp-pattRot] ; DI = pattRot
        xor     ax,ax           ; AX = FFFF (used when pattern == 0)
        not     ax
nxtPat: or      si,si           ; Is SI (from pattern) == 0?
        jz      rotIt           ; If yes, use the FFFF already in AX
        lods    word ptr [si]   ; Otherwise, AX = *SI++
rotIt:  mov     cl,bl           ; Rotate the pattern according to BL
        ror     ax,cl
        mov     dl,ah           ; DX = AX byte-reversed
        mov     dh,al
        mov     [di],dx         ; *DI = DX
        inc     di              ; DI++
        inc     di
        dec     ch              ; CH--
        jnz     nxtPat          ; If more pattern left, go back and do it again
;
; Compute the initial lStrt:
;
        mov     ax,[bp-clipTop] ; AX = clipTop
        xor     bx,bx           ; BX = 0
        shr     ax,1            ; AX = AX/2, with the bottom bit going to CF
        mov     cl,14           ; Shift CL into BX
        rcl     bx,cl
        mov     cl,80           ; AX = 80 * AX (multiple by # of columns)
        mul     cl
        add     ax,bx           ; Add in the bit in BX
        mov     bx,[bp-clipLeft]; BX = clipLeft
        mov     cl,3            ; BX = BX/8 (convert from bits to bytes)
        shr     bx,cl
        and     bx,0FFFEH       ; Convert to word address (clear bottom bit)
        add     bx,ax           ; BX += AX
        mov     [bp-lStrt],bx   ; lStrt = BX
;
; Compute the initial lEnd:
;
        mov     bx,[bp-clipRight]; BX = clipRight
        mov     cl,3            ; BX = BX/8 (convert to word)
        shr     bx,cl
        and     bx,0FFFEH       ; Convert to word address (clear bottom bit)
        add     ax,bx           ; AX += BX (AX had column offset from above)
        mov     [bp-lEnd],ax    ; lEnd = AX
;
; Set up for processing:
;
        mov     di,[bp-lStrt]   ; DI = lStrt
        mov     si,[bp-lEnd]    ; SI = lEnd
;
; Switch on mode:
;
        mov     ax,[bp+mode]    ; AX = mode
        dec     ax              ; AX == 0 (COPY)?
        jns     tryXor          ; If not, go try XOR
;
; Do COPY mode:
;
cpNxt:  push    di              ; Save DI (line start)
        mov     di,[bp-pattCnt] ; DI = pattCnt
        and     di,31           ; DI &= 31 (get bottom five bits)
        mov     bx,[bp-pattRot+di] ; BX = pattRot[DI] (the pattern word)
        inc     di              ; DI++
        inc     di              ; DI++
        mov     [bp-pattCnt],di ; pattCnt = DI
        pop     di              ; Restore line start DI
        mov     ax,[bp-lSMask]  ; AX = lSMask
        cmp     di,si           ; Line start == line end?
        je      cpLast          ; If yes, go process it as one word
        mov     dx,ax           ; If not, copy line start mask to DX
        not     ax              ; Complement start mask in AX
        and     ax,es:[di]      ; AX &= *DI (old word with new bits cleared)
        and     dx,bx           ; DX &= BX (new word, with old bits cleared)
        or      ax,dx           ; AX |= DX (combine new and old)
        stos    word ptr es:[di]; *DI++ = AX (store processed word)
        mov     cx,si           ; CX = (SI-DI)/2 (converted from byte to word)
        sub     cx,di
        shr     cx,1
        jz      cpLast2         ; If CX == 0 (SI and DI in same word), branch
        mov     ax,bx           ; AX = BX (the pattern)
        rep stos word ptr es:[di] ; Block move the words between the 1st & last
cpLast2: xor    ax,ax           ; AX = FFFF
        not     ax
cpLast: and     ax,[bp-lEMask]  ; AX &= lEMask (AX is now mask of last word)
        mov     dx,ax           ; Copy mask to DX
        not     ax              ; Complement mask in AX
        and     ax,es:[di]      ; AX &= *DI (AX = old word with new bits clear)
        and     dx,bx           ; DX &= BX (DX = new word with old bits clear)
        or      ax,dx           ; AX |= DX (combine the two)
        mov     es:[di],ax      ; *DI = AX (put the processed word back)
;
; Increment line start and line end pointers to next line:
        mov     di,[bp-lStrt]   ; DI = lStrt
        xor     si,2000H        ; Toggle even/odd line bit in SI and DI
        xor     di,2000H
        mov     ax,di           ; DI & 0x2000?
        and     ax,2000H
        jnz     cpOdd           ; If yes (odd), don't add in 80
        add     si,80           ; If no (even), add 80 to the start and end
        add     di,80
cpOdd:  mov     [bp-lStrt],di   ; lStrt = DI
        dec     word ptr [bp-lCnt] ; lCnt--
        jnz     cpNxt           ; If we're not done, go back a do another line
        jmp     getOut          ; Otherwise, exit
;
; Try exclusive-or:
;
tryXor: dec     ax              ; Exclusive-or mode?
        jns     tryBst          ; If not, go try bit set
;
; Do EXCLUSIVE-OR mode:
;
xrNxt:  push    di              ; Save DI (line start)
        mov     di,[bp-pattCnt] ; DI = pattCnt
        and     di,31           ; DI &= 31 (get bottom five bits)
        mov     bx,[bp-pattRot+di] ; BX = pattRot[DI] (the pattern word)
        inc     di              ; DI++
        inc     di              ; DI++
        mov     [bp-pattCnt],di ; pattCnt = DI
        pop     di              ; Restore line start DI
        mov     ax,[bp-lSMask]  ; AX = lSMask
        cmp     di,si           ; Line start == line end?
        je      xrLast          ; If yes, go process it as one word
        and     ax,bx           ; AX &= BX (clip pattern to mask into AX)
        xor     ax,es:[di]      ; AX ^= *DI (exclusive-or in old word)
        stos    word ptr es:[di]; *DI++ = AX (store processed word)
        mov     cx,si           ; CX = (SI-DI)/2 (converted from byte to word)
        sub     cx,di
        shr     cx,1
        jz      xrLast2         ; If CX == 0 (SI and DI in same word), branch
xrLoop: mov     ax,es:[di]      ; AX = *DI (get next word to process)
        xor     ax,bx           ; AX ^= BX (exclusive-or word with pattern)
        stos    word ptr es:[di]; *DI++ = AX (store processed word)
        loopnz  xrLoop          ; Repeat until all word are processed
xrLast2: xor    ax,ax           ; AX = FFFF
        not     ax
xrLast: and     ax,[bp-lEMask]  ; AX &= lEMask (AX is now mask of last word)
        and     ax,bx           ; AX &= BX (clip pattern to mask into AX)
        xor     ax,es:[di]      ; AX ^= *DI (exclusive-or word into AX)
        mov     es:[di],ax      ; *DI = AX (put the processed word back)
;
; Increment line start and line end pointers to next line:
        mov     di,[bp-lStrt]   ; DI = lStrt
        xor     si,2000H        ; Toggle even/odd line bit in SI and DI
        xor     di,2000H
        mov     ax,di           ; DI & 0x2000?
        and     ax,2000H
        jnz     xrOdd           ; If yes (odd), don't add in 80
        add     si,80           ; If no (even), add 80 to the start and end
        add     di,80
xrOdd:  mov     [bp-lStrt],di   ; lStrt = DI
        dec     word ptr [bp-lCnt] ; lCnt--
        jnz     xrNxt           ; If we're not done, go back a do another line
        jmp     getOut          ; Otherwise, exit

;
; Try bit set:
;
tryBst: dec     ax              ; Bit set mode?
        jns     tryBcl          ; If not, go try bit clear
;
; Do BIT SET mode:
;
bsNxt:  push    di              ; Save DI (line start)
        mov     di,[bp-pattCnt] ; DI = pattCnt
        and     di,31           ; DI &= 31 (get bottom five bits)
        mov     bx,[bp-pattRot+di] ; BX = pattRot[DI] (the pattern word)
        inc     di              ; DI++
        inc     di              ; DI++
        mov     [bp-pattCnt],di ; pattCnt = DI
        pop     di              ; Restore line start DI
        mov     ax,[bp-lSMask]  ; AX = lSMask
        cmp     di,si           ; Line start == line end?
        je      bsLast          ; If yes, go process it as one word
        and     ax,bx           ; AX &= BX (clip pattern to mask into AX)
        or      ax,es:[di]      ; AX |= *DI (or in old word)
        stos    word ptr es:[di]; *DI++ = AX (store processed word)
        mov     cx,si           ; CX = (SI-DI)/2 (converted from byte to word)
        sub     cx,di
        shr     cx,1
        jz      bsLast2         ; If CX == 0 (SI and DI in same word), branch
bsLoop: mov     ax,es:[di]      ; AX = *DI (get next word to process)
        or      ax,bx           ; AX |= BX (or word with pattern)
        stos    word ptr es:[di]; *DI++ = AX (store processed word)
        loopnz  bsLoop          ; Repeat until all word are processed
bsLast2: xor    ax,ax           ; AX = FFFF
        not     ax
bsLast: and     ax,[bp-lEMask]  ; AX &= lEMask (AX is now mask of last word)
        and     ax,bx           ; AX &= BX (clip pattern to mask into AX)
        xor     ax,es:[di]      ; AX ^= *DI (exclusive-or word into AX)
        mov     es:[di],ax      ; *DI = AX (put the processed word back)
;
; Increment line start and line end pointers to next line:
        mov     di,[bp-lStrt]   ; DI = lStrt
        xor     si,2000H        ; Toggle even/odd line bit in SI and DI
        xor     di,2000H
        mov     ax,di           ; DI & 0x2000?
        and     ax,2000H
        jnz     bsOdd           ; If yes (odd), don't add in 80
        add     si,80           ; If no (even), add 80 to the start and end
        add     di,80
bsOdd:  mov     [bp-lStrt],di   ; lStrt = DI
        dec     word ptr [bp-lCnt] ; lCnt--
        jnz     bsNxt           ; If we're not done, go back a do another line
        jmp     getOut          ; Otherwise, exit
;
; Try bit clear:
;
tryBcl: dec     ax              ; Bit clear mode?
        jns     tryBcl          ; If not, exit (not a legal mode)
;
; Do BIT CLEAR mode:
;
bcNxt:  push    di              ; Save DI (line start)
        mov     di,[bp-pattCnt] ; DI = pattCnt
        and     di,31           ; DI &= 31 (get bottom five bits)
        mov     bx,[bp-pattRot+di] ; BX = pattRot[DI] (the pattern word)
        inc     di              ; DI++
        inc     di              ; DI++
        mov     [bp-pattCnt],di ; pattCnt = DI
        pop     di              ; Restore line start DI
        mov     ax,[bp-lSMask]  ; AX = lSMask
        cmp     di,si           ; Line start == line end?
        je      bcLast          ; If yes, go process it as one word
        and     ax,bx           ; AX &= BX (clip pattern to mask into AX)
        not     ax              ; AX = ~AX (prepare to clear bits)
        and     ax,es:[di]      ; AX &= *DI (clear bits in old word)
        stos    word ptr es:[di]; *DI++ = AX (store processed word)
        mov     cx,si           ; CX = (SI-DI)/2 (converted from byte to word)
        sub     cx,di
        shr     cx,1
        jz      bcLast2         ; If CX == 0 (SI and DI in same word), branch
bcLoop: mov     ax,bx           ; AX = BX (make a copy of the pattern)
        not     ax              ; AX = ~AX (prepare to clear bits)
        and     ax,es:[di]      ; AX &= *DI (clear bits in next word)
        stos    word ptr es:[di]; *DI++ = AX (store processed word)
        loopnz  bcLoop          ; Repeat until all word are processed
bcLast2: xor    ax,ax           ; AX = FFFF
        not     ax
bcLast: and     ax,[bp-lEMask]  ; AX &= lEMask (AX is now mask of last word)
        and     ax,bx           ; AX &= BX (clip pattern to mask into AX)
        xor     ax,es:[di]      ; AX ^= *DI (exclusive-or word into AX)
        mov     es:[di],ax      ; *DI = AX (put the processed word back)
;
; Increment line start and line end pointers to next line:
        mov     di,[bp-lStrt]   ; DI = lStrt
        xor     si,2000H        ; Toggle even/odd line bit in SI and DI
        xor     di,2000H
        mov     ax,di           ; DI & 0x2000?
        and     ax,2000H
        jnz     bcOdd           ; If yes (odd), don't add in 80
        add     si,80           ; If no (even), add 80 to the start and end
        add     di,80
bcOdd:  mov     [bp-lStrt],di   ; lStrt = DI
        dec     word ptr [bp-lCnt] ; lCnt--
        jnz     bcNxt           ; If we're not done, go back a do another line
        jmp     getOut          ; Otherwise, exit
;
; Save registers and return:
;
getOut: pop     es              ; Restore the ES register
        pop     si              ; Restore the SI register
        pop     di              ; Restore the DI register
        mov     sp,bp           ; Free the local variable stack space
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_rect   endp                    ; End of subroutine

_TEXT   ends                    ; End of code segment
        end                     ; End of assembly code


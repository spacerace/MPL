;====================================================================
; ASMVIDEO.ASM - Macro Assembler video modules for 
;                small model C programs.
;====================================================================

              .MODEL SMALL
              .CODE
              PUBLIC _DispString, _DispCharAttr, _GetCharAttr
              
;--------------------------------------------------------------------
; _DispString writes a string to the active video buffer.
;--------------------------------------------------------------------
_DispString   PROC
              push   bp                ;save registers and establish
              mov    bp,sp             ;  stack frame addressability
              push   si
              push   di
              mov    bx,[bp+12]        ;get pointer to VideoInfo

              mov    si,[bp+4]         ;get pointer to string
              mov    dh,[bp+6]         ;calculate video address
              mov    dl,[bp+8]
              call   AddressOf
              mov    es,[bx+8]         ;point ES to video buffer
              mov    ah,[bp+10]        ;get video attribute in AH
              cld                      ;clear DF

dstr1:        lodsb                    ;get next character
              or     al,al             ;terminate on null byte
              jz     dstr3
              test   byte ptr [bx+4],1 ;branch if SyncFlag is clear
              jz     dstr2
              call   WriteWithWait     ;write during retrace
              jmp    dstr1
dstr2:        stosw                    ;write one character/attribute
              jmp    dstr1             ;loop back for more

dstr3:        pop    di                ;restore registers and exit
              pop    si
              pop    bp
              ret
_DispString   ENDP

;--------------------------------------------------------------------
; _DispCharAttr      writes a character and attribute to the active 
;                    video buffer.
;--------------------------------------------------------------------
_DispCharAttr PROC
              push   bp                ;save registers and establish
              mov    bp,sp             ;  stack frame addressability
              push   di
              mov    bx,[bp+12]        ;get pointer to VideoInfo

              mov    dh,[bp+6]         ;calculate video address
              mov    dl,[bp+8]
              call   AddressOf
              mov    es,[bx+8]         ;point ES to video buffer
              mov    al,[bp+4]         ;get character/attribute
              mov    ah,[bp+10]        ;  in AX

              test   byte ptr [bx+4],1 ;branch if SyncFlag is clear
              jz     dca1
              call   WriteWithWait     ;write during retrace
              jmp    short dca2        ;then exit
dca1:         stosw                    ;write one character/attribute

dca2:         pop    di                ;restore registers and exit
              pop    bp
              ret
_DispCharAttr ENDP

;--------------------------------------------------------------------
; _GetCharAttr       returns the character and attribute at the 
;                    specified address.
;--------------------------------------------------------------------
_GetCharAttr  PROC
              push   bp                ;save registers and establish
              mov    bp,sp             ;  stack frame addressability
              push   di
              mov    bx,[bp+8]         ;get pointer to VideoInfo

              mov    dh,[bp+4]         ;calculate video address
              mov    dl,[bp+6]
              call   AddressOf         ;offset in DI
              mov    es,[bx+8]         ;point ES to video buffer
              test   byte ptr [bx+4],1 ;branch if SyncFlag is clear
              jz     gca3

              mov    dx,3DAh           ;point DX to CRTC status reg
gca1:         in     al,dx             ;wait for non-retrace period
              test   al,1
              jnz    gca1
gca2:         in     al,dx             ;wait for next horizontal
              test   al,1              ;  retrace
              jz     gca2

gca3:         mov    ax,es:[di]        ;get character and attribute
                     pop   di          ;restore registers and exit
                     pop   bp
                     ret
_GetCharAttr  ENDP

;--------------------------------------------------------------------
; AddressOf          returns the video buffer address that 
;                    corresponds to the specified row and column.
;
;   Entry:  DS:BX - address of VideoInfo structure
;           DH,DL - row and column
;   Exit:   DI    - offset address
;--------------------------------------------------------------------
AddressOf     PROC   NEAR
                     mov   al,[bx+2]   ;get columns per row
                     mul   dh          ;multiply by row number
                     xor   dh,dh       ;add column number
                     add   ax,dx
                     shl   ax,1        ;multiply by 2
                     add   ax,[bx+6]   ;add offset of active page
                     mov   di,ax       ;transfer result to DI
                     ret               ;  and exit
AddressOf     ENDP

;--------------------------------------------------------------------
; WriteWithWait      writes a character and attribute to a CGA during 
;                    the horizontal blanking interval.
;
;   Entry:  ES:DI - video buffer address
;           AH,AL - character/attribute
;--------------------------------------------------------------------
WriteWithWait PROC   NEAR
                     mov   cx,ax       ;save character/attribute
                     mov   dx,3DAh     ;point DX to Status Register
                     cli               ;disable interrupts
www1:                in    al,dx       ;wait for non-retrace period
                     test  al,1
                     jnz   www1
www2:                in    al,dx       ;wait for next horizontal
                     test  al,1        ;  retrace
                     jz    www2
                     xchg  ax,cx       ;retrieve character/attribute
                     stosw             ;write them to video memory
                     sti               ;enable interrupts
                     ret
WriteWithWait ENDP

                     END

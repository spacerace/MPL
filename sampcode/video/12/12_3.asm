                TITLE   'Listing 12-3'
                NAME    VRMCGA
                PAGE    55,132

;
; Name:         VRMCGA
;
; Function:     Vertical Interrupt Service routine for MCGA
;
; Caller:       Microsoft C:
;
;                       int EnableISR0A();      /* returns 0 if installed ok */
;
;                       void DisableISR0A();
;

ADDR_6845       EQU     63h

DGROUP          GROUP   _DATA

_TEXT           SEGMENT byte public 'CODE'
                ASSUME  cs:_TEXT,ds:DGROUP

ISR0A           PROC    far             ; Interrupt handler for INT 0Ah

                push    ax              ; preserve registers
                push    dx
                push    ds

                mov     ax,seg DGROUP
                mov     ds,ax           ; DS -> DGROUP

                mov     dx,Port3x4      ; DX := CRTC Address reg number
                in      al,dx
                push    ax              ; preserve CRTC Address reg value

; determine whether a vertical interrupt has occurred

                mov     al,IContReg     ; AL := register number
                out     dx,al
                jmp     $+2             ; wait for MCGA to respond
                inc     dx              ; DX := 3D5H
                in      al,dx           ; AL := current Interrupt Control
                                        ;  register value
                dec     dx
                test    al,40h          ; test bit 6
                jnz     L10             ; jump if vertical interrupt

; not a vertical interrupt so chain to previous interrupt handler

                pushf                   ; simulate an INT to the
                call    ds:PrevISR0A    ;  previous INT 0Ah handler
                jmp     short Lexit

; handle a vertical interrupt

L10:            mov     ax,DefaultICont ; AH := default value for
                                        ;  Interrupt Control register
                                        ; AL := 11h (register number)
                and     ah,11101111b    ; AH bit 4 := 0 (clear interrupt latch)
                out     dx,ax           ; update Interrupt Control reg
                jmp     $+2             ; wait for MCGA to respond

; send End of Interrupt to Programmable Interrupt Controller
;  to allow subsequent IRQ2 interrupts to occur

                mov     al,20h          ; PIC I/O port
                out     20h,al          ; send nonspecific EOI to PIC
                jmp     $+2             ; wait for PIC to respond
                sti                     ; enable interrupts

; do something useful ...

                inc     word ptr _VRcount       ; increment a counter

; enable CRTC to generate another interrupt

                cli                     ; disable interrupts
                mov     ax,DefaultICont ; AH := default value for
                                        ;  Interrupt Control register
                                        ; AL := 11h (register number)
                and     ah,11011111b    ; AH bit 5 := 0 (enable vert int)
                or      ah,00010000b    ; AH bit 4 := 1 (enable int latch)
                out     dx,ax
                jmp     $+2

Lexit:          pop     ax
                out     dx,al           ; restore previous 3D4H value

                pop     ds              ; restore registers and exit
                pop     dx
                pop     ax
                iret

ISR0A           ENDP

;
; EnableISR0A -- enable Vertical Interrupt Handler
;
                PUBLIC  _EnableISR0A
_EnableISR0A    PROC    near

                push    bp              ; preserve caller registers
                mov     bp,sp
                push    si
                push    di

                mov     ax,40h
                mov     es,ax           ; ES -> video BIOS data area

; save default CRTC register values

                mov     dx,es:[ADDR_6845]  ; DX := CRTC Address port
                mov     Port3x4,dx      ; save port address

                mov     ax,1A00h        ; AH := 1AH (INT 10H function number)
                                        ; AL := 0 (read Display Combination)
                int     10h             ; AL := 1AH if function 1AH supported
                                        ; BL := active video subsystem
                cmp     al,1Ah
                jne     L20             ; jump if not an MCGA

                cmp     bl,0Bh
                je      L21             ; jump if MCGA

                cmp     bl,0Ch
                je      L21             ; jump if MCGA

L20:            mov     ax,0FFFFh       ; return 0FFFFh if not an MCGA
                jmp     short L23

; get default value for MCGA Interrupt Control register

L21:            mov     al,IContReg     ; AL := Interrupt Control reg number
                cli
                out     dx,al
                jmp     $+2
                inc     dx              ; DX := 3D5H
                in      al,dx           ; AL := current value for register
                sti

                mov     IContValue,al   ; save this value

; save old interrupt 0Ah vector

                mov     ax,350Ah        ; AH := 35H (INT 21h function number)
                                        ; AL := 0AH (interrupt number)
                int     21h             ; ES:BX := previous INT 0AH vector

                mov     word ptr PrevISR0A,bx
                mov     word ptr PrevISR0A+2,es ; save previous vector

; update interrupt 0AH vector with address of this handler

                push    ds              ; preserve DS
                mov     dx,offset ISR0A
                push    cs
                pop     ds              ; DS:DX -> ISR0A
                mov     ax,250Ah        ; AH := 25H (INT 21H function number)
                                        ; AL := 0AH (interrupt number)
                int     21h             ; update INT 0AH vector
                pop     ds              ; restore DS

; enable IRQ2 by zeroing bit 2 of the PIC's mask register

                cli                     ; clear interrupts
                mov     dx,21h          ; DX := PIC mask register
                in      al,dx           ; AL := mask register value
                and     al,11111011b    ; reset bit 2
                out     dx,al

; enable vertical interrupts

                mov     dx,Port3x4      ; DX := CRTC Address port
                mov     ax,DefaultICont

                and     ah,11001111b
                out     dx,ax           ; clear bits 4 and 5 of Int Control reg
                jmp     $+2             ; wait for MCGA to respond
                or      ah,00010000b
                out     dx,ax           ; set bit 4
                jmp     $+2
                sti                     ; enable interrupts

                xor     ax,ax           ; AX := 0 (return value)

L23:            pop     di              ; restore registers and exit
                pop     si
                mov     sp,bp
                pop     bp
                ret

_EnableISR0A    ENDP

;
; DisableISR0A -- disable Vertical Interrupt Handler
;
                PUBLIC  _DisableISR0A
_DisableISR0A   PROC    near

                push    bp
                mov     bp,sp
                push    si
                push    di
                push    ds

; disable vertical interrupts

                cli                     ; disable interrupts
                mov     dx,Port3x4
                mov     ax,DefaultICont
                out     dx,ax           ; restore Interrupt Control register
                jmp     $+2
                sti                     ; enable interrupts

; restore previous interrupt 0Ah handler

                lds     dx,PrevISR0A    ; DS:DX := previous INT 0AH vector
                mov     ax,250Ah        ; AH := 25H (INT 21H function number)
                                        ; AL := 0AH (interrupt number)
                int     21h

                pop     ds              ; restore registers and exit
                pop     di
                pop     si
                mov     sp,bp
                pop     bp
                ret

_DisableISR0A   ENDP

_TEXT           ENDS

_DATA           SEGMENT word public 'DATA'

                EXTRN   _VRcount:word   ; declared in C caller

PrevISR0A       DD      ?               ; save area for old int 0Ah vector
Port3x4         DW      ?               ; 3B4h or 3D4h

DefaultICont    LABEL   word
IContReg        DB      11h             ; Interrupt Control register number
IContValue      DB      ?               ; default value for Int Control reg

_DATA           ENDS

                END

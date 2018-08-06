;               Serial Port Direct Interface Library

        BUFSIZE = 2000                  ; Size of buffer to use for receive

_BSS    segment word public 'BSS'       ; Global variables segment
_BSS    ends

_TEXT   segment byte public 'CODE'      ; Code segment
_TEXT   ends

DGROUP  group _BSS                      ; Global variables are in this group

_BSS    segment                 ; Declare variables

intSav  dw      4 dup(?)        ; Storage for saved interrupt vectors
bufIn   dw      2 dup(?)        ; Buffer input pointers
bufOut  dw      2 dup (?)       ; Buffer output pointers
buff0   db      BUFSIZE dup(?)  ; The COM1 circular buffer
buff1   db      BUFSIZE dup(?)  ; The COM2 buffer

_BSS    ends                    ; End of variables

_TEXT   segment                 ; Place the code in the code segment

        assume CS:_TEXT, DS:DGROUP ; Assume CS points to code segment, and
                                   ;   DS points to DRGOUP segment (where
                                   ;   the variables are)

; _serInit(port,config)
;
; Function: Initialize and configure serial port port (0 for COM1, 1 for
; COM2). See the main text of the chapter for details on the format of the
; config parameter. Also set up incoming interrupts and buffering.
;
; Algorithm: Call the ROM BIOS to configure and initialize the port. Then
; save the existing interrupt vector and replace it with our own. Enable
; interrupts at the port, and return.

        public  _serInit        ; Routine is available to other modules

        port = 4                ; Offset from BP to parameter port
        config = 6              ; Offset to parameter config

; These two tables are used to get the buffer pointers based on the
; port number. The first table contains pointers to the beginning of
; the buffers; the second contains pointers to the first byte immediately
; after the buffers.
bufAddr dw      offset DGROUP:buff0, offset DGROUP:buff1
bufLim  dw      offset DGROUP:buff0+BUFSIZE, offset DGROUP:buff1+BUFSIZE

_serInit proc near              ; NEAR type subroutine
        push    bp              ; Save the BP register
        mov     bp,sp           ; Set BP to SP; easier to get parameters
        push    di              ; Save the DI register
        push    si              ; Save the SI register
        mov     dx,[bp+port]    ; DX = port number
        mov     al,[bp+config]  ; AL = port configuration
        mov     ah,0            ; Call ROM BIOS port init function
        int     14H
        cli                     ; Disable interrupts
        push    es              ; Save the ES register
        xor     ax,ax           ; ES = 0 (to access low memory)
        mov     es,ax
        mov     si,[bp+port]    ; SI = port # * 2
        shl     si,1
        mov     ax,cs:[si+bufAddr] ; Set the buffer to empty:
        mov     [si+bufIn],ax      ;   bufIn = bufOut = start of buffer
        mov     [si+bufOut],ax
        mov     bx,es:[si+400H] ; BX = base port address of serial port
        mov     si,1            ; SI = (1 - port #) * 4 (offset to vector)
        sub     si,[bp+port]
        shl     si,1
        shl     si,1
        mov     ax,es:[si+2CH]  ; Save the old vector in intSav
        mov     [si+intSav],ax
        mov     ax,es:[si+2CH+2]
        mov     [si+intSav+2],ax
        mov     ax,OFFSET intServ ; Replace the vector with intServ
        mov     es:[si+2CH],ax
        mov     ax,_TEXT
        mov     es:[si+2CH+2],ax
        mov     dx,bx           ; Read the serial port to clear it
        in      al,dx
        add     dx,4            ; Enable serial port receive interrupts
        in      al,dx
        or      al,8
        out     dx,al
        sub     dx,3
        mov     al,1
        out     dx,al
        in      al,21H          ; Enable the 8259 interrupt controller
        mov     ah,0EFH
        mov     cl,[bp+port]
        ror     ah,cl
        and     al,ah
        out     21H,al
        sti                     ; Enable interrupts
        pop     es              ; Restore ES
        pop     si              ; Restore the SI register
        pop     di              ; Restore the DI register
        pop     bp              ; Restore the BP register
        ret                     ; Return to calling program
_serInit endp                   ; End of subroutine

; intServ
;
; Function: Serial receive interrupt service routine. Handle an interrupt
; from a serial port and return.
;
; Algorithm: Save registers. Figure out which port the interrupt is from.
; Figure out what kind of interrupt it is, and process it accordingly. 
; Acknowledge the interrupt at the 8259. Then restore registers and return
; from interrupt.

intServ proc near               ; NEAR type subroutine (doesn't matter which
                                ;   type actually, since we do a RETI rather
                                ;   than a RET)
        push    ax              ; Save the AX register
        push    bx              ; Save BX
        push    cx              ; Save CX
        push    dx              ; Save DX
        push    si              ; Save SI
        push    ds              ; Save DS
        push    es              ; Save ES
        mov     ax,DGROUP       ; Set DS to DGROUP for access to variables
        mov     ds,ax
        xor     ax,ax           ; Set ES to 0 for access to low memory
        mov     es,ax
;
; Find which port the interrupt is from:
;
        xor     si,si           ; SI = 0 (assume COM1)
        mov     dx,es:400H      ; DX = base port address for COM1
        add     dx,2            ; Get IIR address
        in      al,dx           ; Get IIR contents
        mov     ah,al           ; Make a copy of the input
        and     ah,1            ; Any interrupts here?
        jz      intC1           ; If there is an interrupt, go process it
        mov     dx,es:402H      ; Otherwise, DX = base port address for COM2
        add     dx,2            ; Get IIR address for COM2
        in      al,dx           ; Get IIR contents
        mov     ah,al           ; Make a copy of it
        and     ah,1            ; Are there interrupts?
        jnz     notC2           ; If not, forget this interrupt
        add     si,2            ; If yes, increment SI for use with COM2
;
; Find out which type of interrupt it is, and handle it:
;
intC1:  cmp     al,0            ; Is it a modem status change?
        jne     notMSt          ; If not, try the next interrupt type
        add     dx,4            ; If it is, read the modem status to clear it
        in      al,dx
        jmp     endInt          ; And exit
notMSt: cmp     al,2            ; Is it a transmit register empty interrupt?
        jne     notTRE          ; If not, try other types
        jmp     endInt          ; If it is, exit (we shouldn't get these)
notTRE: cmp     al,4            ; Is it a receiver register full interrupt?
        jne     notRRF          ; If not, try other types
        sub     dx,2            ; If yes, read in the received character
        in      al,dx
        call    putb            ; Put it in the buffer
        jmp     endInt          ; And exit
notRRF: cmp     al,6            ; Is it a receive line status interrupt?
        jne     endInt          ; If not, it's no known type -- ignore it
        add     dx,3            ; If yes, read the status to clear it
        in      al,dx
;
; Send end-of-interrupt to the 8259:
;
endInt: mov     al,20H          ; Send EOI
        out     20H,al
;
; Restore registers and exit:
;
notC2:  pop     es              ; Restore the ES register
        pop     ds              ; Restore DS
        pop     si              ; Restore SI
        pop     dx              ; Restore DX
        pop     cx              ; Restore CX
        pop     bx              ; Restore BX
        pop     ax              ; Restore AX
        iret                    ; Return from interrupt
intServ endp                    ; End of routine

; serClose(port)
;
; Function: Close the port; restore it to the state it was in before serInit
; was called.
;
; Algorithm: Turn off interrupts and restore the interrupt vector to its
; original state.

        public  _serClose       ; Routine is available to other modules

        port = 4                ; Offset from BP to parameter port

_serClose proc near             ; NEAR type subroutine
        push    bp              ; Save the BP register
        mov     bp,sp           ; Set BP to SP; easier to access parameters
        push    di              ; Save the DI register
        push    si              ; Save the SI register
        push    es              ; Save the ES register
        xor     ax,ax           ; Set ES to 0, for access to low memory
        mov     es,ax
        cli                     ; Disbale interrupts
        mov     si,[bp+port]    ; SI = 2*port #
        shl     si,1
        mov     bx,es:[si+400H] ; BX = I/O port base address
        mov     si,1            ; SI = 4*(1 - port #)
        sub     si,[bp+port]
        shl     si,1
        shl     si,1
        mov     ax,[si+intSav]  ; Restore the interrupt from intSav
        mov     es:[si+2CH],ax
        mov     ax,[si+intSav+2]
        mov     es:[si+2CH+2],ax
        mov     dx,bx           ; Turn off serial interrupts
        add     dx,4
        in      al,dx
        and     al,0F7H
        out     dx,al
        sub     dx,3
        xor     al,al
        out     dx,al
        in      al,21H          ; Turn off 8259 controller ints
        mov     ah,10H
        mov     cl,[bp+port]
        ror     ah,cl
        or      al,ah
        out     21H,al
        mov     al,20H
        out     20H,al
        sti                     ; Re-enble interrupts at the processor
        pop     es              ; Restore the ES register
        pop     si              ; Restore SI
        pop     di              ; Restore DI
        pop     bp              ; Restore BP
        ret                     ; Return to calling program
_serClose endp                  ; End of subroutine

; _serSend(port,char)
;
; Function: Send the character char out over serial port port.
;
; Algorithm: Wait for the transmit holding register to be empty, and then
; output the character to be sent to that register.

        public  _serSend        ; Routine is available to other modules

        port = 4                ; Offset from BP to parameter port
        char = 6                ; Offset to parameter char

_serSend proc near              ; NEAR type subroutine
        push    bp              ; Save the BP register
        mov     bp,sp           ; Set BP to SP; easier to access parameters
        push    di              ; Save the DI register
        push    si              ; Save the SI register
        push    es              ; Save ES
        mov     dx,[bp+port]    ; DX = port #
        xor     ax,ax           ; ES = 0 (for access to low memory)
        mov     es,ax
        mov     si,[bp+port]    ; SI = 2*port #
        shl     si,1
        mov     dx,es:[si+400H] ; DX = I/O port base address
        add     dx,5            ; DX = address of line status register
sendWt: in      al,dx           ; Get line status
        and     al,20H          ; Transmit holding register empty?
        jz      sendWt          ; If not, keep asking...
        sub     dx,5            ; If yes, DX = address of trasmit holding reg.
        mov     al,[bp+char]    ; Send character
        out     dx,al
        pop     es              ; Restore the ES register
        pop     si              ; Restore SI
        pop     di              ; Restore DI
        pop     bp              ; Restore BP
        ret                     ; Return to caller
_serSend endp                   ; End of subroutine

; _serRecv(port)
;
; Function: If a character is available from serial port port, return it.
; Otherwise, return -1.
;
; Algorithm: Get the port number, turn off interrupts (so one doesn't sneak
; in and mess us up), and call getb to get a character out of the input
; buffer (if there's a character available).

        public  _serRecv        ; Routine is available to other modules

        port = 4                ; Offset from BP to parameter port

_serRecv proc near              ; NEAR type subroutine
        push    bp              ; Save the BP register
        mov     bp,sp           ; Set BP to SP; easier to access parameters
        push    di              ; Save the DI register
        push    si              ; Save the SI register
        mov     si,[bp+port]    ; SI = 2 * port #
        shl     si,1
        cli                     ; Disable interrupts
        call    getb            ; Get a character from the buffer
        sti                     ; Enable interrupts
        pop     si              ; Restore SI register
        pop     di              ; Restore DI
        pop     bp              ; Restore BP
        ret                     ; Return to caller
_serRecv endp                   ; End of subroutine

; _serStat(port)
;
; Function: Return the status of the serial port specified.
;
; Algorithm: Call the ROM BIOS serial status function.

        public  _serStat        ; Routine is available to other modules

        port = 4                ; Offset from BP to the parameter port

_serStat proc near              ; NEAR type subroutine
        push   bp               ; Save the BP register
        mov    bp,sp            ; Set BP to SP; easier to access parameters
        push   di               ; Save the DI register
        push   si               ; Save the SI register
        mov    dx,[bp+port]     ; DX = port #
        mov    ah,3             ; AH = 3 (serial status function)
        int    14H              ; Call ROM BIOS serial port interrupt
        pop    si               ; Restore the SI register
        pop    di               ; Restore DI
        pop    bp               ; Restore BP
        ret                     ; Return to caller
_serStat endp                   ; End of subroutine

; putb(AL = byte, SI = offset)
;
; Function: Put a byte into a circular buffer; AL contains the byte, 
; SI contains a word offset within the buffer pointers (0 for the COM1
; buffer, 2 for the COM2 buffer). If the character didn't fit in the
; buffer, putb returns with AH == -1; otherwise AH == 0.
;
; Algorithm: Get the bufIn pointer. Compute what the new bufIn will be.
; If it equals bufOut, the buffer is full. Otherwise, the character can
; be stored and bufIn updated.

putb    proc near               ; NEAR type subroutine
        mov     ah,-1           ; Assume the buffer'll be full
        mov     bx,[si+bufIn]   ; BX = bufIn
        mov     cx,bx           ; CX = (BX+1) modulo buffer
        inc     cx              ; CX++
        cmp     cx,cs:[si+bufLim] ; Is it past the end of the buffer?
        jne     putb2           ; If not, continue
        mov     cx,cs:[si+bufAddr] ; If yes, reset it to the beginning
putb2:  cmp     cx,[si+bufOut]  ; Is the buffer full?
        je      putb3           ; If it is, don't store this byte
        mov     [bx],al         ; Otherwise, store the byte
        xor     ah,ah           ; Set AH to indicate success
        mov     [si+bufIn],cx   ; Update bufIn
putb3:  ret                     ; Return
putb    endp                    ; End of subroutine

; getb(SI = offset); returns AL = char
;
; Function: Get a byte into a circular buffer; SI contains a word 
; offset within the buffer pointers (0 for the COM1 buffer, 2 for 
; the COM2 buffer). On return, AX will contain a character from the
; buffer, or -1 if there were no bytes left in the buffer.
;
; Algorithm: Get the bufOut pointer. If it equals bufIn, the buffer
; is full. Otherwise, get a byte and increment bufOut.

getb    proc near               ; NEAR type subroutine
        mov     ax,-1           ; Assume we'll fail
        mov     bx,[si+bufOut]  ; BX = bufOut
        cmp     bx,[si+bufIn]   ; bufOut == bufIn?
        je      getb2           ; If yes, go exit
        mov     al,[bx]         ; Otherwise, get byte from buffer
        xor     ah,ah           ; Clear top byte of AX
        inc     bx              ; Increment bufOut
        cmp     bx,cs:[si+bufLim] ; Past end of buffer?
        jne     getb3           ; If not, continue
        mov     bx,cs:[si+bufAddr] ; If yes, reset to the beginning
getb3:  mov     [si+bufOut],bx  ; Update bufOut
getb2:  ret                     ; Return
getb    endp                    ; End of subroutine

_TEXT   ends                    ; End of code segment
        end                     ; End of assembly code


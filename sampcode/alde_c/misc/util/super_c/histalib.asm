;               Histogram Assembly Library

        HISTSIZE = 4100         ; The size of the histogram table

_BSS    segment word public 'BSS' ; Define data segment
_BSS    ends

_TEXT   segment byte public 'CODE' ; Define code segment
_TEXT   ends

DGROUP  group _BSS              ; Define the DGROUP group

_BSS    segment                 ; Place variables in data segment

histtab db      HISTSIZE dup(?) ; The histogram table
oldVIP  dw      ?               ; Storage for old timer vector IP
oldVCS  dw      ?               ; Storage for old timer vector CS
intCnt  db      ?               ; Count of times thru interrupt

_BSS    ends                    ; End of variable storage

_TEXT   segment                 ; Place code in the code segment

        assume CS:_TEXT, DS:DGROUP, ES: DGROUP ; Assume CS points to code
                                               ;   segment, and DS and ES
                                               ;   to data segment

        extrn   _prHist:near    ; We'll use prHist from the C module

; _startH()
;
; Function: Initialize and start IP histogram collection.
;
; Algorithm: Zero the histogram table. Save the old timer interrupt
; vector and replace it with the new one. And reset the timer interrupt
; rate.

        public  _startH         ; Routine is available to other modules

_startH proc near               ; NEAR type subroutine
        push    bp              ; Save the BP register
        mov     bp,sp           ; Set BP to SP; easier access to parameters
        push    di              ; Save the DI register
        push    es              ; Save the ES register
        mov     ax,ds           ; Make sure ES = DS
        mov     es,ax
        mov     di,offset DGROUP:histtab ; DI = histtab
        xor     ax,ax           ; AX = 0
        mov     cx,HISTSIZE/2   ; CX = size of histtab in words
        rep stos word ptr [di]  ; Zero histtab
        cli                     ; Disable interrupts
        xor     ax,ax           ; ES = 0
        mov     es,ax
        mov     ax,es:20H       ; Save the old timer vector
        mov     oldVIP,ax
        mov     ax,es:22H
        mov     oldVCS,ax
        mov     ax,OFFSET intServ ; Set the timer vector to intServ
        mov     es:20H,ax
        mov     ax,_TEXT
        mov     es:22H,ax
        mov     al,64           ; Set interrupt count
        mov     intCnt,al
        mov     al,36H          ; Tell timer to prepare for a new count
        out     43H,al
        mov     ax,400H         ; Write count; first low byte
        out     40H,al
        mov     al,ah           ; Then high byte
        out     40H,al
        sti                     ; Enable interrupts
        pop     es              ; Restore ES register
        pop     di              ; Restore DI register
        pop     bp              ; Restore BP register
        ret                     ; Return to caller
_startH endp                    ; End of subroutine

; intServ
;
; Function: Service a timer interrupt, updating histogram counters and
; calling the normal timer interrupt routine if appropriate.
;
; Algorithm: Save registers. Increment the appropriate counter. Check
; if we should call the old timer interrupt vector; if so, do it. Otherwise,
; just ack the interrupt at the 8259, restore registers, and return.

intServ proc far                ; FAR type subroutine (this is used to let
                                ;   us do a RET to the old vector, even if
                                ;   it's in a different segment)
        push    ax              ; Push room for a second return address
        push    ax              ;   (this may be used below)
        push    ax              ; Save the AX register
        push    bx              ; Save BX
        push    cx              ; Save CX
        push    bp              ; Save BP
        push    ds              ; Save DS
        mov     bp,sp           ; Set BP to SP, to access interrupts CS:IP
        mov     ax,DGROUP       ; DS = DGROUP, to access variables
        mov     ds,ax
        xor     bx,bx           ; BX = 0; offset to "other" counter
        mov     ax,[bp+16]      ; AX = interrupted CS
        cmp     ax,_text        ; Is it in the code segment?
        jne     intS2           ; If not, then "other" was correct
        mov     bx,[bp+14]      ; BX = IP
        mov     cl,6            ; BX /= 64 (i.e., make BX counter number)
        shr     bx,cl
        shl     bx,1            ; BX = 4*BX + 4 (i.e., make histogram offset)
        shl     bx,1
        add     bx,4
intS2:  inc     word ptr [histtab+bx] ; Increment low word of counter
        jnz     intS3           ; If no overflow from low word, skip
        inc     word ptr [histtab+2+bx] ; Increment high word of counter
intS3:  dec     intCnt          ; Decrement count of interrupts
        jnz     intS4           ; If 64 haven't gone by yet, go exit
        mov     al,64           ; Otherwise, reload count of interrupts
        mov     intCnt,al
        mov     ax,oldVCS       ; Set up stack for RET to old timer vector
        mov     [bp+12],ax
        mov     ax,oldVIP
        mov     [bp+10],ax
        pop     ds              ; Restore DS register
        pop     bp              ; Restore BP
        pop     cx              ; Restore CX
        pop     bx              ; Restore BX
        pop     ax              ; Restore AX
        ret                     ; Return to old vector address

intS4:  mov     al,20H          ; Send end-of-interrupt to 8259
        out     20H,al
        pop     ds              ; Restore DS register
        pop     bp              ; Restore BP
        pop     cx              ; Restore CX
        pop     bx              ; Restore BX
        pop     ax              ; Restore AX
        pop     ax              ; Pop space for vector return
        pop     ax
        iret                    ; Return to interrupted code
intServ endp                    ; End of subroutine

; _endH()
;
; Function: Turn off histogram collection, and print out the results.
;
; Algorithm: Reset the timer interrupt rate to the old one. Restore the
; timer interrupt vector. And call _prHist to print out the histogram
; results.

        public  _endH           ; Routine is available to other modules

_endH   proc near               ; NEAR type subroutine
        push    bp              ; Save the BP register
        mov     bp,sp           ; Set BP to SP; easier to access parameters
        push    es              ; Save the ES register
        xor     ax,ax           ; ES = 0, to access low memory vectors
        mov     es,ax
        cli                     ; Turn off interrupts
        mov     al,36H          ; Tell timer to accept a new count value
        out     43H,al
        xor     al,al           ; Write zero to the timer counter
        out     40H,al          ; First the low byte
        out     40H,al          ; Then the high byte
        mov     ax,oldVIP       ; Restore the old interrupt vector
        mov     es:20H,ax
        mov     ax,oldVCS
        mov     es:22H,ax
        sti                     ; Turn on interrupts
        pop     es              ; Restore the ES register
        mov     ax,offset DGROUP:histtab ; _prHist(histtab): push histtab
        push    ax
        call    _prHist         ; Call _prHist
        pop     ax              ; Pop histtab
        pop     bp              ; Restore BP
        ret                     ; Return to caller
_endH   endp                    ; End of subroutine

_TEXT   ends                    ; End of code segment
        end                     ; End of assembly code


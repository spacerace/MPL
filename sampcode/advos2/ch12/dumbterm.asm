	title	DUMBTERM -- OS/2 Terminal Program
        page    55,132
        .286

;
; DUMBTERM.ASM
;
; A simple multithreaded OS/2 terminal program that exchanges
; characters between the console and the serial port.
;
; Communication parameters for COM1 are set at 2400 baud,
; 8 data bits, no parity, and 1 stop bit.
;
; Assemble with:  C> masm dumbterm.asm;
; Link with:  C> link dumbterm,,,os2,dumbterm
;
; Usage is:  C> dumbterm
;
; Press Alt-X to terminate the program.
;
; Copyright (C) 1988 Ray Duncan
;

cr      equ     0dh             ; ASCII carriage return
lf	equ	0ah		; ASCII linefeed

kwait   equ     0               ; KbdCharIn parameters
knowait equ     1

stksize equ     2048            ; stack size for threads

exitkey equ     2dh             ; Alt-X key is exit signal

                                ; COM port configuration
com1	equ	1		; nonzero if using COM1
com2	equ	0		; nonzero if using COM2
com3	equ	0		; nonzero if using COM3
baud    equ     2400            ; baud rate for COM port
parity	equ	0		; 0=N, 1=O, 2=E, 3=M, 4=S
databit equ     8               ; 5-8
stopbit equ	0		; 0=1, 1=1.5, 2=2

        extrn   DosAllocSeg:far ; OS/2 API functions
        extrn   DosClose:far    
        extrn   DosCreateThread:far
        extrn   DosDevIOCtl:far
        extrn   DosExit:far
        extrn   DosOpen:far
        extrn   DosRead:far
        extrn   DosSemClear:far
        extrn   DosSemSet:far
        extrn   DosSemWait:far
        extrn   DosSuspendThread:far
        extrn   DosWrite:far
        extrn   KbdCharIn:far
        extrn   KbdGetStatus:far
        extrn   KbdSetStatus:far
        extrn   VioWrtTTY:far

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

cname   label   byte            ; COM port logical name
        if      com1
        db      'COM1',0        ; COM1 device
        endif
        if      com2
        db      'COM2',0        ; COM2 device
        endif
        if      com3
        db      'COM3',0        ; COM3 device
        endif

f41info dw      baud            ; baud rate

f42info db      databit         ; data bits
        db      parity          ; parity 
        db      stopbit         ; stop bits

kbdinfo db      0               ; character code
        db      0               ; scan code
        db      0               ; status flags
        db      0               ; reserved
        dw      0               ; shift state
	dd	0		; timestamp

kbdstat dw      10              ; length of structure
        dw      0               ; keyboard state flags
        db      0,0             ; logical end-of-line
        dw      0               ; interim character flags
        dw      0               ; shift state

exitsem dd      0               ; exit semaphore

chandle dw      0               ; receives COM device handle
action  dw      0               ; receives DosOpen action 
sel     dw      0               ; receives selector
rlen    dw      0               ; receives bytes read count
wlen    dw      0               ; receives bytes written count
cinID   dw      0               ; COM input thread ID
coutID  dw      0               ; COM output thread ID
inchar	db	0		; input character from COM port

msg1    db      cr,lf
        db      "Can't open COM device."
        db      cr,lf
msg1_len equ    $-msg1

msg2    db      cr,lf
        db      "Can't set baud rate."
        db      cr,lf
msg2_len equ    $-msg2

msg3    db      cr,lf
        db      "Can't configure COM port."
        db      cr,lf
msg3_len equ    $-msg3

msg4    db      cr,lf
        db      "Memory allocation failure."
        db      cr,lf
msg4_len equ    $-msg4

msg5    db      cr,lf
        db      "Can't create COM input thread."
        db      cr,lf
msg5_len equ    $-msg5

msg6    db      cr,lf
        db      "Can't create COM output thread."
        db      cr,lf
msg6_len equ    $-msg6

msg7    db      cr,lf
        db      "Communicating..."
        db      cr,lf
msg7_len equ    $-msg7

msg8    db      cr,lf
        db      "Terminating..."
        db      cr,lf
msg8_len equ    $-msg8

_DATA   ends    


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far             ; entry point from OS/2
                                        
                                ; open COM port...
        push    ds              ; device name address
        push    offset DGROUP:cname
        push    ds              ; receives device handle
        push    offset DGROUP:chandle
        push    ds              ; receives action
        push    offset DGROUP:action
        push    0               ; initial allocation (N/A)
        push    0
        push    0               ; attribute (N/A)
        push    1               ; open flag: fail if device
                                ;            does not exist
        push    12h             ; open mode: read/write,
				;	     deny-all
        push    0               ; DWORD reserved
        push    0
        call    DosOpen         ; transfer to OS/2
        or      ax,ax           ; was function successful?
        jz      main1           ; yes, jump

        mov     cx,msg1_len     ; no, display error message
        mov     dx,offset DGROUP:msg1
        jmp     main9           ; and exit

main1:                          ; set baud rate...
        push    0               ; data buffer address
        push    0
        push    ds              ; parameter buffer address
        push    offset DGROUP:f41info
        push    41h             ; function number
        push    1               ; category number
        push    chandle         ; COM device handle
        call    DosDevIOCtl     ; transfer to OS/2
        or      ax,ax           ; was function successful?
        jz      main2           ; yes, jump

        mov     cx,msg2_len     ; no, display error message
        mov     dx,offset DGROUP:msg2
        jmp     main9           ; and exit

main2:                          ; configure parity, stop bits,
                                ; and character length...
        push    0               ; data buffer address
        push    0
        push    ds              ; parameter buffer address
        push    offset DGROUP:f42info
        push    42h             ; function number
        push    1               ; category number
        push    chandle         ; COM device handle
        call    DosDevIOCtl     ; transfer to OS/2
        or      ax,ax           ; was function successful?
        jz      main3           ; yes, jump

        mov     cx,msg3_len     ; no, display error message
        mov     dx,offset DGROUP:msg3
        jmp     main9           ; and exit

main3:                          ; put keyboard in binary mode
                                ; with echo off...

                                ; get keyboard state
        push    ds              ; address of info structure
        push    offset DGROUP:kbdstat
        push    0               ; default keyboard handle
        call    KbdGetStatus    ; transfer to OS/2

                                ; set binary mode, no echo
        and     word ptr kbdstat+2,0fff0h
        or      word ptr kbdstat+2,6

                                ; set keyboard state
        push    ds              ; address of info structure
        push    offset DGROUP:kbdstat
        push    0               ; default keyboard handle
        call    KbdSetStatus    ; transfer to OS/2

        push    ds              ; set exit semaphore
        push    offset DGROUP:exitsem
        call    DosSemSet       ; transfer to OS/2

                                ; allocate thread stack
        push    stksize         ; stack size in bytes
        push    ds              ; receives new selector
        push    offset DGROUP:sel
        push    0               ; not sharable/discardable
        call    DosAllocSeg     ; transfer to OS/2
        or      ax,ax           ; was function successful?
        jz      main4           ; yes, jump

        mov     cx,msg4_len     ; no, display error message
        mov     dx,offset DGROUP:msg4
        jmp     main9           ; and exit

main4:                          ; create COM input thread
        push    cs              ; thread entry point
        push    offset _TEXT:comin
        push    ds              ; receives thread ID    
        push    offset DGROUP:cinID
        push    sel             ; address of stack base
        push    stksize
        call    DosCreateThread ; transfer to OS/2
        or      ax,ax           ; was function successful?
        jz      main5           ; yes, jump

        mov     cx,msg5_len     ; no, display error message
        mov     dx,offset DGROUP:msg5
        jmp     main9           ; and exit

main5:                          ; allocate thread stack
        push    stksize         ; stack size in bytes
        push    ds              ; receives new selector
        push    offset DGROUP:sel
        push    0               ; not sharable/discardable
        call    DosAllocSeg     ; transfer to OS/2
        or      ax,ax           ; was function successful?
        jz      main6           ; yes, jump

        mov     cx,msg4_len     ; no, display error message
        mov     dx,offset DGROUP:msg4
        jmp     main9           ; and exit

main6:                          ; create COM output thread
        push    cs              ; thread entry point
        push    offset _TEXT:comout
        push    ds              ; receives thread ID    
        push    offset DGROUP:coutID
        push    sel             ; address of stack base
        push    stksize
        call    DosCreateThread ; transfer to OS/2
        or      ax,ax           ; was function successful?
        jz      main7           ; yes, jump

        mov     cx,msg6_len     ; no, display error message
        mov     dx,offset DGROUP:msg6
        jmp     main9           ; and exit

main7:                          ; display "Communicating..."
        push    ds              ; message address
        push    offset DGROUP:msg7
        push    msg7_len        ; message length
        push    0               ; default video handle
        call    VioWrtTTY       ; transfer to OS/2
        
                                ; wait for exit signal
        push    ds              ; semaphore handle
        push    offset DGROUP:exitsem
        push    -1              ; wait indefinitely
        push    -1
        call    DosSemWait      ; transfer to OS/2

                                ; suspend COM input thread
        push    cinID           ; thread ID
        call    DosSuspendThread ; transfer to OS/2

                                ; suspend COM output thread
        push    coutID          ; thread ID
        call    DosSuspendThread ; transfer to OS/2
        
main8:				; display "Terminating"...
        push    ds              ; message address
        push    offset DGROUP:msg8
        push    msg8_len        ; message length
        push    0               ; default video handle
        call    VioWrtTTY       ; transfer to OS/2

                                ; final exit to OS/2...
        push    1               ; terminate all threads
	push	0		; return code = 0 (success)
        call    DosExit         ; transfer to OS/2

main9:                          ; display error message...
                                ; DS:DX = msg, CX = length
        push    ds              ; address of message
        push    dx
        push    cx              ; length of message
        push    0               ; default video handle
        call    VioWrtTTY       ; transfer to OS/2

                                ; final exit to OS/2...
        push    1               ; terminate all threads
	push	1		; return code = 1 (error)
        call    DosExit         ; transfer to OS/2
        
main    endp


; COM input thread: Reads characters one at a time
; from the COM port, and writes them to the display.

comin   proc    far

				; read character from COM...
        push    chandle         ; COM device handle
        push    ds              ; receives COM data
        push    offset DGROUP:inchar
        push    1               ; length to read
        push    ds              ; receives read count
        push    offset DGROUP:rlen
        call    DosRead         ; transfer to OS/2

				; send character to display...
        push    ds              ; address of character
        push    offset DGROUP:inchar
        push    1               ; length to write
        push    0               ; default video handle
        call    VioWrtTTY       ; transfer to OS/2

	jmp	comin		; wait for next character

comin   endp


; COM output thread: Reads characters from the keyboard
; in raw mode and sends them to the COM port.  If the
; exit key is detected, the main thread is signaled to
; clean up and terminate the process.

comout  proc    far

				; read keyboard character...
        push    ds              ; receives keyboard data
        push    offset DGROUP:kbdinfo
        push    kwait           ; wait if necessary
        push    0               ; default keyboard handle
        call    KbdCharIn       ; transfer to OS/2

        mov     cx,1            ; assume writing one byte

        cmp     kbdinfo,0       ; check for extended key
        jz      comout1         ; jump, extended key
        cmp     kbdinfo,0e0h
        jnz     comout2         ; jump, not extended key

comout1:                        ; extended key detected
        mov     cx,2            ; must write 2 bytes

                                ; check for exit key
        cmp     kbdinfo+1,exitkey
        jnz     comout2         ; not exit key, jump

                                ; clear exit semaphore...
        push    ds              ; semaphore address
        push    offset DGROUP:exitsem
        call    DosSemClear     ; transfer to OS/2
        jmp     comout          ; discard exit key

comout2:			; send character to COM port...
        push    chandle         ; COM device handle
        push    ds              ; address of character
        push    offset DGROUP:kbdinfo
        push    cx              ; length to write
        push    ds              ; receives write count
        push    offset DGROUP:wlen
        call    DosWrite        ; transfer to OS/2

        jmp     comout          ; wait for next key

comout  endp

_TEXT   ends

        end     main

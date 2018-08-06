        title   SNAP -- Sample OS/2 Device Monitor
        page    55,132
        .286    

;
; SNAP.ASM
;
; A sample OS/2 device monitor that captures the current display into
; the file SNAPxx.IMG, where xx is the session number.  SNAP works in
; character mode only and may not be used in a PM window.  The
; following keys are defined as defaults:
;
; Alt-F10   hot key to capture a screen
; Ctrl-F10  hot key to deinstall SNAP.EXE
;
; Assemble with:  C> masm snap.asm;
; Link with:  C> link snap,,,os2,snap
;
; Usage is:  C> snap
;
; Copyright (C) 1988 Ray Duncan
;

cr      equ     0dh                     ; ASCII character codes
lf      equ     0ah

					; hot key definitions:
snapkey equ     71h                     ; snapshot   Alt-F10
exitkey equ     67h                     ; exit       Ctrl-F10

stksize equ     2048                    ; stack size for threads

        extrn   DosAllocSeg:far
        extrn   DosBeep:far
        extrn   DosBufReset:far
        extrn   DosClose:far
        extrn   DosCloseSem:far
        extrn   DosCreateSem:far
        extrn   DosCreateThread:far     
        extrn   DosExecPgm:far
        extrn   DosExit:far
        extrn   DosGetInfoSeg:far       
        extrn   DosOpenSem:far          
        extrn   DosMonClose:far
        extrn   DosMonOpen:far          
        extrn   DosMonRead:far
        extrn   DosMonReg:far
        extrn   DosMonWrite:far
        extrn   DosOpen:far             
        extrn   DosSemClear:far
        extrn   DosSemSet:far
        extrn   DosSemWait:far
        extrn   DosSetPrty:far
        extrn   DosSleep:far
        extrn   DosSuspendThread:far
        extrn   DosWrite:far
        extrn   VioEndPopUp:far
        extrn   VioGetMode:far
        extrn   VioPopUp:far
        extrn   VioReadCharStr:far      
        extrn   VioWrtCharStr:far

jerr    macro   p1,p2,p3                ;; Macro to test return code
	local	zero			;; in AX and jump if nonzero
        or      ax,ax                   ;; Uses JMP DISP16 to avoid
        jz      zero                    ;; branch out of range errors
        mov     dx,offset DGROUP:p2     ;; p2 = message address
        mov     cx,p3                   ;; p3 = message length
        jmp     p1                      ;; routine p1 displays message
zero:
        endm

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

exitsem dd      0                       ; semaphore for final exit
snapsem dd      0                       ; semaphore for 'snap' thread

sname   db      '\SEM\SNAP'             ; system semaphore name
sname1  db      'nn.LCK',0
shandle dd      0                       ; system semaphore handle

pflags  dw      0                       ; VioPopUp flags

wlen    dw      ?                       ; receives length written
action  dw      ?                       ; receives DosOpen action

watchID dw      ?                       ; keyboard thread ID
snapID  dw      ?                       ; snapshot thread ID

sel     dw      ?                       ; selector from DosAllocSeg

kname   db      'KBD$',0                ; keyboard device name
khandle dw      0                       ; keyboard monitor handle
                                
fname   db      '\SNAP'                 ; name of snapshot file
fname1  db      'nn.IMG',0
fhandle dw      0                       ; handle for snapshot file

scrbuf  db      80 dup (0)              ; receives screen data
slen    dw      $-scrbuf                ; length of screen buffer       

newline db      cr,lf                   ; carriage return-linefeed
nl_len  equ     $-newline

gseg	dw	?			; global info segment selector
lseg	dw	?			; local info segment selector

obuff	db	64 dup (0)		; receives name of dynlink
obuff_len equ   $-obuff                 ; causing DosExecPgm to fail

kbdin   dw      128,64 dup (0)          ; input and output buffers
kbdout  dw      128,64 dup (0)          ; for keyboard monitor

kbdpkt  db      128 dup (0)             ; keyboard data packet
kpktlen dw      ?                       ; length of buffer/packet

pname   db      'SNAP.EXE',0            ; child process name
retcode dd      0                       ; child process info

vioinfo label   byte                    ; receives display mode
        dw      8                       ; length of structure
        db      0                       ; display mode type
        db      0                       ; colors
cols    dw      0                       ; number of columns
rows    dw      0                       ; number of rows

msg1    db      'SNAP utility installed!'
msg1_len equ    $-msg1

msg2    db      'Alt-F10 to capture screen into file SNAP.IMG,'
msg2_len equ    $-msg2

msg3    db      'Ctrl-F10 to shut down SNAP.'
msg3_len equ    $-msg3
 
msg4    db      'SNAP utility deactivated.'
msg4_len equ    $-msg4

msg5    db      'Error detected during SNAP installation:'
msg5_len equ    $-msg5

msg6    db      'Can''t create SNAP system semaphore.'
msg6_len equ    $-msg6

msg7    db      'Can''t start child copy of SNAP.'
msg7_len equ    $-msg7

msg8    db      'SNAP is already loaded.'
msg8_len equ    $-msg8

msg9    db      'Can''t open KBD$ monitor connection.'
msg9_len equ    $-msg9

msg10   db      'Can''t register as KBD$ monitor.'
msg10_len equ   $-msg10

msg11   db      'Can''t allocate thread stack.'
msg11_len equ   $-msg11

msg12   db      'Can''t create keyboard thread.'
msg12_len equ   $-msg12

msg13   db      'Can''t create snapshot thread.'
msg13_len equ   $-msg13

msg14   db      'Can''t create snapshot file.'
msg14_len equ   $-msg14

divider db      79 dup ('-'),cr,lf
divider_len equ $-divider

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far                     ; entry point from OS/2

					; get info segment selectors
	push	ds			; receives global info selector
        push    offset DGROUP:gseg
	push	ds			; receives local info selector
        push    offset DGROUP:lseg
        call    DosGetInfoSeg           ; transfer to OS/2

                                        ; build system semaphore
                                        ; and snapshot file names
        mov     es,gseg                 ; get foreground screen group
        mov     al,es:[0018h]
        aam                             ; convert to ASCII
        add     ax,'00'
        xchg    ah,al
        mov     word ptr fname1,ax      ; store into filename
        mov     word ptr sname1,ax      ; store into semaphore name

                                        ; does SNAPxx.LCK exist?
        push    ds                      ; receives semaphore handle
        push    offset DGROUP:shandle
        push    ds
        push    offset DGROUP:sname     ; semaphore name
        call    DosOpenSem              ; transfer to OS/2
        or      ax,ax                   ; was open successful?
        jz      main1                   ; jump, we're child SNAP

                                        ; we're the parent SNAP,
                                        ; create system semaphore
	push	1			; make it nonexclusive
        push    ds                      ; receives semaphore handle
        push    offset DGROUP:shandle
        push    ds                      ; system semaphore name
        push    offset DGROUP:sname
        call    DosCreateSem            ; transfer to OS/2
        jerr    error,msg6,msg6_len     ; jump if create failed

                                        ; set the semaphore...
        push    word ptr shandle+2      ; semaphore handle
        push    word ptr shandle
        call    DosSemSet               ; transfer to OS/2

                                        ; launch child SNAP...
        push    ds                      ; object name buffer
        push    offset DGROUP:obuff     ; receives failed dynlink
        push    obuff_len               ; length of buffer
        push    4                       ; child detached
        push    0                       ; NULL argument pointer
        push    0
        push    0                       ; NULL environment pointer
        push    0
        push    ds                      ; receives child info
        push    offset DGROUP:retcode
        push    ds                      ; pathname for child
        push    offset DGROUP:pname
        call    DosExecPgm              ; request launch of child
        jerr    error,msg7,msg7_len     ; jump if launch failed

                                        ; wait for child to load
        push    word ptr shandle+2      ; semaphore handle
        push    word ptr shandle
        push    -1                      ; timeout = indefinite
        push    -1
        call    DosSemWait              ; transfer to OS/2

                                        ; close the semaphore...
        push    word ptr shandle+2      ; semaphore handle
        push    word ptr shandle
        call    DosCloseSem             ; transfer to OS/2

        jmp     main3                   ; now exit

main1:                                  ; come here if child SNAP...
                                        ; check if already resident
        push    word ptr shandle+2      ; semaphore handle
        push    word ptr shandle
        push    0                       ; timeout = 0 
        push    0                 
        call    DosSemWait              ; transfer to OS/2
        or      ax,ax                   ; is semaphore clear?
        jnz     main2                   ; no, proceed

                                        ; yes, don't load again
        mov     dx,offset DGROUP:msg8   ; address of warning message
        mov     cx,msg8_len             ; length of message
        jmp     error                   ; display message and exit

main2:                                  ; initialize semaphores...
        push    ds                      ; address of exit semaphore
        push    offset DGROUP:exitsem           
        call    DosSemSet               ; transfer to OS/2

        push    ds                      ; address of snapshot semaphore
        push    offset DGROUP:snapsem           
        call    DosSemSet               ; transfer to OS/2

                                        ; open monitor connection ...
        push    ds                      ; address of device name
        push    offset DGROUP:kname
        push    ds                      ; receives monitor handle
        push    offset DGROUP:khandle
        call    DosMonOpen              ; transfer to OS/2
        jerr    error,msg9,msg9_len     ; jump if open failed

                                        ; register as keyboard monitor
        push    khandle                 ; handle from DosMonOpen
        push    ds                      ; monitor input buffer address
        push    offset DGROUP:kbdin
        push    ds                      ; monitor output buffer address
        push    offset DGROUP:kbdout
        push    1                       ; position = front of list
        mov     es,gseg                 ; foreground session number
        mov     al,byte ptr es:[0018h]  ; from global info segment
        xor     ah,ah
        push    ax
        call    DosMonReg               ; transfer to OS/2
        jerr    error,msg10,msg10_len   ; jump if register failed

        push    stksize                 ; allocate stack for WATCH thread
        push    ds                      ; variable to receive selector
        push    offset DGROUP:sel
	push	0			; not shareable
        call    DosAllocSeg             ; transfer to OS/2
        jerr    error,msg11,msg11_len   ; jump, can't allocate stack

                                        ; create keyboard thread
        push    cs                      ; initial execution address
        push    offset _TEXT:watch
        push    ds                      ; receives thread ID
        push    offset DGROUP:watchID
        push    sel                     ; address of thread's stack
        push    stksize
        call    DosCreateThread         ; transfer to OS/2
        jerr    error,msg12,msg12_len   ; jump, can't create thread

                                        ; promote keyboard thread
        push    2                       ; scope = single thread
        push    3                       ; class = time critical
        push    0                       ; delta = 0
        push    watchID                 ; thread ID
        call    DosSetPrty              ; transfer to OS/2

        push    stksize                 ; allocate stack for SNAP thread
        push    ds                      ; variable to receive selector
        push    offset DGROUP:sel
	push	0			; not shareable
        call    DosAllocSeg             ; transfer to OS/2
        jerr    error,msg11,msg11_len   ; jump, can't allocate stack

                                        ; create snapshot thread
        push    cs                      ; initial execution address
        push    offset _TEXT:snap
        push    ds                      ; receives thread ID
        push    offset DGROUP:snapID
        push    sel                     ; address of thread's stack
        push    stksize
        call    DosCreateThread         ; transfer to OS/2
        jerr    error,msg13,msg13_len   ; jump, can't create thread

        call    signon                  ; announce installation

                                        ; tell parent we are running
        push    word ptr shandle+2      ; semaphore handle
        push    word ptr shandle
        call    DosSemClear             ; transfer to OS/2      

                                        ; block on exit semaphore...
        push    ds                      ; semaphore handle
        push    offset DGROUP:exitsem
        push    -1                      ; timeout = indefinite
        push    -1
        call    DosSemWait              ; transfer to OS/2

        push    watchID                 ; suspend keyboard thread
        call    DosSuspendThread        ; transfer to OS/2      

        push    snapID                  ; suspend snapshot thread
        call    DosSuspendThread        ; transfer to OS/2

                                        ; close monitor connection
        push    khandle                 ; monitor handle
        call    DosMonClose             ; transfer to OS/2

                                        ; close system semaphore
        push    word ptr shandle+2      ; semaphore handle
        push    word ptr shandle
        call    DosCloseSem             ; transfer to OS/2

                                        ; close snapshot file
        push    fhandle                 ; file handle
        call    DosClose                ; transfer to OS/2

	call	signoff 		; announce deinstallation

main3:  push    1                       ; terminate all threads
        push    0                       ; return success code
        call    DosExit                 ; final exit to OS/2

main    endp


error   proc    near                    ; fatal error encountered
					; DS:DX = message, CX = length

        test    khandle,-1              ; monitor active?
        jz      error1                  ; no, jump

                                        ; yes, shut it down
        push    khandle                 ; monitor handle
        call    DosMonClose             ; transfer to OS/2

error1: mov     ax,word ptr shandle     ; system semaphore open?
        or      ax,word ptr shandle+2
        jz      error2                  ; no, jump

                                        ; clear semaphore, in case
                                        ; we're the child SNAP
        push    word ptr shandle+2      ; semaphore handle
        push    word ptr shandle
        call    DosSemClear             ; transfer to OS/2

                                        ; close the semaphore
        push    word ptr shandle+2      ; semaphore handle
        push    word ptr shandle
        call    DosCloseSem             ; transfer to OS/2

error2: mov     ax,1                    ; get popup window
        call    popup

                                        ; display title...
        push    ds                      ; message address
        push    offset DGROUP:msg5
        push    msg5_len                ; message length
        push    10                      ; Y
        push    (80-msg5_len)/2         ; X (center it)
	push	0			; Vio handle
        call    VioWrtCharStr           ; transfer to OS/2

                                        ; display error message...
        push    ds                      ; message address
        push    dx
        push    cx                      ; message length
        push    12                      ; Y
        mov     ax,80                   ; X (center it)
        sub     ax,cx
        shr     ax,1
        push    ax
	push	0			; Vio handle
        call    VioWrtCharStr           ; transfer to OS/2

	push	0			; pause for 3 seconds
	push	3000
        call    DosSleep                ; transfer to OS/2

        call    unpop                   ; release popup window

        push    1                       ; terminate all threads
        push    1                       ; return error code
        call    DosExit                 ; exit program

error   endp


watch   proc    far                     ; keyboard thread, monitors
					; for snapshot or exit hot keys

        mov     kpktlen,kpktlen-kbdpkt  ; max buffer length for read

                                        ; get keyboard data packet...
        push    ds                      ; monitor input buffer address
        push    offset DGROUP:kbdin
        push    0                       ; wait until data available
        push    ds
        push    offset DGROUP:kbdpkt    ; receives keyboard data packet
        push    ds
        push    offset DGROUP:kpktlen   ; contains/receives length
        call    DosMonRead              ; transfer to OS/2

        cmp     kbdpkt+2,0              ; is this extended code?
        jnz     watch1                  ; no, pass it on

	cmp	kbdpkt+3,exitkey	; is it exit hot key?
        jz      watch2                  ; jump if exit key

	cmp	kbdpkt+3,snapkey	; is it snapshot hot key?
        jnz     watch1                  ; no, jump

        cmp     word ptr kbdpkt+12,0    ; is it break packet?
        jnz     watch                   ; yes, ignore it

					; snapshot hot key detected
                                        ; clear snapshot semaphore...
        push    ds                      ; semaphore handle
        push    offset DGROUP:snapsem
        call    DosSemClear             ; transfer to OS/2
	jmp	watch			; discard this hot key

watch1: 				; not hot key, pass character
        push    ds                      ; monitor output buffer address
        push    offset DGROUP:kbdout
        push    ds                      ; keyboard data packet address
        push    offset DGROUP:kbdpkt    
        push    kpktlen                 ; length of data packet
        call    DosMonWrite             ; transfer to OS/2
        
        jmp     watch                   ; get another packet

watch2: 				; exit hot key detected...
        cmp     word ptr kbdpkt+12,0    ; is it break packet?
        jnz     watch                   ; yes, ignore it

                                        ; clear exit semaphore...
        push    ds                      ; semaphore handle
        push    offset DGROUP:exitsem
        call    DosSemClear             ; transfer to OS/2

        jmp     watch                   ; let thread 1 shut down

watch   endp


snap    proc    far                     ; This thread blocks on the 
                                        ; snapshot semaphore, then
                                        ; dumps the screen contents
					; to the file SNAPxx.IMG.
        
                                        ; open/create snapshot file
        push    ds                      ; address of filename
        push    offset DGROUP:fname
        push    ds                      ; variable to receive file handle
        push    offset DGROUP:fhandle
        push    ds                      ; variable to receive action taken
        push    offset DGROUP:action    
        push    0                       ; initial file size
        push    0
        push    0                       ; normal file attribute 
        push    12h                     ; create or replace file
        push    21h                     ; write access, deny write
        push    0                       ; DWORD reserved
        push    0
        call    DosOpen                 ; transfer to OS/2
        jerr    error,msg14,msg14_len   ; jump if can't create
 
snap1:                                  ; write divider line 
        push    fhandle                 ; file handle
        push    ds                      ; address of divider string
        push    offset DGROUP:divider
        push    divider_len             ; length of string
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

                                        ; force disk update...
        push    fhandle                 ; file handle
        call    DosBufReset             ; transfer to OS/2

                                        ; wait on snapshot semaphore
        push    ds                      ; semaphore handle
        push    offset DGROUP:snapsem
        push    -1                      ; timeout = indefinite
        push    -1
        call    DosSemWait              ; transfer to OS/2

	mov	ax,3			; pop-up in transparent mode
        call    popup                   ; to read screen contents

                                        ; get screen dimensions...
        push    ds                      ; receives video mode info
        push    offset DGROUP:vioinfo
	push	0			; Vio handle
        call    VioGetMode              ; transfer to OS/2

        mov     bx,0                    ; BX := initial screen row 

snap2:                                  ; read line from screen...
        mov     ax,cols                 ; width to read
        mov     slen,ax
        push    ds                      ; address of screen buffer
        push    offset DGROUP:scrbuf
        push    ds                      ; contains/receives length
        push    offset DGROUP:slen
        push    bx                      ; screen row
        push    0                       ; screen column
	push	0			; Vio handle
        call    VioReadCharStr          ; transfer to OS/2

        push    ds                      ; scan backwards from end
        pop     es                      ; of line to find last 
	mov	cx,slen 		; nonblank character
        mov     di,offset DGROUP:scrbuf
        add     di,slen
        dec     di
        mov     al,20h
        std     
        repe scasb
        cld     
        jz      snap3                   ; if Z = True, line was empty
        inc     cx                      ; otherwise correct the length

snap3:                                  ; write line to file...
        push    fhandle                 ; file handle
        push    ds                      ; address of data
        push    offset DGROUP:scrbuf
        push    cx                      ; clipped line length
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

					; write newline (CR-LF)
        push    fhandle                 ; file handle
        push    ds
	push	offset DGROUP:newline	; address of newline
	push	nl_len			; length of newline
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2      

        inc     bx                      ; bump screen row counter
        cmp     bx,rows                 ; whole screen done yet?
        jne     snap2                   ; no, write another

        push    440                     ; reward user with some
        push    200                     ; audible feedback
        call    DosBeep                 ; transfer to OS/2

        call    unpop                   ; release the screen

                                        ; done with screen capture,
                                        ; reset snapshot semaphore
        push    ds                      ; semaphore handle
        push    offset DGROUP:snapsem
        call    DosSemSet               ; transfer to OS/2

        jmp     snap1                   ; go wait on semaphore
        
snap    endp


signon  proc    near                    ; announce installation,
                                        ; display help message

        mov     ax,1                    ; put up popup window
	call	popup			; mode = wait, nontransparent

        push    ds                      ; message address
        push    offset DGROUP:msg1
        push    msg1_len                ; message length
        push    10                      ; Y
        push    (80-msg1_len)/2         ; X (center it)
	push	0			; Vio handle
        call    VioWrtCharStr           ; transfer to OS/2

        push    ds                      ; message address
        push    offset DGROUP:msg2
        push    msg2_len                ; message length
        push    13                      ; Y
        push    (80-msg2_len)/2         ; X (center it)
	push	0			; Vio handle
        call    VioWrtCharStr           ; transfer to OS/2

        push    ds                      ; message address
        push    offset DGROUP:msg3
        push    msg3_len                ; message length
        push    15                      ; Y
        push    (80-msg3_len)/2         ; X (center it)
	push	0			; Vio handle
        call    VioWrtCharStr           ; transfer to OS/2

	push	0			; pause for 4 seconds
	push	4000			; so user can read message
        call    DosSleep                ; transfer to OS/2      

        call    unpop                   ; take down popup window
        ret                             ; back to caller

signon  endp


signoff proc	near			; announce deinstallation

        mov     ax,1                    ; put up popup window
	call	popup			; mode = wait, nontransparent

        push    ds                      ; message address
        push    offset DGROUP:msg4
        push    msg4_len                ; message length
        push    12                      ; Y
        push    (80-msg4_len)/2         ; X (center it)
        push    0                       ; VIO handle
        call    VioWrtCharStr

	push	0			; pause for 2 seconds
	push	2000			; so user can read message
        call    DosSleep                ; transfer to OS/2

        call    unpop                   ; take down popup window
        ret                             

signoff endp


popup   proc    near                    ; put up popup window
                                        ; AX = VioPopUp flags
                                        ; bit 0 = 0 no wait
					;	  1 wait for pop-up
					; bit 1 = 0 nontransparent
                                        ;         1 transparent

	mov	pflags,ax		; set pop-up mode

	push	ds			; address of popup flags
        push    offset DGROUP:pflags
	push	0			; Vio handle
        call    VioPopUp                ; transfer to OS/2
        ret                             ; back to caller

popup   endp


unpop   proc    near                    ; take down popup window

	push	0			; Vio handle
        call    VioEndPopUp             ; transfer to OS/2
        ret                             ; back to caller

unpop   endp

_TEXT   ends

        end     main




        title   EXEC2.ASM Demo of asynchronous child process
        page    55,132
        .286
; 
; EXEC2.ASM   Demonstrates use of DosExecPgm and DosCwait to 
;             run CHKDSK as an asynchronous child process.
; Copyright (C) 1987 Ray Duncan
;
; Build:        MASM EXEC2;
;               LINK EXEC2,,,OS2,EXEC2
;
; Usage:        EXEC2
;
; Note:         File CHKDSK.COM must be available in PATH.

stdin   equ     0                       ; standard input handle
stdout  equ     1                       ; standard output handle
stderr  equ     2                       ; standard error handle

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII line feed

        extrn   DosCwait:far            ; reference to OS/2 API
        extrn   DosExecPgm:far          
        extrn   DosExit:far
        extrn   DosWrite:far

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

objbuff db      64 dup (0)      ; receives name of dynamic link
objbuff_len equ $-objbuff       ; causing DosExecPgm failure    

                                ; argument strings for child
argblk  db      'chkdsk',0      ; simple filename of child
        db      ' *.*',0        ; simulated command tail
        db      0               ; extra null byte terminates block

                                ; receives DosExecPgm info
cinfo   dw      0               ; PID of child process
        dw      0               ; not used

                                ; receives DosCwait info
retcode dw      0               ; termination code for child
        dw      0               ; result code from child's DosExit

pname   db      'chkdsk.com',0  ; pathname of child program

wlen    dw      ?

msg1    db      cr,lf,'Unexpected OS/2 Error',cr,lf
msg1_len equ $-msg1

msg2    db      cr,lf,'Exec function successful',cr,lf
msg2_len equ $-msg2

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far

                                        ; run CHKDSK as an asyn-
                                        ; chronous child process...
        push    ds                      ; receives module/entry
        push    offset DGROUP:objbuff   ; point if dynlink fails
        push    objbuff_len             ; length of object buffer
        push    2                       ; 2 = execute asynchronously
        push    ds                      ; address of argument block
        push    offset DGROUP:argblk
        push    0                       ; address of environment
        push    0                       ; (0 = inherit parent's) 
        push    ds                      ; receives child's PID
        push    offset DGROUP:cinfo
        push    ds                      ; name of child program
        push    offset DGROUP:pname
        call    DosExecPgm              ; transfer to OS/2
        or      ax,ax                   ; did child process run?
        jnz     error                   ; jump if function failed

                                        ; we could do other 
                                        ; other processing here...

                                        ; now resynchronize with
                                        ; child process...
        push    0                       ; 0=immediate child only
        push    0                       ; 0=wait till child ends
        push    ds                      ; receives termination info
        push    offset DGROUP:retcode
        push    ds                      ; receives PID (N/A here)
        push    offset DGROUP:cinfo+2
        push    cinfo                   ; PID of process to wait for
        call    DosCwait                ; transfer to OS/2

exit:                                   ; display success message...
        push    stdout                  ; standard output handle
        push    ds                      ; message address
        push    offset msg2
        push    msg2_len                ; message length
        push    ds                      ; receives bytes written
        push    offset wlen
        call    DosWrite                ; transfer to OS/2

        push    1                       ; terminate all threads
        push    0                       ; return success code
        call    DosExit                 ; exit program

error:                                  ; display error message...
        push    stdout                  ; standard output handle
        push    ds                      ; message address
        push    offset msg1
        push    msg1_len                ; message length
        push    ds                      ; receives bytes written
        push    offset wlen               
        call    DosWrite                ; transfer to OS/2

        push    1                       ; terminate all threads
        push    1                       ; return error code
        call    DosExit                 ; exit program

main    endp

_TEXT   ends

        end     main


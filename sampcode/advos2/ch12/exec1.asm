        title   EXEC1.ASM Demo of synchronous child process
        page    55,132
        .286
; 
; EXEC1.ASM   Demonstrates use of DosExecPgm to run
;             CHKDSK as a synchronous child process.
; Copyright (C) 1987 Ray Duncan
;
; Build:        MASM EXEC1;
;               LINK EXEC1,,,OS2,EXEC1
;
; Usage:        EXEC1 
;               
; Note:         File CHKDSK.COM must be available in PATH.

stdin   equ     0                       ; standard input handle
stdout  equ     1                       ; standard output handle
stderr  equ     2                       ; standard error handle

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII line feed

        extrn   DosExecPgm:far          ; reference to OS/2 API
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

                                ; receives return codes from child...
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

                                        ; run CHKDSK as a syn-
                                        ; chronous child process...
        push    ds                      ; receives module/entry
        push    offset DGROUP:objbuff   ; point if dynlink fails
        push    objbuff_len             ; length of object buffer
        push    0                       ; 0=execute synchronously
        push    ds                      ; address of argument block
        push    offset DGROUP:argblk
        push    0                       ; address of environment
        push    0                       ; (0 = inherit parent's) 
        push    ds                      ; receives child's exit
        push    offset DGROUP:retcode   ; and termination codes
        push    ds                      ; name of child program
        push    offset DGROUP:pname
        call    DosExecPgm              ; transfer to OS/2
        or      ax,ax                   ; did child process run?
        jnz     error                   ; jump if function failed

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
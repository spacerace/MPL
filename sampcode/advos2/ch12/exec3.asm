        title   EXEC3.ASM Demo of DosStartSession
        page    55,132
        .286
; 
; EXEC3.ASM  Demonstration of DosStartSession, runs CMD.EXE
;            in a new background session to display a directory,
;            then waits for a keypress before exiting.
;            You can use the Task Manager to switch to the
;            new session and end that session by entering EXIT.
;            Child session will also be terminated unilaterally
;            when you press a key to terminate this process.
; Copyright (C) 1987 Ray Duncan
;
; Build:        MASM EXEC3;
;               LINK EXEC3,,,OS2,EXEC3
;
; Usage:        EXEC3
;               
; Note:         File CMD.EXE must be available in PATH.

stdin   equ     0                       ; standard input handle
stdout  equ     1                       ; standard output handle
stderr  equ     2                       ; standard error handle

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII line feed

        extrn   DosExit:far             ; references to OS/2 API
        extrn   DosStartSession:far
        extrn   DosWrite:far
        extrn   KbdCharIn:far

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

argstr  db      ' /k dir/w',0           ; argument string for child
pname   db      'cmd.exe',0             ; pathname for child
stitle  db      'Child Command Processor',0

childID dw      0                       ; child process ID
sesID   dw      0                       ; child session ID

                                        ; session data...
sinfo   dw      24                      ; length of structure
        dw      1                       ; 0=unrelated, 1=related
        dw      1                       ; 0=foreground, 1=background
        dw      0                       ; 0=nontraceable, 1=traceable
        dd      stitle                  ; session title
        dd      pname                   ; program pathname
        dd      argstr                  ; argument strings
        dd      0                       ; termination queue (not used)

kbdinfo db      10 dup (0)              ; character input data
wlen    dw      ?                       ; receives actual bytes written

msg1    db      cr,lf,'Unexpected OS/2 Error',cr,lf
msg1_len equ $-msg1

msg2    db      cr,lf,'Press any key to terminate parent...',0
msg2_len equ $-msg2

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far

                                        ; run CMD.EXE in a new
                                        ; session as a child process
        push    ds                      ; address of session data
        push    offset DGROUP:sinfo
        push    ds                      ; receives new session ID
        push    offset DGROUP:sesID
        push    ds                      ; receives child process ID
        push    offset DGROUP:childID
        call    DosStartSession         ; transfer to OS/2
        or      ax,ax                   ; did call succeed?
        jnz     error                   ; jump if function failed

                                        ; display message,
                                        ; 'Press any key'...
        push    stdout                  ; standard output handle
        push    ds                      ; address of message
        push    offset msg2
        push    msg2_len                ; length of message
        push    ds                      ; receives bytes written 
        push    offset wlen
        call    DosWrite                ; transfer to OS/2

                                        ; now wait for key...
        push    ds                      ; receives keyboard data
        push    offset DGROUP:kbdinfo
        push    0                       ; 0=wait for character
        push    0                       ; keyboard handle
        call    KbdCharIn               ; transfer to OS/2

exit:                                   ; final exit to OS/2...
        push    1                       ; terminate all threads
        push    0                       ; return success code
        call    DosExit                 ; exit program

error:                                  ; display error message
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
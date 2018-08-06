	title	  EXECSORT -- Run SORT.EXE as Child
        page      55,132
        .286
	.sall

;
; EXECSORT.ASM
;
; Demonstration of use of DosExecPgm to run the OS/2 filter SORT.EXE
; as a child process, redirecting its input to MYFILE.DAT and its
; output to MYFILE.SRT.
;
; Assemble with:  C> masm execsort.asm;
; Link with:  C> link execsort,,,os2,execsort
;
; Usage is:  C> execsort
;
; Copyright (C) 1988 Ray Duncan
;

stdin   equ     0               ; standard input device
stdout  equ     1               ; standard output device        
stderr  equ     2               ; standard error device

cr      equ     0dh             ; ASCII carriage return
lf      equ     0ah             ; ASCII linefeed

        extrn   DosClose:far
        extrn   DosDupHandle:far
        extrn   DosExecPgm:far
        extrn   DosExit:far
        extrn   DosOpen:far
        extrn   DosWrite:far

jerr	macro	target		;; Macro to test AX
	local	zero		;; and jump if AX nonzero
        or      ax,ax
	jz	zero		;; Uses JMP DISP16 to avoid
	jmp	target		;; branch out of range errors
zero:
        endm


DGROUP  group   _DATA

_DATA   segment word public 'DATA'

iname   db      'MYFILE.DAT',0  ; name of input file
oname   db      'MYFILE.SRT',0  ; name of output file

ihandle dw      ?               ; handle for input file
ohandle dw      ?               ; handle for output file

action  dw      ?               ; receives DosOpen action

oldin   dw      -1              ; dup of old stdin handle
oldout  dw      -1              ; dup of old stdout handle      

newin   dw      stdin           ; forced to track ihandle
newout  dw      stdout          ; forced to track ohandle

pname   db      'SORT.EXE',0    ; pathname of SORT filter

objbuff db      64              ; receives failing dynlink
objbuff_len equ $-objbuff

pcodes  dw      0,0             ; PID, exit code of child

msg     db      cr,lf,'SORT was executed as child.',cr,lf
msg_len equ $-msg 

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far             ; entry point from OS/2

                                ; prepare stdin and stdout
                                ; handles for child SORT...

                                ; dup handle for stdin...
        push    stdin           ; standard input handle
        push    ds              ; receives new handle
        push    offset DGROUP:oldin
        call    DosDupHandle    ; transfer to OS/2
        jerr    main1           ; exit if error

                                ; dup handle for stdout...
        push    stdout          ; standard output handle
        push    ds              ; receives new handle
        push    offset DGROUP:oldout    
        call    DosDupHandle    ; transfer to OS/2
        jerr    main1           ; exit if error

                                ; open input file...
        push    ds              ; address of filename
        push    offset DGROUP:iname
        push    ds              ; receives file handle
        push    offset DGROUP:ihandle
        push    ds              ; receives DosOpen action
        push    offset DGROUP:action
	push	0		; file size (not used)
        push    0               
        push    0               ; attribute (not used)
        push    1               ; action: open if exists
                                ;         fail if doesn't
        push    40h             ; access: read-only
        push    0               ; reserved DWORD 0
        push    0
        call    DosOpen         ; transfer to OS/2
        jerr    main1           ; exit if error
        
                                ; create output file...
        push    ds              ; address of filename
        push    offset DGROUP:oname
        push    ds              ; receives file handle
        push    offset DGROUP:ohandle
        push    ds              ; receives DOSOPEN action
        push    offset DGROUP:action
	push	0		; initial file size
        push    0               
        push    0               ; attribute = normal 
        push    12h             ; action: create/replace
        push    41h             ; access: write-only
        push    0               ; reserved DWORD 0
        push    0
        call    DosOpen         ; transfer to OS/2
        jerr    main1           ; exit if error

				; make stdin track
				; input file handle...
        push    ihandle         ; handle from DOSOPEN
        push    ds              ; standard input handle
        push    offset DGROUP:newin
        call    DosDupHandle    ; transfer to OS/2
        jerr    main1           ; exit if error

				; make stdout track
				; output file handle...
        push    ohandle         ; handle from DOSOPEN
        push    ds              ; standard output handle
        push    offset DGROUP:newout
        call    DosDupHandle    ; transfer to OS/2
        jerr    main1           ; exit if error

                                ; run SORT.EXE as child...
        push    ds              ; receives failing dynlink
        push    offset DGROUP:objbuff
        push    objbuff_len     ; length of buffer
        push    0               ; 0 = synchronous execution
        push    0               ; argument strings ptr
        push    0
        push    0               ; environment pointer
        push    0
        push    ds              ; receives PID, exit code
        push    offset DGROUP:pcodes
        push    ds              ; child program pathname
        push    offset DGROUP:pname
        call    DosExecPgm      ; transfer to OS/2
        jerr    main1           ; exit if error

				; restore stdin handle
				; to original meaning...
        push    oldin           ; dup of original stdin
        push    ds              ; standard input handle
        push    offset DGROUP:newin
        call    DosDupHandle    ; transfer to OS/2
        jerr    main1           ; exit if error

				; restore stdout handle
				; to original meaning...
        push    oldout          ; dup of original stdout
        push    ds              ; standard output handle
        push    offset DGROUP:newout
        call    DosDupHandle    ; transfer to OS/2
        jerr    main1           ; exit if error

        push    oldin           ; close dup of stdin
        call    DosClose        ; transfer to OS/2
        jerr    main1           ; exit if error

        push    oldout          ; close dup of stdout
        call    DosClose        ; transfer to OS/2
        jerr    main1           ; exit if error

        push    ihandle         ; close input file 
        call    DosClose        ; transfer to OS/2
        jerr    main1           ; exit if error

        push    ohandle         ; close output file
        call    DosClose        ; transfer to OS/2
        jerr    main1           ; exit if error

                                ; display success message...
        push    stdout          ; standard output handle
        push    ds              ; address of message
        push    offset DGROUP:msg
        push    msg_len         ; length of message
        push    ds              ; receives bytes written
        push    offset DGROUP:action
        call    DosWrite        ; transfer to OS/2
        jerr    main1           ; exit if error

                                ; exit point if no errors
        push    1               ; terminate all threads
        push    0               ; exit code = 0 (success)
        call    DosExit         ; transfer to OS/2

main1:                          ; exit point if error
        push    1               ; terminate all threads
        push    1               ; exit code = 1 (error)
        call    DosExit

main    endp                    

_TEXT   ends

        end     main            ; defines entry point

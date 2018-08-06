        name    execsort
        title   'EXECSORT --- demonstrate EXEC of filter'
        .sall
;
; EXECSORT.ASM --- demonstration of use of EXEC to run the SORT
; filter as a child process, redirecting its input and output.
; This program requires the files SORT.EXE and MYFILE.DAT in
; the current drive and directory.
; 
; Ray Duncan, June 1987
;

stdin   equ     0                       ; standard input
stdout  equ     1                       ; standard output
stderr  equ     2                       ; standard error

stksize equ     128                     ; size of stack

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII linefeed

jerr    macro   target                  ;; Macro to test carry flag
        local   notset                  ;; and jump if flag set.
        jnc     notset                  ;; Uses JMP DISP16 to avoid
        jmp     target                  ;; branch out of range errors
notset:
        endm


DGROUP  group   _DATA,_STACK            ; 'automatic data group'


_TEXT   segment byte public 'CODE'      ; executable code segment

        assume  cs:_TEXT,ds:DGROUP,ss:_STACK


stk_seg dw      ?                       ; original SS contents
stk_ptr dw      ?                       ; original SP contents 


main    proc    far                     ; entry point from MS-DOS

        mov     ax,DGROUP               ; set DS = our data segment
        mov     ds,ax

                                        ; now give back extra memory so
                                        ; child SORT has somewhere to run...
        mov     ax,es                   ; let AX = segment of PSP base
        mov     bx,ss                   ; and BX = segment of stack base
        sub     bx,ax                   ; reserve seg stack - seg psp 
        add     bx,stksize/16           ; plus paragraphs of stack
        mov     ah,4ah                  ; fxn 4AH = modify memory block
        int     21h                     ; transfer to MS-DOS
        jerr    main1                   ; jump if resize block failed

                                        ; prepare stdin and stdout
                                        ; handles for child SORT process

        mov     bx,stdin                ; dup the handle for stdin
        mov     ah,45h
        int     21h                     ; transfer to MS-DOS
        jerr    main1                   ; jump if dup failed
        mov     oldin,ax                ; save dup'd handle     

        mov     dx,offset DGROUP:infile ; now open the input file
        mov     ax,3d00h                ; mode = read-only
        int     21h                     ; transfer to MS-DOS
        jerr    main1                   ; jump if open failed

        mov     bx,ax                   ; force stdin handle to
        mov     cx,stdin                ; track the input file handle
        mov     ah,46h
        int     21h                     ; transfer to MS-DOS
        jerr    main1                   ; jump if force dup failed

        mov     bx,stdout               ; dup the handle for stdout
        mov     ah,45h
        int     21h                     ; transfer to MS-DOS
        jerr    main1                   ; jump if dup failed
        mov     oldout,ax               ; save dup'd handle

        mov     dx,offset dGROUP:outfile ; now create the output file
        mov     cx,0                    ; normal attribute
        mov     ah,3ch
        int     21h                     ; transfer to MS-DOS 
        jerr    main1                   ; jump if create failed

        mov     bx,ax                   ; force stdout handle to
        mov     cx,stdout               ; track the output file handle
        mov     ah,46h
        int     21h                     ; transfer to MS-DOS
        jerr    main1                   ; jump if force dup failed

                                        ; now EXEC the child SORT, 
                                        ; which will inherit redirected
                                        ; stdin and stdout handles

        push    ds                      ; save EXECSORT's data segment
        mov     stk_seg,ss              ; save EXECSORT's stack pointer
        mov     stk_ptr,sp

        mov     ax,ds                   ; set ES = DS
        mov     es,ax
        mov     dx,offset DGROUP:cname  ; DS:DX = child pathname
        mov     bx,offset DGROUP:pars   ; EX:BX = parameter block
        mov     ax,4b00h                ; function 4BH, subfunction 00H
        int     21h                     ; transfer to MS-DOS

        cli                             ; (for bug in some early 8088s)
        mov     ss,stk_seg              ; restore execsort's stack pointer
        mov     sp,stk_ptr
        sti                             ; (for bug in some early 8088s)
        pop     ds                      ; restore DS = our data segment

        jerr    main1                   ; jump if EXEC failed

        mov     bx,oldin                ; restore original meaning of
        mov     cx,stdin                ; standard input handle for
        mov     ah,46h                  ; this process
        int     21h
        jerr    main1                   ; jump if force dup failed

        mov     bx,oldout               ; restore original meaning
        mov     cx,stdout               ; of standard output handle
        mov     ah,46h                  ; for this process
        int     21h
        jerr    main1                   ; jump if force dup failed

        mov     bx,oldin                ; close dup'd handle of 
        mov     ah,3eh                  ; original stdin
        int     21h                     ; transfer to MS-DOS
        jerr    main1                   ; jump if close failed

        mov     bx,oldout               ; close dup'd handle of 
        mov     ah,3eh                  ; original stdout
        int     21h                     ; transfer to MS-DOS
        jerr    main1                   ; jump if close failed

                                        ; display success message
        mov     dx,offset DGROUP:msg1   ; address of message
        mov     cx,msg1_len             ; message length
        mov     bx,stdout               ; handle for standard output
        mov     ah,40h                  ; fxn 40H = write file or device
        int     21h                     ; transfer to MS-DOS
        jerr    main1
        
        mov     ax,4c00h                ; no error, terminate program 
        int     21h                     ; with return code = 0

main1:  mov     ax,4c01h                ; error, terminate program
        int     21h                     ; with return code = 1 

main    endp                            ; end of main procedure

_TEXT   ends


_DATA   segment para public 'DATA'      ; static & variable data segment

infile  db      'MYFILE.DAT',0          ; input file for SORT filter
outfile db      'MYFILE.SRT',0          ; output file for SORT filter

oldin   dw      ?                       ; dup of old stdin handle
oldout  dw      ?                       ; dup of old stdout handle      

cname   db      'SORT.EXE',0            ; pathname of child SORT process

pars    dw      0                       ; segment of environment block
                                        ; (0 = inherit parent's)
        dd      tail                    ; long address, command tail
        dd      -1                      ; long address, default FCB #1
                                        ; (-1 = none supplied)
        dd      -1                      ; long address, default FCB #2
                                        ; (-1 = none supplied)

tail    db      0,cr                    ; empty command tail for child

msg1    db      cr,lf,'SORT was executed as child.',cr,lf
msg1_len equ    $-msg1 

_DATA   ends


_STACK  segment para stack 'STACK'

        db      stksize dup (?)

_STACK  ends


        end     main                    ; defines program entry point

; SHELL.ASM   A simple program to run an application as an
;             MS-DOS shell program. The program name and
;             startup parameters must be adjusted before
;             SHELL is assembled.
;
; Written by William Wong
;
; To create SHELL.COM:
;
;             C>MASM SHELL;
;             C>LINK SHELL;
;             C>EXE2BIN SHELL.EXE SHELL.COM

stderr  equ 2                ; standard error
cr      equ 0dh              ; ASCII carriage return
lf      equ 0ah              ; ASCII linefeed
cseg    segment para public 'CODE'
;
; ----  Set up DS, ES, and SS:SP to run as .COM  ----
;
        assume  cs:cseg
start   proc    far
        mov     ax,cs           ; set up segment registers
        add     ax,10h          ; AX = segment after PSP
        mov     ds,ax
        mov     ss,ax           ; set up stack pointer
        mov     sp,offset stk
        mov     ax,offset shell
        push    cs              ; push original CS
        push    ds              ; push segment of shell
        push    ax              ; push offset of shell
        ret                     ; jump to shell
start   endp
;
; ----  Main program running as .COM  ----
;
; CS, DS, SS = cseg
; Original CS value on top of stack
;
        assume cs:cseg,ds:cseg,ss:cseg
seg_size equ (((offset last) - (offset start)) + 10fh)/16
shell   proc    near
        pop     es              ; ES = segment to shrink
        mov     bx,seg_size     ; BX = new segment size
        mov     ah,4ah          ; AH = modify memory block
        int     21h             ; free excess memory
        mov     cmd_seg,ds      ; setup segments in
        mov     fcb1_seg,ds     ; parameter block for EXEC
        mov     fcb2_seg,ds
        mov     dx,offset main_loop
        mov     ax,2523h        ; AX = set Control-C handler
        int     21h             ; set handler to DS:DX
        mov     dx,offset main_loop
        mov     ax,2524h        ; AX = set critical error handler
        int     21h             ; set handler to DS:DX
                                ; Note: DS is equal to CS
main_loop:
        push    ds              ; save segment registers
        push    es
        mov     cs:stk_seg,ss   ; save stack pointer
        mov     cs:stk_off,sp
        mov     dx,offset pgm_name
        mov     bx,offset par_blk
        mov     ax,4b00h        ; AX = EXEC/run program
        int     21h             ; carry = EXEC failed
        mov     ss,cs:stk_seg   ; restore stack pointer
        mov     sp,cs:stk_off
        pop     es              ; restore segment registers
        pop     ds
        jnc     main_loop       ; loop if program run
        mov     dx,offset load_msg
        mov     cx,load_msg_length
        call    print           ; display error message
        mov     ah,08h          ; AH = read without echo
        int     21h             ; wait for any character
        jmp     main_loop       ; execute forever
shell   endp
;
; ----  Print string  ----
;
; DS:DX = address of string
; CX    = size
;
print   proc    near
        mov     ah,40h          ; AH = write to file
        mov     bx,stderr       ; BX = file handle
        int     21h             ; print string
        ret
print   endp
;
; ----  Message strings  ----
;
load_msg db cr,lf
         db 'Cannot load program.',cr,lf
         db 'Press any key to try again.',cr,lf
load_msg_length equ $-load_msg
;
; ----  Program data area  ----
;
stk_seg  dw     0               ; stack segment pointer
stk_off  dw     0               ; save area during EXEC
pgm_name db     '\NEWSHELL.COM',0 ; any program will do
par_blk  dw     0               ; use current environment
         dw     offset cmd_line ; command-line address
cmd_seg  dw     0               ; fill in at initialization
         dw     offset fcb1     ; default FCB #1
fcb1_seg dw     0               ; fill in at initialization
         dw     offset fcb2     ; default FCB #2
fcb2_seg dw     0               ; fill in at initialization
cmd_line db     0,cr            ; actual command line
fcb1     db     0
         db     11 dup (' ')
         db     25 dup ( 0 )
fcb2     db     0
         db     11 dup (' ')
         db     25 dup ( 0 )
         dw     200 dup ( 0 )   ; program stack area
stk      dw     0
last     equ    $               ; last address used
cseg     ends
         end    start

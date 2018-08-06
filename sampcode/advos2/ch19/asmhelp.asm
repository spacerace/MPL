	title	ASMHELP -- Sample MASM DLL
        page    55,132
	.286

; 
; ASMHELP.ASM
;
; Source code for the MASM dynlink library ASMHELP.DLL.
;
; Assemble with:  C> masm asmhelp.asm;
; Link with:  C> link asmhelp,asmhelp.dll,,os2,asmhelp
;
; Exports two routines for use by MASM programs:
;
; ARGC  Returns count of command line arguments.
;       Treats blanks and tabs as whitespace.
;
;       Calling sequence:
;
;       push    seg argcnt      ; receives argument count
;       push    offset argcnt
;       call    ARGC
;
; ARGV  Returns address and length of specified
;       command line argument.  If called for 
;       argument 0, returns address and length
;	of fully qualified pathname of program.
;
;       Calling sequence:
;
;       push    argno           ; argument number
;       push    seg argptr      ; receives argument address
;       push    offset argptr
;       push    seg arglen      ; receives argument length
;       push    offset arglen
;       call    ARGV
;
; Copyright (C) 1988 Ray Duncan
;

tab     equ     09h             ; ASCII tab
blank   equ     20h             ; ASCII space character

        extrn   DosGetEnv:far

DGROUP  group   _DATA


_DATA   segment word public 'DATA'

envseg  dw      ?               ; environment selector
cmdoffs dw      ?               ; command tail offset

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

                                ; parameter for argc
argcnt  equ     [bp+6]          ; receives argument count

        public  argc
argc    proc    far             ; count command line arguments

        push    bp              ; make arguments addressable
        mov     bp,sp

        push    ds              ; save registers
        push    es
        push    bx
        push    cx

        mov     ax,seg DGROUP   ; point to instance data
        mov     ds,ax

        mov     es,envseg       ; set ES:BX = command line
        mov     bx,cmdoffs

        mov     ax,1            ; initialize argument count

argc0:  inc     bx              ; ignore useless first field
        cmp     byte ptr es:[bx],0      
        jne     argc0

argc1:  mov     cx,-1           ; set flag = outside argument

argc2:  inc     bx              ; point to next character 

        cmp     byte ptr es:[bx],0
        je      argc3           ; exit if null byte

        cmp     byte ptr es:[bx],blank
        je      argc1           ; outside argument if ASCII blank

        cmp     byte ptr es:[bx],tab    
        je      argc1           ; outside argument if ASCII tab

                                ; otherwise not blank or tab,
        jcxz    argc2           ; jump if already inside argument

        inc     ax              ; else found argument, count it
        not     cx              ; set flag = inside argument
        jmp     argc2           ; and look at next character

argc3:                          ; store result into
                                ; caller's variable...

        les     bx,argcnt       ; get address of variable
        mov     es:[bx],ax      ; store argument count

        pop     cx              ; restore registers
        pop     bx
        pop     es
        pop     ds
        pop     bp

        xor     ax,ax           ; signal success

        ret     4               ; return to caller      
                                ; and discard parameters
argc    endp



				; parameters for argv...
argno   equ     [bp+14]         ; argument number
argptr  equ     [bp+10]         ; receives argument pointer
arglen  equ     [bp+6]          ; receives argument length

        public  argv
argv	proc	far		; get address and length of
                                ; command tail argument

        push    bp              ; make arguments addressable
        mov     bp,sp

        push    ds              ; save registers
        push    es
        push    bx
        push    cx
        push    di      

        mov     ax,seg DGROUP   ; point to instance data
        mov     ds,ax

        mov     es,envseg       ; set ES:BX = command line
        mov     bx,cmdoffs

        mov     ax,argno        ; get argument number
        or      ax,ax           ; requesting argument 0?
        jz      argv8           ; yes, get program name

argv1:  inc     bx              ; scan off first field
        cmp     byte ptr es:[bx],0      
        jne     argv1

        xor     ah,ah           ; initialize argument counter

argv2:  mov     cx,-1           ; set flag = outside argument

argv3:  inc     bx              ; point to next character 

        cmp     byte ptr es:[bx],0
        je      argv7           ; exit if null byte

        cmp     byte ptr es:[bx],blank
        je      argv2           ; outside argument if ASCII blank

        cmp     byte ptr es:[bx],tab    
        je      argv2           ; outside argument if ASCII tab

                                ; if not blank or tab...
        jcxz    argv3           ; jump if inside argument

        inc     ah              ; else count arguments found
        cmp     ah,al           ; is this the one?
        je      argv4           ; yes, go find its length

        not     cx              ; no, set flag = inside argument
        jmp     argv3           ; and look at next character

argv4:                          ; found desired argument, now
                                ; determine its length...
        mov     ax,bx           ; save param. starting address 

argv5:  inc     bx              ; point to next character

        cmp     byte ptr es:[bx],0
        je      argv6           ; found end if null byte

        cmp     byte ptr es:[bx],blank
        je      argv6           ; found end if ASCII blank

        cmp     byte ptr es:[bx],tab    
        jne     argv5           ; found end if ASCII tab

argv6:  xchg    bx,ax           ; set ES:BX = argument address
        sub     ax,bx           ; and AX = argument length
        jmp     argv10          ; return to caller

argv7:  mov     ax,1            ; set AX != 0 indicating  
                                ; error, argument not found
        jmp     argv11          ; return to caller

argv8:				; special handling for argv = 0
        xor     di,di           ; find the program name by
        xor     al,al           ; first skipping over all the
        mov     cx,-1           ; environment variables...
        cld

argv9:	repne scasb		; scan for double null (can't use SCASW
	scasb			; because it might be odd address)
        jne     argv9           ; loop if it was a single null

        mov     bx,di           ; save program name address
        mov     cx,-1           ; now find its length... 
        repne scasb             ; scan for another null byte

        not     cx              ; convert CX to length 
        dec     cx
        mov     ax,cx           ; return length in AX

argv10:                         ; at this point AX = length,
                                ; ES:BX points to argument

        lds     di,argptr       ; address of 1st variable
        mov     ds:[di],bx      ; store argument pointer
        mov     ds:[di+2],es

	lds	di,arglen	; address of second variable
        mov     ds:[di],ax      ; store argument length

	xor	ax,ax		; AX = 0 to signal success

argv11:                         ; common exit point

        pop     di              ; restore registers
        pop     cx
        pop     bx
        pop     es
        pop     ds
        pop     bp

        ret     10              ; return to caller      
                                ; and discard parameters
argv    endp


init    proc    far             ; DLL instance initialization

                                ; get environment selector 
                                ; and offset of command tail
                                ; for this process...

        push    seg DGROUP      ; receives environment selector
        push    offset DGROUP:envseg
        push    seg DGROUP      ; receives command tail offset
        push    offset DGROUP:cmdoffs
        call    DosGetEnv       ; transfer to OS/2      
        or      ax,ax           ; call successful?
        jnz     init1           ; no, initialization error

        mov     ax,1            ; initialization OK,
        ret                     ; return AX = 1 for success

init1:  xor     ax,ax           ; initialization failed,
        ret                     ; return AX = 0 for error

init    endp


_TEXT   ends

        end     init            ; initialization entry point

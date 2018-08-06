	title	PORTIO --  Read/Write I/O Ports
        page    55,132
        .286

;
; PORTIO.ASM
;
; General purpose port read/write routines for C or MASM programs.
;
; Assemble with:  C> masm portio.asm;
;
; When this module is linked into a program, the following lines
; must be present in the program's module definition (DEF) file:
;
; SEGMENTS
;   IO_TEXT IOPL
;
; EXPORTS
;   RPORT 1
;   WPORT 2
;
; The SEGMENTS and EXPORTS directives are recognized by the Linker
; and cause information to be built into the EXE file header for
; the OS/2 program loader.  The loader is signalled to give I/O
; privilege to code executing in the segment IO_TEXT, and to build
; call gates for the routines 'RPORT' and 'WPORT'.
;
; Copyright (C) 1988 Ray Duncan
;

IO_TEXT segment word public 'CODE'
        
        assume  cs:IO_TEXT


; RPORT: Read 8-bit data from I/O port.  Port address
; is passed on stack; data is returned in register AX
; with AH zeroed.  Other registers are unchanged.
; 
; C syntax:     unsigned port, data;
;               data = rport(port);

        public  rport
rport   proc    far

        push    bp              ; save registers and
        mov     bp,sp           ; set up stack frame
        push    dx

        mov     dx,[bp+6]       ; get port number 
        in      al,dx           ; read the port
        xor     ah,ah           ; clear upper 8 bits
                
        pop     dx              ; restore registers
        pop     bp

        ret     2               ; discard parameters,
                                ; return port data in AX
rport   endp


; WPORT: Write 8-bit data to I/O port.	Port address and
; data are passed on stack.  All registers are unchanged.
; 
; C syntax:     unsigned port, data;
;               wport(port, data);

        public  wport
wport   proc    far

        push    bp              ; save registers and
        mov     bp,sp           ; set up stack frame
        push    ax
        push    dx

        mov     ax,[bp+6]       ; get data to write
        mov     dx,[bp+8]       ; get port number
        out     dx,al           ; write the port

        pop     dx              ; restore registers
        pop     ax
        pop     bp

        ret     4               ; discard parameters,
                                ; return nothing
wport   endp

IO_TEXT ends

        end

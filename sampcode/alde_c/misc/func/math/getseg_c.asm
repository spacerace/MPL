
; Name          getseg -- Get current Segment Values
;
;
; Synopsis      getseg(pcs, pds, pes, pss, psi, pdi, psp, pflag);
;
;               unsigned *cs Pointer to where code segment address goes
;               unsigned *ds Pointer to data segment
;               unsigned *es Pointer to extra segment
;               unsigned *ss Pointer to stack segment
;               unsigned *si Pointer to si register
;               unsigned *di Pointer to di register
;               unsigned *sp Pointer to sp register
;               unsigned *flag Returns flag
;
;
; Returns       cs              value of Code segment
;               ds              value of Data segment
;               es              value of Extra segment
;               ss              value of Stack segment
;               si              value of SI register ****NOT RELIABLE****
;               di              value of DI register
;               sp              value of Stack Pointer
;               flag            value of Flags register
;
;
;


pgroup  group    prog
prog    segment  byte public 'PROG' ; Combine with C 'PROG' program segment
        assume   cs:pgroup
        public   getseg
getseg proc     near
        push     bp                 ; Save the frame pointer
        mov      bp,sp
        mov      si,[bp + 04]       ; Get the values for the registers
        mov      ax,[si]
        mov      si,[bp + 06]
        mov      bx, [si]
        mov      si,[bp + 08]
        mov      cx, [si]
        mov      si,[bp + 10]
        mov      dx,[si]
;==============================================================================
        mov ax, cs                  ;Get value of code segment
        mov bx, ds                  ;data segment
        mov cx, es                  ;extra segment
        mov dx, ss                  ;stack segment


;==============================================================================

uret:   mov      bp,sp              ; Now recover the values of the
        mov      si,[bp + 04]       ; parameters
        mov      [si],ax
        mov      si, [bp + 06]
        mov      [si], bx
        mov      si, [bp + 08]
        mov      [si], cx
        mov      si, [bp + 10]
        mov      [si], dx
        mov      si, [bp + 12]
        mov      [si], si
        mov      si, [bp + 14]
        mov      [si], di
        mov      si, [bp + 16]
        mov      [si], sp
        mov      al, 00             ;zero out al
        lahf                        ;load flag into ah
        mov      si, [bp + 18]
        mov      [si], ax


        mov      ax,0               ; No error

quit:   pop      bp                 ; Get the original frame pointer.
        ret
getseg endp
prog    ends
        end



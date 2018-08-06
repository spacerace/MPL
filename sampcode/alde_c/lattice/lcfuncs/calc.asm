       TITLE     CALC      Calculator program using Lattice C library
       page      60,132

       include   lattice.mac
       PSEG

_MAIN   proc     near              ; you MUST have a procedure _MAIN !!!

        _fopen   fname,imode,fp1   ; Open input file    (console)
        _fopen   fname,omode,fp2   ; Open output file   (console)

mloop:  _printf  fp2,prompt        ; Print prompt
        mov      si,offset arg1x   ; point to  string
        mov      bx,offset op1     ; operator
        call     getnum            ; read ascii string till break

        cmp      ds:byte ptr op1,'q' ; if sQuare or Arctan then don't
        jz       skip1               ; read second number.
        cmp      ds:byte ptr op1,'a'
        jz       skip1

        mov      si,offset arg2x   ; point to string, second number
        mov      bx,offset op2
        call     getnum
;
; Check to see if first argument is Null, and if it is, use
; the last result as the first argument, calculator-style.
;
skip1:  cmp      ds:byte ptr arg1x,0  ; first argument is null
        jz       skip
        _sscanf  arg1x,Dfmt,num1      ; Convert string to double

skip:   cmp      ds:byte ptr arg2x,0  ; Second argument is null
        jz       skip2
        _sscanf  arg2x,Dfmt,num2      ; Convert string to double

skip2:  mov      al,ds:byte ptr op1   ; What operation to perform?

        cmp      al,'+'               ; Add
        jnz      c0
        jmp      plus

c0:     cmp      al,'-'               ; Subtract
        jnz      c1
        jmp      minus

c1:     cmp      al,'/'               ; Divide
        jnz      c2
        jmp      divide

c2:     cmp      al,'*'               ; Multiply
        jnz      c2a
        jmp       mult

c2a:    cmp      al,'q'               ; Square root
        jnz      c3
        jmp      root

c3:     cmp      al,'^'               ; power
        jnz      c4
        jmp      power

c4:     cmp      al,'a'               ; ArcTangent
        jz       atn
        jmp      equals

atn:    _atan    num1,num1
        jmp      equals

root:   comp     num1,zero           ; square root, Must be >= 0.0
        jge      okroot
        _printf  fp2, error          ; can't get root of negative number!
        jmp      mloop

okroot: _sqrt    num1,num1
        jmp      equals

power:  _pow     num1,num2,num1
        jmp      equals

mult:   _mul     num1,num2,num1
        jmp      equals

plus:   _add     num1,num2,num1
        jmp      equals

minus:  _sub     num1,num2,num1
        jmp      equals

divide: comp    num2,zero            ; Can't divide by zero
        jg      okdiv                ; second arg is > 0.0
        _printf fp2, error           ; second arg = 0, ERROR.
        jmp     mloop
okdiv:  _div    num1,num2,num1       ; Should do error checking here.

equals: _printd  fp2,outfmt,num1      ; display result.

        jmp      mloop

abort:  ret

_MAIN  endp

; GETNUM - Read an ascii number from console, read til {+ - / * = }
;          SI = Address of string
;          BX = Address of operator

getnum  proc     near
        push     bx
get1:   push     si
        mov      ah,1
        int      21h
        pop      si
        cmp      al,'+'       ; Check for arithmetic operators
        jz       getx
        cmp      al,'-'
        jz       getx
        cmp      al,'*'
        jz       getx
        cmp      al,'/'
        jz       getx
        cmp      al,'='
        jz       getx
        cmp      al,'q'
        jz       getx
        cmp      al,'a'
        jz       getx
        cmp      al,'^'
        jz       getx
        cmp      al,'.'       ; Decimal point
        jz       get2
        cmp      al,'0'       ; Must be between 0 and 9
        jb       get1         ; if <0 then ignore
        cmp      al,'9'
        ja       get1         ; if >9 then ignore
get2:   mov      ds:byte ptr [si],al ; store digit
        inc      si
        jmp      get1
getx:   pop      bx
        mov      ds:byte ptr [bx],al  ;Store operator
        mov      ds:byte ptr [si],0
        ret
getnum  endp

        ENDPS

        DSEG
fp1    dw        ?                  ; file for input
fp2    dw        ?                  ; file for output

arg1x  db        20 dup (20h)
op1    db        '+'
arg2x  db        20 dup (20h)
op2    db        '='

fname  db        'CON:',0           ; name of console device

imode  db        'r',0              ; mode for read-only
omode  db        'w',0              ; mode for write-only

Dfmt   db        '%lf',0            ; format for numeric Input
outfmt db        '   %lf',0         ; format for numeric Output

prompt db        0Ah,'> ',0
error  db        0AH,'illegal operation ',0

num1   dq        ?
num2   dq        ?
num3   dq        ?
zero   dq        0.0e0
       ENDDS                       ; End of data segment
       end

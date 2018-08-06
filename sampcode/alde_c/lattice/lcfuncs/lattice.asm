       page      60,132

       .8087

       include   lattice.mac
       PSEG

_MAIN  proc      near              ; you MUST have a procedure _MAIN !!!

       _fopen    fname,imode,fp1   ; Open input file    (console)
       _fopen    fname,omode,fp2   ; Open output file   (console)
       _printf   fp2,mess1         ; Print opening message
       _scanf    fp1,Dfmt,num1     ; Read double precision number
       _printf   fp2,mess1a        ; Ask for second number
       _scanf    fp1,Dfmt,num2     ; And input it

       FINIT                       ; Initialize NDP
       FLD       qword ptr ds:num1 ; push number onto stack
       FADD      qword ptr ds:num2 ; Add it to second number
       FSTP      qword ptr ds:num3 ; Put back into memory

       _printd   fp2,sum,num1      ; print first number,
       _printd   fp2,Dfmt,num2     ; then second number,
       _printd   fp2,mess2,num3    ; and the sum.

       _mul      num1,num2,num3    ; Library func to Multiply
       _printd   fp2,prod,num3
       _div      num1,num2,num3    ; Library func to divide
       _printd   fp2,quot,num3
       _sqrt     num1,num3
       _printd   fp2,root,num3
       _atan     one,num3          ; Arctangent of 1
       _mul      num3,four,num2    ; times 4 is PI
       _printd   fp2,pi,num2       ; print PI

       ret

_MAIN  endp

       ENDPS

       DSEG
fp1    dw        ?                  ; file for input
fp2    dw        ?                  ; file for output

one    dq        1.0e0              ; Integer constant
four   dq        4.0e0

fname  db        'CON:',0           ; name of file

imode  db        'r',0              ; mode for read-only
omode  db        'w',0              ; mode for write-only

Sfmt   db        '%s',0             ; format for string output
Dfmt   db        '%lf',0            ; format for numeric output

mess1  db        'Enter Decimal number ',0
mess1a db        0Ah,'Enter Second number ',0
sum    db        0Ah,'%lf + ',0
mess2  db        ' = %lf',0
prod   db        0Ah,'Library multiply     %lf',0
quot   db        0Ah,'Library divide       %lf',0
root   db        0Ah,'Library square root  %lf',0
pi     db        0Ah,'Calculated PI is     %lf',0

num1   db        8 dup (?)
num2   db        8 dup (?)
num3   db        8 dup (?)
       ENDDS                       ; End of data segment
       end

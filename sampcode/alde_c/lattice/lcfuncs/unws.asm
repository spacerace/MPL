        TITLE    UNWS     Un-Wordstar a file.

        page     60,132

        NOMATH   = 1                         ; Force not-include of math

        include  lattice.mac
        PSEG

_MAIN   proc     near
        _fopen   console,imode,conin         ; Open console
        _fopen   console,omode,conout

        _printf  conout,ask1                 ; Ask for input filename
        _scanf   conin,sfmt,iname            ; Get input filename
        _printf  conout,ask2
        _scanf   conin,sfmt,oname            ; Get Output filename

        _fopen   iname,imode,fp1             ; Try to open input file
        cmp      ds:word ptr fp1,0           ; Any errors?
        jz       opner1                      ; Yes.

        _fopen   oname,omode,fp2             ; Try to open output file
        cmp      ds:word ptr fp2,0           ; Check for open error
        jnz      mloop                       ; no, continue

        _printf  conout,err1                 ; error on output file.
        jmp      abort

opner1: _printf  conout,err2                 ; error on input file
        jmp      abort

mloop:  cmp      ds:byte ptr tab,0
        jnz      cont1
        mov      ds:byte ptr tab,8
cont1:  _fgetc   fp1                         ; get a character from input
        cmp      al,0Ah                      ; End of a line?
        jnz      cont
        inc      ds:word ptr lcount
        mov      ds:byte ptr tab,8

cont:   cmp      al,-1                       ; End of file?
        je       m_exit                      ; If so, then quit.

        and      al,7Fh                      ; Strip WS hi bit
        cmp      al,0DH                      ; Let CR thru
        jz       write
        cmp      al,0Ah                      ; Let LF thru
        jz       write

        cmp      al,09h                      ; Tab character?
        jnz      cont2                       ; No, continue
        mov      cl,ds:byte ptr tab
        xor      ch,ch
loop2:  push     cx
        mov      al,20h                      ; yes, output spaces
        _fputc   fp2
        pop      cx
        loop     loop2
        mov      ds:byte ptr tab,8
        jmp      mloop

cont2:  cmp      al,' '                      ; ' ' ó CHAR ò '~'
        jb       mloop
        cmp      al,7Eh
        ja       mloop
write:  _fputc   fp2                         ; otherwise output the char
        dec      ds:byte ptr tab
        jmp      mloop                       ; and go back for more

m_exit: _printi  conout,ifmt,lcount
        _fclose  fp1
        _fclose  fp2                         ; Close output file
abort:  ret                                  ; Back to dos.


_MAIN   endp

        ENDPS
        DSEG
console db       'CON:',0
iname   db       20 dup (?)                  ; Name of input file
oname   db       20 dup (?)                  ; Name of output file
imode   db       'r',0
omode   db       'w',0
sfmt    db       '%s',0
ifmt    db       0Ah,'Processed %d lines',0
ask1    db       'Un-Wordstar a file',0Ah,'Enter input filename: ',0
ask2    db       0Ah,'Enter output filename: ',0
err1    db       0Ah,'Cannot create file',0
err2    db       0Ah,'File not found',0
mess1   db       0Ah,'lines Processed : ',0
lcount  dw       0
tab     db       9

; FILE HANDLES

conin   dw       ?
conout  dw       ?
fp1     dw       ?
fp2     dw       ?

        ENDDS
        end

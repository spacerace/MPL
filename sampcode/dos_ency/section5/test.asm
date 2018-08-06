memS    =       0                ;Small memory model
?PLM    =       0                ;C calling conventions
?WIN    =       0                ;Disable Windows support

include cmacros.inc
include cmacrosx.inc

sBegin  CODE                     ;Start of code segment
assumes CS,CODE                  ;Required by MASM

        ;Microsoft C function syntax:
        ;
        ;     int addnums(firstnum, secondnum)
        ;         int firstnum, secondnum;
        ;
        ;Returns firstnum + secondnum

cProc   addnums,PUBLIC           ;Start of addnums functions
parmW   firstnum                 ;Declare parameters
parmW   secondnum
cBegin
        mov     ax,firstnum
        add     ax,secondnum
cEnd
sEnd    CODE
        end

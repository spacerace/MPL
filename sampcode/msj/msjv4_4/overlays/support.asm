                .model  LARGE,C
                .code

                public  errputs
errputs         proc    far C uses ax bx cx di ds es, string:ptr
                les     di,string               ;es:di -> string
                push    di                      ;save original offset
                mov     al,0                    ;terminator byte
                mov     cx,0100h                ;length to examine for null
                repne   scasb                   ;look for it
                pop     ax                      ;ax = orig pointer
                sub     di,ax
                dec     di                      ;di = length
                mov     cx,di
                mov     bx,2                    ;stderr
                mov     dx,ax                   ;buffer address
                mov     ah,40h                  ;write to file
                int     21h
                ret
errputs         endp


mylseek         proc    far C uses bx cx, handle:WORD, ofs:DWORD, whence:WORD
                public  mylseek
                mov     ax,whence
                mov     ah,42h
                mov     bx,handle
                push    es
                les     dx,ofs
                mov     cx,es
                pop     es
                int     21h                     ;result left in dx:ax, nicely
                jnc     lseekdone
                mov     ax,-1
lseekdone:      ret
mylseek         endp


myread          proc    far C uses bx ds dx, handle:WORD, buf:PTR, len:WORD
                public  myread
                mov     ah,3fH
                mov     bx,handle
                mov     cx,len
                lds     dx,buf
                int     21h                     ;result nicely in ax for return
                jnc     readdone
                mov     ax,-1
readdone:       ret
myread          endp

myopen          proc    far C uses ds dx, fname:PTR, mode:WORD
                public  myopen
                mov     ax,mode
                mov     ah,3dH
                lds     dx,fname
                int     21h                     ;result nicely in ax for return
                jnc     opendone
                mov     ax,-1
opendone:       ret
myopen          endp

myclose         proc    far C uses bx, handle:WORD
                public  myclose
                mov     ah,3eH
                mov     bx,handle
                int     21h                     ;result nicely in ax for return
                ret
myclose         endp


                end


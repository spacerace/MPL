                extrn   _errputs:far, p1:far, p2:far
                extrn   _executable_name:byte
_text           segment public 'CODE'
_text           ends
_data           segment public 'DATA'
_data           ends

                public  fill_exe_name,ENVSEG,main_msg,call_loop,envstr_loop
                public  fname_loop

ENVSEG          equ     2CH                             ;loc'n of environment pointer
DGROUP          group   _data,_stack        
_text           segment
                assume  cs:_text,ds:DGROUP,ss:DGROUP
_main           proc    far
                mov     ax,DGROUP
                mov     ds,ax
                call    far ptr fill_exe_name
                lea     ax,main_msg
                push    ds
                push    ax
                call    far ptr _errputs
                add     sp,4
                mov     cx,1000
call_loop:
                call    far ptr p1
                call    far ptr p2
                loop    call_loop
                mov     ax,4c00h
                int     21h
_main           endp

fill_exe_name   proc    far
                push    es
                push    di
                push    ds
                push    si
                push    ax
                
                mov     ax,es
                mov     ds,ax
                mov     ax,DGROUP:ENVSEG                ;point at env
                mov     ds,ax
                mov     si,0
                mov     ax,seg _executable_name         ;point at dest string
                mov     es,ax
                lea     di,DGROUP:_executable_name
envstr_loop:
                lodsb                                   ;get first env
                cmp     al,0                            ;end of string?
                jnz     envstr_loop                     ;no, keep looking
                cmp     byte ptr ds:[si],0              ;end of env?
                jnz     envstr_loop                     ;no, get another string
                add     si,3                            ;yes: go past, 2 more
fname_loop:     movsb                                   ;move a byte    
                cmp     byte ptr ds:[si],0              ;end of name?
                jnz     fname_loop                      ;no, keep it up
                mov     byte ptr es:[di],0              ;store zero terminator

                pop     ax
                pop     si
                pop     ds
                pop     di
                pop     es
                ret
fill_exe_name   endp

_text           ends


_data           segment
main_msg        db      'This is main...',13,10,0
_data           ends


_stack          segment stack 'STACK'
                dw      200 dup (?)
_stack          ends

                end     _main



                extrn   _errputs:far
                .model  LARGE
                .data
p1_msg          db      'This is p1...',13,10,0

                .code
p1              proc    far
                public  p1
                lea     ax,p1_msg
                push    ds
                push    ax
                call    _errputs
                add     sp,4
                ret
p1              endp

                end


                extrn   _errputs:far
                .model  LARGE
                .data
p2_msg          db      'This is p2...',13,10,0

                .code
p2              proc    far
                public  p2
                lea     ax,p2_msg
                push    ds
                push    ax
                call    _errputs
                add     sp,4
                ret
p2              endp

                end


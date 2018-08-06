        extrn   C_INIT:far

_TEXT   segment word public 'CODE'

        assume  cs:_TEXT


INIT    proc    far             ; initialization routine

        call    C_INIT          ; call C routine to
                                ; do the actual work

        ret                     ; back to caller with
                                ; AX = status code
                                ; ("value" from C_INIT)
INIT    endp

_TEXT   ends

        end     INIT

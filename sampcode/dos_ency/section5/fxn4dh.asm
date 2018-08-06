        ;************************************************************;
        ;                                                            ;
        ;        Function 4DH: Get Return Code of Child Process      ;
        ;                                                            ;
        ;        int child_ret_code()                                ;
        ;                                                            ;
        ;        Returns the return code of the last                 ;
        ;        child process.                                      ;
        ;                                                            ;
        ;************************************************************;

cProc   child_ret_code,PUBLIC
cBegin
        mov     ah,4dh          ; Set function code.
        int     21h             ; Ask MS-DOS to return code.
        cbw                     ; Convert AL to a word.
cEnd

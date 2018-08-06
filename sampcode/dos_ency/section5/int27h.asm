;***************************************************************;
;                                                               ;
;       Interrupt 27H: Terminate and Stay Resident              ;
;                                                               ;
;       Exit and stay resident, reserving enough memory         ;
;       to protect the program's code and data.                 ;
;                                                               ;
;***************************************************************;

Start:  .
        .
        .
        mov     dx,offset pgm_end  ; DX = bytes to reserve.
        int     27h                ; Terminate, stay resident.
        .
        .
        .
pgm_end equ     $
        end     start

        ;************************************************************;
        ;                                                            ;
        ;         Function 38H: Get/Set Current Country Data         ;
        ;                                                            ;
        ;         int country_info(country,pbuffer)                  ;
        ;             char country,*pbuffer;                         ;
        ;                                                            ;
        ;         Returns -1 if the "country" code is invalid.       ;
        ;                                                            ;
        ;************************************************************;

cProc   country_info,PUBLIC,ds
parmB   country
parmDP  pbuffer
cBegin
        mov     al,country      ; Get country code.
        loadDP  ds,dx,pbuffer   ; Get buffer pointer (or -1).
        mov     ah,38h          ; Set function code.
        int     21h             ; Ask MS-DOS to get country
                                ; information.
        jnb     cc_ok           ; Branch if country code OK.
        mov     ax,-1           ; Else return -1.
cc_ok:
cEnd

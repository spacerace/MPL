        ;************************************************************;
        ;                                                            ;
        ;   Function 63H: Get Lead Byte Table                        ;
        ;                                                            ;
        ;   char far *get_lead_byte_table()                          ;
        ;                                                            ;
        ;   Returns far pointer to table of lead bytes for multibyte ;
        ;   characters.  Will work only in MS-DOS 2.25!              ;
        ;                                                            ;
        ;************************************************************;

cProc   get_lead_byte_table,PUBLIC,<ds,si>
cBegin
        mov     ax,6300h        ; Set function code.
        int     21h             ; Get lead byte table.
        mov     dx,ds           ; Return far pointer in DX:AX.
        mov     ax,si
cEnd

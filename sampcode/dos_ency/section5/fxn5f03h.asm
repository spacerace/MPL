        ;************************************************************;
        ;                                                            ;
        ;    Function 5FH Subfunction 03H:                           ;
        ;                  Make Assign-List Entry                    ;
        ;    int add_alist_entry(psrcname,pdestname,uservalue,type)  ;
        ;        char *psrcname,*pdestname;                          ;
        ;        int  uservalue,type;                                ;
        ;                                                            ;
        ;    Returns 0 if new assign-list entry is made, otherwise   ;
        ;    returns error code.                                     ;
        ;                                                            ;
        ;************************************************************;

cProc   add_alist_entry,PUBLIC,<ds,si,di>
parmDP  psrcname
parmDP  pdestname
parmW   uservalue
parmW   type
cBegin
        mov     bx,type         ; Get device type.
        mov     cx,uservalue    ; Get uservalue.
        loadDP  ds,si,psrcname  ; DS:SI = pointer to source name.
        loadDP  es,di,pdestname ; ES:DI = pointer to destination name.
        mov     ax,5f03h        ; Set function code.
        int     21h             ; Make assign-list entry.
        jb      aa_err          ; Exit if there was some error.
        xor     ax,ax           ; Else return 0.
aa_err:
cEnd

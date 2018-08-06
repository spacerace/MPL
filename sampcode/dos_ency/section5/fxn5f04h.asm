        ;************************************************************;
        ;                                                            ;
        ;    Function 5FH Subfunction 04H:                           ;
        ;                  Cancel Assign-List Entry                  ;
        ;                                                            ;
        ;    int cancel_alist_entry(psrcname)                        ;
        ;        char *psrcname;                                     ;
        ;                                                            ;
        ;    Returns 0 if assignment is canceled, otherwise returns  ;
        ;    error code.                                             ;
        ;                                                            ;
        ;************************************************************;

cProc   cancel_alist_entry,PUBLIC,<ds,si>
parmDP  psrcname
cBegin
        loadDP  ds,si,psrcname  ; DS:SI = pointer to source name.
        mov     ax,5f04h        ; Set function code.
        int     21h             ; Cancel assign-list entry.
        jb      ca_err          ; Exit on error.
        xor     ax,ax           ; Else return 0.
ca_err:
cEnd

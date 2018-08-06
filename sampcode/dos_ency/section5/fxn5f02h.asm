        ;************************************************************;
        ;                                                            ;
        ;    Function 5FH Subfunction 02H:                           ;
        ;                  Get Assign-List Entry                     ;
        ;                                                            ;
        ;    int get_alist_entry(index,                              ;
        ;           plocalname,premotename,                          ;
        ;           puservalue,ptype)                                ;
        ;        int  index;                                         ;
        ;        char *plocalname;                                   ;
        ;        char *premotename;                                  ;
        ;        int  *puservalue;                                   ;
        ;        int  *ptype;                                        ;
        ;                                                            ;
        ;    Returns 0 if the requested assign-list entry is found,  ;
        ;    otherwise returns error code.                           ;
        ;                                                            ;
        ;************************************************************;

cProc   get_alist_entry,PUBLIC,<ds,si,di>
parmW   index
parmDP  plocalname
parmDP  premotename
parmDP  puservalue
parmDP  ptype
cBegin
        mov     bx,index        ; Get list index.
        loadDP  ds,si,plocalname  ; DS:SI = pointer to local name
                                  ; buffer.
        loadDP  es,di,premotename ; ES:DI = pointer to remote name
                                  ; buffer.
        mov     ax,5f02h        ; Set function code.
        int     21h             ; Get assign-list entry.
        jb      ga_err          ; Exit on error.
        xor     ax,ax           ; Else return 0.
        loadDP  ds,si,puservalue ; Get address of uservalue.
        mov     [si],cx         ; Store user value.
        loadDP  ds,si,ptype     ; Get address of type.
        mov     bh,0
        mov     [si],bx         ; Store device type to type.
ga_err:
cEnd

        ;************************************************************;
        ;                                                            ;
        ;      Function 1CH: Get Drive Data                          ;
        ;                                                            ;
        ;      Get information about the disk in the specified       ;
        ;      drive.  Set drive_ltr to 0 for default drive info.    ;
        ;                                                            ;
        ;      int get_drive_data(drive_ltr,                         ;
        ;             pbytes_per_sector,                             ;
        ;             psectors_per_cluster,                          ;
        ;             pclusters_per_drive)                           ;
        ;           int  drive_ltr;                                  ;
        ;           int *pbytes_per_sector;                          ;
        ;           int *psectors_per_cluster;                       ;
        ;           int *pclusters_per_drive;                        ;
        ;                                                            ;
        ;      Returns -1 for invalid drive, otherwise returns       ;
        ;      the disk's type (from the 1st byte of the FAT).       ;
        ;                                                            ;
        ;************************************************************;

cProc   get_drive_data,PUBLIC,<ds,si>
parmB   drive_ltr
parmDP  pbytes_per_sector
parmDP  psectors_per_cluster
parmDP  pclusters_per_drive
cBegin
        mov     si,ds           ; Save DS in SI to use later.
        mov     dl,drive_ltr    ; Get drive letter.
        or      dl,dl           ; Leave 0 alone.
        jz      gdd
        and     dl,not 20h      ; Convert letter to uppercase.
        sub     dl,'A'-1        ; Convert to drive number: 'A' = 1,
                                ; 'B' = 2, etc.
gdd:
        mov     ah,1ch          ; Set function code.
        int     21h             ; Ask MS-DOS for data.
        cbw                     ; Extend AL into AH.
        cmp     al,0ffh         ; Bad drive letter?
        je      gddx            ; If so, exit with error code -1.
        mov     bl,[bx]         ; Get FAT ID byte from DS:BX.
        mov     ds,si           ; Get back original DS.
        loadDP  ds,si,pbytes_per_sector
        mov     [si],cx         ; Return bytes per sector.
        loadDP  ds,si,psectors_per_cluster
        mov     ah,0
        mov     [si],ax         ; Return sectors per cluster.
        loadDP  ds,si,pclusters_per_drive
        mov     [si],dx         ; Return clusters per drive.
        mov     al,bl           ; Return FAT ID byte.
gddx:
cEnd

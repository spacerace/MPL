title Touch - Reset the date and time of each file found.
page 60,132
cgroup  group   code_seg, data_seg
        assume  cs:cgroup,ds:cgroup
data_seg segment        public
path_name       db      80 dup (0)      ; space for 64 char path name
                                        ; and 13 char file name
file_name       db      13 dup (0)      ; save room for full dos name
file_handle     dw
date            dw
time            dw
fnf             db      'File not found$'
disk_area               db      21 dup(?)
        attribute       db
        file_time       dw
        file_date       dw
        file_size       dd
        name_found      db      13 dup(?)
disk_transfer_areas     label   byte    ; this starts at the end of the whereis
data_seg        ends
page
code_seg        segment
        org     100h
touch           proc    near
        mov     si,82H                  ; start of command line
        mov     di,offset cgroup:path_name
get_search_name:
        lodsb                           ; get first character
        cmp     al,0dh                  ; carriage return character?
        je      done_reading_name       ; yes, end of name
        stosb
        jmp     get_search_name
done_reading_name:
        xor     al,al                   ; write a 0 at the end
        stosb
        mov     dx, offset cgroup:disk_area
        mov     ah,1aH
        int     21H                     ; set the dta
        mov     dx, offset cgroup:path_name    ; point to the file name
        mov     cx,0                    ; set for normal files only
        mov     ah,4eH                  ; find first file
        int     21H                     ; do it
        cmp     ax, 12H                 ; another error ?
        je      file_not_found
        jmp     found_a_file            ; process it
file_not_found:
        mov     dx, offset cgroup:fnf   ; offset to error message
        mov     ah, 9                   ; print string
        int     21H                     ; dos call
        mov     al, 1                   ; error code 1
        mov     ah,4cH                  ; exit with error code
        int     21h                     ; bye bye
no_more_files:
        mov     al, 0                   ; error code 0
        mov     ah,4ch                  ; exit with error code
        int     21h                     ; bye bye
found_a_file:
        mov     dx,offset cgroup:name_found ; found file name
        mov     al, 2                   ; open for read/write
        mov     ah, 3dH                 ; open file
        int     21H                     ; do it
        mov     file_handle, ax         ; mov file handle to mem

        mov     ah, 2aH                 ; get date
        int     21h                     ; do it
        sub     cx,1980                 ; get the date offset from 1980
        shl     cx,1                    ; have the year set up
        shl     cx,1                    ; have the year set up
        shl     cx,1                    ; have the year set up
        shl     cx,1                    ; have the year set up
        shl     cx,1                    ; have the year set up
        shl     cx,1                    ; have the year set up
        shl     cx,1                    ; have the year set up
        shl     cx,1                    ; have the year set up
        shl     cx,1                    ; have the year set up
        xor     bx,bx                   ; clear bx
        mov     bl,dh                   ; move month to bl
        shl     bx,1                    ; set up month
        shl     bx,1                    ; set up month
        shl     bx,1                    ; set up month
        shl     bx,1                    ; set up month
        shl     bx,1                    ; set up month
        or      cx,bx                   ; mov month to date
        xor     bx,bx                   ; clear bx
        mov     bl,dl                   ; mov day to bl
        or      cx,bx                   ; mov day to cx
        mov     date, cx                ; mov to mem

        mov     ah, 2cH                 ; get time
        int     21H                     ; do it
        shl     ch,1                    ; shift left 3 bits
        shl     ch,1                    ; shift left 3 bits
        shl     ch,1                    ; shift left 3 bits
        xor     bx,bx                   ; clear bx
        mov     bl,cl                   ; mov minutes to bx
        xor     cl,cl                   ; clear cl
                                        ; now hours are in ch
                                        ; and minutes in bx
        shl     bx,1                    ; shift left 5 bits
        shl     bx,1                    ; shift left 5 bits
        shl     bx,1                    ; shift left 5 bits
        shl     bx,1                    ; shift left 5 bits
        shl     bx,1                    ; shift left 5 bits
        or      cx,bx                   ; now hours are in bits 15-11
                                        ; and mins  are in bits 10-5
                                        ; now to work on seconds.
        xor     bx,bx                   ; clear bx
        shr     dh,1                    ; now we have 2 second value
        mov     bl,dh                   ; mov to bl
        or      cx,bx                   ; or with cx
                                        ; now we have converted the
                                        ; time to file format
        mov     time, cx                ; mov to mem

        mov     al, 01                  ; set file date/time
        mov     cx, time                ; mov time
        mov     dx, date                ; mov date
        mov     bx, file_handle         ; mov file_handle
        mov     ah, 57H                 ; set/get file date/time
        int     21H
        mov     bx, file_handle         ; mov file_handle
        mov     ah, 3eH                 ; close file
        int     21H                     ; do it
        ; check to see if there are more files
        mov     ah, 4fH                 ; find next file
        int     21H
        cmp     al, 12H                 ; no more files?
        jne     do_it_again             ; if not do it again
        jmp     no_more_files           ; otherwise exit.
do_it_again:
        jmp     found_a_file
touch   endp
code_seg ends
end     touch

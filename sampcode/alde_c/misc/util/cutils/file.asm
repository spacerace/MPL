                        TITLE   File Manipulation Routines for Lattice
                        NAME    FILES
                        INCLUDE DOS.MAC
 
COMMENT #
                AUTHOR          Jon Wesener
                DATE            July 9, 1985
                PURPOSE           To supply fast routines for C
                                and Assembler programs.  This section
                                is for file manipulation and needs the
                                include file FILES.H .
                (C) Copyright 1985- Jon Wesener.
        #
 
                DSEG
BUFSIZE         EQU     128
buffer          db      BUFSIZE dup (?)
bcnt            dw      0
bufptr          dw      ?
                ENDDS
 
                PSEG
                PUBLIC  FCREATE, FOPEN, FCLOSE, FREAD, FWRITE, FMOVE, FDEL
                PUBLIC  FREN, GETCH
; Create a newfile or truncate an existing file.
;  fcreate(name,attr)
; RETURN: -1= error     ?= channel
 
FCREATE         PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, 3c00h       ; dos function for create
                mov     cx, [bp+6]      ; get the file attribute
                mov     dx, [bp+4]      ; get the file name
                int     21h
                jnc     fcrex           ; no problem
                mov     ax, -1          ; signal error
fcrex:          pop     bp
                ret
FCREATE         ENDP
 
; open an existing file for read, write, or update io.
;  fopen(name,access)
; RETURN: -1= error    ?= channel
 
FOPEN           PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, [bp+6]      ; get file access
                mov     ah, 3dh         ; dos open file function
                mov     dx, [bp+4]      ; get file name
                int     21h
                jnc     fopex
                mov     ax, -1          ; signal error
fopex:          pop     bp
                ret
FOPEN           ENDP
 
; fclose closes a channel opened with fcreate or fopen.
;  fclose(channel)
; RETURN: 0= successful         ?= error
 
FCLOSE          PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, 3e00h       ; dos close file function
                mov     bx, [bp+4]      ; get channel
                int     21h
                jc      fclex
                xor     ax, ax          ; signal success
fclex:          pop     bp
                ret
FCLOSE          ENDP
 
; fread reads a number of bytes from an open channel
;  fread(channel, buffer, count)
; RETURN: Number of bytes read.
 
FREAD           PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, 3f00h       ; dos read function
                mov     bx, [bp+4]      ; get channel
                mov     cx, [bp+8]      ; get count
                mov     dx, [bp+6]      ; get buffer
                int     21h
                pop     bp
                ret
FREAD           ENDP
 
; fwrite writes a number of bytes to an open channel
;  fwrite(channel, buffer, count)
; RETURN: Number of bytes written
 
FWRITE          PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, 4000h       ; dos read function
                mov     bx, [bp+4]      ; get channel
                mov     cx, [bp+8]      ; get count
                mov     dx, [bp+6]      ; get buffer
                int     21h
                pop     bp
                ret
FWRITE          ENDP
 
; fmove moves the file pointer a number of bytes relative to base
;  fmove(channel, long count, base)
; RETURN: 0= success    ?= error
 
FMOVE           PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, [bp+10]     ; get base
                mov     ah, 42h         ; dos move file pointer function
                mov     bx, [bp+4]      ; get channel
                mov     cx, [bp+6]      ; get low count
                mov     dx, [bp+8]      ; get high count
FMOVE           ENDP
 
; fdel deletes the named file
;  fdel(name)
; RETURN: 0= success    ?= error
 
FDEL            PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, 4100h       ; dos delete file function
                mov     dx, [bp+4]      ; get file name
                int     21h
                jc      fdex
                xor     ax, ax          ; signal success
fdex:           pop     bp
                ret
FDEL            ENDP
 
; Fren will rename a file.
;  err= fren(to, from);
; RETURN:       0= Success      ?= Error
 
FREN            PROC    NEAR
                push    bp
                mov     bp, sp
                mov     ax, 5600h
                mov     di, [bp+4]      ; get new name
                mov     dx, [bp+6]      ; get old name
                int     21h             ; change it
                jc      frenex
                xor     ax, ax          ; signal success
frenex:         pop     bp
                ret
FREN            ENDP
 
; getch gives buffered single character io for files
;  getch(&ch , channel);
; RETURN: 0= EOF        1= character ready
; NOTE:         Use this for 1 file only, and consistently.
 
GETCH           PROC    NEAR
                push    bp
                mov     bp, sp
                test    bcnt, 0ffffh    ; anything in the buffer
                jnz     getc1           ; yes
                mov     dx, offset buffer
                mov     bufptr, dx      ; save ptr into buffer
                mov     cx, BUFSIZE
                mov     bx, [bp+6]      ; channel to read
                mov     ax, 3f00h
                int     21h             ; read it in
                and     ax, ax          ; is it eof?
                jz      getc2
                mov     bcnt, ax        ; store buffer cnt
getc1:          mov     si, bufptr      ; get ptr into buffer
                mov     di, [bp+4]      ; get ptr to character
                movsb
                inc     bufptr          ; pt to next character
                dec     bcnt            ; set count
                mov     ax, 1           ; signal success
getex:          pop     bp
                ret
getc2:          xor     ax, ax          ; signal error
                jmp     getex
GETCH           ENDP
                ENDPS
                END

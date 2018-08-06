;;	Microsoft Systems Journal
;;	Volume 2; Issue 4; September, 1987

;;	pp. 63-76

;; Author(s): Dan Mick
;; Title:     Microsoft QuickBASIC: Everyone's First PC Language Gets Better



;;	Figure 1
;;	========



;;	DSEARCH.ASM
;;	Written by Dan Mick


        page    62,132

data    segment public 'data'
dta_ofs dw      ?               ;save area for BASIC's DTA
dta_seg dw      ?
fill    db      ?               ;"fill array" flag
selflg  db      ?               ;"selective search" flag
search_attr     db      ?       ;files' search attribute
filcnt  dw      ?               ;count of files found
path    db      64 dup (0)      ;search path, passed from BASIC

;DTA structure for findfirst, findnext calls (see DOS documentation)

dta     db      21 dup (0)      ;reserved area (used for findnext)
attrib  db      ?               ;attribute of file found
time    dw      ?               ;time of last write
date    dw      ?               ;date
fsize   dd      ?               ;filesize (32 bit integer)
fname   db      13 dup (0)      ;ASCIIZ filename: name.ext,00
data    ends

dgroup  group   data

code    segment byte public 'code'
        assume  cs:code, ds:dgroup
        .xlist
;
;CALL DSEARCH(path$, attr, count, selective, array ofs)
; path$ is a normal string variable with the path for the
; directory search.  It be a full pathname, including filename
; and extension fields.  To get all the files in the default
; directory on C:, for instance, path$="C:*.*", or all the files
; in subdir on the current drive, path$="subdir\*.*".
; attr is the MS-DOS file attribute for the search.  See below for
; a description.  Each bit represents a file attribute.
; count is an input and an output parameter.  On input, if it is
; zero, the files will not be put in the array, only counted.
; This is provided so that a dynamic array may be allocated after
; the size is determined by dsearch().  If count is non-zero on
; input, the filenames will be placed in the array.
; In either case, the count of filenames found is returned.
; selective is a flag indicating how to do the counting and
; searching process.  If the file attribute 10 is specified, for
; example, DOS will return all those filenames that have the
; subdir attribute as well as those that don't.  If selective is
; set nonzero, the non-subdir files will not be returned.  What
; this does, basically, is allow a "filter normal files" selection
; so that subdirs and volume labels may easily be found.
; array offset is a pointer (offset only for strings) to a string
; array big enough to hold all files found in path$.  The size
; required may be determined as described below.
;
; The attribute byte:
;
;           -------------------------------------
;           | X | X | X | A | D | V | S | H | R |
;           -------------------------------------
;
;  X is don't care, A = archive, D = subdirectory, V = volume label,
;  S = system, H = hidden, R = read-only.
;
        .list

stk     struc
bbp     dw      ?               ;BASIC's bp
retadd  dd      ?               ;return address
arrofs  dw      ?               ;param: array offset
select  dw      ?               ;param: filter normal files flag
count   dw      ?               ;param: count flag/count return
attr    dw      ?               ;param: search attr
path$   dw      ?               ;param: pathname to search
stk     ends

;calculate value to pop with RET n at end of proc (length of params)
popval  =       (offset path$) + (size path$) - (offset arrofs)

strdes  struc                   ;BASIC string descriptor structure
len     dw      ?               ;word length of string
strptr  dw      ?               ;pointer to string space
strdes  ends

dsearch proc    far             ;it's a far proc to BASIC
        public  dsearch         ;let LINK know about this one

        push    bp              ;save BASIC's bp
        mov     bp,sp           ;address stack as it exists now

        mov     [filcnt],0      ;initialize file count

        mov     si,path$[bp]    ;get pointer to string descriptor
        mov     cx,len[si]      ;cx = path length
        mov     si,strptr[si]   ;si -> path string data
        lea     di,path         ;di -> place to fill
        rep     movsb           ;move pathname to our data area
        mov     byte ptr es:[di],0 ;make sure it's an ASCIIZ string

        mov     si,count[bp]    ;get pointer to fill flag in di
        mov     cx,[si]         ;cx = fill flag
        mov     byte ptr [fill],0 ;set flag to "no" first
        or      cx,cx           ;nonzero?
        jz      nofill          ;nope
        mov     byte ptr [fill],0ffh ;yes, set flag

nofill: mov     di,select[bp]   ;get pointer to selective flag in di
        mov     cx,[di]         ;cx = selective flag
        mov     byte ptr [selflg],0 ;set flag to "no" first
        or      cx,cx           ;nonzero?
        jz      nosel           ;nope
        mov     byte ptr [selflg],0ffh ;yes, set flag

nosel:  mov     di,attr[bp]     ;point at search attribute param
        mov     cx,[di]         ;and get it

        mov     ah,2fh          ;get BASIC's DTA
        int     21h
        mov     [dta_ofs],bx
        mov     [dta_seg],es    ;and save it
        lea     dx,dta          ;set DTA to our area here
        mov     ah,1ah
        int     21h

        mov     di,arrofs[bp]   ;di -> first string descriptor
        mov     di,[di]         ;di = first string descriptor offset
        mov     bx,ds           ;set up es to string array segment
        mov     es,bx           ; (BASIC's data segment)
                                ;now es:di -> first string descriptor

        mov     ah,4eh          ;find first matching file
        xor     ch,ch           ;clear hi part of attribute
        lea     dx,path         ;ds:dx points to search path
        int     21h
        jnc     ok              ;if error, cy set
        cmp     ax,18           ;no files found?
        jz      exit            ;yes, don't report error
        mov     [filcnt],0ffffh ;set count to -1 to report path error
        jmp     short exit      ;and leave

ok:     call    countit         ;rets cy if should save, incs filcnt
        jz      findnext        ;no fill if zr set
        call    move_filename   ;do the move

findnext:
        mov     ah,4fh          ;find next function
        int     21h             ;do it
        jc      exit            ;if error, must be no more files
        call    countit         ;count, return cy if should save
        jz      findnext        ;if zr set, don't save name
        call    move_filename   ;move it
        jmp     short findnext  ;and keep hunting

exit:   push    ds
        lds     dx,dword ptr [dta_ofs] ;get BASIC's DTA
        mov     ah,1ah                 ;set DTA fn.
        int     21h
        pop     ds
        mov     di,count[bp]    ;di -> fattr for count return
        mov     ax,[filcnt]     ;get file count
        mov     [di],ax         ;put file count in fattr
        pop     bp              ;restore BASIC's BP
        ret     popval          ;return and pop parameter pointers
dsearch endp

countit         proc
;

;Check file attribute if selective, and update count or not based on
;result. Return flag saying whether or not to move filename, too,
;based on updated count and the "fill" flag.

;
         cmp     [selflg],0ffh  ;are we selective?
         jnz     bump           ;nope, count it
         cmp     [attrib],cl    ;is the attr just what we want?
         je      bump           ;yes, count this file
         cmp     cl,cl          ;nope, set ZF
         jmp     short dontfill ;and skip to exit
bump:    inc     [filcnt]       ;update counter
         cmp     [fill],0       ;now, are we filling?
dontfill:                       ;get here from jne, so NZ
         ret                    ; return with NZ if filling
countit         endp

move_filename   proc    near
;
;es:di points to string descriptor to fill with filename from DTA
;
        push    di              ;save pointer to descr. length part
        mov     di,es:strptr[di] ;and point instead to string data
        lea     si,fname        ;si -> filename (ASCIIZ)
moveloop:
        lodsb                   ;get filename character
        or      al,al           ;is it 0? (end of string?)
        jz      done            ;yes, quit moving data
        stosb                   ;no, store
        jmp     short moveloop  ;and continue
done:   test    byte ptr [attrib],10h ;is this a subdir?
        jz      notsub          ;no
        mov     al,'\'          ;yes, store a trailing backslash
        stosb
notsub: pop     di              ;di -> length in s.d. again
        add     di,4            ;move to next s.d. pointer
        ret
move_filename   endp

code    ends
        end     dsearch

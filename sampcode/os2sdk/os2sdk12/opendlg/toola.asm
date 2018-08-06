;/*
; *   File Dialog Library
; *   Created by Microsoft Corporation, 1989
; */
.xlist
include cmacros.inc
.list

errnz   ?PLM-1          ; this module must use Pascal convention

TRUE   = 1

externFP        <WinCreateHeap, WinDestroyHeap>

sBegin DATA
externW vhModule
externD vhheap
globalW __acrtused,0
sEnd    DATA

sBegin CODE

assumes cs,CODE
assumes ds,NOTHING

; The following trick is to avoid any need for a DS where a constant
;   string is needed, any (PSZ)"foo" is replaced by (PSZ)szfoo with
;   szfoo created in the Code Selector. As we can't do that with C,
;   we do it with MASM.
;
	PUBLIC	szStarStar
szStarStar  LABEL   BYTE
	DB	'*','.','*',0
        PUBLIC  szDot
szDot   LABEL   BYTE
        DB      '.',0
 
;======================================================
; LCopyStruct(pbSrc, pbDst, cb)
;
cProc   LCopyStruct, <PUBLIC, NODATA, FAR>, <ds,si,di>
ParmD lpchSrc
ParmD lpchDst
ParmW cb

cBegin
        mov     cx, cb
        jcxz    lcopydone           ; all done if crc   == 0
        lds     si, lpchSrc
        les     di, lpchDst
        cmp     si,di
        jae     lcopyok
        mov     ax,cx
        dec     ax
        add     si,ax
        add     di,ax
        std
        rep     movsb
        cld
        jmp     short lcopydone
lcopyok:
        cld
        rep     movsb
lcopydone:

cEnd


;======================================================
; LFillStruct(pbSrc, pbDst, cb)
;
cProc   LFillStruct, <PUBLIC, NODATA, FAR>, <di>
ParmD lpchDst
ParmW cb
ParmB fillByte

cBegin
        mov     cx, cb
        jcxz    lfilldone           ; all done if crc   == 0
        les     di, lpchDst
        cld
        mov     al,fillByte
        rep     stosb
lfilldone:
cEnd
;======================================================
;
; lstrlen: Same as strlen except for taking long ptrs
;

cProc   lstrlen,<PUBLIC, NODATA, FAR>
;       parmD   pStr
cBegin  nogen
        mov     bx,sp
        push    di
        les     di,ss:[bx+4]
        cld
        xor     ax,ax                   ; get zero in ax
        mov     cx,-1                   ; at most 64 k to move
        repnz   scasb                   ; look for end
        mov     ax,cx
        neg     ax
        dec     ax
        dec     ax
        pop     di
        ret     4
cEnd    nogen


lstrsetup:
        pop     dx
        mov     bx,sp

        push    ds
        push    si
        push    di

        lds     si,ss:[bx+4]
        les     di,ss:[bx+8]
        regptr  dssi,ds,si
        regptr  esdi,es,di
        cld
        jmp     dx

lstrfinish  proc    far
        pop     di
        pop     si
        pop     ds
        ret     8
lstrfinish  endp


;======================================================
;
;lstrcpy: strcpy with long pointers
;
cProc   lstrcpy,<PUBLIC, NODATA, FAR>
;       parmD   pDst                    ; [bx+8]
;       parmD   pSrc                    ; [bx+4]

cBegin  nogen
        call    lstrsetup
lcp1:   lodsb
        stosb
        or      al,al
        jnz     lcp1
        mov     ax,di                   ; point at last byte copied
        dec     ax
        mov     dx,es                   ; and its segment
        jmp     lstrfinish
cEnd    nogen


;======================================================
;
;lstrcat: Same as strcat except with long ptrs.
;
cProc   lstrcat,<PUBLIC, NODATA, FAR>
;   parmD   pDst
;   parmD   pSrc

cBegin  nogen
        call    lstrsetup
        xor     ax,ax                   ; get zero in ax
        mov     cx,-1                   ; at most 64 k to look
        repnz   scasb                   ; look for end
        dec     di                      ; Point at null byte
        jmp     lcp1                    ; jump to lstrcpy loop
cEnd    nogen

        ; convert lower case to upper, must preserve es,di,cx
        public  MyUpper
MyUpper:
        cmp     al,'a'
        jb      myu2
        cmp     al,'z'
        jbe     myu1
        ja      myu2
myu1:   sub     al,'a'-'A'
myu2:   ret

        ; convert upper case to lower, must preserve es,di,cx
        public  MyLower
MyLower:
        cmp     al,'A'
        jb      myl2
        cmp     al,'Z'
        jbe     myl1

        ja      myl2
myl1:   add     al,'a'-'A'
myl2:   ret

;=========================================
;
;lstrcmp:   long ptr lVersion of strcmp
;
cProc   lstrcmp,<PUBLIC, NODATA, FAR>
;       parmD   ps1
;       parmD   ps2
cBegin  nogen
        call    lstrsetup
lcmploop:
        xor     ax,ax                   ; AH needs to be zero inside this loop
        cmp     byte ptr [si],al
        je      lsidone                 ; si is finished check for di finish
        cmp     es:byte ptr [di],al
        je      ldismall                ; di finished before si
        lodsb
        mov     bx,ax
        mov     al,es:[di]
        inc     di
        cmp     ax,bx
        je      lcmploop                ; still equal
        mov     al,0                    ; preverve flags
        jb      lsismall                ; si is less than di
ldismall:
        inc     ax
        jmp     lstrfinish
lsidone:
        cmp     byte ptr es:[di],0
        je      lstrfinish
lsismall:
        dec     ax
        jmp     lstrfinish
cEnd    nogen
sEnd CODE


createSeg _INIT,INIT,byte,public,CODE

sBegin  INIT
externNP    <InitLibrary>
assumes CS,INIT
assumes DS,DATA

;======================================================
; BOOL FAR PASCAL LibInit ()
; 
; Initializes the library
; int dataSelector       /* DS = automatic data selector */
; int wHeapSize;         /* SI = heap size */
; int hmod;           /* DI = module handle */
;     {
;     vhModule = hmod;
;     if (!(vhheap = WinCreateHeap(dataSelector, wHeapsize, 0, 0, 0, 0)))
;         return FALSE;;
;     return (InitLibrary());
;     }
;
; DS already usSet
;
cProc   LibInit,<FAR, PUBLIC>
cBegin  nogen
        mov     [vhModule],di           ; Remember module handle

        sub     ax,ax
        cCall   WinCreateHeap,<ds, si, ax, ax, ax, ax>
        mov     bx, ax
        or      ax, dx
        jz      loaddone                ; Heap wasn't created
        mov     word ptr vhheap, bx
        mov     word ptr vhheap + 2, dx

        cCall   InitLibrary                ; continue initialization
        or      ax,ax
        jnz     loaddone                ; OK, init done
        cCall   WinDestroyHeap,<vhheap> ; Non OK, destroy the heap
;
; Return non-zero to indicate successful initialization
;
loaddone:
        ret
cEnd

sEnd    INIT
        END LibInit

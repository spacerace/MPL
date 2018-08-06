;  DDE manager library initialization routine

.286p

?WIN=1      ; Use Windows prolog/epilog
?PLM=1      ; Use PLM calling convention
DOS5=1
.xlist
include cmacros.inc
.list

sBegin  DATA

assumes DS,DATA

externW hmodDmg
externW usHugeShift

sEnd    DATA

sBegin  CODE
assumes cs,CODE
assumes ds,DATA

externNP        SemInit

;
; Registers set up by DosLoadModule...
;
;   SI = heap size
;   DI = module ID
;   DS = library's automatic data segment
;
cProc   LoadProc,<FAR,PUBLIC>
cBegin  LoadProc
;       int     3
        mov     hmodDmg, di
        call    SemInit
cEnd    LoadProc

;
; FillBlock(PBYTE pDst, USHORT cb, BYTE b)
;
cProc	FillBlock,<PUBLIC, NEAR>,<DI, DS>
ParmD	pDst
ParmW	cb
ParmW	b
cBegin
	les	di,pDst
	mov	cx,cb
	mov	ax,b
	cld
	rep	stosb
cEnd

;
; CopyBlock(pbSrc, pbDst, cb)
;
LabelNP <PUBLIC, CopyBlock>
        mov     bx,sp
        push    si
        push    di
        mov     dx,ds               ; preserve DS

        mov     cx,ss:[bx+2]
        jcxz    copydone           ; all done if crc   == 0
        les     di,ss:[bx+2+2]
        lds     si,ss:[bx+2+2+4]
        cmp     si,di
        jae     copyok
        mov     ax,cx
        dec     ax
        add     si,ax
        add     di,ax
        std
        rep     movsb
        cld
        jmp     short copydone
copyok:
        cld
        rep     movsb
copydone:

        mov     ds,dx
        pop     di
        pop     si
        ret     10


cProc HugeOffset,<NEAR, PUBLIC>
parmD   pSrc
parmD   cb
cBegin
        mov     dx, SEG_cb
        mov     ax, OFF_pSrc
        add     ax, OFF_cb
        adc     dx, 0
        mov     cx, usHugeShift
        shl     dx, cl
        add     dx, SEG_pSrc
cEnd  


LabelFP <PUBLIC, DdeDebugBreak>
        int     3
        retf    0

;
; Returns segment size or 0 on error.
;
LabelNP <PUBLIC, CheckSel>
;    parmW   Selector    ; selector to validate
cBegin  nogen
        mov     bx,sp               ; BX = selector to validate
        mov     bx,ss:[bx].2
        lar     ax,bx               ; See if valid selector
        jnz     invalid_selector
        
        lsl     ax,bx
        or      ax,ax               ; zero sized?
        jnz     valid_selector      ; nope, ok.

invalid_selector:
        xor     ax,ax               ; Return zero just to be nice
        
valid_selector:
        ret     2
        
cEnd    nogen
        
sEnd    CODE
end     LoadProc

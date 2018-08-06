?PLM = 1;
?WIN = 1;
memS = 1

include cmacros.inc

externFP    <LocalInit>
externFP    <UnlockSegment>

; PUBLIC definition of LIBINST, the library instance
sBegin  DATA
        assumes DS,DATA
        PUBLIC  _LIBINST
_LIBINST dw     ?
sEnd    DATA

sBegin CODE
assumes CS,CODE

cProc myastart,<PUBLIC,FAR>
cBegin
        ;;
        ;; DS = automatic data segment.
        ;; CX = size of heap.
        ;; DI = module handle.
        ;; ES:SI = address of command line (not used).
        ;;

        mov    _LIBINST,di
        xor    ax,ax
        cCall  LocalInit,<ds,ax,cx>
        mov    ax,0ffffh
        cCall  UnlockSegment,<ax>
cEND    myastart

sEnd   CODE

end myastart

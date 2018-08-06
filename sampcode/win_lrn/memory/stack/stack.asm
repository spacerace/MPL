;
; WINDOWS STACK USAGE CHECKER
;
; This code checks for the amount of stack being used by a Windows application.
; It does this check by initializing the stack to a known value, then going 
; from the end of stack up until it finds the first value changed.  If the
; first value checked is changed, then it assumes the stack overflowed.
;
; The way it finds the end of the stack is by the variable rsrvptrs[6].  This
; is initialized by the Windows initialization code.  Also, the start of the
; stack is rsrvptrs[A].  rsrvptrs[8] is the stack low-water mark, but this is
; only set if _chkstk is called, and Windows doesn't call this on its entry
; points (why, I don't know).
;
; The routine InitStackUsed() initializes the stack to a known value.  The
; routine ReportStackUsed() returns an integer value which is the amount of
; stack space used, -1 if stack overflow was detected.
;
; Written 10/87 by Bill Turner.  Original concept by Bill Turner (modified from
; similar code for another environment)
;

;
; See what memory model to use for the stack checking routines.  This is
; specified when the module is assembled by specifying -D?S for small,
; -D?M for medium.
;

if1
    ifdef ?S
        memS = 1
        %out ! Compiling for SWINLIBC.LIB
    else
        memM = 1
        %out ! Compiling for MWINLIBC.LIB/LWINLIBC.LIB
    endif
endif

;
;   Now include the Windows macro definition file.
;

.xlist
?PLM = 1;
?WIN = 1;
include cmacros.inc
.list

sBegin  DATA
        externW rsrvptrs
sEnd    DATA

sBegin  CODE
assumes CS,CODE
assumes DS,DATA

        PUBLIC  InitStackUsed
        PUBLIC  ReportStackUsed

;
; void PASCAL InitStackUsed();
;
; This routine initializes the stack to a known value.  It uses the STOSW
; instruction, and therefore wipes ES, DI, AX, and CX.
;

cProc   InitStackUsed
cBegin

        push    ss
        pop     es
        mov     di, rsrvptrs[6]         ; minimum SP value
        mov     cx, sp
        sub     cx, di                  ; space from end of stack to current
                                        ; SP value (in bytes)
        shr     cx,1                    ; convert to word count
        mov     ax, 0AAAAh              ; "known value" for comparison
        cld                             ; move to high memory   
        rep     stosw                   ; fill memory

cEnd


;
; int PASCAL ReportStackUsed();
;
; This checks to see how much stack was actually used.  If the min SP value
; has been changed (ss:rsrvptrs[6]), then assume a stack overflow has occured.
; Otherwise, scan through memory until a changed address is found, and subtract
; the address from the maximum SP value (rsrvptrs[A]).  This is the amount of
; stack space used.  This routine wipes out ES, DI, AX, and CX.
;

cProc ReportStackUsed
cBegin

        xor     cx, cx
        dec     cx              ; put -1 into CX -- so that REPZ will go until
                                ; an address that changed is found.
        push    ss
        pop     es
        mov     di, rsrvptrs[6]
        mov     ax, 0AAAAh      ; "known value" stack was initialized to
        cld                     ; move to high memory
        repz    scasw
;
; At this point, DI is the next word after the change was detected.  Decrement
; by one word, and calculate stack usage.
;
        cmp     di, rsrvptrs[6]         ; same as minimum value:
        jz      overflowed              ; YES, report overflow
        sub     di, rsrvptrs[0Ah]       ; difference in bytes (negative value)
        mov     ax, di                  ; move into AX for return.
        neg     ax                      ; and make a positive number
        jmp     do_rtn

overflowed:
        xor     ax, ax
        dec     ax                      ; move -1 into AX

do_rtn:                                 ; dummy label for end of procedure

cEnd

sEnd    CODE

        end


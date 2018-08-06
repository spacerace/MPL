


include cmacros.inc

externFP   <GlobalCompact>

createSeg   ASMCODE,ASMCODE,PARA,PUBLIC,ASMCODE

;;
;;  Int21Function4B cannot be a discardable segment.  If the segment
;;  were discardable, the int21 interrupt might cause it to be discarded
;;  and then reread from disk.  This would set stackSS and stackSP to 0.
;;  Upon return from int21, SS would be set to 0 and SP would be set to
;;  0.
;;
;;  Warning:  the code is not reentrant.  Multiple sp's cannot be saved.
;;  Warning:  In applications SS == DS by default.  If the DS should move
;;  the stored SS would be invalidated.  For maximum reliability it is
;;  recommended that LockData(), UnlockData() call bracket the call to
;;  int21function4B.
;;  Warning:  Should the code segment move using the debugging KERNEL,
;;  the segment will be checksummed and the write of SS:SP into the
;;  code segment detected as a fatal error.  To avoid this extraneous
;;  ( in this one instance ) error condition,
;;  use the non-debugging kernel or place the code in a fixed segment.
;;

assumes CS,ASMCODE
assumes DS,DATA


sBegin DATA
sEnd   DATA

sBegin ASMCODE
	stackSS dw 0
	stackSP dw 0

cProc   Int21Function4B,<PUBLIC,FAR>,<ax,bx,cx,dx,si,di,es,ds>
	parmB mode
	parmD path
	parmD execblock
cBegin
      mov ax,-1
      cCall GlobalCompact,<ax,ax>
	mov cs:[stackSS],ss         ;; EXEC destroys all register. Save SS:SP.
	mov cs:[stackSP],sp
	mov al,mode
	lds dx,path
	les bx,execblock
	mov ah,4bh
	int 21h
	mov ss,cs:[stackSS]
	mov sp,cs:[stackSP]
; AX is return value
cEnd

sEnd ASMCODE
END

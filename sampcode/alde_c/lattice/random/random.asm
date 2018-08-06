PAGE   54,130
; RANDOM   - Random number generator for C programs
;
; This routine returns 16-bit unsigned pseudo-random numbers
; of uniform distribution.  The numbers are computed according
; to the Linear Congruential method as described in Knuth's
; "Seminumerical Algorithms",  using  the formula (ax) mod (w+1)
; as described on pp 11-12.
;
; No parameters are passed to this routine; the return is unsigned.
;
; The seed for each call is preserved form the previous call.
; The routine is "self seeding"; if the seed is found to contain
; zeros a DOS Get Time (x'2C') call is used to obtain the time, and
; the seconds and 100ths of seconds are used as the seed for
; subsiquent calls.
;
; However, for applications which require duplicatable series of terms
; (such as when testing), the seed, XRAND, is defined as an EXTERN
; and may be initialized by the calling program (via assignment)
; prior to calling RANDOM.
;
; The initial values for the multiplier and constant are borrowed
; from "A Guide to PL/M Programming for Microcomputer Applications",
; by Daniel D. McCracken (Addison-Wesley, 1978).
;
PGROUP	GROUP  PROG
PARAMS	STRUC
OLD_BP	DW     ?		   ; Caller's BP Save
RETN	DW     ?		   ; Return address from call
PARAMS	ENDS
;
DGROUP	GROUP  DATA
DATA	SEGMENT WORD PUBLIC 'DATA'
	ASSUME DS:DGROUP
	PUBLIC XRAND
XRAND	DW     0		   ; "X" in formula - next/last term
MULTIP	DW     2053		   ; multiplier
MODULUS DW     65535		   ; constant
DATA	ENDS
PROG	SEGMENT BYTE PUBLIC 'PROG'
	ASSUME CS:PGROUP
	PUBLIC RANDOM
;
RANDOM	PROC   NEAR
	PUSH   BP		   ; save caller's frame pointer
	MOV    BP,SP		   ; set up our frame pointer
	MOV    AX,XRAND 	   ; move last random number to
	CMP    AX,0		   ; is it uninitialized?
	JNE    GETNXT		   ; ..no, skip this stuff
	MOV    AH,2CH
	INT    21H		   ; .. get time from DOS
	MOV    AX,DX		   ; initialize w/ seconds, 100ths
GETNXT: NEG    AX		   ; negate the register
	MUL    MULTIP		   ; 32-bit multipl goes in DX,AX
	MOV    XRAND,AX 	   ; save low order word for seed
	SUB    DX,XRAND 	   ; subtract low word from high
	JS     EXIT		   ; if negative, skip next steps
	INC    AX
	INC    AX		   ; increment term by 2
	ADD    AX,MODULUS	   ; add the increment
EXIT:	POP    BP		   ; restore the caller's frame pointer
	RET			   ; return to caller
RANDOM	ENDP
;
PROG	ENDS
	END

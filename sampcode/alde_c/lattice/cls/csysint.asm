PAGE   54,130
; CSYSINT  - Lattice "C" interface to the world
;
; This program was adapted from PSYSINT, a program listed
; and described by Will Fastie in SOFTALK Magazine, which
; provided access to system calls from IBM Pascal programs.
; It in turn was taken from George Eberhart's version for the
; Computer Inovations C86 Compiler, with permission.
;
; Modified by T. Cox to match the Microsoft C Compiler
; assembly language interface call conventions, and renamed
; to CSYSINT.
;
; C calling sequence:
;
;     flags = csysint(interrupt,&sreg,&rreg);
;
; Where sreg and rreg are structures of four words each representing
; the 8088 registers AX, BX, CX, and DX.  "regsetT is a structure
; defining these as all the half-register names (e.g.,AL,AH).
;
; interrupt is the number of the system interrupt desired.
;
; The return value is a 16-bit unsigned integer. It contains
; the machine status register.
;
; This C function calling sequence passes the interrupt code as a value
; and the two register sets as pointers to their respective structures.
;
; The incoming stack looks like this in memory locations "mem".  Note
; that the call is a NEAR call (page 1-36 of the Microsoft C manual),
; so only the return address (and not the segment address) is pushed.
;
;   High				      <-- Caller's BP
;	     mem + 06:	 interrupt code value
;	     mem + 04:	 SREG address
;	     mem + 02:	 RREG address
;	     mem + 00:	 caller's saved BP
;   Low 				      <-- BP, SP
;
PGROUP	GROUP  PROG
PARAMS	STRUC
OLD_BP	DW     ?		   ; Caller's BP Save
RETN	DW     ?		   ; Return address from call
ARG1	DW     ?		   ; First arguement
ARG2	DW     ?		   ; Second arguement
ARG3	DW     ?		   ; Third arguement
PARAMS	ENDS
;
PROG	SEGMENT BYTE PUBLIC 'PROG'
	ASSUME CS:PGROUP
	PUBLIC CSYSINT
;
CSYSINT PROC   NEAR
	PUSH   BP		   ; save caller's frame pointer
	MOV    BP,SP		   ; set up our frame pointer
;
	CALL   DUMMY		   ; trick - push the IP
DUMMY:
	POP    AX		   ; get it
	SUB    AX,OFFSET DUMMY	   ; calculate the address of the INT
	ADD    AX,OFFSET PINT
	MOV    DI,AX		   ; move it to the index register
	MOV    AX,[BP].ARG1	   ; get the desired interrupt number
	MOV    CS:[DI+1],AL	   ; put it in the INT instruction
	CALL   REGSIN		   ; get the registers from SREG
	PUSH   BP		   ; we'll need our own BP later
PINT:	INT    00H		   ; perform the requested interrupt
	POP    BP		   ; get our BP back
	PUSHF			   ; hang onto the flags for the return
	CALL   REGSOUT		   ; put the registers into RREG
	POP    AX		   ; flags are the return value
	POP    BP		   ; restore the caller's frame pointer
	RET			   ; return to caller
CSYSINT ENDP
;
; ---------------------------------
; Subroutines to move the registers in and out
;
NRREGS	EQU    4		   ; this version supports only four regs
;
REGSIN	PROC   NEAR		   ; -- Move Registers In
	MOV    BX,[BP].ARG2	   ; get address of register set SREG
	MOV    CX,NRREGS	   ; ...and how many registers to move
INLOOP:
	PUSH   WORD PTR [BX]	   ; push one
	INC    BX		   ; point to the next one
	INC    BX
	LOOP   INLOOP		   ; ..do it some more
	POP    DX		   ; now pop them into their proper places
	POP    CX
	POP    BX
	POP    AX
	RET			   ; all done
REGSIN	ENDP
;
REGSOUT PROC   NEAR		   ;--Move Registers Out
	PUSH   DX		   ; push all the registers in reverse order
	PUSH   CX
	PUSH   BX
	PUSH   AX
	MOV    BX,[BP].ARG3	   ; get the address of RREG
	MOV    CX,NRREGS	   ; ...and how many registers to move
LOOPOUT:
	POP    WORD PTR [BX]	   ; recover the register
	INC    BX		   ; point to the next
	INC    BX
	LOOP   LOOPOUT		   ; do it again
	RET
REGSOUT ENDP
;
PROG	ENDS
	END

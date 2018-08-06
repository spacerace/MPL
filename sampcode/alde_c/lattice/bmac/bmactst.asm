	INCLUDE	TITLE.MAC
	.TITLE	<BMACTST -- Test of BMAC Macros>
	.SBTTL	Declarations

; bmactst.asm  28 Nov 83  Craig Milo Rogers at USC/ISI
;	Convert to new title system.
; bmactst.asm  19 Oct 83  Craig Milo Rogers at USC/ISI
;	This file tests the BMAC macros.
;

if1
	INCLUDE	DOS.MAC
	INCLUDE BMAC.MAC
endif

	.SBHED	<TEST1 -- Simplest Function>

	PSEG
	BENTRY	TEST1
	BEND	TEST1
	ENDPS

	.SBHED	<TEST2 -- Simple Function and Call>

	PSEG
	BENTRY	TEST2
	BCALL	TEST1
	BEND	TEST2
	ENDPS

	.SBHED	<TEST3 -- Function With Arguments>
	PAGE

	PSEG
	BENTRY	TEST3	<ARG1,ARG2,ARG3>
	MOV	AX,ARG1
	MOV	AX,ARG2
	MOV	AX,ARG3
	BEND	TEST3
	ENDPS

	.SBHED	<TEST4 -- Call With Arguments>

	PSEG
	BENTRY	TEST4	<ARG1,ARG2,ARG3>
	BCALL	TEST3	<ARG1,ARG2,ARG3>
	BEND	TEST4
	ENDPS

	.SBHED	<TEST5 -- Save Some Registers>

	PSEG
	BENTRY	TEST5
	SAVE	<AX,BX,CX,DX>
	BEND	TEST5
	ENDPS

	.SBHED	<TEST6 -- Allocate Local Variables>

	PSEG
	BENTRY	TEST6
	AUTO	<VAR1,VAR2,VAR3>
	MOV	AX,VAR1
	MOV	AX,VAR2
	MOV	AX,VAR3
	BEND	TEST6
	ENDPS

	.SBHED	<TEST7 -- Save Regs and Allocate Locals>

	PSEG
	BENTRY	TEST7
	AUTO	<VAR1,VAR2,VAR3>
	SAVE	<AX,BX,CX,DX>
	MOV	AX,VAR1
	MOV	BX,VAR2
	MOV	CX,VAR3
	BEND	TEST7
	ENDPS

	.SBHED	<TEST8 -- Redefine Names with New Values>

	PSEG
	BENTRY	TEST8	<ARG3,ARG2,ARG1>
	AUTO	<VAR3,VAR2,VAR1>
	SAVE	<DX,BX,CX,AX>
	MOV	AX,ARG1
	MOV	BX,ARG2
	MOV	CX,ARG3
	MOV	AX,VAR1
	MOV	BX,VAR2
	MOV	CX,VAR3
	BEND	TEST8
	ENDPS

	END

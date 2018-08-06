/*
** 8088/8086 version
** rand(seed) - Generate a random number using a linear
** congruence generator.  Seed is the initial seed for
** the generator.  On subsequent calls to rand() seed should
** be set to the value of the previeous random number. i.e.:
** newrand = rand(oldrand);
*/
rand(seed) int seed; {
#asm
	MOV	BP,SP
	MOV	AX,2[BP]	;Get seed
	MOV	BX,58653
	MUL	BX
	ADD	AX,13849
	MOV	BX,AX
	XOR	CX,CX		;For Small-C
#endasm
}
	
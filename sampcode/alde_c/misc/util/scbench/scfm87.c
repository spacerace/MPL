/*
** BYTE Small C Floating Point Coprocessor Library
** BYTE Small C based on Small C by J.E. Hendrix
** 8087 version for 8088/8086
** Version 1, Feb. 17, 1988
**
** Conventions:
** All variables are manipulated OUTSIDE the coprocessor in
** 8-byte form.
*/

int	fsword;		/* Storage for status word */
int	fsword1;	/* Extra storage for same */
/*
** Initialize coprocessor
** Call this at the beginning and end of the program
*/
finit() {
#asm
	FINIT
#endasm
}

/*
** fint2float(ptr)
** Converts integer pointed to by (ptr) into 8-byte floating point
** form and stores it back into (ptr);
*/
fint2float(ptr) int ptr[]; {
#asm
	MOV	BP,SP
	MOV	SI,2[BP]	;Get pointer to 8-byte block
	FILD	QWORD PTR [SI]	;Load the block in
	FSTP	QWORD PTR [SI]	;Put it back out
#endasm
}

/*
** ffloat2int(ptr)
** Converts 8-byte floating point number at ptr back to an integer.
*/
ffloat2int(ptr) int ptr[];
{
#asm
	MOV	BP,SP
	MOV	SI,2[BP]	;Get pointer to 8-byte block
	FLD	QWORD PTR [SI]	;Load it in
	FISTP	QWORD PTR [SI]	;Save it back
#endasm
}

/*
** f2add(fac1,fac2,dest)
** Add fac1 to fac2, store the result in dest
*/
f2add(fac1,fac2,dest) int fac1[],fac2[],dest[];
{
#asm
	MOV	BP,SP
	MOV	SI,6[BP]	;Get first factor
	FLD	QWORD PTR [SI]	;Push it
	MOV	SI,4[BP]	;Get second factor
	FADD	QWORD PTR [SI]	;Add it
	MOV	SI,2[BP]	;Get destination
	FSTP	QWORD PTR [SI]	;Store
#endasm
}

/*
** f2sub(fac1,fac2,dest)
** Subtract fac2 from fac1, store result in dest
*/
f2sub(fac1,fac2,dest) int fac1[],fac2[],dest[];
{
#asm
	MOV	BP,SP
	MOV	SI,6[BP]	;Get fac1
	FLD	QWORD PTR [SI]	;Push it
	MOV	SI,4[BP]	;Get fac2
	FSUB	QWORD PTR [SI]	;Subtract
	MOV	SI,2[BP]	;Destination
	FSTP	QWORD PTR [SI]	;Store
#endasm
}

/*
** f2mult(fac1,fac2,dest)
** Multiply fac1 by fac2, result in dest
*/
f2mult(fac1,fac2,dest) int fac1[],fac2[],dest[];
{
#asm
	MOV	BP,SP
	MOV	SI,6[BP]	;get fac1
	FLD	QWORD PTR [SI]	;Push it
	MOV	SI,4[BP]	;Get fac2
	FLD	QWORD PTR [SI]
	FMUL
	MOV	SI,2[BP]	;Destination pointer
	FSTP	QWORD PTR [SI]	;Store
#endasm
}

/*
** f2div(fac1,fac2,dest)
** Divide fac1 by fac2, result in dest
*/
f2div(fac1,fac2,dest) int fac1[],fac2[],dest[];
{
#asm
	MOV	BP,SP
	MOV	SI,6[BP]	;Get fac1
	FLD	QWORD PTR [SI]	;Push it
	MOV	SI,4[BP]	;Get fac2
	FDIV	QWORD PTR [SI]	;Divide
	MOV	SI,2[BP]	;Destination
	FSTP	QWORD PTR [SI]	;Store it
#endasm
}

/*
** fload(ptr)
** Load floating point number given by ptr
*/
fload(ptr) int ptr[];
{
#asm
	MOV	BP,SP
	MOV	SI,2[BP]
	FLD	QWORD PTR [SI]
#endasm
}

/*
** fstore(ptr)
** Store floating point number given by ptr - pops floating point stack
*/
fstore(ptr) int ptr[];
{
#asm
	MOV	BP,SP
	MOV	SI,2[BP]
	FSTP	QWORD PTR [SI]
#endasm
}

/*
** fadd(ptr)
** Add contents of ptr to top of floating point stack -- result on stack
*/
fadd(ptr) int ptr[];
{
#asm
	MOV	BP,SP
	MOV	SI,2[BP]
	FADD	QWORD PTR [SI]
#endasm
}

/*
** fsub(ptr)
** Subtract contents of ptr from top of stack -- result on top of stack
*/
fsub(ptr) int ptr[];
{
#asm
	MOV	BP,SP
	MOV	SI,2[BP]
	FSUB	QWORD PTR [SI]
#endasm
}

/*
** fmult(ptr)
** Multiply contents of ptr by top of stack -- result on top of stack
*/
fmult(ptr) int ptr[];
{
#asm
	MOV	BP,SP
	MOV	SI,2[BP]
	FMUL	QWORD PTR [SI]
#endasm
}

/*
** fdiv(ptr)
** Divide top of stack by contents of ptr -- result on top of stack
*/
fdiv(ptr) int ptr[];
{
#asm
	MOV	BP,SP
	MOV	SI,2[BP]
	FDIV	QWORD PTR [SI]
#endasm
}

/*
** fabs()
** Set top of stack to its absolute value
*/
fabs()
{
#asm
	FABS
#endasm
}

/*
** fsqrt()
** Take square root of top of floating point stack
*/
fsqrt()
{
#asm
		FSQRT
#endasm
}

/*
** fcompz(ptr)
** Compares number at ptr with zero
** Returns: -1 if less, 0 if equal, 1 if greater
*/
fcompz(ptr) int ptr[];
{
#asm
	FLDZ			;get zero on stack
	MOV	BP,SP
	MOV	SI,2[BP]
	FLD	QWORD PTR [SI]	;Get the number
	FCOMPP			;Compare
	FSTSW	_fsword		;Store the control word
	FWAIT			;Make sure the coprocessor is done
	MOV	AX,_fsword	;Get the control word
	XOR	BX,BX
	SAHF			;Move into flags
	JZ	FCZX
	JNC	FCZ1
	DEC	BX
	JMP	FCZX
FCZ1:	INC	BX
FCZX:

#endasm
}
/*
** fcomp(ptr1,ptr2)
** ptr1 and ptr2 point to floating-point numbers.
** This function returns:
** 1 if ptr1>ptr2; 0 if ptr1=ptr2; -1 if ptr1<ptr2
*/
fcomp(ptr1,ptr2) int ptr1[],ptr2[];
{
#asm
	MOV	BP,SP
	MOV	SI,2[BP]	;Get ptr2
	FLD	QWORD PTR [SI]	;Load it in

	MOV	SI,4[BP]
	FLD	QWORD PTR [SI]	;Get ptr1
	FCOMPP			;Compare
	FSTSW	_fsword		;Store the control word
	FWAIT			;Make sure the coprocessor is done
	MOV	AX,_fsword	;Get the control word
	XOR	BX,BX
	SAHF			;Move into flags
	JZ	FCMX
	JNC	FCM1
	DEC	BX
	JMP	FCMX
FCM1:	INC	BX
FCMX:
#endasm
}

/*
** ffact(n) int n;
** Calculate n!.  n! is left on top of stack.
*/
ffact(n) int n;
{
#asm
	MOV	BP,SP		;get value
	MOV	AX,2[BP]
	FLD1
	FLD1
FFCT0:	DEC	AX		;done?
	JZ	FFCT1
	FLD1
	FADD	ST,ST(2)	;bump
	FST	ST(2)
	FMUL			;Multiply
	JMP	SHORT FFCT0
FFCT1:	FFREE	ST(1)		;Clear n
#endasm
}

/*
** fsin(ptr,ptr1)
** Calculate sine of quantity at ptr...returns sine in ptr1
*/
fsin(ptr,ptr1) int ptr[],ptr1[];
{
#asm
	MOV	BP,SP
	MOV	SI,4[BP]	;Get pointer
	FLD	QWORD PTR [SI]	;Get argument
;See if arg is 0 -- if so, we can skip a lot
	FTST
	FSTSW	_fsword
	FWAIT
	MOV	AX,_fsword
	SAHF
	JZ	FSIN1
	FFREE	ST(0)
	FLD1			;Get a 1
	FLD	ST(0)		;Dup it
	FADD			;2 now at stack top
	FLD	ST(0)		;Dup it
	FLD	QWORD PTR [SI]	;Get argument
	FDIVR			;arg/2
	FPTAN			;Get partial tangent
	FDIVR			;Calc y/x
	FST	ST(2)
	FMUL			;2*(y/x) on top
	FXCH
	FMUL	ST(0),ST	;(y/x)^2
	FLD1			;Get 1 again
	FADD			;(1+(y/x)^2)
	FDIV
FSIN1:	MOV	SI,2[BP]
	FSTP	QWORD PTR [SI]
#endasm
}

/*
** fxtoy(x,y,z)
** Given x and y are pointers to floating point numbers,
** calculates x^y and returns the result in z.
** Not that this routine is only good for returning roots
** of things.
*/
fxtoy(x,y,z) int x[],y[],z[];
{
#asm
	MOV	BP,SP
	MOV	SI,4[BP]	;Get y
	FLD	QWORD PTR [SI]	;y in st(0)
	MOV	SI,6[BP]
	FLD	QWORD PTR [SI]	;x on top
	FYL2X			;y*log2(x)
	CALL	_ftwo2x		;2^(y*log2(x))
	MOV	SI,2[BP]	;Get z address
	FSTP	QWORD PTR [SI]	;Store
#endasm
}

/*
** fex(ptr,ptr1)
** Raises e^x, where x is in ptr.  Places result in ptr1.
**/
fex(ptr,ptr1) int ptr[],ptr1[];
{
#asm
	MOV	BP,SP
	MOV	SI,4[BP]
	FLD	QWORD PTR [SI]	;x in st(0)
	FLDL2E			;log2(e)
	FMUL			;x*log2(e)
	CALL	_ftwo2x		;2^(x*log2(e))
	MOV	SI,2[BP]	;Get destination
	FSTP	QWORD PTR [SI]	;Store
#endasm
}

/*
** fround(ptr)
** Assume ptr points to quadword floating point.
** Rounds contents of ptr to integer and stores result in ptr
*/
fround(ptr) int ptr[];
{
#asm
	MOV	BP,SP
	MOV	SI,2[BP]	;Pointer to si
	FLD	QWORD PTR [SI]
	FRNDINT
	FSTP	QWORD PTR [SI]
#endasm
}

/*
** fstoreb(ptr)
** Store top of stack as BCD into ptr
*/
fstoreb(ptr) int ptr[];
{
#asm
	MOV	BP,SP
	MOV	SI,2[BP]	;Get pointer
	FBSTP	[SI]		;Store it
	FWAIT
#endasm
}

/*
** ftwo2x()
** Raises two to the power of x.
** x is on top of the stack.
** Leaves result on top of stack.
*/
ftwo2x()
{
#asm
;First save existing control word
	FNSTCW	_fsword
	MOV	AX,_fsword
	AND	AX,0F3FFH	;Round toward nearest even
	MOV	_fsword1,AX
	FLDCW	_fsword1	;New control word
;Now break exponent into integer and fractional part
	FLD	ST(0)		;Dup x
	FRNDINT			;Int part in ST(0)
	FSUB	ST(1),ST	;Fractional part in ST(1)
	FXCH			;Switch - int in st(1),
				;frac. in st(0)
	FTST			;frac. < 0 ?
	FSTSW	_fsword1
	FWAIT
	F2XM1			;2^(f)-1
;See if fractional part is negative
	MOV	AX,_fsword1
	SAHF
	JNC	T2X1
;Fractional part is neg.
	FCHS			;Make it positive
	FLD	ST(0)		;dup
	FLD1			;1 on top
	FADD			;2^(-f) on top
	FDIV			;(2^(-f)-1)/2^(-f)
	FCHS			;2^f-1
T2X1:	FLD1
	FADD			;2^f
	FSCALE			;2^x
	FFREE	ST(1)		;Clear out i
;Restore control word
	FLDCW	_fsword
#endasm
}

/*
** fconst(val) int val;
** Load top of FPU stack with constant based on val:
** val     constant
** 0        0.0
** 1        1.0
** 2        pi
** 3        log2(e)
** 4        log2(10)
** 5        log10(2)
** 6        loge(2)
*/
fconst(val) int val;
{
	switch(val) {
	 default:
	 case 0:
#asm
	  FLDZ
#endasm
	   break;

	 case 1:
#asm
	  FLD1
#endasm
	   break;

	 case 2:
#asm
	  FLDPI
#endasm
	   break;

	 case 3:
#asm
	  FLDL2E
#endasm
	   break;

	 case 4:
#asm
	  FLDL2T
#endasm
	   break;

	 case 5:
#asm
	  FLDLG2
#endasm
	   break;

	 case 6:
#asm
	  FLDLN2
#endasm
	  break;
	}
	return;
}

/*
** fltprint(n,num)
** Entry:
**  num points to an 8-byte floating-point number
**  n is the number of characters to print
** This function prints the floating-point number in scientific
** notation: -x.xxxxxx E-xx
*/
fltprint(n,num) int n,num[];
{
	int ten[4];		/* Holder for ten */
	char tmp[10];		/* Holder for one */
	int msign;		/* Mantissa sign */
	int powcnt;		/* Counter for power */
	int i;			/* Loop count */
	int j;			/* Counter */
	int hld[4];		/* Holding val */

/*
** Copy the number we want to print out into a holding area,
** since we'll be altering it to print it out.
*/

	for(i=0;i<4;++i)
		hld[i]=num[i];

/* First see what sign the number is.  Save in msign.
** If less than zero...change the sign of the number.
** If equal to zero, print it out and we can go home.
*/
	msign=fcompz(hld);
	if(msign==0) {
	  printf("0.0 E0");
	  return;
	}
	if(msign<0) {
		fload(hld);
		fabs();
		fstore(hld);
	}

/*
** Now see if the number is less than ten.  If so, multiply the number
** by ten repeatedly until it's not.  For each multiply, decrement a
** counter so we can keep track of the exponent.
*/
	powcnt=0;
	ten[0]=10;
	ten[1]=ten[2]=ten[3]=0;
	fint2float(ten);	/* Get floating-point 10 */
	while(fcomp(hld,ten)==-1) {
		--powcnt;
		f2mult(hld,ten,hld);
	}

/*
** Do the reverse of the above.  As long as the number is greater than or
** equal to 10, divide it by 10.  Increment the exponent counter for
** each multiply.
*/

	while(fcomp(hld,ten)!=-1) {
		++powcnt;
		f2div(hld,ten,hld);
	}

/*
** We now know the number is in the range 10 > n >= 1, and its
** exponent is in powcnt.  Print the number out as follows:
** Print out a minus sign if the number was less than 0.
** Multiply by 10 as needed to get the number of digits the
** caller wants printed to the left of the decimal point.
** Store the result in packed BCD format.
** Extract the BCD digits and print them out.
*/

	if(msign<0) printf("-");
	fload(hld);
	for(i=0;i<n-1;++i)
		fmult(ten);
	fstoreb(tmp);
	for(i=0;i<n;++i) {
		j=tmp[(n-i-1)/2];
		if(((n-i)&1)==0) j=j>>4;
		j=j&15;
		printf("%d",j);
		if(i==0) printf(".");
	}

/* Now print out exponent */

	printf("E%d",powcnt);
	return;
}


/*
** BYTE Hard disk seek benchmark
** Version 1 for 8088/8086/80286/80386
** March 1988
** Written in BYTE Small-C
** Based on Small-C by J.E. Hendrix
**
** Operation:
**  1. Determine geometry of disk drive (how many cylinders)
**  2. Perform COUNT iterations of the following (reporting
**     times for each):
**     (a) Adjacent track seeks for innermost cylinder
**     (b) Adjacent track seeks for outermost cylinder
**     (c) Half-platter seeks
**     (d) Full-platter seeks
**  3. Exit
**
*/

#define COUNT 100	/* Repititions */

int tblock[4];		/* Timer block */
int dblock[4];		/* Disk block */
int sourc;		/* Source cylinder */
int destc;		/* Destination cylinder */
int accum;		/* Time accumulator */
int i;			/* index */
char dbuff[516];	/* Buffer needed for disk read */

extern gtime();		/* Get time */
extern calctim();	/* Calc. elapsed time */

main()
{

	printf("BYTE Hard Disk Seek Benchmark\n");
	printf("All times given are in 1/100ths of a second\n");
/* Get the hard disk parameters */
	dblock[0]=128;	/* First hard disk */
	gdinfo(dblock);

	printf("Max cylinder %d\n\n",dblock[2]);

	printf("All tests done for %d iterations\n\n",COUNT*2);

/* Do outer track-track seek */
	sourc = 0;
	destc = 1;
	doseek();
	printf("***Outer track-track time***\n");
	printf("Total time:%d\n\n",accum);

/* Do inner track-track seek */
	sourc = dblock[2]-2;
	destc = dblock[2]-1;
	doseek();
	printf("***Inner track-track time***\n");
	printf("Total time:%d\n\n",accum);

/* Do half-platter seek */
	sourc = 0;
	destc = dblock[2]/2;
	doseek();
	printf("***Half-platter time***\n");
	printf("Total time:%d\n\n",accum);

/* Do full-platter seek */
	sourc = 0;
	destc = dblock[2] - 1;
	doseek();
	printf("***Full-platter time***\n");
	printf("Total time:%d\n\n",accum);

/* Exit */
	exit(0);
}

/*
** doseek()
** Uses global variables:
** sourc = starting cylinder
** destc = destination cylinder
** accum = accumulated time
*/
doseek()
{
	accum=0;
	hdseek(dbuff,sourc);	/* Init. track to source */
	for(i=0;i<COUNT;++i)
	{
		gtime(tblock);
		hdseek(dbuff,destc);
		hdseek(dbuff,sourc);
		calctim(tblock);	/* Get elapsed time */
		accum+=tblock[3]+100*(tblock[2]+60*tblock[1]);
	}
	return;
}

/*
** hdseek(buff,cyl) char *buff; int cyl;
** Hard disk seek to cylinder cyl
** Buff is a pointer to a buffer to read the sector in.
** NOTE: We assume ES=DS
** 
*/
hdseek(buff,cyl) char *buff; int cyl;
{
#asm
	MOV	BP,SP
	MOV	BX,4[BP]		;Pointer to buffer
	MOV	CX,2[BP]		;Get cylinder
	XCHG	CH,CL			;Swap parts
	ROR	CL,1			;Set up low part
	ROR	CL,1
	INC	CL			;Set to sector 1
	MOV	DX,80H			;Set hard disk no.
	MOV	AH,02H			;Function
	MOV	AL,1			;Read 1 sector
	INT	13H			;do the int
	XOR	CX,CX			;Just in case
#endasm
}
/*
** 8088/8086 version
** gdinfo(dblock) int dblock[]; {
** Get disk information.
** dblock[] is assumed to be a 4-element array.  Upon entry,
** dblock[0] should have the disk number in it
** (0=A, 1=B...80H=First hard disk...etc.)
** Upon return, dblock holds:
** dblock[0] = # of hard disks on first controller
** dblock[1] = Number of heads
** dblock[2] = Number of cylinders
** dblcok[3] = Sectors per track
*/
gdinfo(dblock) int dblock[]; {
#asm
	MOV	BP,SP
	MOV	SI,2[BP]	;Pointer to dblock
	MOV	DL,[SI]		;Get drive number
	MOV	AH,08H		;Get params
	INT	13H
	XOR	AX,AX		;Clear high byte
	MOV	AL,DL
	MOV	[SI],AX		;# Hard disks on 1st controller
	MOV	AL,DH
	INC	AL		;Starts at zero
	MOV	2[SI],AX	;Number of heads
	MOV	AL,CL
	AND	AL,3FH		;Sectors/track
	MOV	6[SI],AX
	MOV	AL,CL
	SHL	AX,1
	SHL	AX,1
	MOV	AL,CH
	INC	AX		;Starts at zero
	MOV	4[SI],AX	;Max cylinders
	XOR	CX,CX		;clear CX
#endasm
}

	

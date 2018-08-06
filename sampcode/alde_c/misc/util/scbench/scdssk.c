
/*
** BYTE DOS-level disk seek and read benchmark
** Version 1 for 8088/8086/80286/80386
** March 1988
** Written in BYTE Small-C
** Based on Small-C by J.E. Hendrix
**
** Operation:
** 1. Prompt operator for which disk device to test.
** 2. Determine maximum number of DOS sectors on device
** 3. Seek across DOS partition in tenths, reading 1 sector.
** 4. Report time for (3)
** 5. Seek across DOS partition in tenths, reading 8 sectors.
** 6. Report time for (5)
** 7. Exit 
*/
#define COUNT 50	/* # of iterations */
#define stdin 0		/* Standard in */
#define stdout 1	/* Standard out */

char *buff;		/* Buffer for sectors */
int tblock[4];		/* Timer block */
int ddblock[4];		/* DOS disk information block */
int drive;		/* Drive designation */
int sect;		/* Sector size */
int accum;		/* Time accumulator */
int tsects;		/* Total # of sectors */
int ssize;		/* Step size */
int i,j;		/* For loops */

main()
{

/* Announce program and determine drive to use */
	printf("BYTE DOS Disk Seek-And-Read Benchmark\n");
	printf("(Times reported are in 1/100ths of a second\n");
	printf("Which drive (1=A, 2=B ...)?");
	fscanf(stdin,"%d",&drive);

/* Get information on selected drive */
	ddblock[0]=drive; 	/*Load drive info to send to gddinfo*/
	gddinfo(ddblock);
	tsects=ddblock[3]*ddblock[0];	/* Total Clusters * Clusters/Sector */
	printf("Total sectors for this disk:%u\n",tsects);

/* Allocate memory for up to 8 sectors */
	buff=malloc(ddblock[2]*8);

/* First test...read across the disk in tenths reading 1 sector */
/* Assembler routine does an unsigned divide (tsects/10) to determine*/
/* step size (ssize)  */
#asm
	MOV	AX,_tsects
	XOR	DX,DX
	MOV	BX,10
	DIV	BX
	MOV	_ssize,AX
#endasm
	accum=0;		/* Initialize time counter */
	for(i=0;i<COUNT;++i)	/* Perform test COUNT times*/
	{
		ddseek(drive,0,1,buff);	/* Start at 0 */
		sect=0;
		for(j=0;j<10;++j)	/*Step through 10 steps */
		{
			sect+=ssize;
			gtime(tblock);	/* Start timer */
			ddseek(drive,sect,1,buff);
			calctim(tblock);	/* Stop timer */
			accum+=tblock[3]+100*(tblock[2]+
				60*tblock[1]);	/*Increment time counter */
		}
	}
	printf("Step 10th of total/ Read 1 sector\n");
	printf("# of Seeks Timed:%d\n",COUNT*10);
	printf("Total time:%d\n",accum);

/* Second test... Same as first test except data is read in eight sector
** blocks */
	accum=0;
	for(i=0;i<COUNT;++i)
	{
		ddseek(drive,0,1,buff);	/* Start at 0 */
		sect=0;
		for(j=0;j<10;++j)
		{
			sect+=ssize;
			/* Don't fall off the end */
			if(sect>(tsects-8)) sect=tsects-8;
			gtime(tblock);
			ddseek(drive,sect,8,buff);  /* Read eight sectors */
			calctim(tblock);
			accum+=tblock[3]+100*(tblock[2]+
				60*tblock[1]);
		}
	}
	printf("Step 10th of total/ Read 8 sectors\n");
	printf("# of Seeks Timed:%d\n",COUNT*10);
	printf("Total time:%d\n",accum);



/* Exit */
	exit(0);
}

/*
**
** gddinfo(ddblock) - Get DOS disk info
** ddblock is a 4-element array
** Entry: ddblock[0] holds drive # (0=A, 1=B...)
** Exit: ddblock[0] = sectors/cluster
**       ddblock[1] = available clusters
**       ddblock[2] = bytes per sector
**       ddblock[3] = total clusters
*/
gddinfo(ddblock) int ddblock[];
{
#asm
	MOV	BP,SP
	MOV	SI,2[BP]	;Pointer to ddblock[0]
	MOV	DL,[SI]		;Drive #
	MOV	AH,36H		;Function #
	PUSH	SI		;Save just in case
	INT	21H		;Get disk info
	POP	SI		;Recover ddblock pointer
	MOV	[SI],AX		;Sectors/cluster
	MOV	2[SI],BX	;Avail clusters
	MOV	4[SI],CX	;Bytes/sector
	MOV	6[SI],DX	;Total clusters
	XOR	CX,CX		;For Small-C
#endasm
}

/*
**
** ddseek(drive,sector,num,buff) int drive,sector,num; char *buff;
** DOS disk seek to sector on drive.  Read in num sectors into buff.
** NOTE: This routine decrements drive so that 0=A, 1=B..etc.
** 
*/
ddseek(drive,sector,num,buff) int drive,sector,num;
char *buff;
{
#asm
	MOV	BP,SP
	MOV	AL,8[BP]	;Get drive
	DEC	AL		;Decrement so in interrupt format
	MOV	DX,6[BP]	;Get sector #
	MOV	CX,4[BP]	;Number of sectors
	MOV	BX,2[BP]	;Buffer address
	INT	25H		;Read
	POP	DX		;Dump the extra word
	XOR	CX,CX		;For Small-C
#endasm
}

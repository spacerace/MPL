/*
** BYTE Small-C String Move Benchmark
** Version 1 for 8088/8086/80286
** March, 1988
** Written in BYTE Small-C
** Based on Small-C by J.E. Hendrix
**
** This benchmark moves strings from on location to another.
** The strings are moved byte-at-a-time and word-at-a-time.
**
** Operation:
** 1. Allocate source and destination buffers.
** 2. Turn on stopwatch.
** 3. Move contents of source buffer to destination buffer,
**    COUNT times, byte-at-a-time.
** 4. Turn off stopwatch.
** 5. Report results.
** 6. Modify source and destination buffer pointers to point
**    to odd byte locations.
** 7. Turn on stopwatch.
** 8. Move contents of source buffer to destination buffer,
**    COUNT times, word-at-a-time.
** 9. Turn off stopwatch.
** 10. Report results.
** 11. Modify source and destination buffer pointers to point
**     to even byte locations.
** 12. Turn on stopwatch.
** 13. Move contents of source buffer to destination buffer,
**     COUNT times, word-at-a-time.
** 14. Turn off stopwatch.
** 15. Report results.
** 16. Free allocated space and exit.
*/

#include stdio.h

#define BUFSIZE	10000		/* Buffer size */
#define COUNT	10000		/* Repetition count */

int tblock[4];			/* Timer block */
char *sbbuff;			/* Byte pointer to source buffer */
char *dbbuff;			/* Byte pointer to dest. buffer */
int *swbuff;			/* Integer pointer to source buffer */
int *dwbuff;			/* Integer pointer to dest. buffer */

main()
{

	/* Announce yourself */
	printf("BYTE Small-C String Move Benchmark\n");


	/* Now get source and destination buffers */
	sbbuff=malloc(BUFSIZE+4);
	if (sbbuff==NULL) {
		printf("Not enough memory for source buff.\n");
		exit(0);
	}
	swbuff=sbbuff;

	dbbuff=malloc(BUFSIZE+4);
	if (dbbuff==NULL) {
		printf("Not enough memory for dest. buff.\n");
		exit(0);
	}
	dwbuff=dbbuff;

	/* Do byte-wide move */
	printf("Byte-wide move---\n");
	printf("bytes:%d repetitions:%d\n",BUFSIZE,COUNT);

	gtime(tblock);
	lsmovb(sbbuff,dbbuff,BUFSIZE,COUNT);
	calctim(tblock);

	printf("Time: %d:%d:%d:%d (HH:MM:SS:1/100ths)\n\n\n",
	  tblock[0],tblock[1],tblock[2],tblock[3]);

	/* Do word-wide move */
	printf("Word-wide move---\n");

	/* Do first move on odd boundaries */
	if((swbuff&1)!=1) swbuff|=1;
	if((dwbuff&1)!=1) dwbuff|=1;

	printf("Odd byte boundary\n");
	printf("bytes:%d repetitions:%d\n",BUFSIZE,COUNT);

	gtime(tblock);
	lsmovw(swbuff,dwbuff,BUFSIZE/2,COUNT);
	calctim(tblock);

	printf("Time: %d:%d:%d:%d (HH:MM:SS:1/100ths)\n\n",
	  tblock[0],tblock[1],tblock[2],tblock[3]);

	/* Now do even boundaries */
	swbuff^=1;
	dwbuff^=1;

	printf("Even byte boundary\n");
	printf("bytes:%d repetitions:%d\n",BUFSIZE,COUNT);

	gtime(tblock);
	lsmovw(swbuff,dwbuff,BUFSIZE/2,COUNT);
	calctim(tblock);

	printf("Time: %d:%d:%d:%d (HH:MM:SS:1/100ths)\n\n",
	  tblock[0],tblock[1],tblock[2],tblock[3]);

	/* Free up space we claimed */
	free(dbbuff);
	free(sbbuff);

	/* Go home */
	exit(0);
}


/*
** lsmovb(src,dest,n,cnt)
** Moves n bytes from src to dest.  Repeats cnt time.
*/
lsmovb(src,dest,n,cnt)
char *src,*dest;
int n,cnt;
{
#asm
	MOV	BP,SP		;Get pointer to args
	CLD			;Clear direction
	MOV	DX,2[BP]	;Repeat count
LSMOVB1:
	MOV	CX,4[BP]	;Count
	MOV	DI,6[BP]	;Destination
	MOV	SI,8[BP]	;Source
	REP	MOVSB		;Do the move
	DEC	DX		;Decr. repeat count
	JNZ	LSMOVB1
	XOR	CX,CX		;For Small C
#endasm
}

/*
/*
** lsmovw(src,dest,n,cnt)
** Moves n words from src to dest.  Repeats cnt times.
*/
lsmovw(src,dest,n,cnt)
int *src,*dest;
int n,cnt;
{
#asm
	MOV	BP,SP		;Get pointer to args
	CLD			;Clear direction
	MOV	DX,2[BP]	;Repeat count
LSMOVW1:
	MOV	CX,4[BP]	;Count
	MOV	DI,6[BP]	;Destination
	MOV	SI,8[BP]	;Source
	REP	MOVSW		;Do the move
	DEC	DX		;Decr. repeat count
	JNZ	LSMOVW1
	XOR	CX,CX		;For Small C
#endasm
}


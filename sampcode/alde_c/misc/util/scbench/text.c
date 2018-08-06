/*
** BYTE Text Display Benchmark
** Version 1 for 8088/8086/80286/80386
** March 1988
** Written in BYTE Small-C
** Based on Small-C by J.E. Hendrix
**
** This program executes the following steps:
**  1. Saves the current video mode
**  2. Prompts the user for which text mode he wishes to
**     run the benchmark.
**  3. Calculates the screen width for the mode the user has
**     selected and constructs a string of the appropriate width.
**  4. Begins timing.
**  5. Switches to the new mode.
**  6. Writes COUNT strings to the screen.
**  7. Stops timing.
**  8. Switches to the original video mode.
**  9. Reports its results.
** 10. Exits.
**
** This program writes to the screen using Small-C's
** Umsdos call directed to stdout. (Int 21h function 40H)
*/

/*
** Note: COUNT should have 40 and 80 as common denominator.
*/

#define COUNT 8000	/* Number of chars */

#define stdin 0		/* Standard input */
#define stdout 1	/* Standard output */
#define WRITE 16384	/* MSDOS write function */

int tblock[4];	/* Timer block */
int vblock[3]; /* Array for video mode stuff */
char buffer[82];  /* Char array for string */
int lines;	/* Number of lines */

extern gtime();		/* Start stopwatch */
extern calctim();	/* Stop stopwatch */
extern gvmode();	/* Get video mode */
extern svmode();	/* Set video mode */

main() {
	int mode;	/* video mode */
	int len;	/* string length */
	int i;		/* For loop */
	int row,col,rowinc,colinc;  /* Row and column vars */
	char c;		/* Temporary holding char */
	
/* First get current video mode */
	gvmode(vblock);

/* Now prompt for mode to test */
	printf(" BYTE Text Video Benchmark\n");
	printf(" What video mode?");
	fscanf(stdin,"%d",&mode);

/* Select length */
	switch(mode) {
		case 0:
		case 1: len=40;
		         break;
		case 2:
		case 3:
		case 7: len=80;
		        break;
		default:
		   printf("Illegal mode\n");
		   exit(0);
	}

/* Build string */
	c='A';
	for (i=0;i<len;++i) {
		buffer[i]=c++;
		if(c>'Z') c='A';
	}

/* Set # of lines to print */
	lines = COUNT / len;

/* Initialize row/col outside of timing loop */
	row = col = 1;
	rowinc=1; colinc=1;

/* Begin timing */
	gtime(tblock);
/* Switch modes */
	svmode(mode);
/* Do the loop for writing stuff */
	for (i=0; i<lines; ++i)
		Umsdos(buffer,len,stdout,WRITE);
/* Do cursor positioning loop */
	for (i=0; i<COUNT; ++i)
	{
		curpos(row,col);
		Umsdos(&c,1,stdout,WRITE);
		if(row==25) rowinc=-1;
		if(row==0) rowinc=1;
		if(col==len) colinc=-1;
		if(col==0) colinc=1;
		row+=rowinc;
		col+=colinc;
		if(++c>'Z') c='A';
	}
/* Stop timing */
	calctim(tblock);
/* Switch mode back */
	mode=vblock[0];
	svmode(mode);
/* Display results */
	printf(" Elapsed time (HH:MM:SS:1/100ths):\n");
	printf(" %d:%d:%d:%d\n",tblock[0],tblock[1],tblock[2],
		tblock[3]);
	exit(0);
}

/*
** curpos(row,col) int row,col;
** position cursor to row/col using BIOS int 10h function
*/
curpos(row,col) int row,col;
{
#asm
	MOV	BP,SP
	MOV	DL,2[BP]	;column
	MOV	DH,4[BP]	;row
	MOV	AH,2		;Function
	INT	10H
	XOR	CX,CX
#endasm
}

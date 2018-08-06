/*
** BYTE Small-C Floating-Point Benchmark
** Version 1 for 8088/8086/80286
** March, 1988
** Written in BYTE Small-C
** Based on Small-C by J.E. Hendrix
**
** Operation:
** 1. Initialize the coprocessor.
** 2. Execute fourbang(), which:
**    a. Generates space for temporaries and constants.
**    b. Turns on stopwatch.
**    c. Executes loop FCOUNT times.  Loop consists of:
**       8 each of floating add, subtract, multiply, divide.
**    d. Turns off stopwatch.
**    e. Calculates time for an empty loop.
**    f. Reports time and result of operations.
** 3. Executes finteg(), which:
**    a. Generates space for temporaries and constants.
**    b. Turns on stopwatch.
**    c. Executes a trapezoidal integration method for sin(x)
**       from 0 to pi/2.
**    d. Turns off stopwatch.
**    e. Calculates time for an empty loop.
**    f. Reports time and result of operation.
**    g. Turns on stopwatch.
**    h. Executes a trapezoidal integration method for e^(x)
**       from 0 to 1.
**    i. Turns off stopwatch.
**    j. Calculates time for an empty loop.
**    k. Reports time and result of operation.
** 4. Exits.
**
** NOTE:
** Since Small-C does not support floating-point, we simply
** manipluate floating-point numbers as 4-element integer
** arrays.  The floating-point library functions handle the
** actual calls to the coprocessor, including the routine
** to print out a floating-point number.
**
** Expected results:
** For first test: 1.000000000E1  (10.0)
** For trapezoidal of sine(x): 1.000000000E0  (1.0)
** For trapezoidal of e^x: 1.718281828E0 (e-1)
*/

#include stdio.h

#define FCOUNT 20000	/* Number of times the four-banger test */
			/* is repeated.				*/
#define ICOUNT 32000	/* Stepsize for the integration test	*/

/* Timer holding variables */
int tblock[4];
int mtblock[4];		/* For empty loop timing */

main()
{
	/* Announce yourself */
	printf("BYTE Small-C Floating-Point Coprocessor Benchmark\n\n");
	
	/* Initialize the math coprocessor */
	finit();

	/* Do four-banger test */
	fourbang();

	/* Do integration */
	finteg();

	/* Go home */
	finit();

	printf("Press RETURN to exit:");
	fgetc(stdin);

	exit(0);
}

/*
** fourbang()
** Executes a loop of floating add, subtract, multiplies, and divides.
*/
fourbang()
{
	int ten[4];	/* Holder for 10 */
	int one[4];	/* Holder for one */
	int temp[4];	/* Temporary storage */
	int i;

	/* Announce yourself */
	printf("Basic Math Test (+,-,*,/)\n");

	/* First set up constants */
	ten[0]=10;
	ten[1]=ten[2]=ten[3]=0;
	fint2float(ten);
	fconst(1);
	fstore(one);

	/* Initialize temp location */
	fload(ten);
	fstore(temp);

	gtime(tblock);
	/* Do the operation */
	for(i=0;i<FCOUNT;++i)
	{
		f2add(temp,one,temp);
		f2sub(temp,one,temp);
		f2mult(temp,ten,temp);
		f2div(temp,ten,temp);
		f2sub(temp,one,temp);
		f2mult(temp,ten,temp);
		f2add(temp,ten,temp);
		f2div(temp,ten,temp);
		f2add(temp,one,temp);
		f2sub(temp,one,temp);
		f2mult(temp,ten,temp);
		f2div(temp,ten,temp);
		f2sub(temp,one,temp);
		f2mult(temp,ten,temp);
		f2add(temp,ten,temp);
		f2div(temp,ten,temp);
		f2add(temp,one,temp);
		f2sub(temp,one,temp);
		f2mult(temp,ten,temp);
		f2div(temp,ten,temp);
		f2sub(temp,one,temp);
		f2mult(temp,ten,temp);
		f2add(temp,ten,temp);
		f2div(temp,ten,temp);
		f2add(temp,one,temp);
		f2sub(temp,one,temp);
		f2mult(temp,ten,temp);
		f2div(temp,ten,temp);
		f2sub(temp,one,temp);
		f2mult(temp,ten,temp);
		f2add(temp,ten,temp);
		f2div(temp,ten,temp);
	}
	calctim(tblock);

	/* Now calculate an empty loop */
	gtime(mtblock);
	for(i=0;i<FCOUNT;++i) ;
	calctim(mtblock);

	/* Report results */
	printf("***Results: (All times are HH:MM:SS:1/100ths\n");

	printf("Total time: %d:%d:%d:%d\n",tblock[0],tblock[1],
		tblock[2],tblock[3]);

	printf("Empty loop time: %d:%d:%d:%d\n",mtblock[0],mtblock[1],
		mtblock[2],mtblock[3]);
	printf("Value:");
	fltprint(10,temp);
	printf("\n\n");

	return;
}

/*
** finteg()
** Do integration.
**
*/

finteg()
{

	int two[4];		/* Holder for two */
	int pitwo[4];		/* Holder for pi/2 */
	int temp[4];		/* Temp location */
	int sinex[4];		/* Sine value */
	int ex[4];		/* e^x value */
	int accum[4];		/* Accumulator */
	int x[4];		/* Holder for x */
	int i;

	/* Announce yourself */
	printf("Trapezoidal rule for sin(x) 0->x->pi/2 \n");

	/* Generate 2 */
	two[0]=2;
	two[1]=two[2]=two[3]=0;
	fint2float(two);

	/* Generate pi/2 */
	fgetpi2();		/* Get pi over two */
	fstore(pitwo);		/* Store pi over two */

	/* Generate stepsize */
	temp[0]=ICOUNT;
	temp[1]=temp[2]=temp[3]=0;
	fint2float(temp);
	f2div(pitwo,temp,temp);	/* Stepsize in temp */

	/* Clear accumulator */
	fconst(0);
	fstore(accum);

	/* Store x(0) */
	fconst(0);
	fstore(x);

/* Do trapezoidal rule for sine(x) */
	gtime(tblock);
	for(i=0;i<ICOUNT+1;++i)
	{ 
		fsin(x,sinex);		/* Get sinex */
		f2add(accum,sinex,accum);
		if((i!=0)&&(i!=ICOUNT))
		  f2add(accum,sinex,accum);
		f2add(x,temp,x);	/* Increment by step */
	}
	f2mult(accum,temp,accum);	/* Times stepsize */
	f2div(accum,two,accum);		/* Divided by 2 */

	/* Calculate time */
	calctim(tblock);

	/* Get time for an empty loop */
	gtime(mtblock);
	for(i=0;i<ICOUNT+1;++i) ;
	calctim(mtblock);

	/* Report results */
	printf("**** Results: (All times are HH:MM:SS:1/100ths)\n");

	printf("Total time:  %d:%d:%d:%d\n",tblock[0],tblock[1],
		tblock[2],tblock[3]);
	printf("Empty loop time: %d:%d:%d:%d\n",mtblock[0],mtblock[1],
		mtblock[2],mtblock[3]);

	printf("Value:");
	fltprint(10,accum);
	printf("\n\n");

/* Now do trapezoidal rule for e^x */

	printf("Trapezoidal rule for e^x  0->x->1\n");

	/* Generate stepsize */
	temp[0]=ICOUNT;
	temp[1]=temp[2]=temp[3]=0;
	fint2float(temp);
	fconst(1);

	fdiv(temp);
	fstore(temp);		/* Stepsize in temp */

	/* Clear accumulator */
	fconst(0);
	fstore(accum);

	/* Store x(0) */
	fconst(0);
	fstore(x);

	/* Do trapezoidal rule */
	gtime(tblock);
	for(i=0;i<ICOUNT+1;++i)
	{ 
		fex(x,ex);		/* Get sinex */
		f2add(accum,ex,accum);
		if((i!=0)&&(i!=ICOUNT))
		  f2add(accum,ex,accum);
		f2add(x,temp,x);	/* Increment by step */
	}
	f2mult(accum,temp,accum);	/* Times stepsize */
	f2div(accum,two,accum);		/* Divided by 2 */

	/* Calculate time */
	calctim(tblock);

	/* Get time for an empty loop */
	gtime(mtblock);
	for(i=0;i<ICOUNT+1;++i) ;
	calctim(mtblock);

	/* Report results */
	printf("**** Results: (All times are HH:MM:SS:1/100ths)\n");

	printf("Total time:  %d:%d:%d:%d\n",tblock[0],tblock[1],
		tblock[2],tblock[3]);
	printf("Empty loop time: %d:%d:%d:%d\n",mtblock[0],mtblock[1],
		mtblock[2],mtblock[3]);

	printf("Value:");
	fltprint(10,accum);
	printf("\n\n");
	
	/* Go home */
	return;
}

/*
** fgetpi2()
** Puts the value pi/2 on top of the floating-point stack.
*/
fgetpi2()
{
	int two[4];	/* Holder for 2 */

	two[0]=2;
	two[1]=two[2]=two[3]=0;
	fint2float(two);

	fconst(2);	/* Get pi on top of stack */
	fdiv(two);	/* pi/2 now on floating point stack top */
	return;
}

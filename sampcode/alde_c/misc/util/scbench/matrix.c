/*
** BYTE Small-C Matrix Benchmark
** Version 1 for 8088/8086/80286
** March, 1988
** Written in BYTE Small-C
** Based on Small-C by J.E. Hendrix
**
** NOTES: 
** 1) This benchmark constructs only square matrices.
** 2) Small C only supports 1 dimentional arrays.  The
**    the mathematics required to support 2-dimensional
**    arrays therefore appears at the C-code level.
** 3) The routine printmat() is provided at the end of the
**    program if you wish to print out the arrays and verify
**    that the matrix operations function properly.
**
** Operation:
** 1. Allocate space for 3 square matrices, each
**    MATSIZ x MATSIZ big.
** 2. Fill the first 2 matrices with random numbers.
** 3. Turn on the stopwatch
** 4. Add the first 2 matrices, result in third.
** 5. Turn off the stopwatch.
** 6. Add elapsed time to total time.
** 7. Fill the first 2 matrices with random numbers.
** 8. Turn on the stopwatch.
** 9. Multiply the first 2 matrices, result in third.
** 10. Turn off the stopwatch.
** 11. Add elapsed time to total time.
** 12. Turn on the stopwatch.
** 13. Transpose matrix 1, result in matrix 2.
** 14. Turn off the stopwatch.
** 15. Add elapsed time to total time.
** 16. Report results.
** 17. Calculate empty loop time.
** 18. Report results.
** 19. Free the space claimed by the matrices.
*/

#include stdio.h

#define MATSIZ	60		/* Matrix dimension */
#define BPINT	2		/* Bytes per integer */
#define CEIL	15		/* Largest number allowed */
int *mat1,*mat2,*mat3;		/* Pointers for matrices */
int tblock[4];			/* Timer block */
int ttblock[4];			/* Total time accumulator */
int seed;			/* Seed for random number generator */


main()
{
	int i;

	/* Announce yourself */
	printf("BYTE Small-C Matrix Benchmark\n");
	ttblock[0]=ttblock[1]=ttblock[2]=ttblock[3]=0;

	/* Set up the matrices */
	mat1=calloc(MATSIZ*MATSIZ,BPINT);
	if(mat1==NULL) {
	  printf("No space for matrix 1\n");
	  exit(0);
	}

	mat2=calloc(MATSIZ*MATSIZ,BPINT);
	if(mat2==NULL) 	{
	  printf("No space for matrix 2\n");
	  exit(0);
	}

	mat3=calloc(MATSIZ*MATSIZ,BPINT);
	if(mat3==NULL) {
	  printf("No space for matrix 3\n");
	  exit(0);
	}

	/* Now fill up the first two matrices */
	seed=0;
	for(i=0;i<MATSIZ*MATSIZ;++i) {
	  mat1[i]=randwc(CEIL);
	  mat2[i]=randwc(CEIL);
	}

	/* Add the two matrices */
	printf("Adding two matrices...\n");
	gtime(tblock);
	madd(mat1,mat2,mat3,MATSIZ,MATSIZ);
	calctim(tblock);
	acctime();

	/* Regenerate the matrices */
	seed=0;
	for(i=0;i<MATSIZ*MATSIZ;++i) {
	  mat1[i]=randwc(CEIL);
	  mat2[i]=randwc(CEIL);
	}

	/* Multiply the two matrices */
	printf("Multiplying two matrices...\n");
	gtime(tblock);
	mmult(mat1,mat2,mat3,MATSIZ,MATSIZ,MATSIZ,MATSIZ);
	calctim(tblock);
	acctime();

	/* Transpose matrix 1 into matrix 2 */
	printf("Transpose...\n\n");
	gtime(tblock);
	mtrans(mat1,mat2,MATSIZ,MATSIZ);
	calctim(tblock);
	acctime();

	/* Report total time */
	printf("Results:  (HH:MM:SS:1/100ths)\n");
	printf("Time: %d:%d:%d:%d\n",ttblock[0],ttblock[1],
	  ttblock[2],ttblock[3]);
	/* Calculate empty loop time */
	gtime(tblock);
	mtlptime(MATSIZ);
	calctim(tblock);
	printf("Empty loop: %d:%d:%d:%d\n\n",tblock[0],tblock[1],
	  tblock[2],tblock[3]);

	/* Free up the space you claimed */
	free(mat3);
	free(mat2);
	free(mat1);

	printf("Press RETURN to exit:");
	fgetc(stdin);

	exit(0);
}

/*
** madd(aray1,aray2,aray3,trows,tcols)
** Adds matrix aray1 to matrix aray2, stores result in aray3.
** trows and tcols are the total rows and colums of the arrays
*/
madd(aray1,aray2,aray3,trows,tcols)
int aray1[],aray2[],aray3[];
int trows,tcols;
{
	int i,j;

	for(i=0;i<trows;++i)
	  for(j=0;j<tcols;++j)
	    aray3[i*tcols+j]=aray1[i*tcols+j]+
	      aray2[i*tcols+j];
	return;
}

/*
** mmult(aray1,aray2,aray3,row1,col1,row2,col2)
** Multiplies aray1 by aray2, stores result in aray3.
** row1,col1 = dimension of aray1
** row2,col2 = dimension of aray2
*/
mmult(aray1,aray2,aray3,row1,col1,row2,col2)
int aray1[],aray2[],aray3[];
int row1,col1,row2,col2;
{
	int i,j,k;

	for(i=0;i<row1;++i)
	  for(j=0;j<col2;++j)
	  {  aray3[i*col1+j]=0;
	     for(k=0;k<col1;++k)
	       aray3[i*col1+j]+=aray1[i*col1+k]*
		                aray2[k*col2+j];
	  }
	  return;
}

/*
** mtrans(aray1,aray2,row,col)
** Perform the matrix transpose of aray1, store result in aray2.
** row,col are the dimensions of the matrix (must be square)
*/
mtrans(aray1,aray2,row,col)
int aray1[],aray2[];
int row,col;
{
	int i,j;

	for(i=0;i<col;++i)
	  for(j=0;j<row;++j)
	    aray2[i*col+j]=aray1[j*row+i];
	return;
}

/*
** randwc(x)
** Get a random number (positive) with ceiling x.
*/
randwc(x)
int x;
{

	if(x==0) return(0);
	seed=rand(seed);	/* Get a random number */
	return(seed%x);
}

/*
** Take the time interval stored in tblock[] and add that to
** the array holding total accumulated time (ttblock[]).
*/
acctime()
{
	ttblock[3]+=tblock[3];	/* Hundredths */
	if(ttblock[3]>=100) {
		ttblock[2]+=1;
		ttblock[3]-=100;
	}
	ttblock[2]+=tblock[2]; /* Seconds */
	if(ttblock[2]>=60) {
		ttblock[1]+=1;
		ttblock[2]-=60;
	}
	ttblock[1]+=tblock[1]; /* Minutes */
	if(ttblock[1]>=60) {
		ttblock[0]+=1;
		ttblock[1]-=60;
	}
	ttblock[0]+=tblock[0];	/* Hours */
	return;
}

/*
** mtlptime(msize)
** This routine executes a bunch of empty loops that
** are equivalent to the loops that occur inside the
** matrix add, multiply, and transpose functions.
** This function assumes the matrices are square.
*/
mtlptime(msize)
int msize;

{	int i,j,k;

	/* Empty loop for an add */
	for(i=0;i<msize;++i)
	  for(j=0;j<msize;++j) ;

	/* Empty loop for a multiply */
	for(i=0;i<msize;++i)
	  for(j=0;j<msize;++j)
	    for(k=0;k<msize;++k) ;

	/* Empty loop for a transpose */
	for(i=0;i<msize;++i)
	  for(j=0;j<msize;++j) ;

	return;
}

/*
** printmat(aray,rows,cols)
** Print out a matrix
** This routine was added so you can print out matrices after
** operations were performed on them to verify that the results
** are correct.
*/
printmat(aray,rows,cols)
int aray[],rows,cols;
{
	int i,j;

	for(i=0;i<rows;++i) {
	  for(j=0;j<cols;++j)
	    printf("%d ",aray[i*cols+j]);
	  printf("\n");
	}
	return;
}

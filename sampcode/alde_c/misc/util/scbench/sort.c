
/*
** BYTE Sort Benchmark
** Version 1 for 8088/8086/80286/80386
** Feb. 17, 1988
** Written in BYTE Small-C
** Based on Small-C by J.E. Hendrix
**
** Sorts integer array of random numbers using 3 methods:
** QUICKSORT
** SHELLSORT
** HEAPSORT
**
** The QUICKSORT and HEAPSORT algorithms used were adapted from
** examples given by G.H. Gonnet in his excellent HANDBOOK OF
** ALGORITHMS AND DATA STRUCTURES -- Addison Wesley
** ISBN 0-201-14218-X.
** The SHELLSORT algorithms was adapted from a version presented
** in Neill Graham's MICROPROCESSOR PROGRAMMING FOR COMPUTER
** HOBBYISTS, Tab Books, ISBN 0-8306-6952-3.
**
** The benchmark operates as follows:
** 1. Build an array of random integers.
** 2. Turn on the stopwatch.
** 3. Perform QUICKSORT
** 4. Turn off the stopwatch.
** 5. Accumulate total time.
** 6. Re-build array of random integers.
** 7. Turn on the stopwatch.
** 8. Perform SHELLSORT.
** 9. Turn off the stopwatch.
** 10. Add elapsed time to total.
** 11. Re-build array of random integers.
** 12. Turn on the stopwatch.
** 13. Perform HEAPSORT.
** 14. Turn off the stopwatch.
** 15. Add elapsed time to total.
** 16. Print out results.
**
*/

#include stdio.h		/* Standard I/O */
#define ASIZE 8000		/* Array size */

int tblock[4];			/* Timer block */
int tacc[4];			/* Timer accumulator */
int seed;			/* Random number seed */

main()
{
	int sarray[ASIZE];	/* Array to sort */

	/* Announce yourself */
	printf("BYTE Sort Benchmark\n\n");

	/* Build random array */
	seed=0;
	getrand(sarray);

	printf("--QUICKSORT\n");
	/* Turn on timer */
	gtime(tblock);

	/* QUICKSORT */
	qsort(sarray,0,ASIZE-1);

	/* Turn off timer and accumulate time */
	calctim(tblock);
	acctime();

	/* Build another random array */
	seed=0;
	getrand(sarray);

	printf("--SHELLSORT\n");
	/* Turn on timer */
	gtime(tblock);

	/* SHELLSORT */
	shsort(sarray,0,ASIZE-1);

	/* Turn off timer and accumulate time */
	calctim(tblock);
	acctime();

	/* Build another random array */
	seed=0;
	getrand(sarray);

	/* Turn on timer */
	gtime(tblock);

	printf("--HEAPSORT\n");
	/* HEAPSORT */
	hsort(sarray,0,ASIZE-1);

	/* Turn off timer and accumulate time */
	calctim(tblock);
	acctime();

	/* Report */

	printf("Total sorting time: (HH:MM:SS:1/100)\n");
	printf("%d:%d:%d:%d\n\n",tacc[0],tacc[1],tacc[2],tacc[3]);

	printf("Press RETURN for operating system:");
	fgetc(stdin);
	exit(0);

}

/* QUICKSORT 
** qsort(array,bot,top)
** int array[] - array to sort
** bot - Index to first element in array[]
** top - Index to last element in array[]
*/
qsort(aray,bot,top) int aray[],bot,top;
{
	int i,j,temp;

  while(bot<top) {

	/* Set ranges and choose partitioning element */
	i=bot;
	j=top;
	temp=aray[bot];

	/* Partition array */
	while(i<j) {
	  while(aray[j]>temp) j-=1;
	  aray[i]=aray[j];
	  while((i<j)&&(aray[i]<=temp)) i+=1;
	  aray[j]=aray[i];
	}
	aray[i]=temp;

	/* Call qsort recursively */
	qsort(aray,bot,i-1);
	bot=i+1;
  }
}

/*
** SHELLSHORT
** shsort(array,bot,top)
** int array[] - array to sort
** bot - Index to first element in array[]
** top - Index to last element in array[]
*/
shsort(aray,bot,top) int aray[],bot,top;
{
	int i,gap,nex,temp;

	
	gap=(top-bot)/2;	/* Set gap width */
	do {
	  do {
		nex=1;		/* No exchanges yet */
		for(i=0;i<=top-gap;++i )
		{  if(aray[i]>aray[i+gap])
		   {  temp=aray[i];
		      aray[i]=aray[i+gap];
		      aray[i+gap]=temp;
		      nex=0;	/* An exchange happened */
		   }
		}
	  } while(nex==0);
	  gap=gap/2;
	} while(gap!=0);

}

/*
** HEAPSORT
** hsort(aray,bot,top)
** int array[] - array to sort
** bot - Index to first element in array[]
** top - Index to last element in array[]
*/
hsort(aray,bot,top) int aray[],bot,top;
{
	int i,temp;

	/* First...make a heap */
	for(i=(top/2);i>1;--i)
		sift(aray,i,top);
	/* Extract maximum */
	for(i=top;i>1;--i) {
	  sift(aray,0,i);
	  temp=aray[0];
	  aray[0]=aray[i];
	  aray[i]=temp;
	}
}

sift(aray,i,j) int aray[],i,j;
{
	int k,temp;

	while((2*i)<=j) {
	  k=2*i;
	  if(k<j)
	    if(aray[k]<aray[k+1]) ++k;

	  if(aray[i]<aray[k]) {
	    temp=aray[k];
	    aray[k]=aray[i];
	    aray[i]=temp;
	    i=k;
	  }
	  else i=j+1;
	}
	return;
}

/*
** Getrand()
** Builds an array of random numbers.
*/
getrand(array) int array[];
{
	int i;

	for(i=0;i<ASIZE;++i) {
		seed=rand(seed);
		array[i]=seed;
	}
}

/*
** acctime()
** Accumulate time stored in tblock[] into tacc[]
*/
acctime()
{
	tacc[3]+=tblock[3];	/* Hundredths */
	if(tacc[3]>=100) {
		tacc[2]+=1;
		tacc[3]-=100;
	}
	tacc[2]+=tblock[2]; /* Seconds */
	if(tacc[2]>=60) {
		tacc[1]+=1;
		tacc[2]-=60;
	}
	tacc[1]+=tblock[1]; /* Minutes */
	if(tacc[1]>=60) {
		tacc[0]+=1;
		tacc[1]-=60;
	}
	tacc[0]+=tblock[0];	/* Hours */
	return;
}


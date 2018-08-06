/*
** BYTE File I/O Benchmark
** Version 1 for 8088/8086/80286/80386
** March 1988
** Written in BYTE Small-C
** Based on Small-C by J.E. Hendrix
**
** Operation:
** 1. Create N files of predetermined size
** 2. Extend those N files a set amount
** 3. Enter a loop, executing:
**    (a) 3 reads
**    (b) 1 write
**    Each read/write operation is performed on a random
**    number of bytes.
** 4. Report:
**    (a) Accumulated seek time
**    (b) Accumulated read time and total bytes read
**    (c) Accumulated write time and total bytes written.
** 5. Exit
**
*/

#include stdio.h

int tblock[4];		/* Timer block */
int rtblock[4];		/* Timer block for read time */
int wtblock[4];		/* Timer block for write time */
int stblock[4];		/* Timer block for seek time */

int twbytes[2];		/* Total bytes written */
int trbytes[2];		/* Total bytes read */

int seed;		/* Seed for random number generator */

#define BUFSIZ 8000
char *tstring;		/* Pointer to string to write */
char *rbuff;		/* Buffer for reading */

/*
** COUNT defines the number of times the read-read-read-write
** loop is performed.
*/

#define COUNT 120

main()

{
	int i;

	/* Init the seed */
	seed=0;

	/* Announce yourself */
	printf("BYTE File I/O Benchmark\n\n");

	/* First build tstring */

	tstring=malloc(BUFSIZ);		/* Get memory */
	if(tstring==NULL) {
	  printf("No memory for tstring\n");
	  exit(0);
	}
	for(i=0;i<BUFSIZ;++i)
		tstring[i]='A';		/* Load it up */

	rbuff=malloc(BUFSIZ);		/* Get memory */
	if(rbuff==NULL) {
	  printf("No memory for rbuff\n");
	  exit(0);
	}

	/* Make sure all accumulators are empty */

	for(i=0;i<4;++i)
		rtblock[i]=stblock[i]=wtblock[i]=0;

	twbytes[0]=twbytes[1]=0;
	trbytes[0]=trbytes[1]=0;

	/* Create the files */
	printf("...Creating files\n");
	for(i=0;i<10;++i)
		kreate(i);

	/* Extend them */
	printf("...Extending files\n");
	for(i=0;i<10;++i)
		appnd(i);

	/* Do the random I/O stuff */
	printf("...Performing random reads/writes\n");
	dorands();

	/* Tell user what happened */
	report();

	exit(0);
}

/*
** kreate(n)
** Create file n.  Write the assigned number of bytes.
** Close the file.
** This routine accumulates the number of bytes written
** and the time to write them.
*/
kreate(n) int n;
{
	int fd;		/* File handle */
	int nb;		/* Number of bytes to write */

	switch(n) {

	  case 0: 
		  nb=4000;
	 	  break;
	  case 1: 
		  nb=10000;
		  break;
	  case 2: 
		  nb=500;
		  break;
	  case 3: 
		  nb=2800;
		  break;
	  case 4: 
		  nb=25000;
		  break;
	  case 5: 
		  nb=14000;
		  break;
	  case 6: 
		  nb=8000;
		  break;
	  case 7: 
		  nb=8800;
		  break;
	  case 8: 
		  nb=300;
		  break;
	  case 9: 
		  nb=21111;
		  break;
	}

	fd=fopen(gfname(n),"w");	/* Open the file */
	accwbytes(nb);		/* Accumulate total bytes written */
	while(nb!=0) {
	  if(nb>4000) n=4000; else n=nb;
	  gtime(tblock);		/* Turn on timer */
	  write(fd,tstring,n);
	  calctim(tblock);	/* Turn off the timer */
    	  accwtime();		/* Accumulate total write time */
	  nb-=n;
	}
	fclose(fd);		/* Close the file */
	return;
}
/*
** appnd(n)
** Open nth file.  Append the appropriate number of bytes
*/
appnd(n)
int n;
{

	int fd;		/* File handle */
	int nb;		/* Number of bytes */

	switch(n) {

	  case 0:  nb=12000;
		   break;
	  case 1:  nb=20300;
		   break;
	  case 2:  nb=31111;
		   break;
	  case 3:  nb=3400;
		   break;
	  case 4:  nb=9099;
		   break;
	  case 5:  nb=20755;
		   break;
	  case 6:  nb=7000;
		   break;
	  case 7:  nb=400;
		   break;
	  case 8:  nb=22000;
		   break;
	  case 9:  nb=27000;
		   break;
	}

	fd=fopen(gfname(n),"a");	/* Open in append mode */
	accwbytes(nb);		/* Accumulate total bytes written */
	while(nb!=0) {
	  if(nb>4000) n=4000; else n=nb;
	  gtime(tblock);		/* Turn on timer */
	  write(fd,tstring,n);
	  calctim(tblock);	/* Turn off the timer */
    	  accwtime();		/* Accumulate total write time */
	  nb-=n;
	}
	fclose(fd);
	return;
}

/*
** dorands()
** Do the random reads and writes.  Accumulating as you go.
**
*/
dorands()
{
	int i,j,nb;
	int fd,offsetl,offseth;

/* Do the whole thing COUNT times */

	for(i=0;i<COUNT;++i)
	{

	  /* For each COUNT, do 3 reads, 1 write */
	  for(j=0;j<3;++j)
	  {
		fd=fopen(gfname(randwc(10)),"r");
		seek(fd,0,0,2);
		tell(fd,&offsetl,&offseth);	/* Get file len */
		if(offseth!=0) offseth=randwc(offseth);
		if(offsetl<0)
			offsetl=seed=rand(seed);
		else
			offsetl=randwc(offsetl);
		gtime(tblock);
		seek(fd,offsetl,offseth,0);
		calctim(tblock);
		accstime();		/* Accumulate seek time */
		nb=randwc(8000);	/* Bytes to read */
		gtime(tblock);
		nb=read(fd,rbuff,nb);
		calctim(tblock);
		if(nb==ERR) {
		  printf("Error reading\n");
		  exit(0);
		}
		accrbytes(nb);		/* Accumulate bytes read */
		accrtime();
		fclose(fd);
	}
		fd=fopen(gfname(randwc(10)),"u");
		seek(fd,0,0,2);
		tell(fd,&offsetl,&offseth);	/* Get file len */
		if(offseth!=0) offseth=randwc(offseth);
		if(offsetl<0)
			offsetl=seed=rand(seed);
		else
			offsetl=randwc(offsetl);
		gtime(tblock);
		seek(fd,offsetl,offseth,0);
		calctim(tblock);
		accstime();		/* Accumulate seek time */
		nb=randwc(8000);	/* Bytes to write */
		gtime(tblock);
		nb=write(fd,tstring,nb);
		calctim(tblock);
		if(nb==ERR) {
		  printf("Error writing\n");
		  exit(0);
		}
		accwbytes(nb);		/* Accumulate bytes written */
		accwtime();
		fclose(fd);
	}
	return;
}

/*
** report()
** Report results
*/
report()
{

	printf("***Results:  (All times are HH:MM:SS:1/100)\n\n");

	printf(" Total seek time: %d:%d:%d:%d\n\n",stblock[0],
		 stblock[1],stblock[2],stblock[3]);

	printf(" Total read time: %d:%d:%d:%d\n\n",rtblock[0],
		 rtblock[1],rtblock[2],rtblock[3]);
	printf("            bytes:");

	writebig(trbytes);
	printf("\n\n");

	printf(" Total write time: %d:%d:%d:%d\n\n",wtblock[0],
		 wtblock[1],wtblock[2],wtblock[3]);
	printf("             bytes:");

	writebig(twbytes);
	printf("\n\n");

	printf("P.S. Don't forget to delete the files.\n");

}


/*
** Take the time interval stored in tblock[] and add that to
** the array holding total accumulated write time (wtblock[]).
*/
accwtime()
{
	wtblock[3]+=tblock[3];	/* Hundredths */
	if(wtblock[3]>=100) {
		wtblock[2]+=1;
		wtblock[3]-=100;
	}
	wtblock[2]+=tblock[2]; /* Seconds */
	if(wtblock[2]>=60) {
		wtblock[1]+=1;
		wtblock[2]-=60;
	}
	wtblock[1]+=tblock[1]; /* Minutes */
	if(wtblock[1]>=60) {
		wtblock[0]+=1;
		wtblock[1]-=60;
	}
	wtblock[0]+=tblock[0];	/* Hours */
	return;
}

/*
** Take the time interval stored in tblock[] and add that to
** the array holding total accumulated read time (rtblock[]).
*/
accrtime()
{
	rtblock[3]+=tblock[3];	/* Hundredths */
	if(rtblock[3]>=100) {
		rtblock[2]+=1;
		rtblock[3]-=100;
	}
	rtblock[2]+=tblock[2]; /* Seconds */
	if(rtblock[2]>=60) {
		rtblock[1]+=1;
		rtblock[2]-=60;
	}
	rtblock[1]+=tblock[1]; /* Minutes */
	if(rtblock[1]>=60) {
		rtblock[0]+=1;
		rtblock[1]-=60;
	}
	rtblock[0]+=tblock[0];	/* Hours */
	return;
}

/*
** Take the time interval stored in tblock[] and add that to
** the array holding total accumulated seek time (rtblock[]).
*/
accstime()
{
	stblock[3]+=tblock[3];	/* Hundredths */
	if(stblock[3]>=100) {
		stblock[2]+=1;
		stblock[3]-=100;
	}
	stblock[2]+=tblock[2]; /* Seconds */
	if(stblock[2]>=60) {
		stblock[1]+=1;
		stblock[2]-=60;
	}
	stblock[1]+=tblock[1]; /* Minutes */
	if(stblock[1]>=60) {
		stblock[0]+=1;
		stblock[1]-=60;
	}
	stblock[0]+=tblock[0];	/* Hours */
	return;
}

/*
** accwbytes(n)
** Add n to total accumulated bytes written array.
*/
accwbytes(n)
int n;
{
	while(n>10000) {
		twbytes[0]+=1;
		n-=10000;
	}
	twbytes[1]+=n;
	if(twbytes[1]>=10000) {
		twbytes[0]+=1;
		twbytes[1]-=10000;
	}
	return;
}

/*
** accrbytes(n)
** Add n to total accumulated bytes read array.
*/
accrbytes(n)
int n;
{
	while(n>10000) {
		trbytes[0]+=1;
		n-=10000;
	}
	trbytes[1]+=n;
	if(trbytes[1]>=10000) {
		trbytes[0]+=1;
		trbytes[1]-=10000;
	}
	return;
}

/*
** gfname(n)
** Get the nth file's filename (returned as pointer).
** Note that n is 0-based.
*/
gfname(n)
int n;
{
	switch(n) {
	  case 0: return("FILE01.DAT");
	  case 1: return("FILE02.DAT");
	  case 2: return("FILE03.DAT");
	  case 3: return("FILE04.DAT");
	  case 4: return("FILE05.DAT");
	  case 5: return("FILE06.DAT");
	  case 6: return("FILE07.DAT");
	  case 7: return("FILE08.DAT");
	  case 8: return("FILE09.DAT");
	  case 9: return("FILE10.DAT");
	}
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
	return(abs(seed)%x);
}

/*
** writebig(block)
** Write a big number.  number=10000*block[0]+block[1]
*/
writebig(block) int block[];
{
	printf("%d",block[0]);
	if(block[1]>999) printf("%u",block[1]);
	else {
	 printf("0");
	 if(block[1]>99) printf("%u",block[1]);
	 else {
	  printf("0");
	  if(block[1]>9) printf("%u",block[1]);
	  else {
	   printf("0%u",block[1]);
	  }
	 }
	}
  return;
}

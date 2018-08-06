/*
** BYTE Small-C  Disk I/O benchmark
** Version 1 for 8088/8086/80286/80386
** Feb. 17, 1988
** Written in BYTE Small-C
** Based on Small-C by J.E. Hendrix
**
** Test times the writing of a 1 million byte file
** Requires support routines gtime and calctim.
*/

#include stdio.h

#define BUFSIZ 10000	/* Size of write buffer */
#define LOOPS  100	/* number of times to write BUFSIZ characters to get
			   1000000 byte file. */
char *wbuff;		/* Pointer to write buffer */
int tblock[4];		/* Timer block */

main()
{
	int i;
	int fd;		/* File handle */

	/* Announce program */
	printf("BYTE Large File Write Benchmark\n\n");

	/* Allocate space for the write if possible*/
	wbuff=malloc(BUFSIZ);
	if(wbuff==NULL) {
		printf("Not enough memory for wbuff\n");
		exit(0);
	}

	/* Fill up the buffer */
	for(i=0;i<BUFSIZ;++i)
		wbuff[i]='A';

	/* Open the file */
	fd=fopen("BIGFILE.DAT","w");

	/* Turn on the timer */
	gtime(tblock);

	/* Loop and write the file */
	for(i=0;i<LOOPS;++i)
		write(fd,wbuff,BUFSIZ);

	/* Turn off timer and close file */
	calctim(tblock);
	fclose(fd);

	/* Report results */
	printf("Results: (HH:MM:SS:1/100ths)\n");
	printf("Time to write file: %d:%d:%d:%d\n",tblock[0],tblock[1],
	 tblock[2],tblock[3]);

	exit(0);

}
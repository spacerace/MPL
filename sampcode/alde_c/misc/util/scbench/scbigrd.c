/*
** BYTE Large File Read Benchmark
** Version 1 for 8088/8086/80286/80386
** Feb. 17, 1988
** Written in BYTE Small-C
** Based on Small-C by J.E. Hendrix
**
** Routine times reading of a 1000000 byte file.
**
*/

#include stdio.h

#define BUFSIZ 10000	/* Size of read buffer */
#define LOOPS  100	/* Number of times to read BUFSIZ characters
			   to get 1000000 bytes read. */
char *rbuff;		/* Pointer to read buffer */
int tblock[4];		/* Timer block */

main()
{
	int i;
	int fd;

	/* Announce program */
	printf("BYTE Large File Read Benchmark\n\n");

	/* Now allocate (if you can) space for the read */
	rbuff=malloc(BUFSIZ);
	if(rbuff==NULL) {
		printf("Not enough memory for rbuff\n");
		exit(0);
	}

	/* Open the file */
	fd=fopen("BIGFILE.DAT","r");

	/* Turn on the timer */
	gtime(tblock);

	/* Loop and read the file */
	for(i=0;i<LOOPS;++i)
		read(fd,rbuff,BUFSIZ);

	/* Turn off timer and close file */
	calctim(tblock);
	fclose(fd);

	/* Report results */
	printf("Results: (HH:MM:SS:1/100ths)\n");
	printf("Time to read file: %d:%d:%d:%d\n",tblock[0],tblock[1],
	 tblock[2],tblock[3]);

	exit(0);
}


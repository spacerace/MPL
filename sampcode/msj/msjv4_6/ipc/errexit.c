/* errexit.c RHS 7/15/89
 *
 */
#define INCL_DOS
#include<os2.h>
#include<stdio.h>

#include"errexit.h"

void error_exit(USHORT err, char *msg)
	{
	printf("Error %u returned from %s\n",err,msg);
	DosExit(EXIT_PROCESS,0);
	}



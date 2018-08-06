/* nprmode.c RHS 5/1/89
 *
 *
 This program opens the named pipe created by NPSERVER from the DOS
 compatbility environment. After starting NPSERVER in a protected mode
 session, you can bring the DOS box session into the foreground and run
 this program with:

 NPRMODE

 */
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<io.h>
#include<share.h>
#include<sys\types.h>
#include<sys\stat.h>

#include"nmpipe.h"

#if !defined(TRUE)
#define TRUE 1
#endif

char message[80];

void main(void);

void main(void)
	{
	char *mess = "Message from the DOS box!";
	int	pipehandle, err, count = strlen(mess);

	if((pipehandle = sopen(NAMEDPIPE,(O_BINARY | O_RDWR),SH_DENYNO)) == -1)
		{
		printf("sopen failed, errno = %d\n",errno);
		exit(0);
		}

	while(TRUE)
		{
		strcpy(message,mess);
			
		if((err = write(pipehandle,message,count)) == -1)
			{
			printf("write failed, errno = %d\n",errno);
			exit(0);
			}

		count = 79;
		if((err = read(pipehandle,message,count)) == -1)
			{
			printf("read failed, errno = %d\n",errno);
			exit(0);
			}
			
		if(err)
			{
			message[err] = NULL;
			printf("Message received from the protected world: \"%s\"\n",message);
			}
		else
			printf("No message from protected world\n");

		}

	close(pipehandle);
	exit(0);
	}


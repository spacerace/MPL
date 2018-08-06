

/*****************************************************************
**								**
**	  Inference -- (C) Copyright 1985 George Hageman	**
**								**
**	    user-supported software:				**
**								**
**		    George Hageman				**
**		    P.O. Box 11234				**
**		    Boulder, Colorado 80302			**
**								**
*****************************************************************/

/*****************************************************************
**
**	runRoutine(antecedent)
**
**	spawns the process with the path name specified in
**
**	ruleBuff[antecedent].string
**
**	and returns the value as TRUE or FALSE  depending on
**	the value returned from the exit() command from the spawnd routine.
**
**	the routine returns TRUE if there is any problem with spawning
**	the specified executable.
**
******************************************************************/
#define	MAX_ARGS	20

#include <stdio.h>

#ifdef MSDOS
#include <process.h>
#endif

#include <errno.h>

#include "expert.h"
#include "inference.h"
#include "routine.h"

int	runRoutine(cnsquent)
	int	cnsquent ;
{
extern int errno ;

#ifdef UNIXSV
int	pnid ;
#endif

int	i,argc,p_value,numChars ;
char	buffer[MAX_STR_LEN], *string_p;
char	*argv[MAX_ARGS] ;

/*
** copy string to buffer to get parameters
*/

string_p = &strBuff[ruleBuff[cnsquent].string] ;

for (numChars = 0 ; numChars < MAX_STR_LEN ; numChars++)
	{
	buffer[numChars]= *( string_p + numChars ) ; 
	if(buffer[numChars] == NULL)
		break ;
	}
	
#ifdef DEBUG
printf("\nDEBUG -- runroutine -- copied string is %s\n",buffer) ;
#endif

/*
** set the argv(alues) to the proper location within the buffer
*/

argc = 1 ;
argv[0] = buffer ;

for(i=0;i<numChars;i++)
	{
#ifdef DEBUG
printf("\n DEBUG -- runroutine parameters i = %d, char = %c \n",i,buffer[i]) ;
#endif
	if(buffer[i] == NULL)
		{
		break ;
		}
	if(buffer[i] == BLANK)
		{
		buffer[i] = NULL ;
		while( buffer[++i] == BLANK ) ;
		if(buffer[i] == NULL)
			{
			break ;
			}
		argv[argc++] = &buffer[i] ;
		if(argc == MAX_ARGS)
			{
			printf("\n maximum arguments exceeded for %s\n",string_p);
			argc -= 1 ;
			break ;
			}
		}
	}
argv[argc]=NULL ;
#ifdef DEBUG
for( i = 0 ; i < argc ; i++)
	{
	printf("\n argv[%d] = %s\n",i,argv[i]) ;
	}

printf("\nRunning Routine %s ",argv[0] ) ;
#endif

#ifdef MSDOS
p_value=spawnv(P_WAIT,argv[0],argv ) ;
#endif


#ifdef UNIXSV
pnid = fork() ;
if(pnid == -1)
	{
	printf("\nFork failure! Running %s -- returning TRUE \n",argv[0]) ;
	return(TRUE) ;
	}
if(pnid != 0 ) 		/* parent */
	{
	wait(&p_value) ;
	if(p_value != -1)
		p_value = (p_value >> 8 ) & 0x0ff ;
	}
else			/* child */
	{
	execv(argv[0],argv) ;
	}
#endif

/*
**	The return value is set by the routine having an exit(X)  where
**	X is the value to be returned...
**	There is a problem since the return value can also provide an
**	indication that there was some problem with the attempt as follows:
*/

if(p_value == RETURN_ROUTINE_TRUE)
	{
#ifdef DEBUG
	printf(" -- TRUE\n") ;
#endif
	return(TRUE) ;
	}
if(p_value == RETURN_ROUTINE_FALSE)
	{
#ifdef DEBUG
	printf(" -- FALSE \n") ;
#endif
	return(FALSE) ;
	}
if(p_value)
	switch(errno)
		{
		case ENOENT :
			printf("\n Executable file %s not found assumed TRUE\n",argv[0]) ;
			return(TRUE) ;
		case ENOEXEC :
			printf("\n File %s is not executable assumed TRUE\n",argv[0]) ;
			return(TRUE) ;
		case ENOMEM :
			printf("\ Not enough memory to run -- assumed TRUE\n") ;
			return(TRUE) ;
		}	
printf("\n Routine did not return either ROUTINE_TRUE or ROUTINE_FALSE assumed TRUE\n") ;
return(TRUE) ;
}


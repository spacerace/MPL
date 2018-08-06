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

/*******************************************************************
**
**	inference engine main routine
**
*******************************************************************/

#include <stdio.h>
#include <string.h>

#include "expert.h"
#include "routine.h"

#define	MAX_KNOWN	100

int	numHypot, hypStack[MAX_HYPS],strBuffOfst ;
char	strBuff[MAX_STRING_BUFF] ;
int	ruleBuffOfst ;
int	knownTrue[MAX_KNOWN], knownFalse[MAX_KNOWN] ;
int	numTrue, numFalse ;
struct  rule_statement_r ruleBuff[MAX_RULE_STATEMENTS] ;

int	main(argc,argv)
int argc ;
char **argv ;

{
int	i,consequent ;
int	proved ;
int	p_value ;
FILE	*infile ;

for( proved = 0; proved < MAX_STRING_BUFF; proved ++ )
	strBuff[proved] = 0 ;

proved = FALSE ;

#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP argc=%d ",argc) ;
#endif

if(argc != 2 )
	{
	fprintf(stdout, "\n\n Usage: inference in.file \n " ) ;
	exit(RETURN_ROUTINE_FALSE) ;
	}
infile = fopen( argv[1], "rb" );
if(infile == NULL)
	{
	fprintf(stdout,"\n\n Cannot open input file %s \n ", argv[1]);
	exit(RETURN_ROUTINE_FALSE) ;
	}
for(i=0;i<MAX_KNOWN;i++)
	knownTrue[i]=knownFalse[i]=0 ;
/*
**
**	read in the compiled rules
**
*/

fread(&numHypot,sizeof(int),1,infile) ;
fread(hypStack,sizeof(int),numHypot,infile) ;
fread(&ruleBuffOfst,sizeof(int),1,infile) ;
fread(ruleBuff,2*sizeof(int),ruleBuffOfst,infile) ;
fread(&strBuffOfst,sizeof(int),1,infile) ;
fread(strBuff,1,strBuffOfst,infile) ;

#ifdef	DEBUG
	
printf("\nDEBUG-main, numHypot = %d",numHypot) ;
for(i=0;i<numHypot;i++)
	{
	printf("\nDEBUG-main, consequent[%d]= %d, string=%s",i,hypStack[i],
			&strBuff[(ruleBuff[hypStack[i]].string)] ) ;
	}
printf("\nDEBUG-main, number of rules = %d",ruleBuffOfst) ;
printf("\nDEBUG-main, number of bytes in stringbuffer=%d",strBuffOfst) ;
for(i=0;i<ruleBuffOfst;i++)
	{
	if(ruleBuff[i].flag !=0)
		{
		printf("\nDEBUG-main, rule[%d].flag=%d, rule[%d].string= %d->%s"
        		  ,i,ruleBuff[i].flag,i,ruleBuff[i].string,&strBuff[ruleBuff[i].string]) ;
		}
	}
printf("\n") ;

#endif

for( i=0 ; i < numHypot ; i++ )

	{
	consequent = hypStack[i] ;

#ifdef DEBUG
	printf("\nDEBUG-main, consequent = %d ",consequent ) ;
#endif

	if(weKnow(consequent,&p_value) == TRUE)
		{
#ifdef DEBUG
	printf("\nDEBUG-main, we knew this consequent was ") ;
	if(p_value == TRUE)
		printf("-- TRUE") ;
	else
		printf("-- FALSE") ;
#endif
		continue ;
		}
		
	if ( verify(consequent) == TRUE )
		{
		
#ifdef DEBUG
			printf("\nDEBUG-main, consequent verified TRUE " ) ;
#endif
		
		if( (ruleBuff[consequent].flag == ROUTINE_TRUE) ||
			(ruleBuff[consequent].flag == ROUTINE_TRUE_HYP) )
			{
			if(weKnow(consequent,&p_value) == TRUE)
				continue ;
#ifdef DEBUG
			printf("\nRuning Routine %s",&strBuff[ruleBuff[consequent].string]) ;
#endif
			if (runRoutine(consequent) == TRUE)
				{
				knownTrue[numTrue++]=ruleBuff[consequent].string ;
#ifdef DEBUG
				printf(" -- TRUE\n") ;
#endif
				if(ruleBuff[consequent].flag == ROUTINE_TRUE_HYP)
					{
					printf("\nCONCLUSION\n") ;
					exit(RETURN_ROUTINE_TRUE) ;
					}
				}
			else
				{
				knownFalse[numFalse++]=ruleBuff[consequent].string ;
#ifdef DEBUG
				printf(" -- FALSE\n") ;
#endif
				}
			}
		else
			{
			knownTrue[numTrue++]=ruleBuff[consequent].string ;
			proved = TRUE ;
			printf("\nI infer that : %s\n",&strBuff[ruleBuff[consequent].string]) ;
			if(ruleBuff[consequent].flag == STRING_TRUE_HYP)
				{
				printf("\nCONCLUSION\n") ;
				exit(RETURN_ROUTINE_TRUE) ;
				}
			}
		}
	else
		{
#ifdef DEBUG
			printf("\nDEBUG-main, consequent not proved " ) ;
#endif
		}
	}
if(proved == FALSE)
	{
	printf("\n I can't prove anything\n") ;
	exit(RETURN_ROUTINE_FALSE) ;
	}
exit(RETURN_ROUTINE_TRUE) ;
}

/* --<<hidden tof */

/******************************************************
**
**	known(hypot,knownFile,numKnown)
**
**	checks to see if hypot is in the stack knownFile of length
**		numKnown
**
**	returns true if known, false otherwise
**
******************************************************/



known(hypot,knownFile,numKnown)
	int 	hypot,numKnown ;
	int	knownFile[] ;
{
int i ;
for (i = 0 ; i < numKnown ; i++ )
	if (ruleBuff[hypot].string == knownFile[i] )
		return(TRUE) ;
return(FALSE) ;
}



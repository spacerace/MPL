
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
**	Expert system rule compiler
**
**	This compiler produces a file of the form:
**
**	Number_of_bytes_in_hypstack_section
**	Hypstack_section
**	Number_of_bytes_in_rule_section
**	Rule base section
**	Number_of_bytes_in_string_section
**	String section
**	EOF
**	
**	The rule section consits of integer flags and long-integer
**	pointers to strings in the string section.
**	
**	All strings in the string section will be unique nomatter how
**	many times they are repeated in the rules which are compiled
**	into the rule base by this compiler.
**
**
**	Usage:
**
**	rulecomp rule.file object.file
**
*****************************************************************/

/*****************************************************************
**
**	rule compiler main routine
**
*******************************************************************/

#include <stdio.h>
#include <string.h>

#include "expert.h"
#include "keywords.h"

int	main(argc,argv)
int argc ;
char **argv ;

{
char	strBuff[MAX_STRING_BUFF] ;
int	i ;
int	state ;
int	strAddr;
int	numHypot, hypStack[MAX_HYPS],strBuffOfst ;
int	c ;
int	ruleBuffOfst ;
int	keyWrdNum ;
int	getKewWord() ;
int	putString() ;
int	lineNum ;

struct  rule_statement_r ruleBuff[MAX_RULE_STATEMENTS] ;

int	done ;
FILE	*infile, *outfile ;

for( done = 0; done < MAX_STRING_BUFF; done ++ )
	strBuff[done] = 0 ;
for( done = 0; done < MAX_HYPS ; done++ )
	hypStack[done]=0 ;
done = FALSE ;

#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP argc=%d ",argc) ;
#endif

if(argc != 3 )
	{
	fprintf(stdout, "\n\n Usage: rulecomp in.file out.file\n\n" ) ;
	exit() ;
	}
infile = fopen( argv[1], "r" );
if(infile == NULL)
	{
	fprintf(stdout,"\n\n Cannot open input file %s ", argv[1]);
	exit() ;
	}
outfile = fopen(argv[2], "wb" ) ;
if(outfile == NULL)
	{
	fprintf(stdout,"\n\n Cannot open output file %s ",argv[2]);
	exit() ;
	}
done = FALSE ;

numHypot = 0 ;				/* number of hypothesis is zero */
strBuffOfst = 0 ;			/* pointer in buffer is zero */
ruleBuffOfst = 0 ;			/* rule buffer offset is zero */


state = ANTECEDENT ;

/* Compilation states:
**
**	Antecedent group in progress -- 1
**	Consequent group in progress -- 2
**
**	If state 1 and you get a consequent then output group barrier.
**		change state to 2.
**	If state 2 and you get an antecedent then output group barrier.
**		change state to 1.
*/
lineNum = 1 ;
printf("\n") ;
while( !done )
	{

/*
**   	get the key word number from the
**	input file
*/
	printf("%04d  ",lineNum++) ;
	keyWrdNum = getKeyWord(infile,keyWords,&lineNum);
/*
**	error occured on line clear the line and
**	start over.
*/
	if(keyWrdNum == KEY_EOF)
		{
		done = TRUE ;
		continue ;
		}
	if(keyWrdNum == KEY_WORD_ERROR)
		{
		while( ( (c = getc(infile))) != EOL && (c != EOF) )
			putchar(c) ;
		fprintf(stdout," **** KEY WORD not found on line " );
		if(c == EOF)
			{
			done = TRUE ;
			break ;
			}
		putchar(c) ;
		continue;
		}
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP keyWrdNum = %d",keyWrdNum ) ;
#endif
/*
**	based on the key Word build the
**	rule files
*/

	switch(keyWrdNum)
		{
		case AND_N :
		case ANDIF_N :
		case IF_N :
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP case AND_N, ANDIF_N, IF_N ") ;
#endif
			if(state == CONSEQUENT)
				{
				state = ANTECEDENT ;
				ruleBuff[ruleBuffOfst].flag = NULL ;
				ruleBuff[ruleBuffOfst++].string = NULL ;
				}
			ruleBuff[ruleBuffOfst].flag = STRING_TRUE ;
			strAddr=putString(infile,strBuff,&strBuffOfst) ;
			ruleBuff[ruleBuffOfst++].string = strAddr ;
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP strAddr= %d",strAddr ) ;
#endif
			break ;
		case ANDRUN_N :
		case ANDIFRUN_N :
		case IFRUN_N :
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP case AND_N, ANDIF_N, IF_N ") ;
#endif
			if(state == CONSEQUENT)
				{
				state = ANTECEDENT ;
				ruleBuff[ruleBuffOfst].flag = NULL ;
				ruleBuff[ruleBuffOfst++].string = NULL ;
				}
			ruleBuff[ruleBuffOfst].flag = ROUTINE_TRUE ;
			strAddr=putString(infile,strBuff,&strBuffOfst) ;
			ruleBuff[ruleBuffOfst++].string = strAddr ;
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP strAddr= %d",strAddr ) ;
#endif
			break ;
		case ANDNOT_N :
		case IFNOT_N  :
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP case ANDNOT_N, IFNOT_N    ") ;
#endif
			if(state == CONSEQUENT)
				{
				state = ANTECEDENT ;
				ruleBuff[ruleBuffOfst].flag = NULL ;
				ruleBuff[ruleBuffOfst++].string = NULL ;
				}
			ruleBuff[ruleBuffOfst].flag = STRING_FALSE ;
			strAddr=putString(infile,strBuff,&strBuffOfst) ;
			ruleBuff[ruleBuffOfst++].string = strAddr ;
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP strAddr=%d",strAddr) ;
#endif
			break ;
		case ANDNOTRUN_N :
		case IFNOTRUN_N  :
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP case ANDNOT_N, IFNOT_N    ") ;
#endif
			if(state == CONSEQUENT)
				{
				state = ANTECEDENT ;
				ruleBuff[ruleBuffOfst].flag = NULL ;
				ruleBuff[ruleBuffOfst++].string = NULL ;
				}
			ruleBuff[ruleBuffOfst].flag = ROUTINE_FALSE ;
			strAddr=putString(infile,strBuff,&strBuffOfst) ;
			ruleBuff[ruleBuffOfst++].string = strAddr ;
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP strAddr=%d",strAddr) ;
#endif
			break ;
		case ANDTHEN_N :
		case THEN_N :
		case THENHYP_N :
		case ANDTHENHYP :
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP case ANDTHEN_N,THEN_N,THENHYP_N");
#endif
			if(state == ANTECEDENT)
				{
				state = CONSEQUENT ;
				ruleBuff[ruleBuffOfst].flag = NULL ;
				ruleBuff[ruleBuffOfst++].string = NULL ;
				}

			if( (keyWrdNum == THENHYP_N) || (keyWrdNum == ANDTHENHYP) )
				ruleBuff[ruleBuffOfst].flag = STRING_TRUE_HYP ;
			else
				ruleBuff[ruleBuffOfst].flag = STRING_TRUE ;

			strAddr=putString(infile,strBuff,&strBuffOfst) ;
			hypStack[numHypot++] = ruleBuffOfst ;
			ruleBuff[ruleBuffOfst++].string = strAddr ;
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP strAddr=%d",strAddr) ;
#endif
			break ;
		case ANDTHENRUN_N :
		case THENRUN_N :
		case THENRUNHYP_N :
		case ANDTHENRUNHYP_N :
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP case ANDTHEN_N,THEN_N,THENHYP_N");
#endif
			if(state == ANTECEDENT)
				{
				state = CONSEQUENT ;
				ruleBuff[ruleBuffOfst].flag = NULL ;
				ruleBuff[ruleBuffOfst++].string = NULL ;
				}

			if( (keyWrdNum == THENRUNHYP_N ) || (keyWrdNum == ANDTHENRUNHYP_N) )
				ruleBuff[ruleBuffOfst].flag = ROUTINE_TRUE_HYP ;
			else
				ruleBuff[ruleBuffOfst].flag = ROUTINE_TRUE ;

			strAddr=putString(infile,strBuff,&strBuffOfst) ;
			hypStack[numHypot++] = ruleBuffOfst ;
			ruleBuff[ruleBuffOfst++].string = strAddr ;
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP strAddr=%d",strAddr) ;
#endif
			break ;
		case KEY_EOF :
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP case KEY_EOF ") ;
#endif
			done = TRUE ;
			break ;
		default:
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP case DEFAULT "  ) ;
#endif
			fprintf(stdout, " \n\n illegal keyword number found " );
		}
	}

/* 
**	set up some blank space at the end of the rule file 
*/

ruleBuff[ruleBuffOfst].flag = NULL ;
ruleBuff[ruleBuffOfst++].string = NULL ;
ruleBuff[ruleBuffOfst].flag = NULL ;
ruleBuff[ruleBuffOfst++].string = NULL ;
ruleBuff[ruleBuffOfst].flag = NULL ;
ruleBuff[ruleBuffOfst++].string = NULL ;
if(ruleBuffOfst%2 == 0 )
	{
	ruleBuff[ruleBuffOfst].flag = NULL ;
	ruleBuff[ruleBuffOfst++].string = NULL ;
	}
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-RULECOMP ruleBoffOfst=%d", ruleBuffOfst ) ;
#endif
/*
**	ruleBuffOfst -3 is the number of rule statements processed 
*/

/*
**
**	Write out all of the compiled information as follows:
**
**		numHypot*2	Number_of_bytes_in_hypstack_section
**		hypStack	Hypstack_section
**		ruleBuffOfst	Number_of_bytes_in_rule_section
**		ruleBuff	Rule base section
**		strBuffOfst	Number_of_bytes_in_string_section
**		strBuff		String section
**		EOF
**
*/
#ifdef DEBUG

	fprintf(stdout,"\nDEBUG numHypot=%d",numHypot ) ;

	for(i=0;i<numHypot;i++)
		fprintf(stdout,"\nDEBUG     hypStack=%d",hypStack[i] ) ;

	fprintf(stdout,"\nDEBUG ruleBuffOfst=%d  ",ruleBuffOfst ) ;

	for(i=0;i<ruleBuffOfst;i++)
		{
		fprintf(stdout,"\nDEBUG ruleBuff[%d].flag=%d ",i,ruleBuff[i].flag ) ;
		fprintf(stdout,"\nDEBUG ruleBuff[%d].string=%d ",i,ruleBuff[i].string ) ;
		}

	fprintf(stdout,"\nDEBUG strBuffOfst=%d",strBuffOfst ) ;

	for(i=0;i<strBuffOfst;i++)
		fprintf(stdout,"\nDEBUG strBuff[%d]=%d(d),%c(c)",i,strBuff[i],strBuff[i] ) ;
#endif
fwrite(&numHypot,sizeof(int),1,outfile) ;
fwrite(hypStack,sizeof(int),numHypot,outfile) ;
fwrite(&ruleBuffOfst,sizeof(int),1,outfile) ;
fwrite(ruleBuff,2*sizeof(int),ruleBuffOfst,outfile) ;
fwrite(&strBuffOfst,sizeof(int),1,outfile) ;
fwrite(strBuff,1,strBuffOfst,outfile) ;
fclose(infile);
fclose(outfile);
#ifdef DEBUG
	fclose(stdout) ;
#endif
printf("\n\n") ;
exit(0) ;
}


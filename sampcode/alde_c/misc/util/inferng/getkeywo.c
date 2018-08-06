
#include <stdio.h>
#include <string.h>
#include "expert.h"


/******************************************************************
**
**
**		GET KEYWORD
**
**
******************************************************************/

/*
**
**	getKeyWord(infile,keyWords,lineNum) ;
**
**	returns either KEY_EOF, or number of keyword, or indications
**		that keyword was not first word on line.
**
*/

int	getKeyWord (infile,keyWords,lineNum)

FILE	*infile ;
char	*keyWords[] ;
int	*lineNum ;

{
int	firstc,c,i,j ;
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-GET_KEYWORD") ;
#endif

/*
**	get first non blank character
*/

i = 0 ;
while( TRUE )
	{
	c = getc(infile);
	putchar(c) ;
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-GET_KEYWORD character= %c,%x",c,c) ;
#endif
	if( c == EOF )
		return (KEY_EOF) ;
	if( (i == 0) && (c == COMMENT_CHAR) )	/* ignore comment line */
		{
		while(  c != EOL )
			{
#ifdef DEBUG
	fprintf(stdout,"\nDEBUG-GET_KEYWORD (delete comment) character=%c,%x",c,c);
#endif
			c = getc(infile) ;
			if( c == EOF)
				return (KEY_EOF) ;
			putchar(c) ;
			} 
		printf("%04d  ",*lineNum) ;
		*lineNum += 1 ;
		i = 0 ;
		}
	else
		{
		i = 1 ;
		if( c != BLANK )
			{
			if(c == EOF)
				return(KEY_EOF) ;
			break ;
			}
		}
	}

/*
**	locate first keyword with matching first character
*/

for( i = 0 ; i < NUM_KEYWORDS ; i++ )
	{
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-GET_KEYWORD i=%d, keyword=%c",i,*(keyWords[i])) ;
#endif
	if ( *(keyWords[i]) == c )
		break ;
	}
if( i == NUM_KEYWORDS )
	return (KEY_WORD_ERROR)  ;

/*
**	find the key word if there
**
**	Note that this search algrorithm is very dependant on having
**	the keyWord file being in strict alphabetical order!!!
**
*/

j=0 ; 		
firstc = c ;

while( i < NUM_KEYWORDS  )
	{
	if( (c=getc(infile)) == *(keyWords[i]+(++j)) )
		{
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-GET_KEYWORD char=%c,keyword=%c",c,*((keyWords[i])+j)) ;
#endif
		putchar(c) ;
		if( c == BLANK )
			return (i) ;
		if( c == EOF )
			return(KEY_EOF) ;
		}
/*
**	increment the keyWord pointer to be tested
**	decrement the character pointer in keyWord
**	and put the last character tested back.
*/

/*
**	since the keyWord array is alphabetical, testing
**	for a first character change would indicate that
**	the possible keywords have been exhausted.
*/

	else
		{
		i++ ;
		j-- ;
		ungetc(c,infile) ;
		if( c == EOF )
			return (KEY_EOF) ;
		if( firstc != *(keyWords[i]))
			return (KEY_WORD_ERROR) ;
		}
	}

/*
**	no keywords were found because we exhausted the keyWord array
*/

return (KEY_WORD_ERROR)  ;
}


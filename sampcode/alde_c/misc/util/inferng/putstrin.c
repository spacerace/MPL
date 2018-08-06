
#include <stdio.h>
#include <string.h>
#include "expert.h"


/*****************************************************************
**
**	PUT STRING
**
**	looks for string on rule line and
**	    either returns the offset into the
**		string buffer if found or places
**		it into the string buffer and returns
**		the offset to it.
**	strBuffOfst is the pointer to the offset to
**		the last used location in the string buffer
**
**
*****************************************************************/

int	putString (infile,strBuff,strBuffOfst) 

	FILE	*infile ;
	char	*strBuff ;
	int	*strBuffOfst ;

{
int	i,offSet,strLen,c,newline ;
char	buff[MAX_STR_LEN] ;

#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-PUTSTRING strBuffOfst= %d",*strBuffOfst) ;
#endif

/*
**
**	get file positioned to first non blank character (after keyword)
**
*/

while( ( c=getc(infile)) == BLANK )
	putchar(c) ;
if( c == EOL )
	{
	putchar(c) ;
	return (LINE_ERROR) ;
	}
if( c == EOF )
	return (KEY_EOF) ;
ungetc(c,infile);

#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-PUTSTRING c = %c", c) ;
#endif
/*
**
** 	load the string on the line into the string buffer for 
**	later comparison to strings in the line buffer.
**
*/
newline = FALSE ;
for( i = 0 ; i < (MAX_STR_LEN-1) ; i++ )
	{
	buff[i]=c=getc(infile) ;

#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-PUTSTRING c =%c, i = %d -- ", c,i) ;
#endif
	putchar(c) ;
	if(c == '\\')
		{
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-PUTSTRING newline is TRUE");
#endif
		newline = TRUE ;
		continue ;
		}
	if( newline == TRUE )
		{
		if( c == 'n' ) 
			{
			i -= 1 ;
			buff[i] = '\n' ;
			}
#ifdef	DEBUG
		fprintf(stdout,"\nDEBUG-PUTSTRING newline is FALSE");
#endif
		newline = FALSE ;
		continue ;
		}
	if(c == EOL)
		{
		buff[i]=0 ;
		break ;
		}
	if(c == EOF)
		{
		buff[i]=0 ;
		ungetc(c,infile) ;
		break ;
		}
	}

/*
**	remove trailing blanks in the buffer 
**
*/
for( --i ; i > -1 ; i-- )
	{
	if( buff[i] != BLANK )
		break ;
	buff[i]=0 ;
	}	
if(i <= 0)
	return(STR_LEN_ERROR) ;
strLen = i ;

buff[MAX_STR_LEN-1] = 0 ;

/*
**	at this point strLen should be the offset to the last character
**	in the string and is also, therefore, the length of the string -1
**
**	search for the string in the string buffer
**	return offset to string if found else put the string
**	in the string buffer and uptate buffer offset returning
**	pointer to new string
**
*/

offSet = 0 ;
while( offSet < *strBuffOfst )
	{
	if( strcmp(&strBuff[offSet], buff) == 0 )
		return(offSet) ;
	while( strBuff[++offSet] ) ;   	/* look for end of null trmntd string */
	++offSet ;			/* move past the null to next string  */
	}
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-PUTSTRING strBuffOfst= %d",*strBuffOfst) ;
	fprintf(stdout,"\nDEBUG-PUTSTRING offSet = %d",offSet) ;
	fprintf(stdout,"\nDEBUG-PUTSTRING strBuff= %s",&strBuff[offSet]);
	fprintf(stdout,"\nDEBUG-PUTSTRING buff   = %s",buff) ;
#endif
strcpy(&strBuff[*strBuffOfst],buff) ;
if(*strBuffOfst == 0 )
	{
	*strBuffOfst += strLen + 2 ;
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-PUTSTRING strBuffOfst= %d",*strBuffOfst) ;
#endif
	return(0) ;
	}
else
	{
	offSet = *strBuffOfst ;
	*strBuffOfst += strLen + 2 ;
#ifdef	DEBUG
	fprintf(stdout,"\nDEBUG-PUTSTRING strBuffOfst= %d",*strBuffOfst) ;
#endif
	return(offSet) ;
	}
}

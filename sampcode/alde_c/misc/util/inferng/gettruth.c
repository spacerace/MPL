

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

/*************************************************
**
**	getTruth(antecedent) 
**
**	asks user for the truth of a string or
**	returns --
**		TRUE if user says the statement is TRUE
**		FALSE if the user says the statement is FALSE
**
*************************************************/

#include <stdio.h>

#ifdef MSDOS
#include <conio.h>
#endif

#include "expert.h"
#include "inference.h"

int getTruth(cnsquent)
	int	cnsquent ;
{
int done,c ;
done = FALSE ;

while(!done)
	{
	printf("\n Is the following statement True?  (T/F,Y/N)\n\n ") ;
	printf("%s    ?",&strBuff[ruleBuff[cnsquent].string]) ;
#ifdef MSDOS
	c = getche() ;
#endif
#ifdef UNIXSV
	c = getchar() ;	
	getchar() ;
#endif
	switch(c) 
		{
		case 'y' :
		case 'Y' :
		case 't' :
		case 'T' :
			printf("\n\n") ;
			return(TRUE) ;
		case 'n' :
		case 'N' :
		case 'f' :
		case 'F' :
			printf("\n\n") ;
			return(FALSE) ;
		case 'w' :
		case 'W' :
			printf("\n Why is not implemented \n") ;
		default :
			printf("\n Please try again \"T\" or \"F\" \n ") ;
		}
	}
}





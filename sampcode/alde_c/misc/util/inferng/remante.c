

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

/***********************************************
**
**	remAnte(antecedent)
**
**	returns the truth value of the fact to be remembered
**	similar to verify except that the fact is a known antecedent
**	and is not a consequent of any rule.  And therefore can be remembered
**	false as well as true.
**
************************************************/

#include <stdio.h>
#include "expert.h"
#include "inference.h"


int	remAnte(antecedent)
	int	antecedent ;
{
int p_value ;

switch(ruleBuff[antecedent].flag)
	{
	case STRING_TRUE :
	case STRING_TRUE_HYP:
		knownTrue[numTrue++]=ruleBuff[antecedent].string ;
		return(TRUE) ;
	case STRING_FALSE :
		knownTrue[numTrue++]=ruleBuff[antecedent].string ;
		return(FALSE) ;
	default:  /* routine to run */
		if(weKnow(antecedent,&p_value) == TRUE)
			return(p_value) ;
		if( runRoutine(antecedent) == TRUE )
			{
			knownTrue[numTrue++]=ruleBuff[antecedent].string ;
			if((ruleBuff[antecedent].flag == ROUTINE_TRUE) ||
				(ruleBuff[antecedent].flag == ROUTINE_TRUE_HYP))
				{
				return(TRUE) ;
				}
			else
				{
				return(FALSE) ;
				}
			}
		else
			{
			knownFalse[numFalse++]=ruleBuff[antecedent].string ;
			if(ruleBuff[antecedent].flag == ROUTINE_FALSE)
				{
				return(TRUE) ;
				}
			else
				{
				return(FALSE) ;
				}
			}
	}
}		 

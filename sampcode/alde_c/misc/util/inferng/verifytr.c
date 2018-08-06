

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

/*****************************************************
**
**	verifyTruth(antecedent) ;
**
**	This routine verifies the truth of a string or routine which
**	is not a consequnt of a rule (ie an antecedent)
**
**	The routine remembers the final state of the antecedent in
**	the knownTrue, or knownFalse stacks
**
**	accomplishes task either by running the routine or by
**	asking the user for the specific truth of a statement
**
**	returns -- 
**		the "true" predicate value of the antecedent
**		according to its .flag:
**
**			TRUE if the antecedent phrase is TRUE and
**			   the antecedent .flag indicates positive,
**			FALSE if the antecedent phrase is TRUE and
**			   the antecedent .flag indicates negative.
**			etc.
**
*******************************************************/

#include <stdio.h>
#include "expert.h"
#include "inference.h"

int	verifyTruth(antecedent) 
	int antecedent ;
{
switch(ruleBuff[antecedent].flag)
	{
	case STRING_TRUE :
	case STRING_TRUE_HYP:
		if( getTruth(antecedent) == TRUE) 
			{
			knownTrue[numTrue++] = ruleBuff[antecedent].string ;
			return(TRUE) ;
			}
		else
			{
			knownFalse[numFalse++] = ruleBuff[antecedent].string ;
			return(FALSE) ;
			}
	case STRING_FALSE :
		if( getTruth(antecedent) == TRUE) 
			{
			knownTrue[numTrue++] = ruleBuff[antecedent].string ;
			return(FALSE) ;
			}
		else
			{
			knownFalse[numFalse++] = ruleBuff[antecedent].string ;
			return(TRUE) ;
			}
	case ROUTINE_TRUE :
	case ROUTINE_TRUE_HYP:
		if( runRoutine(antecedent) == TRUE) 
			{
			knownTrue[numTrue++] = ruleBuff[antecedent].string ;
			return(TRUE) ;
			}
		else
			{
			knownFalse[numFalse++] = ruleBuff[antecedent].string ;
			return(FALSE) ;
			}
	case ROUTINE_FALSE :
		if (runRoutine(antecedent) == TRUE)
			{
			knownTrue[numTrue++] = ruleBuff[antecedent].string ;
			return(FALSE) ;
			}
		else
			{
			knownFalse[numFalse++] = ruleBuff[antecedent].string ;
			return(TRUE) ;
			}
		default:
			printf("\major problem # 0001 -- llegal antecedent flag\n") ;
			return(TRUE) ;
		}
}


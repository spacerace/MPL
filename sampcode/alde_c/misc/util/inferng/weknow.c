

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
**	weKnow(antecedent,&predicate) 
**
**	routines searches both knownTrue and knownFalse
**	stacks to determine whether the antecedent is known.
**
**	the return value of the routine is TRUE if known and FALSE otherwise
**
**	the predicate is set to the "true" value of the antecedent according
**	to whether its phase is TRUE or FALSE and its .flag indicator.
**
*****************************************************************/

#include "expert.h"
#include "inference.h"

int	weKnow(antecedent,p_value)
	int antecedent,*p_value ;
{
if(known(antecedent,knownTrue,numTrue) == TRUE )
	switch(ruleBuff[antecedent].flag)
		{
		case STRING_TRUE:
		case STRING_TRUE_HYP:
		case ROUTINE_TRUE:
		case ROUTINE_TRUE_HYP:
			*p_value = TRUE ;
			return(TRUE)  ;
		case STRING_FALSE:
		case ROUTINE_FALSE :
			*p_value = FALSE ;
			return(TRUE) ;
		}
if(known(antecedent,knownFalse,numFalse) == TRUE )
	switch(ruleBuff[antecedent].flag)
		{
		case STRING_TRUE:
		case STRING_TRUE_HYP:
		case ROUTINE_TRUE:
		case ROUTINE_TRUE_HYP :
			*p_value = FALSE ;
			return(TRUE) ;
		case STRING_FALSE:
		case ROUTINE_FALSE:
			*p_value = TRUE ;
			return(TRUE)  ;
		}
return(FALSE) ;
}


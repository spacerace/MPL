
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

/********************************************************
**
**	verify(consequent)
**
**	finds the truth about the consequent -- returns
**	TRUE if consequent is proved (all antecedents are TRUE)
**	FALSE if any of the antecedents are FALSE
**
********************************************************/

#include <stdio.h>
#include "expert.h"
#include "inference.h"

int	verify(cnsquent) 
	int	cnsquent ;

{
int 	i,j,k,m,n ;
int	antecedent[MAX_ANTECEDENTS] ;
int	consequent[MAX_ANTECEDENTS] ;
int	p_value ;			/* predicate value */

#ifdef DEBUG
	int di ;
	for(di=0;di<numTrue;di++)
		printf("\nDEBUG-verify knownTrueFact = %d",knownTrue[di]) ;
	for(di=0;di<numFalse;di++)
		printf("\nDEBUG-verify knownFalseFact = %d",knownFalse[di]) ;

#endif

/*
**
**	get all of the antecedents for the consequent
**
**	First locate the antecedents -- should be directly in front
**	of the consequent, delimited by a consequent with a flag of zero
*/

#ifdef DEBUG
	printf("\nDEBUG -- verify consequent = %d",cnsquent ) ;
#endif

j = 0 ;

for(i=cnsquent; i>=0 ; i--)
	{
	if(ruleBuff[i].flag == 0 )
		break ;
	}

if(i == -1)
	{
	printf("\n Bad Consequent, has no antecedents! returning TRUE value \n ") ;
	return(TRUE);
	}

for(i = i-1 ;i>=0; i--)
	{
	if(ruleBuff[i].flag != 0)
		antecedent[j++]=i ;
	else
		break ;
	}

if(j==0)
	{
	printf("\n Bad Consequent, has no antecedents! returning TRUE value \n ") ;
	return(TRUE);
	}
	
#ifdef DEBUG
	printf("\nDEBUG -- verify antecedents = ") ;
	for(di=0; di < j; di++)
		printf("\n 	%d -- %d",di,antecedent[di]) ;
#endif

/*
**	at this point we should have some antecedents in the antecedent
**	stack so now lets see if each can be proved, and if there are
**	any consequents which will need verification.
*/

/*
**	main verificaiton loop:
*/

for(i=j-1 ; i >= 0 ; i--) /* for all of the antecedents in our stack */ 
	{

/*
**	determine if the antecedent is itself a consequent
**	
**	compare value of the string pointer of the antecedent needs to
**	be compaired with the value of the string pointer of all of the
**	consequents, if there is a match then the antecedent is a consequent
**
**	There may be more
**	than one consequent and this is handled below since all of these
**	must be false before we give up hope that one will be verified--
**	REMEMBER -- consequents are not remembered FALSE they can only be
**	remembered when they are true -- FALSEness for a consequent merely
**	means that this particular rule did not verify it , some other one
**	might.
**
*/

	for(k=0; k< numHypot; k++ )
		if(ruleBuff[hypStack[k]].string == ruleBuff[antecedent[i]].string)
			break ;

	if(k != numHypot)  /* we have an antecedent which is a consequent */
		{

#ifdef DEBUG
	printf("\nDEBUG -- verify antecedent %d is consequent %d",i, antecedent[i] ) ;
#endif

/*
**	What we have is an antecedent which is a consequent of another
**	rule or rules.  What is needed then is to place each of these consequents
**	into a stack and prove them one at a time.  We will return truth
**	if any of these are proven true and return false only when all of
**	them are false.   We can use veriry recursively in this case to
**	verify each one.
**
**	get all consequents matching hypStack[k].string into a stack
**	look for truth of any consequent.
**	if consequent is true (according to flag) return true.
**	if all consequents are false then return false.
*/
		n = 0 ;
		for( m = 0 ; m < numHypot ; m++)
			{
			if(ruleBuff[antecedent[i]].string == ruleBuff[hypStack[m]].string)
				{
#ifdef DEBUG
				printf("\nDEBUG--consequent stack(%d) = %d",n,hypStack[m]) ;
#endif
				consequent[n++] = hypStack[m] ;
				}
			}
/*
**	verify each consequent
*/

		p_value = FALSE ;
		for(m = 0 ; m < n ; m++)		
			{

#ifdef DEBUG
	printf("\nDEBUG -- verify(2) consequent = %d",consequent[m] ) ;
#endif
			if( verify(consequent[m]) == TRUE )
				{
				p_value = TRUE ;
				if(known(consequent[m],knownTrue,numTrue) == FALSE)
					{
					printf("\nI infer that : %s\n",&strBuff[ruleBuff[consequent[m]].string]) ;
					knownTrue[numTrue++]=ruleBuff[consequent[m]].string ;
					}
				if(remAnte(antecedent[i]) == TRUE)
					{
					break ;
					}
				else
					{
					return(FALSE) ;
					}
				}
			}
		if(p_value == FALSE) /* all of the consequents were not proved */
			{
			switch(ruleBuff[antecedent[i]].flag)
				{
				case STRING_FALSE :
				case ROUTINE_FALSE :
					continue ;
				case STRING_TRUE :
				case STRING_TRUE_HYP :
				case ROUTINE_TRUE :
				case ROUTINE_TRUE_HYP :
					return(FALSE) ;
				}
			}
		else			/* at least one was known */
			{
			switch(ruleBuff[antecedent[i]].flag)
				{
				case STRING_TRUE :
				case STRING_TRUE_HYP :
				case ROUTINE_TRUE:
				case ROUTINE_TRUE_HYP:
					continue ;
				case STRING_FALSE :
				case ROUTINE_FALSE :
					return(FALSE) ;
				}
			}
		}
		
	else	/* we have a plane old string or routine antecedent */
		
		{
		
		if(weKnow(antecedent[i],&p_value) == TRUE)
			{
			if(p_value == TRUE)
				continue ;
			else
				return(FALSE) ;
			}
/*
**	Things arent known and are simple consequents so prove them
**	either by asking the user or by running the routine
*/
		if(verifyTruth(antecedent[i]) == TRUE)
			continue ;
		else
			return(FALSE) ;
		}
	}
/*
**	Everything was TRUE in the big and statement so return it
*/
return(TRUE) ;
}




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

/*
**	the following are the global common variables which
**	are used in the inference engine...
**
**	all routines except inference.c should have this
**	file included.
*/

#define	MAX_KNOWN	500

int	numHypot, hypStack[MAX_HYPS],strBuffOfst ;
char	strBuff[MAX_STRING_BUFF] ;
int	ruleBuffOfst ;
int	knownTrue[MAX_KNOWN], knownFalse[MAX_KNOWN] ;
int	numTrue, numFalse ;
struct  rule_statement_r ruleBuff[MAX_RULE_STATEMENTS] ;


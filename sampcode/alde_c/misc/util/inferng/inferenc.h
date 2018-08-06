

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

/*************************************************************************
**									**
**	The software contained in this distribution is copyright (C)	**
**	by George Hageman 1985 and is released into the public 		**
**	domain with the following restrictions:	 			**
**									**
**		(1)  This software is intended for non-commertial	**
**			usage.						**
**		(2)  I am held save from damages resulting from		**
**			its use, and					**
**		(3)  The following concepts and legal jargon are	**
**			agreed to by the user of this software.		**
**									**
**	User-supported software concept: 				**
**									**	
**	IF    you find use for this software				**
**	ANDIF it saves you some development time			**
**	THEN        send me $10.00					**
**	ANDTHENHYP  you will feel good!					**
**									**
**									**
**	This source code is provided on an "as is" basis without	**
**	warranty of any kind, expressed or implied, including but	**
**	not limited to the implied warranties of merchantability	**
**	and fitness for a particular purpose.  The entire risk as	**
**	to quality and performance of this software is with you. 	**
**	Should the software prove defective, you assume the entire	**
**	cost of all necessary repair, servicing, or correction.  In	**
**	no event will the author be liable to you for any damages,	**
**	including any lost profits, lost savings, or other		**
**	incidental or consequential damages arising out of the  use	**
**	of inability to use this software.  In short my friends, I	**
**	have done  a reasonable ammount of work in debugging this	**
**	software and I think it is pretty good but, as you know,	**
**	there is always some chance that a bug is still lurking 	**
**	around. If you should happen to be lucky enough to  find one,	**
**	please let me know so I	can make an attempt to fix it.		**
**									**
**				Thanks,					**
**									**
**				George Hageman				**
**				P.O. Box 11234				**
**				Boulder, Colorado 80302			**
**									**
*************************************************************************/


/*
Expert system inference engine

This inference engine is backwards-chaining only and features the
running of binary files if:

	1) they are antecedents associated with a particuar consequent
	   being proved, or
	2) they are consequents which have been proven true by verify().
	   their actual predicate value will be determined by their returned
	   result after running.

This inference engine is designed with diagnostics in mind and so will probably
be best suited for this application.  Later revisons will include 
forward-chaining so that the user will have the opportunity to give pre-existant
conditons, or that these my be supplied by the calling process.

(If you cant tell by the above description -- I am very excited about the 
possibilties presented in this form of computer control of the diagnostic
process... Geo.)

See structure design for details of operation,  but basically the 

inference reads in all of the compiled information as produced by the
    rule compiler.
    
it proceeds to attempt to prove each consequent by proving the truth
	or falseness of any antecedent associated with this consequent.
if any antecedent of a consequent turns out to be a consequent itself, then
        the inference engine will recursively attempt to prove this consequent.

the process is complete when all of the predicate values of the consequents has
	been determined.
	
Additional features which I may put in at a later time is the abiltiy for
the inference engine to expalin itself to the user when the user asks why
the inference engine needs to know the predicate value of a particular
antecedent.   This will be done by forward and backward chaining of predicat
clauses, the truth of each will be displayed.
*/
#define	MAX_ANTECEDENTS		25

extern 	int	numHypot, hypStack[],strBuffOfst ;
extern 	char	strBuff[] ;
extern	int	ruleBuffOfst ;
extern	int	knownTrue[], knownFalse[] ;
extern	int	numTrue, numFalse ;
extern	struct  rule_statement_r ruleBuff[] ;

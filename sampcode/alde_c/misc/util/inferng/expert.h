
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
**	These are the structures of the rulebase which will
**	be used to compile the rules into.  
*/

#define FALSE 			0 
#define TRUE  			-1 
#define MAX_STRING_BUFF 	5000
#define MAX_STR_LEN		100
#define MAX_RULE_STATEMENTS 	500
#define MAX_HYPS		250 
#define ANTECEDENT 		1
#define CONSEQUENT 		2
#define COMMENT_CHAR		'!'
#define	BLANK 			0x20
#define EOL			0x0a

#define KEY_EOF			-2 
#define LINE_ERROR		-3
#define KEY_WORD_ERROR		-4
#define ERROR		 	-5	
#define STR_LEN_ERROR		-6

/*
**	Other definitions of key words
*/

#define	AND_N       	 0
#define	ANDIF_N     	 1
#define	ANDIFRUN_N  	 2
#define	ANDNOT_N    	 3	
#define	ANDNOTRUN_N 	 4
#define	ANDRUN_N    	 5
#define	ANDTHEN_N   	 6 
#define ANDTHENHYP       7
#define	ANDTHENRUN_N	 8
#define	ANDTHENRUNHYP_N  9
#define	IF_N        	10
#define	IFNOT_N     	11
#define	IFNOTRUN_N  	12
#define	IFRUN_N     	13
#define	THEN_N      	14
#define	THENHYP_N     	15  
#define THENRUN_N	16
#define THENRUNHYP_N    17
		
/*
**	Flag definitions:
*/

#define	STRING_TRUE	 1
#define	STRING_FALSE	 2
#define	ROUTINE_TRUE	 3
#define ROUTINE_FALSE	 4
#define STRING_TRUE_HYP  5
#define ROUTINE_TRUE_HYP 6

#define NUM_KEYWORDS	18

struct	rule_statement_r
		{
		int flag ;   /* logical flag for inference engine */
		int string ; /* offset into string buffer */
		};


/* 
**	rules are compiled into the array rules in the folloiwng form:
**
**	antecedent-group consequent-group 
**	... 
**	antecedent-group consequent-group
**	end-group
**
**	Each group of consequences and antecedents
**	are compiled in a group like the following:
**
**	flag pointer flag pointer ... flag pointer 0-flag 0-pointer
**
**	The end-group is merely:
**
**	0-flag 0-pointer 0-flag 0-pointer 0-flag 0-pointer
**
**	flags are used by the inference engine to determine what to
**	do with the following string pointer.  
**	string pointers are merely offsets into the string array.
**	The pointers may either point
**	to a string which is a rule statement such as "the animal has wings"
**	or is a UNIX pathname for a particular routine which is to be
**	executed such as "/g1/hageman/Diagnostics/Disk1diag".   This
**	routine will then be executed and will return either a true or
**	false indication.  Latter versions of the inference engin may be
**	capable of returning more than this via some pipe-line mechanism or
**	other.
**
**	Once an anicedent whether string or routine is verified it is placed
**	in either a known-true or known-false stack for later verification 
**	in other rules which use them.  In short they only have to be verified
**	once.
**
**	Examples of a rule structure are:
**
**	IFNOT the animal is a bird
**	AND the animal has wings
**	ANDNOT the animal lives in caves
**	AND the animal is nocternal
**	THEN the animal is a bat
**	IF the animal is a bat
**	ANDRUN /g1/hageman/Src/Expert/speed_of_bat
**	THENHYP the bat is out of hell
**	IF the animal is a bat
**	ANDNOTRUN /g1/hageman/Src/Expert/speed_of_bat
**	THENHYP the bat is out of cave
**
*/

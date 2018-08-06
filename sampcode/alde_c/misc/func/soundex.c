/****************************************************************
*	SOUNDEX ALGORITHM in C					*
*			 					*
*	The basic Algorithm source is taken from EDN Nov.	*
*	14, 1985 pg. 36.					*
*								*
*	As a test Those in Illinois will find that the		*
*	first group of numbers in their drivers license		*
*	number is the soundex number for their last name.	*
*								*
*	RHW  PC-IBBS ID. #1230					*
*								*
****************************************************************/

#include <ctype.h>

char (*soundex(out_pntr, in_pntr))
char *in_pntr;
char *out_pntr;
{
extern char get_scode();
char ch,last_ch;
int count = 0;

	strcpy(out_pntr,"0000");	/* Pre-fill output string for    */
					/* error and trailing zeros.     */
	*out_pntr = toupper(*in_pntr); 	/* Copy first letter             */
	last_ch = get_scode(*in_pntr);	/* code of the first letter      */
					/* for the first 'double-letter  */
					/* check.			 */
					/* Loop on input letters until   */
					/* end of input (null) or output */
					/* letter code count = 3	 */

	while( (ch = get_scode(*(++in_pntr)) ) && (count < 3) )
	{
	if( (ch != '0') && (ch != last_ch) ) /* if not skipped or double */
		*(out_pntr+(++count)) = ch; /* letter, copy to output */
		last_ch = ch;	/* save code of last input letter for */
				/* next double-letter check */
	}
	return(out_pntr);	/* pointer to input string */
}

char get_scode(ch)
char ch;
{
			    /* ABCDEFGHIJKLMNOPQRSTUVWXYZ */
		            /* :::::::::::::::::::::::::: */
static char soundex_map[] =   "01230120022455012623010202";

	/* If alpha, map input letter to soundex code. If not, return 0 */

	if( !isalpha(ch) )	/*error if not alpha */
		return(0);
	else
		return(soundex_map[(toupper(ch) - 'A')] );
}
#include <ctype.h>

main(argc, argv)
int	argc;
char	*argv[];
{
char	*code[10];

int	i;

	if(argc == 1) /* No arguments, give usage */
	{
	printf("\nUsage: soundex (name) (...)\n");
	exit(1);
	}

	for(i = 1; i < argc; i++)
	{
	soundex(code, argv[i]) ;
	printf("The Soundex Code for \"%s\" is: %s\n", argv[i],code);
	}
	exit(0);
}

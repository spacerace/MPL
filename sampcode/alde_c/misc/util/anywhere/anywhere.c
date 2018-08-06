/***************************************************************
*							       *
*	   WHERE					       *
*							       *
* Where is a program to locate files on the PC hard disk.      *
* It requires DOS 2.x or 3.x.				       *
*							       *
* The command line syntax is:				       *
* where [starting directory]filename.ext		       *
*							       *
* Written by Mark S. Ackerman				       *
* PROGRAM IS WRITTEN IN MARK WILLIAMS  M W C 8 6  LANGUAGE     *
* Copyright 1984, 1985 by Mark S. Ackerman.  Permission is     *
* granted for unlimited copies if not sold or otherwise        *
* exchanged for gain.					       *
*  PUBLISHED IN PC TECH JOURNAL - OCT '85 - VOL 3, NO. 10      *
*							       *
*  MODIFIED FOR LATTICE C VER 2.15 BY JOHN TEICHERT NOV 85     *
*     Names shortened to 8 significant characters.	       *
*     Elimination of PTOREG() function			       *
*     flag86 defined to return flags from intdosx()	       *
*     Use segread to set up regs for intdosx() function.       *
*     Program modified to look for drive designator with colon.*
*     DATE structure defined with bits high order to low order.*
*     intrpt.h replaced with dos.h header file		       *
*     rindex() function replaced with strrchr() function.      *
*							       *
*  MODIFIED FOR MICROSOFT V3 BY JOHN TEICHERT JAN 86.	       *
*     flag86 REDEFINED to dos_result for  intdosx() ax reg     *
*     modified to use flag in REGS structure.		       *
*     DATE structure defined with bits low order to high order.*
*     _stack will not produce large stack in ver 3.0 must use  *
*	     link option or exemod program.		       *
*							       *
*   Added Code And Became ANYWHERE  JOHN TEICHERT FEB 86       *
*							       *
*	Taking advantange of V3 access to the environment      *
*	string we set up the following. 		       *
*							       *
*	1. An Environment String indicating what disk drives   *
*	   you want ANYWHERE to search as follows:	       *
*							       *
*	   AWDISK=d:[;d:[;d:;...d:]]			       *
*							       *
*	      where d: is the drive specifier for one or more  *
*	      fixed disk(s).				       *
*							       *
*	      drive specifiers are searched in the order they  *
*	      are entered.				       *
*							       *
*	2. The user can specify the environment string with    *
*	   the use of the set command in the autoexec.bat file.*
*	   As an example:				       *
*							       *
*		set awdisks=c:;e:;d:			       *
*							       *
*	   would be placed into the autoexec.bat file.	       *
*							       *
*	With this modification the user has extended directory *
*	capabilities by automatically searching all disk drives*
*	listed in the environment string or isolated to a      *
*	single drive by placing a drive specifier in the       *
*	command line argument string.			       *
*							       *
*	Be sure to use the /stack option on the link with      *
*	Microsoft V3 C compiler or stack problems will result  *
*	if many subdirectories.  8K seems to work well.        *
*							       *
*							       *
*	Added a few little perks May 1987 - Michael L Kaufman  *
*							       *
***************************************************************/


/***************************************************************
* The C header files					       *
* These identify library routines like printf() and int86x()  *
***************************************************************/

#include <stdio.h>   /* standard i/o			      */
#include <dos.h>     /* functions for DOS interrupt calls     */

/***************************************************************
* Structure for MS-DOS date and time fields		       *
* See pages 4-6 and 4-7 of the DOS 2.1 technical	       *
* reference manual for more information 		       *
* This structure is used in the next structure definition      *
***************************************************************/

struct msdos_date
	{
	unsigned ms_sec    : 5; /* time in 2 sec. int (5 bits)*/
	unsigned ms_min    : 6; /* minutes (6 bits)	      */
	unsigned ms_hour   : 5; /* hours (5 bits)	      */
	unsigned ms_day    : 5; /* day of month (5 bits)      */
	unsigned ms_month  : 4; /* month (4 bits)	      */
	unsigned ms_year   : 7; /* year since 1980 (7 bits)   */
	};

/***************************************************************
* Definition of DOS Disk Transfer Area (DTA)		       *
***************************************************************/

/***************************************************************
* Structure filled in by MS-DOS for interrupt 21 calls	       *
* See page 5-46 of the DOS 2.1 technical reference	       *
* manual for more information				       *
***************************************************************/

struct DTA
	{
	char	DTA_dosinfo[21];       /* used by DOS		*/
	char	DTA_attr;	       /* file attribute byte	*/
	struct msdos_date DTA_date;    /* date struct. as above */
	long	DTA_size;	       /* file size		*/
	char	DTA_filename[13];      /* file name (w/o path)	*/
	};


/***************************************************************
*		Definitions of constants		       *
***************************************************************/

#define carry_set 0x0001 /* mask for flag register	       */
			 /* for carry bit		       */
#define no_type   0x00	 /* no bits set on file attribute byte */
#define directory_type	 0x10 /* directory file bit on file    */
			      /* info word		       */

#define no_more_files	 18	/* DOS return code for		*/
				/* no more files		*/

#define end_of_string	 '\0'   /* C uses a binary zero to      */
				/* signal end of string 	*/

#define backslash '\\'   /* the backslash character           */
#define colon	  ':'    /* Drive separator  JT 11/85           */
#define semicolon ';'   /* Environment string drive separator   */

char *month[] = {
		"Jan","Feb","Mar","Apr","May","Jun",
		"Jul","Aug","Sep","Oct","Nov","Dec"
		};

char *time_of_day[2] = {"AM","PM"};


/***************************************************************
*		Define the type "filename"                     *
*			to be a string of 65 characters  -JT   *
***************************************************************/

typedef char filename[65];		   /* Change to 65 -JT		     */

/***************************************************************
*							       *
* The following filename strings are used in the program:      *
*							       *
*	chk_str 		filename to be searched for    *
*				filename in the command line)  *
*	dir_string		directory name to be searched  *
*	new_dstr		directory name to be searched  *
*				  on next recursive call       *
*	cur_str 		temporary string for searching *
*				  in a specific directory      *
***************************************************************/


/***************************************************************
* Definition of any forward-referenced functions	       *
***************************************************************/

char *DATE();
char *space();

/***************************************************************
*		Global variables			       *
***************************************************************/

filename chk_str;			/* this string "remembers" user input */
union	REGS	r8086;			/* structure to allow access to indiv.*/
					/*   registers for interrupts	      */
struct	SREGS	s8086;			/*   structure for segment registers  */
char		date_str[40];		/* print output string for dates      */
int		count=0;		/* Number of matches dislplayed      */
char		spce[] = "                                           ";
int		lastzero = 41;
unsigned	dos_result;		/* Return code from DOS 	      */

/**
*	FOLLOWING CODE COMMENTED OUT FOR V3.0 SINCE CAN'T FIND A WAY TO DO IT
*	WITH A VARIABLE.
**/

/*int		  _stack = 8192;	   Insure large stack to support */
/*					   recursion in look routine	 */

/***************************************************************
*		MAIN() -- the beginning of the code	       *
***************************************************************/

main( argc, argv, envp )
int	argc;
char	*argv[];
char	*envp[];			/* Version 3 pointer to environ */
{
/**
*	External	function
**/
char			*strrchr();	/* Lattice function which searches */
					/* for the last occurrance of the  */
					/* desired character		   */

char			*strchr();	/* Lattice function which searches */
					/* for the first occurrance of the */
					/* desired character		   */

filename	       dir_string;	/* directory to be searched	   */

char		       *usr_file;	/* address of filename in	   */
					/* command line argument	   */
					/* (ie, the filename)		   */

char *last_loc; 			/* address of last backslash in    */
					/* command line argument	   */

char *dos_parm; 			/* address of			   */
					/* command line argument	   */

int  lst_dchr;				/* last character		   */
					/* in directory string		   */

/** added for ANYWHERE JT **/
int			strcmpi();	/* Microsoft V3 function to do a     */
					/*   string compare without regard   */
					/*   to case.			     */
void			strcat();	/* String concatenate function	     */
#define     MAXEDRV	16		/* Set max number of drives	     */
char			*strdup();	/* MSC string dup function	     */
char			*getcwd();	/* MSC ver3 function to return	     */
					/* current working directory	     */
char			*envdup;	/* pointer to duplicate of env str   */
char			*envdrv[MAXEDRV];   /* pointer to each drive	     */
					/* in envdup			     */
filename		env_dir_string; /* Environment directory string      */
char			**cur_envp;	/* current environmnet array pointer */
char			*env_stp;	/* Current environment string pointer*/
char			*env_chp;	/* environment character pointer     */
static	char		envid[]="AWDISK=";  /* Our environment string id     */
char			*envid_p;	/* current id string ptr	     */
filename		dos_cwd;	/* buffer to hold cur working dir    */
int			env_cnt;	/* count of number of drives	     */
int			envd_len;	/* length of the environment string  */
int			wi;		/* A working integer		     */
struct	{
	unsigned	drive	: 1;	/* drive specifier found in com line */
	unsigned	found	: 1;	/* environment contains AWDISK=          */
	unsigned	srchcwd : 1;	/* Search for current work dir	     */
	unsigned	addcwd	: 1;	/* Add current working directory     */
	} flag;

/********************************************************
*	check number of incoming arguments		*
*	if incorrect, write an error message		*
********************************************************/

if (argc != 2)
    {
    printf( "ERR  usage is:   AW    [starting directory]filename.ext\n\n");
    printf( "Environment string is: AWDISK=d: [;d: [;....d:] ]\n");
    return;
    }

/**	Added for Lattice C Version 2.15 JT
*	Initialize the segment structure for later dos interrupt calls.
**/

segread( &s8086 );		/* initialize segments for interrupt calls */
dos_result = 0; 		/* init interrupt return variable MSCV3.   */

/**
*	dos_parm is set to the first argument in the
*	command line
**/

dos_parm = *(++argv);

/**
*	If a drive specifier is found we do not want to go through overhead
*	of environment string search.		AW - JT
*	If no drive specifier is found we will extract the drive of the
*	    current working directory to be added to the array of environment
*	    drives.
**/
flag.srchcwd = 1;			/* Init true until command line      */
					/*     option added.		     */
if ( strchr( dos_parm, colon ) )
    {
    flag.drive = 1;			/* Set drive specifier true	     */
    flag.found = 0;			/* Set environment found false	     */
    }
else
    {
    /**
    *	Drive flag is set to zero
    *	If the search current working directory flag is true then:
    *	    Current working directory is obtained and the drive isolated from
    *		from the directory string.
    **/
    flag.drive = 0;
    if ( flag.srchcwd )
	{
	getcwd(dos_cwd,sizeof(dos_cwd)-1);
	env_chp = strchr( dos_cwd, colon );
	*++env_chp = end_of_string;
	}
    }
/**
*   We check to see if there was a drive specifier found.  If so we
+	want to continue rather than check the environment string for the
*	AWDISK= parameter
**/
if (flag.drive)
    {
    /**
    *	There was a drive specifier so we zero the env_cnt variable
    **/
    env_cnt = 0;
    }
else
    {
    /**
    *	The drive specifier was  found in the DOS command argument therefore
    *	    the environment strings will be searched for the AWDISK= parameter
    **/
    cur_envp = envp;		    /* init current environ pointer */
    flag.found = 0;		    /* init flag found false	    */
    /**
    *	    Check each environment string to see if it is the AW
    *	    environment string.  The envp array is terminated with
    *	    a NULL pointer.  The search is terminated when found is
    *	    true or the environment pointer is NULL.
    **/
    while( *cur_envp && !flag.found )
	{
	envid_p = envid;	/* init pointer to our environ id */
	env_chp = *cur_envp;	/* init pointer to environ string */
	flag.found = 1; 	/* Set flag found = true to enter loop */
	/**
	*   Compare each character in the string to AWDISK=
	**/
	for ( env_cnt=0; (env_cnt<(sizeof(envid)-1)) && flag.found; env_cnt++)
	    {
	    if ( *envid_p++ != *env_chp++ )
		flag.found = 0;
	    else
		flag.found = 1;
	    }

	/**
	*   If found true then copy string to our buffer
	*   Replace ';' with 0x00
	*   Count the number of drives found
	*   Set addcwd true to add the current working directory
	**/
	if ( flag.found )
	    {
	    flag.addcwd = 1;
	    /**
	    *	Create a duplicate of the environment string
	    *	If it cannot be created we will probably fail but allow
	    *	continuation just in case
	    **/
	    if (!(envdup = strdup(env_chp)))
		{
		fprintf( stderr, "Memory alloc problems\n" );
		env_cnt = 0;		/* zero count cause we're goin out   */
		break;			/* allow current drive to continue   */
		}
	    /**
	    *	Examine the environment string for a ";"
	    *	Replace each semicolon with a "\0" to terminate the string
	    *	Increment the env_cnt variable for each drive in the system
	    *	    that the user wished to examine
	    *	Place the pointer to the beginning of drive in the envdrv
	    *	    variable.
	    **/
	    envd_len = strlen(envdup);	/* get length of environment string  */
	    env_cnt  = 0;		/* Set count of drives to zero	     */
	    env_stp  = envdup;		/* Set the string pointer to current */
	    while ( (envd_len > 0) && (env_cnt < MAXEDRV) )
		{
		/**
		*   Look for the semicolon
		**/
		if (env_chp = strchr(env_stp,semicolon))
		    {
		    /**
		    *	We have a semicolon : See if its false
		    **/
		    if (env_chp-env_stp)
			{
			/**
			*   There is a string so put in array
			*   Decrement the length field
			*   Change the value of the semi colon to a end of str
			*   Set up a new pointer beyond env_stp
			**/
			envdrv[env_cnt++] = env_stp;
			envd_len = envd_len-((env_chp-env_stp)+1);
			*env_chp = end_of_string;
			/**
			*   Check the environment entry against the current
			*	working directory.  If equal set addcwd to
			*	false
			**/
			if ( strcmpi( dos_cwd, env_stp ) == 0 )
			    flag.addcwd = 0;
			env_stp = ++env_chp;
			}
		    else
			{
			/**
			*   There was no string just a semicolon
			*   Decrement the length field
			*   setup a new pointer beyond the semi-colon
			**/
			--envd_len;
			env_stp = ++env_chp;
			}
		    }
		else
		    {
		    /**
		    *	There was no semi-colon so we are on the last drive
		    *	    entry.
		    *	Place address in array and increment count
		    *	Set the remaining string length to zero to exit.
		    *	See if last entry matches current working directory
		    *	    if so don't add it.
		    **/
		    envdrv[env_cnt++] = env_stp;
		    envd_len = 0;
		    if ( strcmpi( dos_cwd, env_stp ) == 0 )
			flag.addcwd = 0;
		    if ( flag.addcwd )
			{
			if ( (env_cnt < MAXEDRV) && (flag.srchcwd) )
			    envdrv[env_cnt++] = dos_cwd;
			}
		    }
		}
	    }
	else
	    {
	    /**
	    *	This environment string is not the one we want therefore
	    *	Increment current environment pointer and continue
	    **/
	    cur_envp++;
	    }
	}   /* End While */
    }	/* End Else */

/**
* The dos_parm is then searched for the last
* occurrence of a backslash to find the end of
* the directory name.
**/

last_loc = strrchr(dos_parm,backslash);


/********************************************************
* If there was not a backslash (and therefore the	*
*     beginning directory is the root directory)	*
* begin 						*
*   copy command line argument into chk_str		*
*   copy root directory into dir_string 		*
* end							*
* else							*
* (if there was a backslash and therefore a beginning	*
*     directory specified in the command line)		*
* begin 						*
*   set the usr_file to the next character		*
*	past the backslash				*
*   copy the usr_file into chk_str			*
*   copy the command line argument into 		*
*	dir_string					*
*   terminate dir_string just after the 		*
*	last backslash (therefore leaving only the	*
*	the directory name in the string)		*
* end							*
********************************************************/

if (last_loc == NULL)
    {
    /**
    * Since no backslash we'll still check for a drive designator LC 2.14 -JT
    **/
    last_loc = strchr(dos_parm, colon);
    if (last_loc == NULL)
	{
	strcpy(chk_str,dos_parm);
	strcpy(dir_string,"\\");
	}
    else
	{
	usr_file = last_loc + 1;
	strcpy(chk_str,usr_file);
	strcpy(dir_string,dos_parm);
	lst_dchr = usr_file - dos_parm;
	dir_string[lst_dchr++] = backslash;	/* Fake path */
	dir_string[lst_dchr]   = end_of_string; /* terminate directory str */
	}
    }
else
    {
    usr_file = last_loc + 1;
    strcpy(chk_str,usr_file);
    strcpy(dir_string,dos_parm);
    lst_dchr = usr_file - dos_parm;
    dir_string[lst_dchr] = end_of_string;
    }


/**
*   if the environment string was found concatenate environment drives
*	with the directory string that was extracted.
*   else look for just the directory string
**/

if ( flag.found)
    {
    for ( wi = 0; wi < env_cnt; wi++ )
	{
	/**
	*   Search each directory in the environment array
	**/
	strcpy( env_dir_string, envdrv[ wi ] );
	strcat( env_dir_string, dir_string );
	LOOK( env_dir_string );
	}
    }
else
    {
    /**
    *	There is no environment loop so look only for directory string
    **/
    LOOK(dir_string);
    }

return;

}

/**/
/*********************************************************
*	LOOK is the recursive procedure in WHERE	 *
*	It is called once for each subdirectory 	 *
*********************************************************/

LOOK(dir_string)
char *dir_string;
{

struct DTA cur_DTA;		/* used to return data from DOS  */

filename new_dstr;		/* the directory to be		 */
				/* searched on the next 	 */
				/* call to LOOK()		 */

filename cur_str;		/* temporary filename		 */
				/* string for searching for	 */
				/* directories			 */


/**
* Form cur_str by copying dir_string and		*
*    and then concatenating "*.*" to look through all   *
*    files						*
**/

strcpy(cur_str,dir_string);
strcat(cur_str,"*.*");

/**
* Set the Disk Transfer Area in DOS to the cur_DTA	*
*    structure						*
* Get the first subdirectory in this directory		*
**/

SET_DTA(&cur_DTA);
GET_FIRST(cur_str,directory_type);

/**
* while there are more subdirectories in this directory *
* begin 						*
*   double check for proper directories (see text)	*
*   if a directory					*
*   begin						*
*     set up the new_dstr for the			*
*     next call to LOOK (see text)			*
*     call LOOK 					*
*     reset Disk Transfer Address (see text)		*
*   end 						*
*   look for next directory				*
* end							*
**/

while (!(r8086.x.cflag & carry_set))
    {
    if (cur_DTA.DTA_attr == directory_type &&
       cur_DTA.DTA_filename[0] != '.')
	{
	strcpy(new_dstr,dir_string);
	strcat(new_dstr,cur_DTA.DTA_filename);
	strcat(new_dstr,"\\");
	LOOK(new_dstr);
	SET_DTA(&cur_DTA);
	}
    GET_NEXT();
    }

/********************************************************
* if there are no more subdirectories in this directory *
*   look for files					*
* else							*
*   print an error message				*
********************************************************/

if (r8086.x.ax == no_more_files)
    GET_FILES(dir_string,&cur_DTA);
else
    printf("problem with looking thru %s\n",dir_string);
return;

}


/********************************************************
* GET_FILES						*
* is called once per directory to look for the		*
*   actual files matching the search string		*
********************************************************/

GET_FILES(dir_string,cur_DTA)
char *dir_string;
struct DTA *cur_DTA;
{

filename cur_str;

/********************************************************
* Form cur_str by copying dir_string into  *
*   it and then concatenating the chk_str onto	   *
*   the end						*
********************************************************/

strcpy(cur_str,dir_string);
strcat(cur_str,chk_str);

/*********************************************************
* Get the first file that matches cur_str	 *
********************************************************/

GET_FIRST(cur_str,no_type);

/********************************************************
* while there are more files that match the search	*
*   string:						*
* begin 						*
*   print the file information				*
*   get the next file					*
* end							*
********************************************************/

while (!(r8086.x.cflag & carry_set))
    {

    /*  If we have gotten a screenful pause for a keystroke */
    if (++count==24) {	
	printf("\t\t<<Hit any key to continue or Q to abort>>");
	count =	getch();
	printf("\r\t\t\t\t\t\t\t\t\r");
	if ((count == 'Q') || (count == 'q'))
		exit(1);
	count = 0;
	}

    printf(" %10ld %s   %s%s\n", (*cur_DTA).DTA_size, 
			DATE(&((*cur_DTA).DTA_date)),
			dir_string, (*cur_DTA).DTA_filename);
    GET_NEXT();
    }

/*********************************************************
* if error in looking for a file			*
*    print error message and return			*
********************************************************/

if (r8086.x.ax != no_more_files)
    printf("problem with looking for %s\n",cur_str);

return;

}

/********************************************************
* GET_NEXT does an interrupt 21h, function 4Fh		*
* Function 4fh is Get next directory entry		*
********************************************************/

GET_NEXT()

{
r8086.x.ax = 0x4f00;

dos_result = int86x( 0x21, &r8086, &r8086, &s8086 );

return;
}


/********************************************************
* SET_DTA does an interrupt 21h, function 1Ah		*
*   The DS:DX pair is set to the address of the 	*
*   cur_DTA data structure			    *
********************************************************/

SET_DTA(cur_DTA)
struct DTA	*cur_DTA;
{

r8086.x.ax = 0x1a00;
r8086.x.dx = (int)cur_DTA;		/* set offset to disk transfer area */

dos_result = int86x(0x21, &r8086, &r8086, &s8086 );

return;

}

/********************************************************
* GET_FIRST does an interrupt 21h, function 4Eh 	*
*   The CX register is set to either normal or		*
*   directory type (see text)				*
*   The DS:DX pair is set to the address of the 	*
*   search string					*
********************************************************/

GET_FIRST(sea_str,filetype)
char *sea_str;
int filetype;
{

r8086.x.ax = 0x4e00;				/* Set DOS function	 */
r8086.x.cx = filetype;				/* Set search attributes */
r8086.x.dx = (int)    sea_str;		  /* Set address of string */

dos_result = int86x( 0x21, &r8086, &r8086, &s8086 );

return;

}

/********************************************************
* DATE takes the date field from the current DTA	*
*   structure and returns a string containing the	*
*   information in formatted ASCII			*
********************************************************/

char *DATE(dateptr)
struct msdos_date *dateptr;
{

sprintf(date_str, "%02d-%02d-%02d %02d:%02d %s",
	dateptr->ms_month, dateptr->ms_day,
	dateptr->ms_year+80, (dateptr->ms_hour)%12,
	dateptr->ms_min, time_of_day[((dateptr->ms_hour)/12)]);
return(date_str);

}

char *space(where)
int where;
{
	spce[lastzero] = ' ';
	spce[where] = '\0';
	return spce;
}
                                                                                                                                                                                  
/* Copyright (C) 1981,1982, 1983 by Manx Software Systems */
#include <errno.h>
#include <fcntl.h>
#ifndef NULL
#define NULL ((void *)0)
#endif

char *get_first(), *get_next(), *sbrk();

#define ARGMAX 256
static char *Argv[ARGMAX];
static int argvsize;
static int Argc;
static char curr_path[128];

noper()
{
	return 0;
}

int (*cls_)() = noper;
extern char _ioflg[];

Croot(cp, first)
register char *cp;
{
	register char *cp2;
	char *save;
	char *wild_match;
	char *index(),*rindex(), *save_str();
	char *path,*copy; int j;

	_ioflg[0] = isatty(0);	/* set flag for i/o routines */
	_ioflg[1] = isatty(1);	/* set flag for i/o routines */
	_ioflg[2] = isatty(2);	/* set flag for i/o routines */


	/* Null out first argument */
	Argv[0] = "";
	Argc = first;

	/* loop through arguments */
	for (;;) 
	{
		/* skip blanks */
		while (*cp == ' ' || *cp == '\t')
			++cp;

		/* if you're at the end of command line, you're done */
		if (*cp == 0)
			break;

		/* find beginning of next argument */
		cp2 = cp;	/* save original pointer to the string */
		*cp2 = (*cp2 == '/' ? '\\' : *cp2);
		while (*++cp2)
		{
			/* if you hit a space char - stick a null in to terminate last
			   argument
			 */
			if (*cp2 == ' ' || *cp2 == '\t') 
			{
				*cp2++ = 0;
				break;
			}
			*cp2 = (*cp2 == '/' ? '\\' : *cp2);
		}

		/* if no wild card characters, do it the old fashioned way */
		if (index(cp,'*') == NULL && index(cp,'?') == NULL)
		{
			/* update the next argv pointer */
			Argv[Argc] = cp;
			/* bump the argument count */
			if (++Argc == ARGMAX)
				abort();
		}
		else
		{
			/* if there is a path included, save it off */
			if ((path = rindex(cp,'\\')) || (path = rindex(cp,'/')))
			{
				copy = cp;
				/* copy to curr_path, mapping / to \ */
				for (j = 0; j < sizeof(curr_path) && copy != path+1; copy++,j++)
					curr_path[j] = (*copy == '/' ? '\\' : *copy);
				/* terminate string */
				curr_path[j] = '\0';
			}
			else
			/* null path */
				curr_path[0] = 0;
			if (wild_match = get_first(cp))
			{
				/* update the next argv pointer */
				Argv[Argc]= save_str(wild_match);
				/* bump the argument count */
				if (++Argc == ARGMAX)
					abort();
				/* get the rest of the matching file names */
				while (wild_match = get_next())
				{
					
					/* update the next argv pointer */
					Argv[Argc] = save_str(wild_match);
					/* bump the argument count */
					if (++Argc == ARGMAX)
						abort();
				}
			}
		}
		cp = cp2;	/* point to beginning of next argument */
	}
	Argv[Argc] = NULL;	
	main(Argc,Argv);
	exit(0);
}

char *save_str(s)
	register char *s;
{
	register char *r;
	int pathlen;
	/* squirrel away matched file name */
	if (NULL == (r = sbrk(strlen(s)+(pathlen = strlen(curr_path))+1)))
		abort();
	strcat(curr_path,s);
	strcpy(r,curr_path);
	curr_path[pathlen] = '\0';
	return r;
}

abort()
{
	write(2, "Too many args.", 14);
	_exit(200);
}

exit(code)
{
	(*cls_)();
	_exit(code);
}

typedef struct
{
	char dos_reserved[21];
	char attribute;
	unsigned file_time;
	unsigned file_date;
	long file_size;
	char file_name[13];
} fcb;
fcb wildcard;

char *get_first(fname)
	char *fname;
{
	register int result;
	/* set the disk transfer address */
	bdos(0x1A,&wildcard);
	result = bdos(0x4E,fname,0);
	/* make the find first call */
	if(2 == result || 18 == result)
		return NULL;
	return &(wildcard.file_name[0]);
}

char *get_next(fname)
	char *fname;
{
	register int result;
	/* set the disk transfer address */
	bdos(0x1A,&wildcard);
	result = bdos(0x4f,0,0);
	/* make the find next call */
	if (18 == result)
		return NULL;
	return &(wildcard.file_name[0]);
}

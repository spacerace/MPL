#include <stdio.h>

typedef struct
{
	char attribute;
	unsigned file_time;
	unsigned file_date;
	long file_size;
	char file_name[13];
} file_desc;

typedef struct
{
	char dos_reserved[21];
	file_desc file;
} fcb;

#define maxfiles 64 

char printbuf[256];
int mode = 0x10;
file_desc *get_first(),*get_next();
char *index(), *rindex();
int verbose=0,column=4,recurse=0;

main(argc,argv)
char *argv[];
{
	int noargs;
	char *current;
	char namebuf[128];
	/*
	 * set up a default search name
	 */
	if (noargs = (argc == 1))
		argc++;
	while(--argc)
	{
		if (noargs)
			current = "*.*";
		else
			current = *(++argv);	/* get current file name */
		if (*current == '-')
		{
			++current;	/* point past - */
			while (*current)
			{
				switch (*current++)
				{
				case 'l':
				case 'L':
					verbose = 1;
					if (column != 1)
						column = 2;
					break;
				case 'c':
				case 'C':
					column = 1;
					break;
				case 'a':
				case 'A':
					mode = 0x2 + 0x4 + 0x10;
					break;
				case 'r':
				case 'R':
					recurse = 1;
					mode = 0x2 + 0x4 + 0x10;
					break;
				default:
					break;
				}
			}
			/* if we're down to one argument after looking at all the
			   switches, we need to set noargs to true */
			if (noargs = (argc == 1))
				argc++;
			continue;
		}
		if (get_first(current)->attribute & 0x10 &&
			NULL == index(current,'?') && NULL == index(current,'*'))
		{
			strcpy(namebuf,current);
			strcat(namebuf,"\\*.*");
			current = namebuf;
		}
		do_dir(current);
	}
}

do_dir(current)
	char *current;
{
	file_desc files[maxfiles];	/* as many as we'll likely need */
	file_desc *curr_file,*get_next();
	unsigned int time,date;
	int i,j,col;
	int files_cmp();
	char atts[4]; /* drw */
	/* look for match */
	i = 0;
	if (!(curr_file = get_first(current)))
	{
		printf(stderr,"ls : no files matching %s\n",current);
		return;	
	}
	files[i++] = *curr_file;
	/* get all matching */
	while ((curr_file = get_next()) && i < maxfiles)
		files[i++] = *curr_file;	
	if (i > 1)
		qsort(files,i,sizeof(file_desc),files_cmp);
	printf("%s\n",current);
	col = 1;
	for (j = 0; j < i; j++)
	{
		register char *c = files[j].file_name;
		if (*c == '.')
			continue;	/* filter out . and .. */
		while (*c)
		{
			*c = tolower(*c);
			c++;
		}
		if (verbose)
		{
			register char att = files[j].attribute;
			atts[3] = 0;	/* terminate string */
			atts[0] = att & 0x10 ? 'd' : '-';
			atts[1] = att & 2 ? '-' : 'r';
			atts[2] = att & 1 ? '-' : 'w';
			if (atts[0] == 'd')
			{
				register int k;
				sprintf(printbuf,"%s %s\\",atts,files[j].file_name);
				write(1,printbuf,strlen(printbuf));
				k = 12 - strlen(files[j].file_name)+7;
				while(k--)
					write(1," ",1);

			}
			else
			{
				sprintf(printbuf,"%s %-12s %-6ld ",atts,files[j].file_name,
						files[j].file_size);
				write(1,printbuf,strlen(printbuf));
			}
			time = files[j].file_time;
			date = files[j].file_date;
			sprintf(printbuf,"%02d/%02d %02d:%02d ",
				((date >> 5) & 0x0F),	/* month */
				date & 0x1F,		/* day	*/
				(time >> 11) & 0x1F,		/* hours */
				(time >> 5) & 0x3F);		/* minutes */
			write(1,printbuf,strlen(printbuf));
		}
		else
			if (files[j].attribute & 0x10)
			{
				register int k;
				sprintf(printbuf,"%s\\",files[j].file_name);
				write(1,printbuf,strlen(printbuf));
				k = 16 - strlen(files[j].file_name);
				while(--k)
					write(1," ",1);

			}
			else
			{
				sprintf(printbuf,"%-13s   ",files[j].file_name);
				write(1,printbuf,strlen(printbuf));
			}
		if (col == column)
		{
			col = 1;
			write(1,"\r\n",2);
		}
		else
			col++;
	}
	write(1,"\r\n",2);
	if (recurse)
		for (j = 0; j < i; j++)
		{
			/* we've got a subdirectory */
			if (files[j].attribute & 0x10 && files[j].file_name[0] != '.')
			{
				char *path;
				char dirname[48];
				if (!strcmp(current,"*.*"))
					dirname[0] = '\0';
				else
					strcpy(dirname,current);
				if (path = rindex(dirname,'\\'))
					*(++path) = '\0';
				strcat(dirname,files[j].file_name);	/* get name */
				strcat(dirname,"\\*.*");
				do_dir(dirname);
			}
		}
}
files_cmp(a,b)
	file_desc *a,*b;
{
	return strcmp(a->file_name,b->file_name);
}

fcb tmp;

file_desc *get_first(fname)
	char *fname;
{
	register int result;
	/* set the disk transfer address */
	bdos(0x1A,&tmp);
	result = bdos(0x4E,fname,mode);
	/* make the find first call */
	if(2 == result || 18 == result)
		return NULL;
	return &(tmp.file);
}

file_desc *get_next()
{
	register int result;
	/* set the disk transfer address */
	bdos(0x1A,&tmp);
	result = bdos(0x4f,0,0);
	/* make the find next call */
	if (18 == result)
		return NULL;
	return &(tmp.file);
}

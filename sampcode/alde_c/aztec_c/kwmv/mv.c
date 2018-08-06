#include <stdio.h>
/* mv.c - implements a version of UNIX mv */
char usage[] = "mv : usage mv file1 [file2 . . fileN] target\r\n";
char target_name[128];
main(argc,argv)
	int argc;
	register char *argv[];
{
	char *target,*fname_part();
	register int i;
	if (argc < 3)
	{
		write(2,usage,sizeof(usage));
		exit(-1);
	}
	target = argv[argc-1];
	/* kill trailing backslashes */
	if (target[i = strlen(target)-1] == '\\')
		target[i] = '\0';
	if (argc == 3)
	{
		/* if the target doesn't exist and it's not a directory then rename */
		if (access(target,0) && !dirp(target))
		{
			fprintf(stderr,"moving %s to %s\n",argv[1],argv[2]);
			rename(argv[1],argv[2]);
		}
		else
		{
			/* if the target is a directory copy to same name that directory */
			if (dirp(target))
			{
				int len;
				strcpy(target_name,target);
				if (target_name[(len = strlen(target_name))-1] != '\\')
				{
					target_name[len = strlen(target_name)] = '\\';
					target_name[len+1] = '\0';
				}
				strcat(target_name,fname_part(argv[1]));
				fprintf(stderr,"moving %s to %s\n",argv[1],target_name);
				filecopy(target_name,argv[1]);
				unlink(argv[1]);
			}
			else
			{
				fprintf(stderr,"moving %s to %s\n",argv[1],target);
				filecopy(target,argv[1]);
				unlink(argv[1]);
			}
		}
		exit(0);
	}
	if (!dirp(target))
	{
		fprintf(stderr,"mv : %s isn't a directory\n",target);
		exit(-1);
	}
	for (i = 1; i < argc-1; i++)
	{
		int len;
		strcpy(target_name,target);
		if (target_name[(len = strlen(target_name))-1] != '\\')
		{
			target_name[len = strlen(target_name)] = '\\';
			target_name[len+1] = '\0';
		}
		strcat(target_name,fname_part(argv[i]));
		if (!filep(argv[i]))
		{
			fprintf(stderr,"mv : %s isn't a file\n",argv[i]);
			continue;
		}
		fprintf(stderr,"moving %s to %s\n",argv[i],target_name);
		filecopy(target_name,argv[i]);
		unlink(argv[i]);
	}
}
#include <fcntl.h>
char buffer[BUFSIZ*16];
filecopy(target,source)
	char *target,*source;
{
	int t,s,r;
	if (-1 == (s = open(source,O_RDONLY)))
	{
		fprintf(stderr,"mv : can't open %s\n",target);
		exit(-1);
	}
	if (-1 == (t = open(target,O_TRUNC)))
	{
		fprintf(stderr,"mv : can't open %s\n",target);
		exit(-1);
	}
	while(0 != (r = read(s,buffer,BUFSIZ*16)) && r != -1)
	{
		if(-1 == write(t,buffer,r))
		{
			fprintf(stderr,"mv : error writing %s\n",target);
			exit(-1);
		}
	}
	close(t); close(s);
}
#include <errno.h>
typedef struct
{
	char dos_reserved[21];
	char attribute;
	unsigned file_time;
	unsigned file_date;
	long file_size;
	char file_name[13];
} fcb;
fcb dir;

dirp(s)
	char *s;
{
	register int result1,result2;
	/* set the disk transfer address */
	bdos(0x1A,&dir);
	/* do a search first for the directory path */
	return (bdos(0x4E,s,0x10) == 0 && bdos(0x4E,s,0) != 0);
}

filep(s)
	char *s;
{
	register int result1,result2;
	/* set the disk transfer address */
	bdos(0x1A,&dir);
	/* do a search first for the directory path */
	return bdos(0x4E,s,0) == 0;
}
char *fname_part(s)
	char *s;
{
	char *r,*rindex();
	if (r = rindex(s,'\\'))
	{
		return r+1;
	}
	return s;
}

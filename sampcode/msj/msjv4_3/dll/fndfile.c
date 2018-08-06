
/*  Source code for FNDFILE.C */


#define	INCL_ERRORS

#include	<stdio.h>
#include	<stdlib.h>
#include	<os2.h>
#include	<dos.h>
#include	<string.h>


USHORT	far	_loadds	pascal debug(USHORT);
USHORT	far	_loadds pascal do_find(PSZ, PHDIR, USHORT,
		PFILEFINDBUF, USHORT, PUSHORT, ULONG);
USHORT	far	_loadds pascal do_list(PSZ, PHDIR, USHORT,
		PFILEFINDBUF, USHORT, PUSHORT, ULONG);

void	do_usage(void);
void	main(void);

void
show(PFILEFINDBUF f_ptr, USHORT cnt)
{
	while (cnt--)
	{
		printf("Len is: %2d. Filename is:%s\n",
				(int)(f_ptr->cchName), f_ptr->achName );
		f_ptr = (PFILEFINDBUF) ((char far *)f_ptr + 24 +
				(int)(f_ptr->cchName));
	}
}

void
do_usage()
{
	printf("\n\nexit        - to exit\n");
	printf("file=<filename> - enter name of message file\ \n");
	printf("hand=<val>      - 1 = default, 0xffff = create\  new handle\n");
	printf("attrb=<attrb>   - Attributes: see pg 98 of Ref\  Manual\n");
	printf("len=<buf_len>   - length of output buffer to\  use. Allocated\n");
	printf("cnt=<cnt>       - Maximum number of files to\  return\n");
	printf("debug=on|off    - turn debug mode on or\  off\n");
	printf("show            - show buffer contents...\n");
	printf("list            - show current parameter\  settings\n");
	printf("go              - call DosFindFirst()\n");
}

void
main()
{
CHAR		tmp_buf[256];
CHAR		name[64];
USHORT	max_len = 0;
char		*buf_ptr = (char *)NULL;
HDIR		handle = 1;
USHORT	attrb = 0;
USHORT	num_files = 0;

	if(!debug(FALSE))
	{
		printf("Error in initial call to debug\n");
		exit(1);
	}
	*name = (CHAR)NULL;
	while(TRUE)
	{
		printf(">");
		gets(tmp_buf);
		strlwr(tmp_buf);
		if(!strncmp(tmp_buf, "exit", 4))
			exit(1);
		else
		if(!strncmp(tmp_buf, "file=", 5))
			strcpy(name, tmp_buf + 5);
		else
		if(!strncmp(tmp_buf, "len=", 4))
		{
			if(buf_ptr)
				free(buf_ptr);
			max_len = atoi(tmp_buf + 4);
			buf_ptr = calloc(max_len, 1);
		}
		else
		if(!strncmp(tmp_buf, "cnt=", 4))
			num_files = atoi(tmp_buf + 4);
		else
		if(!strncmp(tmp_buf, "hand=", 5))
			sscanf(tmp_buf + 5, "%x", &handle);
		else
		if(!strncmp(tmp_buf, "attrb=", 6))
			sscanf(tmp_buf + 6, "%x", &attrb);
		else
		if(!strncmp(tmp_buf, "list", 4))
			do_list(name, (PHDIR)&handle, attrb,
			(PFILEFINDBUF)buf_ptr, max_len,
			(PUSHORT)&num_files, (ULONG)0);
		else
		if(!strncmp(tmp_buf, "debug=", 6))
		{
			if(!debug(!strncmp(tmp_buf + 6, "on", 2)))
			{
				printf("Error in subsequent call to\  debug\n");
				exit(1);
			}
		}
		else
		if(!strncmp(tmp_buf, "show", 4))
			show((PFILEFINDBUF)buf_ptr, num_files);
		else
		if(!strncmp(tmp_buf, "go", 2))
			do_find(name, (PHDIR)&handle, attrb,
			(PFILEFINDBUF)buf_ptr, max_len,
			(PUSHORT)&num_files, (ULONG)0);
		else
		if(*tmp_buf == '?')
			do_usage();
		else
			printf("?Huh?\n");
	}

}


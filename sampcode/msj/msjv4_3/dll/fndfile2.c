/*  Source code for FNDFILE2.C */


#define	INCL_BASE
#define	INCL_ERRORS
#define	INCL_DOS
#define	INCL_DOSMISC
#define	INCL_DOSMODULEMGR

#include	<stdio.h>
#include	<stdlib.h>
#include	<os2.h>
#include	<dos.h>
#include	<string.h>

void show(PFILEFINDBUF f_ptr, USHORT cnt);

USHORT far _loadds pascal do_list(PSZ name_ptr, PHDIR
							hptr, USHORT attrb,
							PFILEFINDBUF
							buf_ptr,
							USHORT buf_len,
							PUSHORT num_ptr,
							ULONG reserved)
{
USHORT	stat;
HDIR		save_handle = *hptr;
USHORT	save_num = *num_ptr;

	printf("File name: %s\n", name_ptr);
	printf("Buffer address is %lx\n", buf_ptr);
	printf("Buffer length: %d\n", buf_len);
	printf("File count: %d, address: %ld\n",
			*num_ptr, num_ptr);
	printf("Handle is: %d\n", *hptr);
	printf("Attribute: %d\n", attrb);

	stat = DosFindFirst(name_ptr, hptr, attrb, buf_ptr,
					buf_len, num_ptr, reserved);
	printf("Return status was:%d, cnt was:%d\n", stat,
			 *num_ptr);
	*hptr = save_handle;
	*num_ptr = save_num;
	return(stat);
}



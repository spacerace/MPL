
/***********************************************************************/
/* BINCOMP:  The Telemacus file comparison utility                     */
/* Copyright (c), 3/88, Telemacus Software Associates                  */
/* Garry J. Vass  [72307,3311]                                         */
/*                                                                     */
/* Usage:  BINCOMP {file spec}  {file spec}                            */
/*                                                                     */
/* Source:  Turbo C (1.5), Large model                                 */
/*                                                                     */
/***********************************************************************/


#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <ctype.h>

/*#define MICROSOFT 1*/
#define TURBOC 1
#ifdef MICROSOFT
#undef  TURBOC
#define COMPILER_NAME    "(MSC-5.1)"
#define _close(x)         close(x)
#define _open(x, y)       open(x, y)
#define _read(x, y, z)    read(x, y, z)
#include <dos.h>
#endif
#ifdef TURBOC
#define  COMPILER_NAME "(TC-1.5)"
#endif

#define PROGRAM_NAME  "BINCOMP:  "
#define BUFFERSIZE    ((int)  (((unsigned) - 1) >> 1))
#define IMIN(a,b)     ((a<b?a:b))
#define CHARVAL(a)    ((isprint(a)?a:46))

typedef struct bincomp_file_type
    {
	unsigned char    *bc_file_name;
	int               bc_file_handle;
	int               bc_read_result;
	long              bc_count_of_total_bytes;
	unsigned char    *bc_index_to_buffer;
	unsigned char    *bc_buffer;
	}
	bc_file_type;

long	     file_offset               = 0L;
int          loop_control              = 0;
int          count_of_lines            = 0;
bc_file_type file1;
bc_file_type file2;
long         count_of_differences      = 0L;

void pause
    (
	bc_file_type *f1,
	bc_file_type *f2
	)
{
++count_of_lines;
if (count_of_lines > 18)
    {
    printf("\n\n%sPausing.  Hit almost any key...", PROGRAM_NAME);
    (void) getch();
    printf("\n\n");
    printf("%sComparing %s to %s\n", PROGRAM_NAME, f1 -> bc_file_name, f2 -> bc_file_name);
    count_of_lines = 0;
    }
}

void initialize_file_record
    (
	unsigned char      *fname,
	bc_file_type       *frec
	)
{
frec -> bc_file_name = (unsigned char *) malloc(strlen(fname));
strcpy(frec -> bc_file_name, fname);
frec -> bc_file_handle = open(frec -> bc_file_name, O_RDWR | O_BINARY);
if (frec -> bc_file_handle < 0)
    {
	printf("%sCannot open %s\n", PROGRAM_NAME, frec -> bc_file_name);
	abort();
	}
frec -> bc_buffer = (unsigned char *) malloc (BUFFERSIZE);
if (!frec -> bc_buffer)
    {
	printf("%sUnable to get memory\n", PROGRAM_NAME);
	abort();
	}
frec -> bc_count_of_total_bytes = 0L;
}

void read_into_buffer
    (
	bc_file_type *frec
	)
{
frec -> bc_read_result = read(frec -> bc_file_handle,
                              frec -> bc_buffer,
                              BUFFERSIZE);
frec -> bc_index_to_buffer       = frec -> bc_buffer;
frec -> bc_count_of_total_bytes += frec -> bc_read_result;
}

main
    (
    int argc,
    unsigned char *argv[]
    )
{
printf("%sThe Telemacus file comparison utility %s\n\n",
         PROGRAM_NAME,
		 COMPILER_NAME);
if (argc < 3)
    {
	printf("Usage  BINCOMP  {file specification}  {file specification}\n\n");
    printf("\tTwo file names are required in the command tail\n");
    printf("\twhich specify the names of the files you want to\n");
    printf("\tcompare.\n\n");
    exit(0);
    }

initialize_file_record(argv[1], &file1);
initialize_file_record(argv[2], &file2);

read_into_buffer(&file1);
read_into_buffer(&file2);

printf("%sComparing %s to %s\n",
        PROGRAM_NAME,
		file1.bc_file_name,
		file2.bc_file_name);

while ((file1.bc_read_result) && (file2.bc_read_result))
    {
    for (loop_control=0;
		 loop_control<IMIN(file1.bc_read_result, file2.bc_read_result);
		 ++loop_control)
		{
		if (*file1.bc_index_to_buffer != *file2.bc_index_to_buffer)
			{
    		printf("Offset:  %5ld   %s:  %02X (%c)   %s:  %02X (%c)\n",
				 file_offset,
				 file1.bc_file_name,
				 *file1.bc_index_to_buffer,
				 CHARVAL(*file1.bc_index_to_buffer),
				 file2.bc_file_name,
				 *file2.bc_index_to_buffer,
				 CHARVAL(*file2.bc_index_to_buffer));
			++count_of_differences;
            pause(&file1, &file2);
			}
		++file1.bc_index_to_buffer;
		++file2.bc_index_to_buffer;
		++file_offset;
		}
	read_into_buffer(&file1);
    read_into_buffer(&file2);
	}

(void) close(file1.bc_file_handle);
(void) close(file2.bc_file_handle);

printf("\n\n");
printf("%s%ld bytes were read from %s\n", PROGRAM_NAME, file1.bc_count_of_total_bytes, file1.bc_file_name);
printf("%s%ld bytes were read from %s\n", PROGRAM_NAME, file2.bc_count_of_total_bytes, file2.bc_file_name);
printf("%s%ld bytes were different\n",PROGRAM_NAME, count_of_differences);
printf("%sProcessing complete\n",PROGRAM_NAME);
return(0);
}
/***********************************************************************/
/* end of bincomp.c                                                    */
/*                                                                     */
/*                                                                     */
/*                                                                     */
/***********************************************************************/


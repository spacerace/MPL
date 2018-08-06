#include "string.h"
#include <stdio.h>
#include "io.h"
#include "fcntl.h"
#include "malloc.h"

#define CONVERT(x,y) (x=y,(char*)&x)
/* The SHUFFLE macros move the buffer pointer x to a specified
position y, convert the value there to the proper format, and then
put the buffer pointer back at its original position */

#define SHUFFLE(x,y) ((char*)x+y)
#define ISHUFFLE(x,y) *((int*)((char*)x+y))
#define DSHUFFLE(x,y) *((double*)((char*)x+y))
						

int BIFF_fp;		/* file handle */
 
typedef struct {
	unsigned long a:24;
} ATTR;
typedef char BYTE;



int create_BIFF(char *filename)  

/* Creates a new BIFF file, replacing any files by the same name.  The
entire filename is required (basename + suffix) */

{
	int i;
	if ((BIFF_fp = open(filename, O_RDWR | O_TRUNC | O_BINARY | O_CREAT, 0000400|0000200)) >= 0) {

		/* This section writes the BOF record */
		write(BIFF_fp, CONVERT(i,9), (unsigned)2);
		write(BIFF_fp, CONVERT(i,4), (unsigned)2);
		write(BIFF_fp, CONVERT(i,2), (unsigned)2);
		write(BIFF_fp, CONVERT(i,0x10), 2);
	}
	return(BIFF_fp);
}

int open_BIFF(char *filename)		
/* Opens an existing BIFF file in readonly mode */
{
	return(BIFF_fp = open(filename, O_RDONLY | O_BINARY));
}

void dimension_BIFF(int frow, int lrow, int fcol, int lcol)
/* sets the dimension record in the BIFF file */
{
	int i;
	
		/* This section writes the dimension record */
	write(BIFF_fp, CONVERT(i,0), 2);
	write(BIFF_fp, CONVERT(i,8), 2);
	write(BIFF_fp, CONVERT(i,frow), 2);
	write(BIFF_fp, CONVERT(i,lrow+1), 2);
	write(BIFF_fp, CONVERT(i,fcol), 2);
	write(BIFF_fp, CONVERT(i,lcol+1), 2);
}
	
void save_BIFF(int type, int row, int col, char *data)
/* saves data to a previously created BIFF file */
{
	unsigned int i;
	static ATTR attribute = {0x000000};
	
	switch(type) {
		case 1:
			/* Writes a blank cell */
			write(BIFF_fp, CONVERT(i,1), 2);
		        write(BIFF_fp, CONVERT(i,7), 2);
			write(BIFF_fp, CONVERT(i,row), 2);
			write(BIFF_fp, CONVERT(i,col), 2);
			write(BIFF_fp, (char *) &attribute, 3);
			break;
		case 2:
			/* Writes integer data */
			write(BIFF_fp, CONVERT(i,2), 2);
		        write(BIFF_fp, CONVERT(i,9), 2);
			write(BIFF_fp, CONVERT(i,row), 2);
			write(BIFF_fp, CONVERT(i,col), 2);
			write(BIFF_fp, (char *) &attribute, 3);
			write(BIFF_fp, data, 2);
			break;
		case 4:
			/* Writes data of number type */
			write(BIFF_fp, CONVERT(i,3), 2);
		        write(BIFF_fp, CONVERT(i,15), 2);
			write(BIFF_fp, CONVERT(i,row), 2);
			write(BIFF_fp, CONVERT(i,col), 2);
			write(BIFF_fp, (char *) &attribute, 3);
			write(BIFF_fp, data, 8);
			break;
		case 8:
			/* Writes a label */
			write(BIFF_fp, CONVERT(i,4), 2);
		        write(BIFF_fp, CONVERT(i,8+strlen(data)), 2);
			write(BIFF_fp, CONVERT(i,row), 2);
			write(BIFF_fp, CONVERT(i,col), 2);
			write(BIFF_fp, (char *) &attribute, 3);
/* this next write depends on low byte order */			
			write(BIFF_fp, CONVERT(i,strlen(data)), 1);
			write(BIFF_fp, data, i);
			break;
		case 16:
			/* Writes boolean type data */
			write(BIFF_fp, CONVERT(i,5), 2);
		        write(BIFF_fp, CONVERT(i,9), 2);
			write(BIFF_fp, CONVERT(i,row), 2);
			write(BIFF_fp, CONVERT(i,col), 2);
			write(BIFF_fp, (char *) &attribute, 3);
			write(BIFF_fp, CONVERT(i,(*data?1:0)), 1);
			write(BIFF_fp, CONVERT(i,0), i);
			break;
	}
}

int close_BIFF()
/* Writes an EOF record, and closes the file */
{
	unsigned long int i = 0x000000a;
	
	write(BIFF_fp, (char *) &i, 4);
	return(close(BIFF_fp));
}

void read_BIFF(int gettype, int (*handler)(int, char *, int))
/* Reads all data records of a certain type */
{

	int type; 	/* the BIFF type of the currently read record */
	int length; 	/* the length of the current record */
	int intype;	/* the Library type of the current record */
	static BYTE buff[2080]; /* buffer containing the current record */
	static int map[7] = {0,1,2,4,8,16,32}; 
	int typeflag;
	BYTE boolflag;
	
	/* the map array serves to map the BIFF record types (e.g. 
		label = record type 4) to the Library record type
		(where label = type 8)
	*/
	
	do {
		read(BIFF_fp, (char *)&type, 2);
		read(BIFF_fp, (char *)&length, 2);
		intype = map[(type<7&&type>0) ? type : 0];
		switch((gettype & intype) | (intype & 32)) {
			case 1: case 2: case 4: case 8: case 16: 
				read(BIFF_fp, (char *) buff, length);
				(*handler)(intype, (char *) buff, length);
				break;
			case 32:
				read(BIFF_fp, (char *) buff, length);
				typeflag = ISHUFFLE(buff, 13);
				boolflag = *SHUFFLE(buff, 7);
				if (typeflag == 0xFFFF) {
				    if ((boolflag == 1) && (gettype & 16))
				        (*handler)(16, (char *) buff, length);
				} else if (gettype & 4)
					(*handler)(4, (char *) buff, length);
				break;
			default:
				lseek(BIFF_fp, (long) length, SEEK_CUR); 
			}
	} while(type != 0x0A);
}


int find_BIFF(int gettype, int row, int col, int (*handler)(int, char *, int))
{

	int type;	   /* the BIFF record type of the current record */
	int intype; 	   /* the Library record type of the current record */
	int length;	   /* the length of the current record */
	int rw, cl;	   /* the row and column of the current record */
	int flag = 0;	   /* used to determine if this is the first row 
				record in a certain block */
	int rwMic, rwMac;  /* the first and last(+1) row in a document */
	int colMic, colMac; /*the first and last(+1) column in a certain row*/
	int dbRtcell;	   /* the offset of the current row records */
	long int offset;   /* the cumulative offset for the row record */
	static BYTE buff[2080]; /* buffer containing the record contents */
	static long blockbuff[100]; 
			  /* contains row block indices, size is arbitrary */
	static int map[7] = {0,1,2,4,8,16,32}; 
			  /* Maps BIFF record types to Library record types */
	int typeflag;
	BYTE boolflag;

	/* Read the index record */
	lseek(BIFF_fp, 8L, SEEK_SET);
	read(BIFF_fp, (char *)&type, 2);
	if (type != 0x0B)
		return(1);	
	read(BIFF_fp, (char *)&length, 2);
	lseek(BIFF_fp, 4L, SEEK_CUR);
	read(BIFF_fp, (char *)&rwMic, 2);
	read(BIFF_fp, (char *)&rwMac, 2);	
	if(col < rwMic || col > rwMac-1)
		return(2);
	read(BIFF_fp, (char *)blockbuff, (rwMac-1-rwMic));

	/* move to the proper row block */
	lseek(BIFF_fp, blockbuff[(row-rwMic)/32] , SEEK_SET);
	do {
		/* Read the current Row record */
		read(BIFF_fp, (char *)&type, 2);
		if (type != 0x08) 
			return(3);
		read(BIFF_fp, (char *)&length, 2);
		read(BIFF_fp, (char *)&rw, 2);
		read(BIFF_fp, (char *)&colMic, 2);
		read(BIFF_fp, (char *)&colMac, 2);	
		
		lseek(BIFF_fp, 5L, SEEK_CUR);
		read(BIFF_fp, (char *)&dbRtcell, 2);

		/* If rgbAttr field is present, move over it */
		if (length == 20)
			lseek(BIFF_fp, 3L, SEEK_CUR);

		/* update offset (see BIFF document section 
		"Finding Cell Values" for more information */
		if (flag == 0) {
			offset = tell(BIFF_fp) + dbRtcell;
			flag = 1;
		} else
			offset += dbRtcell;
	} while (rw < row);

	if (rw == row) {
		if (col < colMic || col > colMac)
			return(4);
		lseek(BIFF_fp, offset, SEEK_SET);

		/* Read the row's records until the proper column is reached*/
		do {
			read(BIFF_fp, (char *)&type, 2);
			read(BIFF_fp, (char *)&length, 2);
			read(BIFF_fp, (char *)&rw, 2);
			read(BIFF_fp, (char *)&cl, 2);
			lseek(BIFF_fp, (long)(length-4), SEEK_CUR);
		}  while (cl < col);
		
		if (cl == col) {
			/* Proper record is found.  Call handler routine */
			intype = map[(type<7&&type>0) ? type : 0];
			switch((gettype & intype) | (intype & 32)) {
			    case 1: case 2: case 4: case 8: case 16:
				lseek(BIFF_fp, (long)(-length), SEEK_CUR);
				read(BIFF_fp, (char *) buff, length);
				(*handler)(intype, (char *) buff, length);
				return(0);
			    case 32:
				lseek(BIFF_fp, (long)(-length), SEEK_CUR);
				read(BIFF_fp, (char *) buff, length);
				typeflag = ISHUFFLE(buff, 13);
				boolflag = *SHUFFLE(buff, 7);
				if (typeflag == 0xFFFF) {
				    if ((boolflag == 1) && (gettype & 16))
				        (*handler)(16, (char *) buff, length);
				} else if (gettype & 4)
					(*handler)(4, (char *) buff, length);
				return(0);

			}
		}
	}
	return(5);
}
		
	
	

void get_blank(char *data, int *row, int *col)
{

	*row = ISHUFFLE(data,0);
	*col = ISHUFFLE(data,2);
}

void get_integer(char *data, int *row, int *col, unsigned int *value)
{
	int *ptr;

	*row = ISHUFFLE(data,0);
	*col = ISHUFFLE(data,2);
	*value = ISHUFFLE(data,7);
			
}

void get_number(char *data, int *row, int *col, double *value)
{
  /* We do not have to check for the formula record case because the
     number value begins at the seventh byte in both cases. */
	
	*row = ISHUFFLE(data,0);
	*col = ISHUFFLE(data,2);
	*value = DSHUFFLE(data,7);
}

void get_label(char *data, int *row, int *col, char **string)
{
	int length;
	
	*row = ISHUFFLE(data,0);
	*col = ISHUFFLE(data,2);
	length = *SHUFFLE(data,7);
	*string = (char *) malloc((length+1) * sizeof(char));
	strncpy(*string, SHUFFLE(data,8), length);
	strncpy((char*)*string+length, '\0', 1);
}

void get_bool(char *data, int *row, int *col, int *value)
{
	int typeflag;
	
	*row = ISHUFFLE(data,0);
	*col = ISHUFFLE(data,2);
	typeflag = ISHUFFLE(data, 13);
	if (typeflag == 0xFFFF)
		*value = *SHUFFLE(data, 9);
	else
		*value = *SHUFFLE(data,7);
}

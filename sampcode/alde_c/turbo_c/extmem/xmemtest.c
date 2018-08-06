#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <dos.h>

#define BLOCKSIZE 16384

int ext_mem_present( void );
unsigned get_size( void );
unsigned long get_loc( void );
unsigned copy( unsigned long source, unsigned long dest, unsigned nbytes );

char far *buf;
unsigned long ext_org;
unsigned ext_size;
unsigned long buf_org;

void init( void )
{
	ext_org = get_loc();
	printf( "Available extended memory begins at address %lX.\n", ext_org );

	ext_size = get_size();
	printf( "Extended memory consists of %uK bytes,\n", ext_size );
	ext_size -= (ext_org - 0x100000L) >> 10;
	printf( "\twith %uK bytes available for use.\n\n", ext_size );

	if ( ext_size == 0 )
		{
		printf( "No external memory present.\n" );
		exit(0);
		}

	if ( (buf = farmalloc( BLOCKSIZE )) == 0L )
		{
		printf( "Unable to allocate buffer.\n" );
		exit(0);
		}

	buf_org = (unsigned long)FP_SEG(buf)*16L + (unsigned long)FP_OFF(buf);
}

void main()
{
unsigned char far *ptr;
int wrong = 0, right = 0;
int i;

	init();

	for ( ptr = buf, i = 0; i < BLOCKSIZE; i++ )
		*ptr++ = i & 0xFF;

	if ( ext_size < 16 )
		{
		printf( "Insufficient extended memory available.\n" );
		exit(0);
		}

	printf( "Copying buffer to extended memory\n" );
	copy( buf_org, ext_org, BLOCKSIZE );

	printf( "Rewriting buffer contents\n" );
	for( ptr = buf, i = 0; i < BLOCKSIZE; i++ )
		*ptr++ = 0;

	printf( "Copying extended memory back to buffer\n" );
	copy( ext_org, buf_org, BLOCKSIZE );

	printf( "Comparing copied buffer with original\n" );
	for( ptr = buf, i = 0; i < BLOCKSIZE; i++ )
		if ( *ptr++ != (i & 0xFF) )
			wrong++;
		else
			right++;

	printf( "\tNumber wrong = %d\n\tNumber right = %d\n", wrong, right );
}

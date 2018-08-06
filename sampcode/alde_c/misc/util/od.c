#include <stdio.h>
#include <ctype.h>

/***
 *	od.c [09/30/84] - binary dump program, like UNIX utility.
 *
 *	Usage:
 *		od [-bcdoxm] [file] [+][offset][.][b]
 *			-b     - output octal bytes
 *			-c     - output character bytes
 *			-d     - output decimal words
 *			-o     - output octal words (default)
 *			-x     - output hex words
 *			-m     - pause for each output screen
 *			file   - input file (stdin default)
 *			offset - start offset
 *
 *		If 'file' is missing input is from standard input and
 *		'offset' must be preceeded by '+'.
 *		File is output starting at `offset` which is an octal
 *		value unless suffixed by `.`, in which case it is decimal.
 *		If `offset` is suffixed by `b` it is multiplied by 512.
 *		(UNIX block size)
 *
 *
 *		It is assumed that ints are two bytes long.
 *
 *		Compiles with CII-C86 ver 2.20d - others may need mods.
 */

/*
 *	Copyright, 1984, Selene Associates, Ltd. - All rights reserved.
 *
 *	Permission is granted for the copying and use of this program
 *	if the following conditions are met:
 *
 *		1)	The use is not for direct commercial benefit.
 *			Permission is NOT granted to sell this program
 *			or to incorporate it in any product or for use
 *			as a promotional tool or inducement to buy any
 *			product or service.
 *		2)	The user agrees that this program is distributed
 *			with no warrenties, expressed or implied, as to
 *			its suitablity for use for any purpose.  And the
 *			user assumes full responsibility for any damages
 *			resulting in the use or the inability to use this
 *			program.
 *		3)	That this entire comment block is included in any
 *			copies or modifications of this program.
 *
 *	If you find this program to be of use to you, a donation of
 *	whatever you think it is worth will be cheerfully accepted.
 *
 *	Written by: David L. Messer, Selene Associates, Ltd.
 *				P.O. Box 19130, Mpls, MN,  55119
 */

int moreflag ;
int nline ;
#define NLINE	23


int flags ;

#define OFLAG	0x01
#define OIND	0
#define DFLAG	0x02
#define DIND	1
#define XFLAG	0x04
#define XIND	2
#define CFLAG	0x08
#define CIND	3
#define BFLAG	0x10
#define BIND	4

int wideflag ;


union {
	unsigned char bbuf[16] ;
	unsigned wbuf[8] ;
	} buf1, buf2 ;


main( argc, argv )
int argc ;
char **argv ;
{
	register i,j,k,n ;
	FILE *stream ;
	char *cp ;
	long addr, acc, acc10 ;

	stream = stdin ;
	flags = 0 ;
	addr = acc = acc10 = 0L ;

	/*
	 * Get flags
	 */

	argv++ ;
	argc-- ;
	if( argc > 0 && **argv == '-' ) {
		cp = *argv ;
		argv++ ;
		argc-- ;

		while( *++cp ) {
			switch ( *cp ) {
				case 'b':
					flags |= BFLAG ;
					wideflag++ ;
					break ;

				case 'c':
					flags |= CFLAG ;
					wideflag++ ;
					break ;

				case 'd':
					flags |= DFLAG ;
					break ;

				case 'o':
					flags |= OFLAG ;
					break ;

				case 'x':
					flags |= XFLAG ;
					break ;

				case 'm':
					moreflag++ ;
					break ;

				default:
					goto usage ;
				}
			}
		}

	/*
	 * Get file name and offset.
	 */

	if( argc > 0 ) {
		if( **argv != '+' ) {
			if( (stream = fopen( *argv, "rb" )) == NULL ) {
				printf( "od: Can`t open %s\n", *argv ) ;
				exit( 1 ) ;
				}
			argv++ ;
			argc-- ;
			}

		if( argc ) {
			cp = *argv ;
			if( *cp == '+' ) cp++ ;

			while( isdigit( *cp ) ) {
				acc <<= 3 ;
				acc10 *= 10 ;
				acc10 += (long)(*cp - '0') ;
				acc += (long)(*cp - '0') ;
				cp++ ;
				}
			if( *cp == '.' ) {
				acc = acc10 ;
				cp++ ;
				}
			if( *cp == 'b' ) {
				acc *= 512L ;
				cp++ ;
				}
			if( *cp != '\0' ) goto usage ;
			argc-- ;
			}
		}
	if( argc > 0 ) goto usage ;

	if( flags == 0 ) flags = OFLAG ;


	/*
	 * Skip bytes for offset.
	 */

	addr = 0 ;
	while( addr != acc ) {
		if( fgetc( stream ) == EOF ) break ;
		addr++ ;
		}


	/*
	 * Print bytes, 16 at a time.
	 */

	n = fread( buf1.bbuf, 1, 16, stream ) ;
	while( n > 0 ) {
		if( n&1 ) buf1.bbuf[n] = 0 ;

		printf( "%07lo", addr ) ;

		for( i=0, k=flags; k; i++ ) {
			if( k&1 ) {
				for( j=0; j<n; j++ ) {
					switch( i ) {
						case OIND:
							if( wideflag ) putchar( ' ' ) ;
							printf( " %06lo",
									(long)(buf1.wbuf[j/2]) & 0xFFFFL ) ;
							j++ ;
							break ;

						case XIND:
							if( wideflag ) putchar( ' ' ) ;
							printf( "   %04lx",
									(long)(buf1.wbuf[j/2]) & 0xFFFFL ) ;
							j++ ;
							break ;

						case DIND:
							if( wideflag ) putchar( ' ' ) ;
							printf( "  %05ld",
									(long)(buf1.wbuf[j/2]) & 0xFFFFL ) ;
							j++ ;
							break ;

						case BIND:
							printf( " %03o", buf1.bbuf[j] ) ;
							break ;

						case CIND:
							if( isprint( buf1.bbuf[j] ) ) {
								printf( "   %c", buf1.bbuf[j] ) ;
								}
							else switch( buf1.bbuf[j] ) {
								case '\0':
									printf( "  \\0" ) ;
									break ;

								case '\b':
									printf( "  \\b" ) ;
									break ;

								case '\f':
									printf( "  \\f" ) ;
									break ;

								case '\n':
									printf( "  \\n" ) ;
									break ;

								case '\r':
									printf( "  \\r" ) ;
									break ;

								case '\t':
									printf( "  \\t" ) ;
									break ;

								default:
									printf( " %03o", buf1.bbuf[j] ) ;
								}
							break ;
						}
					}
				putchar( '\n' ) ;
				more() ;
				k >>= 1 ;
				if( k ) printf( "       " ) ;
				}
			else k >>= 1 ;
			}

		k = 0 ;
		addr += n ;
		if( n < 16 ) break ;
		for(;;) {
			n = fread( buf2.bbuf, 1, 16, stream ) ;
			j = 0 ;
			for( i=0; i<n; i++ ) {
				if( buf1.bbuf[i] != buf2.bbuf[i] ) j=1 ;
				}
			if( j==0 && k==0 && n > 0 ) {
				printf( "*\n" ) ;
				more() ;
				k++ ;
				}
			if( n!=16 || j!=0 ) break ;
			addr += n ;
			}

		for( i=0; i<n; i++ ) {
			buf1.bbuf[i] = buf2.bbuf[i] ;
			}
		}

	printf( "%07o\n", addr ) ;
	more() ;
	exit( 0 ) ;


	usage:
		printf( "od [-bcdoxm] [file] [+][offset][.][b]\n" ) ;
		printf( "	-b     - output octal bytes\n" ) ;
		printf( "	-c     - output character bytes\n" ) ;
		printf( "	-d     - output decimal words\n" ) ;
		printf( "	-o     - output octal words (default)\n" ) ;
		printf( "	-x     - output hex words\n" ) ;
		printf( "	-m     - pause for each output screen\n" ) ;
		printf( "	file   - input file (stdin default)\n" ) ;
		printf( "	offset - start offset\n" ) ;
		exit( 1 ) ;

	} /* main */





/**
 *	more - if moreflag is set, pause every screen.  Must be called
 *	       for every line-feed.
 */

more()
{
	char c ;

	if( !moreflag ) return ;

	if( ++nline < NLINE ) return ;
	nline = 0 ;
	printf( "--More--" ) ;
	while( (c = bdos(8)&0x7F) != ' ' && c != 'q' ) {
		}
	printf( "\r        \r" ) ;
	if( c == 'q' ) exit( 0 ) ;

	} /* more */

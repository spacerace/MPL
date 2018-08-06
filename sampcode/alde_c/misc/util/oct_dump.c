/*
 * od.c 1-16-85 kpc
 * this is a replica of the UNIX od (octal dump) command
 * with some extensions.
 *
 * the spec for this command comes from the 7th edition Berkely UNIX manual
 *
 * NOTE: this program should also dump directories. (But! Alas, it does not.)
 *
 * usage:
 *     od [-bcduoxDUOXw] [-B{odxc}] [-A{odx}] [file] [[+]offset[.][b]]
 *
 *     'file' - is the file to be dumped, if missing stdin is read.
 *     'offset' - specifies the offset into the file where dumping is to begin
 *	       this is normally in octal bytes, unless a period '.' is
 *	       appended to the offset then it is interpreted as decimal bytes
 *	       if a 'b' is appended to the offset then it is interpreted as
 *	       being the number of 512 byte blocks to skip.
 *	       If the file argument is omitted, the offset must be preceeded
 *	       by a '+'.  All offsets start with zero, the first byte of a
 *	       file is at offset zero, the first block of a file is at offset
 *	       zero.
 *
 *     dumping continues until end-of-file.
 *
 *     a - ???
 *     w - produce wide (132 column) output  this prints 24 bytes per line
 *	   versus 16 bytes per line.
 *
 *     b - interpret bytes in octal
 *     c - interpret bytes in ASCII. certain characters appear as C escapes;
 *	   null = \0, bs = \b, ff = \f, nl = \n, cr = \r, tab = \t others are
 *	   displayed as 3-digit octal number.
 *
 *     d - interpret shorts in decimal (16 bits)
 *     o - interpret shorts in octal   (16 bits)
 *     x - interpret shorts in hex     (16 bits)
 *
 *     D - interpret longs in decimal  (32 bits)
 *     O - interpret longs in octal    (32 bits)
 *     X - interpret longs in hex      (32 bits)
 *
 *     A{odx} - specifies the style that the offset should displayed in.
 *	   o - octal bytes, x- hex bytes, d - decimal bytes.
 *
 *     B{odxc} - specifies the style that bytes should be displayed in.
 *	   o - octal bytes, x- hex bytes, d - decimal bytes.
 *	       this is an alternative to the -b and -c opts above
 *
 *[1] EDIT 1	  1-21-85 kpc
 *[1]	  od now prints the offset 'loffset' after hitting EOF, this
 *[1]	  enables the user to determine the exact end of file.
 */

/*  #define DEBUG */

#include <stdio.h>
#include <ctype.h>

static char _sccsid[] ={ "@(#)od.c  v1.1  1-21-85  kpc"};

#define    OCT	   (0x01)
#define    DEC	   (0x02)
#define    USGN    (0x04)
#define    HEX	   (0x08)
#define    ASC	   (0x10)

#define    BLANK   (0x80)


#define    NARROW  (16)     /* number of bytes to print */
#define    WIDE    (24)     /* number of bytes to print */

#define    FALSE   (0)
#define    TRUE    (1)

#define    BLOCK_SIZ   (512)
#define    MAX_BUF     (48)

#define    SWITCHAR    '-'


/* mode for fopen() */
char   opn_mode[] ={ "rb"};

int    Off_prflg; /* whether or not the offset has been printed out yet */


main( argc, argv)
int    argc;
char   **argv;
{
   int	   ap,	   /* int pointer into argv */
	   fn_p,   /* where the file name is in argv */
	   off_p;  /* where the offset spec is in argv */

   int	   width;  /* width of output */

   int	   off_opt,    /* print fmt for the offset */
	   byt_opt,    /* print fmt for the byt specs */
	   sht_opt,    /* print fmt for the short specs */
	   lng_opt;    /* print fmt for the long specs */

   int	   num_byt;    /* number of bytes in the buffer */
   char    buf[ MAX_BUF + 1];  /* guess */
   char    l_buf[MAX_BUF + 1]; /* the last buf that was read in */
			       /* used to implement output compaction */
   int	   quiet_flg;  /* this flag keeps track of the number of duplicate */
		       /* output lines so we can shut up during blocks of  */
		       /* duplicate records  */

   FILE    *fp;


   long    loffset,	/* cur offset into file */
	   ioffset;	/* the initial offset */

   int	   eof;
   int	   c;	       /* input character */
   int	   msk;        /* msk to index the option flags */

   extern FILE *fopen();

   /* set defaults */
   off_opt = OCT;
   sht_opt = 0;

   byt_opt = 0;
   lng_opt = 0;

   fp = stdin;
   loffset = 0L;
   ioffset = 0L;

   width = NARROW;


   /* scan argv */
   /* set our flag/pointers */
   fn_p = -1;
   off_p = -1;

   /* now we're ready; if there were no opts, the loop falls through */
   /* and the defaults hold */
   for( ap = 1; ap < argc; ap++)
       if( argv[ap][0] == SWITCHAR )
       {
	   /* then process as an option */
	   /* we need a few local variables to rip the strings apart */
	   char    *cp;

	   /* here is a special case of only a '-' being entered */
	   if( argv[ap][1] == '\0')
	       /* this deserves the usage message */
	       pr_usage();

	   for( cp = &argv[ap][1]; *cp; cp++)
	       switch( *cp)
	       {
		   /* bytes as octal */
		   case 'b':
		       byt_opt |= OCT;
		       break;

		   /* bytes as... 'B' uses the next char as an argument */
		   case 'B':
		       switch( *(++cp))
		       {
			   case 'o':
			       byt_opt |= OCT;
			       break;

			   case 'd':
			       byt_opt |= DEC;
			       break;

			   case 'x':
			       byt_opt |= HEX;
			       break;

			   case 'c':
			       byt_opt |= ASC;
			       break;

			   default:
			       fprintf(stderr,
				   "od: bad option 'B' argument: (%c)\n",*cp);
			       pr_usage();
		       }
		       break;

		   /* bytes as asc */
		   case 'c':
		       byt_opt |= ASC;
		       break;


		   /* shorts as decimal */
		   case 'd':
		       sht_opt |= USGN;
		       break;

		   /* shorts as unsigned decimal */
		   case 'u':
		       sht_opt |= USGN;
		       break;

		   /* shorts as octal */
		   case 'o':
		       sht_opt |= OCT;
		       break;

		   /* shorts as hex */
		   case 'x':
		       sht_opt |= HEX;
		       break;


		   /* longs as decimal */
		   case 'D':
		       lng_opt |= USGN;
		       break;

		   /* longs as unsigned decimal */
		   case 'U':
		       lng_opt |= USGN;
		       break;

		   /* longs as octal */
		   case 'O':
		       lng_opt |= OCT;
		       break;

		   /* longs as hex */
		   case 'X':
		       lng_opt |= HEX;
		       break;


		   /* wide output */
		   case 'w':
		       width = WIDE;
		       break;

		   /* offset displayed as... */
		   case 'A':
		       /* look for an argument */
		       switch( tolower( *(++cp) ) )
		       {
			   case 'o':
			       off_opt = OCT;
			       break;

			   case 'd':
			       off_opt = DEC;
			       break;

			   case 'x':
			       off_opt = HEX;
			       break;

			   default:
			       fprintf(stderr,
				   "od: bad option 'A' argument: (%c)\n",*cp);
			       pr_usage();
		       }
		       break;

		   /* unknown option */
		   default:
		       /* this never returns */
		       pr_usage();
	       }

       }
       else if( argv[ap][0] == '+' )
       {
	   /* special offset indication */
	   off_p = ap;
       }
       else
	   if( fn_p == -1)
	   {
	       /* assume it is the file spec */
	       fn_p = ap;
	   }
	   else if( off_p == -1)
	   {
	       /* we can assume it is the starting offset spec */
	       off_p = ap;
	   }
	   else
	       pr_usage();

   /*=================================*/

   /* the default (short OCT) if nothing else is specified */
   if( byt_opt == 0 && sht_opt == 0 && lng_opt == 0 )
       sht_opt = OCT;

   /* now we can deal with the file name and the offset */
   /* file specified? */
   if( fn_p != -1)
       if( (fp = fopen( argv[fn_p], opn_mode)) == NULL)
       {
	   fprintf(stderr, "od: couldnt open input file (%s)\n", argv[fn_p]);
	   exit( 2);
       }

   /* offset specified? */
   if( off_p != -1)
   {
       /* now this could be a little trickier */
       /* need some local variables */
       char    *cp,	   /* rip through the string */
	       *opt;	   /* used to look for options */
       int     mult = 1,   /* multiplier for block offsets */
	       base = 8;   /* radix of offset */

       cp = argv[off_p];

       /* skip possible indicator */
       if( *cp == '+')
	   cp++;

       /* set opt to the end of string */
       opt = cp;
       while( *opt )
	   opt++;

       opt--;

       if( !isdigit( *opt & 0x7f) )
       {
	   /* could be a 'b' or a '.' */
	   if( tolower( *opt ) == 'b')
	   {
	       mult = BLOCK_SIZ;
	       opt--;
	   }

	   if( *opt == '.')
	       base = 10;
	   else if( !isdigit( *opt & 0x7f))
	   {
	       fprintf(stderr,"od: invalid offset spec: (%s)\n", cp);
	       exit( 3);
	   }
	   else
	       opt++;	   /* dont obliterate a good digit */

	   /* terminate the string */
	   *opt = '\0';
       }

       /* convert to integer */
       while( isdigit( *cp))
	   ioffset = ioffset * base + (*cp++ - '0');

       /* if its a block offset, then... */
       ioffset *= mult;
   }


   /*
    * all options have been dealt with and all flags set accordingly
    * we are ready to to the dump
    *  But first! we must seek to the right spot in the file
    */
   loffset = ioffset;

   if( ioffset != 0L )
   {
       int     fd;
       unsigned    cnt;
       char    eat_buf[ BLOCK_SIZ];

       fd = fileno( fp);

#ifdef DEBUG
       printf("ioffset is: %ld. 0%lo\n", ioffset, ioffset );

       if( isatty( fd) == 1)
	   printf("is tty\n");
#endif

       while( ioffset > 0L)
       {
	   cnt = (ioffset >= BLOCK_SIZ) ? (BLOCK_SIZ) : (ioffset);

	   if( read( fd, eat_buf, cnt ) != cnt )
	   {
	       fprintf(stderr,"od: unable to read %d bytes\n", cnt);
	       exit( 4);
	   }

	   ioffset -= cnt;
       }
   }


   /* ok, now we can dump the file */
   eof = FALSE;

#ifdef DEBUG
   printf("oopt: 0x%02x  bopt: 0x%02x sopt: 0x%02x lopt: 0x%04x\n",
       off_opt, byt_opt, sht_opt, lng_opt);
#endif

   /* clear out the last buffer */
   bclear( l_buf, MAX_BUF + 1);
   quiet_flg = -1;

   while( !eof )
   {
       bclear( buf, MAX_BUF + 1);

       /* get upto MAX_BUF bytes */
       for( num_byt=0; num_byt < width && (c = fgetc( fp)) != EOF; num_byt++)
	   buf[num_byt] = c;

       /* if EOF then remember it */
       if( c == EOF)
	   eof = TRUE;

       /*
	* the following ugly nested if stmnt implements the quiet
	* mode of operation.  ie only printing 2 duplicate lines,
	* and then a '*' line until a record that is different is read.
	*/
       if( bcmp( l_buf, buf, MAX_BUF + 1))
       {
	   if( quiet_flg == 2)
	       /* we have already printed the '*' */
	       continue;
	   else if( quiet_flg == 1)
	   {
	       /* then we don't need to dump this buf */
	       /* but we do need to say something */
	       printf("*\n");
	       quiet_flg = 2;
	       continue;
	   }
	   else if( quiet_flg == 0)
	       /* this and the prev rec are equal, dump it */
	       quiet_flg = 1;
	   else
	       quiet_flg = 0;
       }
       else
       {
	   /* they weren't equal */
	   quiet_flg = 0;
	   bcpy( l_buf, buf, MAX_BUF + 1);
       }


#ifdef DEBUG
       printf("num_byt: %d\n", num_byt);
#endif

       /* print the offset out */
       Off_prflg = FALSE;

       for( msk = OCT; msk <= ASC; msk<<=1)
       {
#ifdef DEBUG
	   printf("msk: 0x%02x\n", msk);
#endif
	   if( byt_opt & msk)
	   {
	       pr_off( off_opt, loffset);
	       pr_byte( byt_opt & msk, num_byt, buf);
	   }

	   if( sht_opt & msk)
	   {
	       pr_off( off_opt, loffset);
	       pr_short( sht_opt & msk, num_byt, buf);
	   }

	   if( lng_opt & msk)
	   {
	       pr_off( off_opt, loffset);
	       pr_long( lng_opt & msk, num_byt, buf);
	   }

       } /* end of for */

       loffset += num_byt;

   } /* end of while */

   /*[1]  1-21-85 kpc should print the offset once more so the person
    *[1] knows where the file ended.
    */
   Off_prflg = FALSE;		   /*[1]*/
   pr_off( off_opt, loffset);	   /*[1]*/
   printf("\n");                   /*[1]*/

} /*  end of main */




pr_usage()
{
   fprintf(stderr,
    "usage: od [-bcdoxDOX B{odxc} w A{odx}] [file] [[+]offset[.][b]]\n"
   );
   exit( 1);
}

bclear( p, n)
char   *p;
int    n;
{
   while( n-- )
       *p++ = '\0';
}

bcmp( d, s, n)
char   *d,
       *s;
int    n;
{
   while( n-- )
       if( *d++ != *s++ )
	   return( FALSE);

   return( TRUE);
}

bcpy( d, s, n)
char   *d,
       *s;
int    n;
{
   while( n--)
       *d++ = *s++;
}


/*
 * pr_byte()
 */
pr_byte( opt, w, p)
int    opt,
       w;      /* width */
char   *p;
{
   char    *fmt;

   switch( opt )
   {
       case OCT:
	   fmt = "%1s%03o";
	   break;

       case DEC:
	   fmt = "%1s%3d";
	   break;

       case HEX:
	   fmt = "%2s%2x";
	   break;

       case ASC:
	   /* here we use our own loop... */
	   for( ; w>0; w--,p++)
	       if( *p >= ' ' && *p <= '~' )    /*GROT ascii only!!! */
		   printf("%4c", *p);
	       else if( *p == '\0') /* null */
		   printf("  \\0");
	       else if( *p == '\n') /* newline */
		   printf("  \\n");
	       else if( *p == '\f') /* formfeed */
		   printf("  \\f");
	       else if( *p == '\b') /* backspace */
		   printf("  \\b");
	       else if( *p == '\r') /* carriage return */
		   printf("  \\r");
	       else if( *p == '\t') /* tab */
		   printf("  \\t");
	       else
		   printf(" %03o", *p & 0x00ff);

	   printf("\n");
	   return;

       default:
	   return;
   }


   for( ; w>0; w--)
       printf( fmt, " ", *p++ & 0x00ff);

   printf("\n");
}


pr_short( opt, w, p)
int    opt,
       w;      /* width */
short  *p;
{
   char    *fmt;

   switch( opt )
   {
       case OCT:
	   fmt = "%2s%06o";
	   break;

       case DEC:
	   fmt = "%3s%05d";
	   break;

       case USGN:
	   fmt = "%3s%05u";
	   break;

       case HEX:
	   fmt = "%4s%04x";
	   break;

       default:
	   return;
   }


   for( w /= 2 ; w>0; w--)
       printf( fmt, " ", *p++);

   printf("\n");
}


/*
 * pr_long()
 * print out longs   either 4 or 6 longs depending on the width.
 */
pr_long( opt, w, p)
int    opt,
       w;	 /* width */
long   *p;
{
   char    *fmt;

   switch( opt )
   {
       case OCT:
	   fmt = "%4s%012lo";
	   break;

       case DEC:
	   fmt = "%6s%010ld";
	   break;

       case USGN:
	   fmt = "%6s%010lu";
	   break;

       case HEX:
	   fmt = "%8s%08lx";
	   break;

       default:
	   return;
   }


   for( w /= 4; w>0; w--)
       printf( fmt, " ", *p++);

   printf("\n");
}


/*
 * pr_off()
 * print the current offset or enough blanks to cover the space
 */
pr_off( opt, lpos )
int    opt;    /* formating option, OCT DEC HEX */
long   lpos;   /* the current offset */
{
   char    *fmt;

   if( Off_prflg)
       printf("%9s", " ");
   else
   {
       switch( opt)
       {
	   case OCT:
	       fmt = "%08lo ";
	       break;

	   case DEC:
	       fmt = "%8ld.";
	       break;

	   case HEX:
	       fmt = "%08lx ";
	       break;
       }

       Off_prflg = TRUE;
       printf( fmt, lpos);
   }
}
             
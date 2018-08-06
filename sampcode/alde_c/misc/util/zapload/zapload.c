#include <stdio.h>
#include <ctype.h>

	/********************************************************
	*							*
	*	ZAPLOAD -- Binary to Intel hex Conversion	*
	*			Utility				*
	*							*
	*		T. Jennings 26 Sept. 82			*
	*							*
	*							*
	********************************************************/

/*	A nearly universal utility for converting binary to Intel hex
	outputting to one or more devices. See ZAPLOAD.DOC for details.

	A terminating record is sent following the last data record,
	to indicate end of load and to pass the start address. 

	OPERATION: 

	ZAPLOAD <file> <options>

		where <file> is any legal filespec, and
		<options> are one or more of the following options:

		-B <number> Generate records of <number> bytes per record.
			This must be less than 1025.
		-L <number> Set starting address(default 0000)
		-W <number> Delay between character transmission, where
			<number> is a variable. No fixed time scale.
		-M <number> delay at end of line.
		-R <number> Set the run address to <number>. Default 0000 hex.
		-S <number> Skip <number> bytes at the beginning of the file,

		-A	transmit the file to the AUX device.
		-F	Send the data to disk file <file>

		-Z	Generate 1's complement check sums.
		-E	Do even bytes only,
		-O	Do odd bytes only,
		-X 	Enable console output
		-N	No EOF record generated.

	Each record is echoed on the console for verification. */

int	inbuf,outbuf,punch;
char	infile[18];
char	outfile[18];
char	record[1024];		/* this is very big!!! */

int	sum_type;		/* 0 means 1's compl., 1 means 2's */
int	disk_file;		/* true means send to the disk file */
int 	send_tuart;		/* true to send to TUART port C */
int	send_pch;		/* true if sending to the punch, */
int	dsk_err;		/* disk file error */
unsigned load_address;		/* load address */
unsigned start_address;		/* Starting address, */
int	byte_count;		/* number of bytes to send, */
int	delay;			/* software delay counter, */
int	eol_delay;
int	console;		/* true if echoing to the console */
int	alt_byte;		/* true if doing alternate bytes, */
int	lsb;			/* 1 if doing odd bytes, */
int	doeof;			/* true if making an EOF record. */
int	skip;			/* bytes to skip at start of file */
int	byte_addr;		/* byte address in file, */
unsigned datasum;		/* eyeball check checksum */
int	eof;			/* true at end of file */

#define	CR	0x0d
#define	LF	0x0a
#define	ERROR	-1
#define	TRUE	-1
#define	FALSE	0
#define	OK	0
#define	NONE	0
#define	MAX_BYTES 64	/* max # data bytes in a record */
#define MIN_BYTES 8	/* minimum bytes, */

formfeed()
{}

/* Start by verifying the command line; must have at least one
argument, the file. Get any options, and if the guy screws up,
announce the proper way to invoke the program. */

main(argc,argv)
int	argc;
char	**argv;
{

char	*s;
int	is_ok;
char	arg;

	is_ok=FALSE;			/* set as bad cmd. line, */
	send_tuart =FALSE;		/* send to LST: port, */
	send_pch =FALSE;		/* send it to the punch, */
	disk_file =FALSE;		/* no disk file, */
	dsk_err =FALSE;			/* no error yet, */
	doeof =TRUE;			/* Make EOF records, */
	alt_byte =FALSE;		/* no alternate bytes, */
	console =FALSE;			/* default to no echo */
	eof =FALSE;			/* not end of file yet */
	skip =0;			/* dont skip any bytes, */
	sum_type =1;			/* two's complement checksum, */
	load_address =0;		/* load address of 0000 */
	start_address =0;		/* default start address */
	byte_count =32;			/* default byte count */
	delay=0;			/* no delay */
	eol_delay=0;
	byte_addr =0;			/* byte within the file, */

	printf("\nBinary to Intel Hex Conversion Utility");
	printf("\n  Version 2.1a 26 Sept. 1982 by T. Jennings\n");

	if(argc >1) {	
		is_ok= TRUE;
		strcpy(infile,argv[1]);	/* save the filename, */
		argc -=2;			/* two taken, */
		argv +=2;			/* move both, */

		while(argc >0)			/* now do all the options */
		{	arg= toupper(**argv);
			switch(arg)
			{

/* Skip leading dashes on the arguments */
			case '-':
				++*argv;	/* skip leading dashes */
				continue;
				break;

/* Change the default load address */
			case 'L':
				if((--argc >0) && 
				(make_hex(*++argv,&load_address)!=NONE))
				{	printf("Load address: %x\n",load_address);
				}
				else
				{	printf("Bad hex number\n");
					is_ok =FALSE;
				}
				break;

/* Change the default run address */
			case 'R':
				if((--argc >0) && 
				(make_hex(*++argv,&start_address)!=NONE))
				{	printf("Start address: %x\n",start_address);
				}
				else
				{	printf("Bad hex number\n");
					is_ok =FALSE;
				}
				break;

/* Change the default bytes per record */
			case 'B':
				if((--argc >0) && 
				(make_dec(*++argv,&byte_count)!=NONE))
				{	printf("%d Byte records\n",byte_count);
					if(byte_count > 1024)
					{	is_ok=FALSE;
						printf("Byte count must be <1025\n");
					}
				}
				else
				{	printf("Bad decimal number\n");
					is_ok =FALSE;
				}
				break;


/* Enable sending INTEL hex to the punch */
			case 'A':
				send_pch =TRUE;
				printf("Output to AUX\n");
				punch= open("AUX",0x8001);
				break;

/* Set number of bytes to skip at the start of the file. */
			case 'S':
				if((--argc >0) &&(make_dec(*++argv,&skip)!=NONE))
				{	printf("Skip %d bytes\n",skip);
				}
				else
				{	printf("Bad decimal number\n");
					is_ok =FALSE;
				}
				break;
				
/* Open an output file */
			case 'F':
				if(--argc)
				{	strcpy(outfile,*++argv);/* save the filename,*/
					printf("Sending output to %s\n",outfile);
					disk_file=TRUE;
				}
				else
				{	printf("Missing a filename\n");
				}
				break;

/* change the checksum type */
			case 'Z':
				sum_type =0;
				printf("1's complement checksum\n");
				break;

/* Set a software delay between chars */
			case 'W':
				if((--argc >0) && 
				(make_dec(*++argv,&delay)!=NONE))
				{	printf("Delay %d between characters\n",delay);
				}
				else
				{	printf("Bad decimal number\n");
					is_ok=FALSE;
				}
				break;

/* Set a software delay at the end of the line*/
			case 'M':
				if((--argc >0) && 
				(make_dec(*++argv,&eol_delay)!=NONE))
				{	printf("Delay %d at end of record\n",eol_delay);
				}
				else
				{	printf("Bad decimal number\n");
					is_ok=FALSE;
				}
				break;

/* Do only the even bytes */
			case 'E':
				lsb =0;	/* LSB address mask, */
				alt_byte =TRUE;
				printf("Doing only even bytes\n");
				break;
/* Do the odd bytes */
			case 'O':
				lsb =1;
				alt_byte =TRUE;
				printf("Doing only odd bytes\n");
				break;
/* Dont generate an EOF record */
			case 'N':
				doeof =FALSE;
				printf("Don't generate an EOF record\n");
				break;
			case 'X':
				console =TRUE;
				printf("Output to the console\n");
				break;
/* Gets here only on bad arguments */
			default:
				printf("Bad option: %s\n",*argv);
				is_ok=FALSE;	/* none of the above!!! */
				break;
			}

		--argc; ++argv;
		}
	}
/* Now see what we got. Exit now on bad options, and try to open any files.
Make sure theres at least one destination.  */

	if (is_ok) {
		if ((inbuf= open(infile,0x8000)) ==ERROR) {
			printf("\nCan't find %s",infile);
			exit();
		}
		if (disk_file) {
			if ((outbuf =creat(outfile,0x8001)) ==ERROR) {
				printf("\nCan't create %s",outfile);
				exit();
			}
		}
		send_file();
		if(disk_file) {
			close(outbuf);
		}
		exit();
	}
	else {
		printf("Try:  ZAPLOAD <file> <options>, where <options> is");
		printf 		(" one or more of the following:");
		printf("\n\t L <xxxx> set load address,(default 0000)");
		printf("\n\t R <xxxx> set start address,(default 0000)");
		printf("\n\t B <dddd> record byte count, default 32,");
		printf("\n\t S <dddd> skip <dddd> bytes before processing,");
		printf("\n\t W <dddd> delay between characters,(0-65535)");
		printf("\n\t M <dddd> delay at end of line(0-65535),");
		printf("\n\t F <file> send hex to <file>,");
		printf("\n\t A Send to the AUX device,");
		printf("\n\t Z Make 1's complement checksums,");
		printf("\n\t X Output to the console,");
		printf("\n\t E Do only even addressed bytes,");
		printf("\n\t O Do only odd addressed bytes,");
		exit();
	}
}

/* Now the file(s) are open, and the options are set. Send the file, converting
it to INTEL hex records. */

int	send_file()
{
int	total,this_record;
char	dummy;

	while(skip--) {		/* skip any bytes in the file, */
		eof =read(inbuf,&dummy,1);	/* as specified, */
		eof =((eof == ERROR) || (eof == NULL));
	}		
	byte_addr =0;			/* start keeping track, */
	datasum= 0;			/* zero the checksum, */
	total =0;
	while(!eof) {
		this_record =make_record(byte_count); /* make data records, */
		total +=this_record;	/* count bytes, */
	}
	printf("End of file\n");
	if(this_record > 0) {		/* if last record non-empty, */
		make_record(0);		/* then make an EOF record, */
	}				/* else we already did, */
	printf("Total byte count =%dd, data checksum =%xh\n",total,datasum);
	sendc(0x1a);			/* send a control-Z, */
	return;				/* then leave */
}
/* read bytes from the disk file, and make INTEL hex records. */

int	make_record(count)
int	count;
{
int	i,j,b;
int	checksum;
int	check;
char	c;

/* Write out an INTEL hex record. Pad it to the full length if the file
empties out. If we are passed a count of 0, do only a EOF record. */

	checksum =0;				/* zero the checksum, */
	for(i=0; i <count;  ) {
		j= read(inbuf,&c,1);		/* read byte to C */
		if((j ==ERROR) || (j == NONE)) {
			eof =TRUE;
			break;
		}
		check =byte_addr &1;		/* if alternate bytes, */
		if(!alt_byte ||(alt_byte && (check ==lsb) )) { 
			record[i] =c;		/* save each byte, */
			checksum +=c;		/* sum up the data bytes, */
			datasum +=c;		/* add data to the sum also. */
			++i;			/* Count another done, */
		}
		++byte_addr;
	}

/* Now we have an array of data bytes. If its not empty, send it out as a data
record, else send as a terminating record. */

	if(i) {					/* If data record ... */
		sendc(':');				/* record start character, */
		sendb(i);				/* send the byte count, */
		checksum +=i;				/* update the checksum, */
		sendw(load_address);
		checksum +=((load_address>>8) &0xff);/* add the address to the */
		checksum +=( load_address &0xff);	/* checksum, */
		load_address += i;			/* update the address, */
		sendb(0);				/* set record type =data, */
		for(j=0,b=i; b>0; b--)			/*send all the data bytes, */
			sendb(record[j++])
		;
	}
	else if(doeof) {				/*... its end of file */
		sendc(':');				/* record start character, */
		sendb(i);				/* send the byte count, */
		checksum +=i;				/* update the checksum, */
		sendw(start_address);
		checksum +=((start_address>>8) &0xff);/* add the address to the */
		checksum +=( start_address &0xff);	/* checksum, */
		sendb(1);				/* record type =end, */
	}

/* now make the correct checksum */

	if(i || doeof) {
		checksum =( ~checksum);		/* ones complement, */
		checksum += sum_type;			/* correct it, */
		sendb(checksum);			/* send it, */
		sendc(CR);				/* for a pretty screen, */
		sendc(LF);
	}
	return(i);					/* return number written, */
}

/* Data transmission routines */

/* This is the lowest level character I/O routine. It send the character to the
CRT, any open disk file, and the LST: device. If enabled, it does a variable
length software delay for the zap-80. */

int 	sendc(arg)
int	arg;
{
char	character;
int	i,j;

	character= arg;				/* type conversion */
	if(console)
		putchar(character);
	if(disk_file) {
		write(outbuf,&character,1);
	}
	if(arg==CR)
	{	if(eol_delay)
		{
			for(i=eol_delay; i>0; i--)
			{	for(j=0; j<100; j++);
			}
		}
	}
	else
	{	if(delay)
		{
			for(i=delay; i>0; i--)
			{	for(j=0; j<100; j++);
			}
		}
	}
	if(send_pch)
		write(punch,&character,1);
	return;
}

int	sendb(byte)
int	byte;
{
char	h;
	byte =(byte &0xff);
	h=(byte >>4) &0x0f;
	sendc(((h>9) ? h+'A'-10 : h+'0'));
	h=(byte & 0x0f);
	sendc(((h>9) ? h+'A'-10 : h+'0'));
	return;
}
int	sendw(word)
int	word;
{
	sendb((word >>8) &0xff);
	sendb(word &0xff);
	return;
}

/* Make a string into a hex number. Return non-zero if OK. The actual
 return value is set via a calling pointer. */

int	make_hex(string,hex_number)
char	*string;
int	*hex_number;
{
	return(sscanf(string,"%x",hex_number));
}

/* Make a string into a decimal number. Return non-zero if OK. The actual
 return value is set via a calling pointer. */

int	make_dec(string,decimal_numbr)
char	*string;
int	*decimal_numbr;
{
	return(sscanf(string,"%d",decimal_numbr));
}


/*	CFLOWX.C: cflow cross-reference utility for function/macro calls
**
**	Copyright (c) 1985 by:
**
**		Lawrence R. Steeger
**		1009 North Jackson
**		Milwaukee, Wisconsin 53202
**		414-277-8149
*/

#include <stdio.h>			/* CI-C86 header		*/

#define	VOID int

/*	external functions		*/

extern	VOID abort();
extern	char *alloc();
extern	VOID exit();
extern	int fclose();
extern	char *fgets();
extern	FILE *fopen();
extern	VOID fprintf();
extern	VOID free();
extern	char *lower();
extern	VOID printf();
extern	VOID qsort();
extern	char *realloc();
extern	VOID sprintf();
extern	int sscanf();
extern	char *strcat();
extern	int strcmp();
extern	char *strcpy();
extern	int strlen();
extern	int strncmp();
extern	int tolower();

#include "cflowx.h"			/* CFLOW/CFLOWX header		*/

#define	VERSION	85
#define	RELEASE	06
#define	MODIFIC	20

#define	outinfo(S) fputs(S, stdout)

outlogo()
{
	fprintf(stdout, "\n%s V%02d.%02d.%02d - %s",
		"CFLOWX",
		VERSION, RELEASE, MODIFIC,
		"cflow cross-reference generator");
	outinfo("\nCopyright (c) 1985 by Lawrence R. Steeger\n");
}

outhelp()
{
	outinfo("\nusage: cflowx [-[efimr]] infile [>outfile]");
	outinfo("\n note: infile is created by the");
	outinfo(" CFLOW utility using the -x flag\n");
	outinfo("\nreports: -e    function/macro external definition report");
	outinfo("\n         -f    path\\file name report");
	outinfo("\n         -i    function/macro internal definition report");
	outinfo("\n         -m    macro name cross-reference report");
	outinfo("\n         -r    function name cross-reference report");
	outinfo("\n   note: The order of the specified [efimr] flags will");
	outinfo("\n         be the order that the reports will generated in.");
}

#define	OUTHDR	0			/* start output header		*/
#define	OUTUPD	1			/* update output array		*/
#define	OUTFLSH	2			/* flush output line		*/
#define	OUTCLR	3			/* clear output line		*/

typedef	struct _xref {			/* cross-reference record	*/

	struct _xref *_related;		/* related XREF chain		*/

	unsigned char _dup;		/* duplicate entry flag		*/

	int _fnmbr,			/* file name number		*/
	    _level,			/* {...} nest level number	*/
	    _line;			/* line number			*/

	unsigned int _str;		/* record data string offset	*/

	} XREF;

static	XREF *xref = NULL,		/* XREF element pointers	*/
	     *xref1 = NULL,
	     *xref2 = NULL,
	     *xref3 = NULL,
	     *xref4 = NULL,
	     *xchain = NULL;

/*	useability macros for XREF	*/

#define	related xref->_related
#define	dup xref->_dup
#define	fnmbr xref->_fnmbr
#define	level xref->_level
#define	line xref->_line
#define	str xref->_str

#define	related1 xref1->_related
#define	dup1 xref1->_dup
#define	fnmbr1 xref1->_fnmbr
#define	level1 xref1->_level
#define	line1 xref1->_line
#define	str1 xref1->_str

#define	related2 xref2->_related
#define	dup2 xref2->_dup
#define	fnmbr2 xref2->_fnmbr
#define	level2 xref2->_level
#define	line2 xref2->_line
#define	str2 xref2->_str

#define	related3 xref3->_related
#define	dup3 xref3->_dup
#define	fnmbr3 xref3->_fnmbr
#define	level3 xref3->_level
#define	line3 xref3->_line
#define	str3 xref3->_str

#define	related4 xref4->_related
#define	dup4 xref4->_dup
#define	fnmbr4 xref4->_fnmbr
#define	level4 xref4->_level
#define	line4 xref4->_line
#define	str4 xref4->_str

#define	relxc xchain->_related
#define	dupxc xchain->_dup
#define	fnmbrxc xchain->_fnmbr
#define	levelxc xchain->_level
#define	linexc xchain->_line
#define	strxc xchain->_str

typedef	struct _rqueue {		/* xref record queue header	*/

	XREF **__xref;			/* XREF record queue pointer	*/
	unsigned char _sorted;		/* sorted queue flag		*/
	unsigned int _total,		/* offset counter		*/
		     _index;		/* offset index			*/

	} RQUEUE;

static	RQUEUE rqueue[HIGHKEY];		/* xref record queue headers	*/

static	int rkey = 0,			/* xref record queue keys	*/
	    rkey1 = 0,
	    rkey2 = 0;

/*	useability macros for RQUEUE	*/

#define	pxref rqueue[rkey].__xref
#define	sorted rqueue[rkey]._sorted
#define	total rqueue[rkey]._total
#define	index rqueue[rkey]._index

#define	pxref1 rqueue[rkey1].__xref
#define	sorted1 rqueue[rkey1]._sorted
#define	total1 rqueue[rkey1]._total
#define	index1 rqueue[rkey1]._index

#define	pxref2 rqueue[rkey2].__xref
#define	sorted2 rqueue[rkey2]._sorted
#define	total2 rqueue[rkey2]._total
#define	index2 rqueue[rkey2]._index

static	unsigned char *strings = NULL;	/* character strings buffer	*/
static	unsigned int stringl = 0;	/* character strings length	*/

static	char *invrec =			/* standard input error format	*/
	     "invalid record %s- %d: \"%s";

static	int reportc = 0;		/* report function counter	*/
static	PFI *(reports) = NULL;		/* report function pointers	*/

/*	mainline			*/

main(argc, argv)
int argc;
unsigned char **argv;
{
	int i, j;

	outlogo();			/* display utility logo		*/

	if (argc < 2) {			/* too few arguments		*/
		outhelp();
		exit(1);		/* exit abnormal		*/
	}

	for (rkey = 0; rkey < HIGHKEY; rkey++) { /* reset queue headers	*/
		pxref  = NULL;
		sorted = FALSE;
		total  = 0;
		index  = 0;
	}

	for (i = 1; i < argc; i++) {	/* process -flags		*/
		if (*argv[i] == '-') {
			flags(argv[i]);
			for (j = i--, --argc; j < argc; j++)
				argv[j] = argv[j+1];
		}
	}

	if (argc < 2) {			/* no filename specified !	*/
		outhelp();
		exit(1);		/* exit abnormal		*/
	}

	buildq(argv[1]);		/* build CFLOWX record queues	*/

	dupfile();			/* detect duplicate files	*/

	for (i = 0; i < reportc; i++)	/* generate CFLOWX reports	*/
		(*(reports[i]))();

	exit(0);			/* exit normal			*/
}

/*	process command line flags					*/

flags(flag)
unsigned char *flag;
{
	int i;

	PFI function;

	int reporte(),			/* function/macro external	*/
	    reportf(),			/* path\file name		*/
	    reporti(),			/* function/macro internal	*/
	    reportm(),			/* macro cross_reference	*/
	    reportr();			/* function cross_reference	*/

	for (i = 0; flag[++i];) {		/* scan all characters	*/

		function = NULL;

		switch (tolower(flag[i])) {	/* set any valid flag	*/

		case 'e':		/* function/macro external	*/
			function = reporte;
			break;

		case 'f':		/* path\file name		*/
			function = reportf;
			break;

		case 'i':		/* function/macro internal	*/
			function = reporti;
			break;

		case 'm':		/* macro cross_reference	*/
			function = reportm;
			break;

		case 'r':		/* function cross_reference	*/
			function = reportr;
			break;

		default:
			abort("Unknown flag: '%c'\n", flag[i]);
			break;
		}

		if (function != NULL) {
			reports = realloc(reports, (sizeof(PFI)*(reportc+1)));
			if (reports == NULL)
				abort("REALLOC - reports");
			reports[reportc++] = function;
		}
	}

	flag[0] = EOS;				/* terminate flag	*/
}

/*	build CFLOWX record queues					*/

buildq(filename)
unsigned char *filename;
{
	unsigned char *cp,
		      string[MAXBUF];

	unsigned int stradd();		/* add string to char buffer	*/

	int currec,			/* current input record		*/
	    crelxkey,			/* current related base key	*/
	    i,
	    j;

	FILE *fptr;			/* input file pointer		*/

	XREF *crelated;			/* related XREF base pointer	*/

	if ((fptr = fopen(filename, "r+")) == NULL) /* open input file	*/
		abort("FOPEN - %s\n", filename);

	currec = 0;			/* start record counter		*/
	crelated = NULL;		/* clear related chain base	*/

	while (1) {			/* scan XREF input		*/

		if (fgets(string,MAXBUF,fptr)==FALSE) /* get record	*/
			break;

		xref = alloc(sizeof(XREF));	/* next XREF element	*/

		++currec;			/* update line count	*/

		/*	scan for standard fields 			*/

		if ((i=sscanf(string,
				cxref((MAXFLDS-1)),
				&rkey,
				&fnmbr,
				&level,
				&line)
		    ) != ((MAXFLDS-1)*2)) {
			printf("sscanf(%d)\n", i);
			abort(invrec, "sscanf ", currec, string);
		}

		if (rkey >= HIGHKEY)		/* validate record key	*/
			abort(invrec, "type ", currec, string);

		/*	skip over standard fields			*/

		for (i=j=0; j != (MAXFLDS-1) && string[i] != RS; i++)
			if (string[i] == US) ++j;

		cp = &string[i];		/* save data position	*/

		/*	find trailing RS				*/

		for (; string[i] && string[i] != RS; i++);

		string[i] = EOS;		/* truncate at RS	*/

		str = stradd(cp);		/* add string to buffer	*/

		pxref = realloc(pxref, (sizeof(XREF *) * (index+1)));

		if (pxref == NULL) abort("REALLOC - XREC");

		pxref[index++] = xref;
		total++;

		switch (rkey) {		/* build record relationships	*/

		/*	related record base keys			*/

		case WILDPTH:			/* wildcard path	*/
		case FILEPTH:			/* C source path	*/
		case FUNCNME:			/* function name	*/
		case MACNME:			/* macro name		*/
			crelated = xref;	/* set new base record	*/
			crelxkey = rkey;	/* set new base key	*/
			break;

		/*	related record keys				*/

		case WILDNME:			/* wildcard name	*/
			checkrel(WILDPTH,crelxkey,currec,string);
			goto relation;

		case FILENME:			/* C source name	*/
			checkrel(FILEPTH,crelxkey,currec,string);
			goto relation;

		case FUNCDCL:			/* function declaration	*/
			checkrel(FUNCNME,crelxkey,currec,string);
			goto relation;

		case FUNCARG:			/* function argument	*/
			if (crelxkey == FUNCARG)
				goto relation;
			checkrel(FUNCDCL,crelxkey,currec,string);
			goto relation;

		case MACDCL:			/* macro declaration	*/
			checkrel(MACNME,crelxkey,currec,string);
			goto relation;

		case MACEQU:			/* macro equate		*/
			checkrel(MACDCL,crelxkey,currec,string);

		relation:
			if (crelated == NULL)
				abort(invrec,
				"has missing relationship ", currec, string);
			crelated->_related=xref;/* chain related XREFs	*/
			crelated = xref;	/* update chain base	*/
			crelxkey = rkey;	/* update related key	*/
			break;

		default:
			crelated = NULL;	/* break related chain	*/
			crelxkey = NOFUNC;	/* reset related key	*/
			break;
		}
	}

	fclose(fptr);
	return;
}

/*	detect duplicate file names					*/

dupfile()
{
	char tfile[MAXPATH],
	     tfile1[MAXPATH];

	int sindex;

	for (rkey = FILEPTH, index = 0; index < total; index++) {

		xref = pxref[index];
		strcpy(tfile, &strings[str]);

		for (xchain = related;
		     xchain != NULL;
		     xchain = relxc
		    )
			strcat(tfile, &strings[strxc]);

		lower(tfile);

		sindex = index;

		for (rkey1 = FILEPTH, index1 = index + 1;
		     index1 < total1;
		     index1++
		    ) {

			xref1 = pxref1[index1];
			strcpy(tfile1, &strings[str1]);

			for (xchain = related1;
			     xchain != NULL;
			     xchain = relxc
			    )
				strcat(tfile1, &strings[strxc]);

			lower(tfile1);

			if (strcmp(tfile, tfile1) == 0) {
				dup1 = TRUE;

				for (xchain = related1;
				     xchain != NULL;
				     xchain = relxc
				    )
					dupxc = TRUE;
			}
		}

		index = sindex;
	}
}

/*	generate funcation/macro external report			*/

reporte()
{
	int xrefcmp();			/* compare XREF queue elements	*/

	xsort(FUNCREF, xrefcmp);	/* sort function ref queue	*/

	xsort(FUNCNME, xrefcmp);	/* sort function name queue	*/

	xsort(MACNME, xrefcmp);		/* sort macro name queue	*/

	/*	report external functions/macros			*/

	_reporte(FUNCREF,
		 FILENME,
		 FUNCNME,
		 MACNME,
		 "\nFUNCTION/MACRO EXTERNAL REFERENCES:\n",
		 "\n(no external functions/macros)\n"
		);
}

/*	generate header/wildcard/file names listing			*/

reportf()
{
	int _reportf();

	/*	report any -hsystem[,project] specifications		*/

	rkey = HDRSYST;			/* -hsystem specification	*/

	for (index = 0; index < total; index++) {
		xref = pxref[index];
		fprintf(stdout, "\nSYSTEM HEADER: %s", &strings[str]);
	}

	if (total) fprintf(stdout, "\n");

	rkey = HDRPROJ;			/* -h[,project] specification	*/

	for (index = 0; index < total; index++) {
		xref = pxref[index];
		fprintf(stdout, "\nPROJECT HEADER: %s", &strings[str]);
	}

	if (total) fprintf(stdout, "\n");

	/*	report any wildcard path\file names			*/

	if (_reportf(WILDPTH,
		"\nWILDCARD PATH\\FILE NAMES:\n",
		"\n(no wildcard names)\n")
	   )
		fprintf(stdout,
		"\n\t\"# \" before a wildcard indicates no files found\n");

	/*	report all other path\file names			*/

	if (_reportf(FILEPTH,
		"\nC SOURCE PATH\\FILE NAMES:\n",
		"\n(no C source names)\n")
	   )
		fprintf(stdout,
		"\n\t\"* \" before a file name indicates an open error\n");
}

/*	generate function/macro internal report				*/

reporti()
{
	int xrefcmp();			/* compare XREF queue elements	*/

	xsort(FUNCNME, xrefcmp);	/* sort function name queue	*/

	/*	report FUNCNME internal queue				*/

	_reporti(FUNCNME,
		 FILENME,
		 -1,
		 "\nFUNCTION INTERNAL DECLARATIONS:\n",
		 "\n(no functions)\n"
		);

	xsort(MACNME, xrefcmp);		/* sort macro name queue	*/

	/*	report MACNME internal queue				*/

	_reporti(MACNME,
		 FILENME,
		 -1,
		 "\nMACRO INTERNAL DECLARATIONS:\n",
		 "\n(no macros)\n"
		);
}

/*	generate macro cross-reference report				*/

reportm()
{
	int xrefcmp();			/* compare XREF queue elements	*/

	xsort(MACNME, xrefcmp);		/* sort macro name queue	*/

	xsort(FUNCREF, xrefcmp);	/* sort function reference queue*/

	/*	report MACNME queue					*/

	_reporti(MACNME,
		 FILENME,
		 FUNCREF,
		 "\nMACRO CROSS_REFERENCE:\n",
		 "\n(no macros)\n"
		);
}

/*	generate function cross-reference report			*/

reportr()
{
	int xrefcmp();			/* compare XREF queue elements	*/

	xsort(FUNCNME, xrefcmp);	/* sort function name queue	*/

	xsort(FUNCREF, xrefcmp);	/* sort function reference queue*/

	/*	report FUNCNME queue					*/

	_reporti(FUNCNME,
		 FILENME,
		 FUNCREF,
		 "\nFUNCTION CROSS_REFERENCE:\n",
		 "\n(no functions)\n"
		);
}

/*	function/macro external reporter				*/

int _reporte(qkey, qkey1, qkey2, qkey3, title, noreport)
unsigned char *title, *noreport;
int qkey, qkey1, qkey2, qkey3;
{
	char outfnme[MAXPATH];		/* output path\file name string	*/

	int curfile,			/* current FILENME queue number	*/
	    tcount,			/* total reference count	*/
	    _reportr();			/* reference reporter/counter	*/

	fprintf(stdout, title);		/* display report title		*/

	rkey = qkey;			/* function/macro ref queue	*/

	for (tcount = index = 0; index < total;) {

		xref = pxref[index];	/* function/macro ref queue ptr	*/

		if (dup
		|| _reportr(qkey2, FALSE)	/* function dcl found	*/
		|| _reportr(qkey3, FALSE)	/* macro dcl found	*/
		   ) {
			++index;
			continue;		/* not external name	*/
		}

		tcount++;		/* increment external count	*/

		/*	report function/macro external name		*/

		fprintf(stdout, "\n%s:", &strings[str]);

		for (rkey2 = qkey, index2 = index, curfile = -1;
		     index2 < total2;
		     index2++
		    ) {
			xref2 = pxref2[index2];	/* reference queue ptr	*/

			if (strcmp(&strings[str], &strings[str2]) != 0) {
				index = index2;
				break;
			}

			if (curfile != fnmbr2) {	/* new filename	*/
				fmtout(OUTFLSH);
				xref1 = pxref1[(curfile = fnmbr2)];
				strcpy(outfnme, &strings[str1]);

				for (xchain = related1;
					xchain != NULL;
					xchain = relxc
				    )
					strcat(outfnme, &strings[strxc]);

				fmtout(OUTHDR, 0, outfnme);
			}

			if (dup1) {
				fmtout(OUTCLR);
				continue;	/* skip duplicate files	*/
			}

			fmtout(OUTUPD, line2);	/* update output array	*/
		}

		fmtout(OUTFLSH);		/* flush format output	*/
	}

	if (tcount)
		fprintf(stdout, "\n");
	else	fprintf(stdout, noreport);

	fmtout(OUTCLR);

	return (tcount);
}

/*	path\file name reporter						*/

int _reportf(qkey, title, noreport)
unsigned char *title, *noreport;
int qkey;
{
	int filerr;

	filerr = 0;

	fprintf(stdout, title);		/* report title			*/

	rkey = qkey;			/* report queue = qkey		*/

	for (index = 0; index < total; index++) {

		xref = pxref[index];	/* queue XREF pointer		*/

		fprintf(stdout,		/* report number/pathname	*/
			"\n%d: %s",
			(fnmbr+1),
			&strings[str]
		       );

		if (strncmp(&strings[str], "* ", 2) == 0
		||  strncmp(&strings[str], "# ", 2) == 0)
			filerr++;

		for (xchain = related; xchain != NULL; xchain = relxc) {
			fprintf(stdout, "%s", &strings[strxc]);
			if (strncmp(&strings[strxc], "* ", 2) == 0
			||  strncmp(&strings[strxc], "# ", 2) == 0)
				filerr++;
		}
	}

	if (total)
		fprintf(stdout, "\n");
	else	fprintf(stdout, noreport);

	return (filerr);
}

/*	function/macro internal reporter				*/

int _reporti(qkey, qkey1, qkey2, title, noreport)
unsigned char *title, *noreport;
int qkey, qkey1, qkey2;
{
	char *cid;

	int tcount;

	fprintf(stdout, title);		/* display report title		*/

	rkey = qkey;			/* function/macro queue		*/
	rkey1 = qkey1;			/* path\file name queue		*/

	for (tcount = index = 0; index < total; index++, tcount++) {

		xref = pxref[index];	/* function/macro queue ptr	*/
		xref1 = pxref1[fnmbr];	/* path\file name queue ptr	*/

		if (dup && qkey2 != -1)
			continue;		/* ignore duplicates	*/

		/*	report function/macro & 1st path\file segment	*/

		fprintf(stdout, "\n%s:\t%s (%d)",
			&strings[str],		/* function/macro name	*/
			&strings[str1],		/* 1st pathfile segment	*/
			line			/* line where declared	*/
		       );

		/*	report path\file name related chain		*/

		for (xchain = related1; xchain != NULL; xchain = relxc)
			fprintf(stdout, "%s", &strings[strxc]);

		fprintf(stdout, "\n");

		/*	report function/macro related chain		*/

		for (xchain = related, cid = "**";
		     xchain != NULL;
		     xchain = relxc, cid = " *"
		    )
			fprintf(stdout, "\n\t%s %s", cid, &strings[strxc]);

		if (related != NULL) fprintf(stdout, "\n");

		if (qkey2 != -1)
			_reportr(qkey2, TRUE);	/* reference report	*/
		else	fprintf(stdout, "\n");
	}

	if (tcount)
		fprintf(stdout, "\n");
	else	fprintf(stdout, noreport);

	return (tcount);
}

/*	cross_reference reporter					*/

int _reportr(qkey2, display)
int qkey2, display;
{
	char outfnme[MAXPATH];		/* output path\file name string	*/

	int curfile,
	    tcount;

	tcount = 0;
	rkey2 = qkey2;			/* reference queue		*/

	/*	search reference queue for a matching reference		*/

	for (index2 = 0; index2 < total2; index2++) {

		xref2 = pxref2[index2];	/* reference queue ptr		*/

		if (strcmp(&strings[str], &strings[str2]) == 0)
			break;
	}

	if (!(index2 < total2))
		if (display) fprintf(stdout, "\n\t(no references)");

	/*	count/report references					*/

	for (curfile = -1; index2 < total2; tcount++, index2++) {

		xref2 = pxref2[index2];	/* reference queue ptr		*/

		if (strcmp(&strings[str], &strings[str2]) != 0)
			break;

		if (display) {
			if (curfile != fnmbr2) {	/* new filename	*/
				fmtout(OUTFLSH);
				xref1 = pxref1[(curfile = fnmbr2)];
				strcpy(outfnme, &strings[str1]);

				for (xchain = related1;
					xchain != NULL;
					xchain = relxc
				    )
					strcat(outfnme, &strings[strxc]);

				fmtout(OUTHDR, 0, outfnme);
			}

			fmtout(OUTUPD, line2);	/* add to detail line	*/
		}
	}

	if (display) {
		fmtout(OUTFLSH);	/* flush final pending line	*/
		fmtout(OUTCLR);		/* clear all output		*/
		fprintf(stdout, "\n");
	}

	return (tcount);
}

/*	check key relationship, abort on a bad relationship		*/

checkrel(rkey, xkey, currec, string)
int rkey, xkey, currec;
unsigned char *string;
{
	if (rkey != xkey)
		abort(invrec, "relationship ", currec, string);
}

/*	formatted reference output					*/

fmtout(mode, outval, outfnme)
int mode, outval;
char *outfnme;
{
	static	char *outfmt = NULL,	/* output format string		*/
		     *outhdr = NULL,	/* output format header		*/
		     *fmthdr = NULL,	/* format header string		*/
		     *fmtdet = NULL;	/* format detail string		*/

	static	int outcnt = 0,		/* output reference count	*/
		    outhdrl = 0,	/* output header length		*/
		    outlim = 0,		/* output reference maximum	*/
		    outint[8] =		/* output reference array	*/
		    {0,0,0,0,0,0,0,0};

	int outlen;

	switch (mode) {			/* select output mode		*/

	case OUTHDR:			/* start format heading		*/

		if (outfmt == NULL) {		/* 1st time thru only	*/
			outfmt = alloc((MAXPATH * 2));
			outhdr = alloc(MAXPATH);
			fmthdr = alloc(16);
			fmtdet = alloc(8);
		}

		strcpy(fmthdr, "  %*s   ");	/* fprintf() formats	*/
		fmthdr[0] = '\n';
		fmthdr[1] = '\t';
		strcpy(fmtdet, "%8d");

		strcpy(outhdr, outfnme);	/* build format heading	*/
		outhdrl = strlen(outhdr);
		strcpy(outfmt, fmthdr);
		outlen = (80 - (outhdrl + 11));
		for (outlim = 0;
		     outlim < 8 && outlen > 8;
		     outlen -= 8, outlim++
		    )
			strcat(outfmt, fmtdet);

		outcnt = 0;			/* restart array	*/
		break;

	case OUTUPD:			/* update formatted array	*/

		if (outfmt == NULL) break;

		outint[outcnt++] = outval;

		if (outcnt < outlim);
		else {			/* format line full - flush	*/
			fprintf(stdout,
				outfmt, outhdrl, outhdr,
				outint[0], outint[1], outint[2],
				outint[3], outint[4], outint[5],
				outint[6], outint[7]
			       );
			strcpy(outhdr, " ");
			outcnt = 0;
		}
		break;

	case OUTFLSH:			/* flush formatted output line	*/

		if (outfmt == NULL) break;

		if (outcnt) {
			outfmt[(strlen(fmthdr)+(strlen(fmtdet)*outcnt))] = EOS;
			strcat(outfmt, "\n");
			fprintf(stdout,
				outfmt, outhdrl, outhdr,
				outint[0], outint[1], outint[2], outint[3],
				outint[4], outint[5], outint[6], outint[7]
			       );
		}
		else	fprintf(stdout, "\n");

		free(outfmt);
		free(outhdr);
		free(fmthdr);
		free(fmtdet);
		outfmt = outhdr = fmthdr = fmtdet = NULL;
		outcnt = 0;
		break;

	case OUTCLR:			/* clear current format		*/

		if (outfmt == NULL) break;

		free(outfmt);
		free(outhdr);
		free(fmthdr);
		free(fmtdet);
		outfmt = outhdr = fmthdr = fmtdet = NULL;
		outcnt = 0;
		break;

	default:	
		break;
	}
}

/*	add string to character string buffer				*/

unsigned int stradd(string)
unsigned char *string;
{
	unsigned int sp,			/* string offset	*/
		     strfind();			/* search char buffer	*/

	if ((sp = strfind(string)) != -1)
		return (sp);		/* existing string found	*/

	strings = realloc(strings, (stringl + strlen(string) + 1));
	if (strings == NULL) abort("REALLOC - STRINGS");

	sp = stringl;

	strcpy(&strings[stringl], string);
	stringl += (strlen(string) + 1);

	return (sp);
}

/*	search string buffer for matching string

	returns:	unsigned int	string buffer offset if found
			-1		string not found
*/

unsigned int strfind(string)
unsigned char *string;
{
	unsigned int sp;

	for (sp = 0;
	     sp < stringl;
	     sp += (strlen(&strings[sp]) + 1)
	    )
		if (strcmp(&strings[sp], string) == 0)
			 return (sp);		/* match found		*/

	return (-1);				/* no match found	*/
}

/*	XREF qsort() routine						*/

xsort(key,compare)
int key;
PFI compare;
{
	rkey = key;

	if (sorted == FALSE) {

		qsort((char *)pxref,		/* 1st XREF pointer	*/
		      (unsigned)total,		/* XREF pointer count	*/
		      (unsigned)sizeof(XREF *),	/* size of XREF element	*/
		      compare			/* XREF comparison	*/
		     );

		rkey = key;
		sorted = TRUE;			/* set sort completed	*/
	}
}

/*	qsort() XREF comparison routine					*/

int xrefcmp(rec1, rec2)
XREF **rec1, **rec2;
{
	unsigned char *rstring;

	int result;

	xref1 = *rec1;			/* 1st element pointer		*/
	xref2 = *rec2;			/* 2nd element pointer		*/

	rkey1 = FILENME;		/* file name queue header	*/
	xref3 = pxref1[fnmbr1];		/* 1st element file name	*/
	xref4 = pxref1[fnmbr2];		/* 2nd element file name	*/

	if ((result = strcmp(&strings[str1], &strings[str2])) == 0) {

		/*	element names match				*/

		if ((result = strcmp(&strings[str3], &strings[str4])) == 0) {

			/*	file names match			*/

			if (line1 == line2)

				/*	line numbers match		*/

				result = 0;
			else {
				if (line1 > line2)
					result = 1;
				else	result = -1;
			}
		}
	}

	if (result == 0 && !dup1 && !dup2 && (line1 != line2))
		dup2 = TRUE;		/* flag duplicate entries	*/

	if (result == 0 && !dup1 && dup2)
		result = -1;		/* a non-duplicate is lower	*/

	return (result);
}

/*	end of cflowx.c			*/


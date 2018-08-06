/********************************************************************
**  YEARCAL.C   v0.01Td     Copyright (c) 1987, 1988 by Paul M. Sittler.
**  All rights reserved.  The copyright owner hereby authorizes the
**  no-charge, noncommercial making and/or distribution of copies of
**  the entirety of this work unchanged and unincorporated in any
**  other work (except "LiBRary" or "ARChive" disk files for the sole
**  purpose of no-charge noncommercial distribution).  No other
**  reproduction or use is authorized without the express prior
**  written consent of the copyright owner.
**
**  Once upon a time, in a kingdom far away, people shared source
**  code and many of us benefited from the sharing.  This silly
**  little program is here presented with source in hopes that it may
**  stimulate the renewed sharing of our efforts.
**
**  I am open to comment, suggestions for enhancements, and bug
**  reports.
**
**  Paul M. Sittler
**  1106 Berkeley Drive
**  College Station, TX 77840
**
**  Modem (409) 764-0056 300/1200/2400 baud 24 hours/day
**  My Word #2 Fido 117/1
**  GENie User mail address P.M.SITTLER
**
**  Modified for Borland's Turbo-C
**  26 August 1987 <pms>
**
**  Modified for Workaround for Borland's Turbo-C bug on % Operator
**  Replaced calls to scanf with get_int routine.
**  10 September 1987 <pms>
**
**  Modified to produce a "Programmer's" Calendar, which can
**  display dates in Octal or Hexadecimal numbering.
**  11 September 1987 <pms>
**
**  Modified to produce calendars in any of 14 different languages
**  at user's option
**  18 September 1987 <pms>
**
**  Modified to create a Julian date calendar.
**  Modified to add support for Afrikaans language.
**  29 September 1987 <pms>
**
**  Modified to add support for Korean language.
**  Modified Julian date calendar to skip a line every five.
**  Rewrote menu for language selection to save space.
**  14 October 1987 <pms>
**
**  Added Daily and Weekly Scheduling sheets.  User may choose beginning
**  hour for scheduling.
**  16 October 1987 <pms>
**
**  Fixed Daily and Weekly scheduling sheets.
**  Added ability to select number of hours to schedule, program
**  adapts the page for it.
**  25 October 1987 <pms>
**
**  Added Monthly Calendar.
**  Added ability to specify number of lines and columns on a page for
**  file and printer output.  Modified filename routines to shorten
**  program slightly.
**  31 October 1987 <pms>
**
**  Added title lines that include the last month and next month
**  calendars and two user-defined title lines.  Rewrote the
**  program to calculate three years worth of calendars for each
**  year so that previous month's calendars can be displayed even
**  when they are last years.  Added Programmers calendar option
**  to the monthly, daily, and weekly schedule options.
**  Made defaults firm or sticky, such that once user selects one,
**  it stays the same while running program.
**  1 November 1987 <pms>
**
**  Added Czechoslovakian Language Support thanks to Briggs Myrick.
**  3 January 1988 <pms>
**
**  Default Page width changed to 80 cols from 85.
**  Maximum Page width increased to 270 from 200 for 20 CPI on 13.5".
**  5 January 1988 <pms>
**  Added optional pause at end of each printed page defaulted off.
**  Added a C/R after the FF at the end of a page so that dot matrix
**  printers like Epson will process FF without buffering it.
**  Added 3rd, 7th and 8th user-defined title lines.  Explained that Titles
**  #4-#6 are used for dates.  Explained how to remove a title.
**  8 January 1988 <pms>
**  Changed "Marz" to "Maerz" per Brief von Ed Braaten, and "Rizen" to "Rijen"
**  on recommendation of Dr. Skrabanek.
**  22 January 1988 <pms>
**  Allow user to specify "Left Margin indent (spaces) ?"
**  Make customized defaults "permanent" by writing them at end of .EXE file.
**  Tnx to Alan Holub in C-Chest, DDJ, Feb 1988 4 February 1988 <pms>
**  Someday, the following additions. . .
**  Optional IBM/Epson graphic character set for printers/IBM screens.
**  ?? February 1988 <pms>
**************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>	/* DOS-specific file from Borland */
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <string.h>

#define DEF_SIG "(c) 1988, Paul M. Sittler.  All rights reserved."
#define ERROR (-1)
#define TRUE 1
#define FALSE 0
#define BUF 200
#define LIN 80

struct options
{
    char signature[ sizeof(DEF_SIG) ];
    int chksum;
    int bh,	/* Beginning hour for schedules */
      copies,   /* Number of copies to print */
      indent,	/* Left Margin Indent in spaces */
      lang,	/* Language desired for calendar */
      more_yrs, /* Number of successive periods to print */
      nh,	/* Number of Hours to schedule */
      pl,	/* Page Length in lines */
      pw;	/* Page Width in characters or columns */
    char aggie;	/* Aggie calendar flag */
    char base;	/* Number base for Programmers Calendar, 0, H(ex), O(ctal) */
    char fy;	/* Fiscal year flag */
    char julian;/* Julian calendar flag */
    char pause;	/* Page pause flag */
    char sched_type;/* Schedule type M(onthly), W(eekly), D(aily) */
    char title[8][LIN];
    /* This structure also contains fields for every option that
     * the user can change at runtime.  These are the default values
     * that are used if no other values have been specified by the
     * user during a run.
     */
}
Opt;

/* extern int directvideo = 0;	 * Gag me with a spoon! */
				/* An "enhancement" of Turbo-c v1.5 */
/* Zero = use BIOS calls 	One  = write to video ram for PC-Compats */
/* For MSDOS generic, this must now be zero.  And it STILL doesn't work!! */
/* The getch function generates wild interrupts on Z-100 */

/* Suppress some library functions to conserve space */
/* void _setargv () {}
 * void _setenvp () {}
 */

struct month
{
    int  length;
    int  first_day;
};

/* put month's lengths into structure */
static struct month mon[] =
    {
    { 31, 0}, { 28, 0}, { 31, 0},
    { 30, 0}, { 31, 0}, { 30, 0},
    { 31, 0}, { 31, 0}, { 30, 0},
    { 31, 0}, { 30, 0}, { 31, 0}
    };

int lingos = 17;	/* How many languages we gonna have available */
char *lingo[] =
       {
"Afrikaans",	/*  0 */
"Czech",	/*  1 */
"Danish",	/*  2 */
"Dutch",	/*  3 */
"English",	/*  4 */
"Finnish",	/*  5 */
"French",	/*  6 */
"German",	/*  7 */
"Italian",	/*  8 */
"Korean",	/*  9 */
"Norwegian",	/* 10 */
"Polish",	/* 11 */
"Portugese",	/* 12 */
"Serbo-Croatian",/* 13 */
"Spanish",	/* 14 */
"Swedish",	/* 15 */
"Texan"		/* 16 */
       } ;

char *mnam[][12] =
       {
/* Afrikaans	0 */  "JANUARI",   "FEBRUARI",   "MAART",     "APRIL",    "MEI",      "JUNI",     "JULI",     "AUGUSTUS", "SEPTEMBER",  "OKTOBER",     "NOVEMBER",  "DECEMBER",
/* Czech	1 */  "LEDEN",     "UNOR",       "BREZEN",    "DUBEN",    "KVETEN",   "CERVEN",   "CERVENEC", "SRPEN",    "ZARI",       "RIJEN",       "LISTOPAD",  "PROSINEC",
/* Danish	2 */  "JANUAR",    "FEBRUAR",    "MARTS",     "APRIL",    "MAJ",      "JUNI",     "JULI",     "AUGUST",   "SEPTEMBER",  "OKTOBER",     "NOVEMBER",  "DECEMBER",
/* Dutch	3 */  "JANUARI",   "FEBRUARI",   "MAART",     "APRIL",    "MEI",      "JUNI",     "JULI",     "AUGUSTUS", "SEPTEMBER",  "OKTOBER",     "NOVEMBER",  "DECEMBER",
/* English	4 */  "JANUARY",   "FEBRUARY",   "MARCH",     "APRIL",    "MAY",      "JUNE",     "JULY",     "AUGUST",   "SEPTEMBER",  "OCTOBER",     "NOVEMBER",  "DECEMBER",
/* Finnish	5 */  "PRPRNUU",  "HELMIKUU",   "MAALISKUU", "HUHTIKUU", "TOUKOKUU", "KESAKUU",  "HEINAKUU", "ELOKUU",   "SYYSKUU",    "LOKAKUU",     "MARRASKUU", "JOULUKUU",
/* French	6 */  "JANVIER",   "FEVRIER",    "MARS",      "AVRIL",    "MAI",      "JUIN",     "JUILLET",  "AOUT",     "SEPTEMBRE",  "OCTOBRE",     "NOVEMBRE",  "DECEMBRE",
/* German	7 */  "JANUAR",    "FEBRUAR",    "MAERZ",     "APRIL",    "MAI",      "JUNI",     "JULI",     "AUGUST",   "SEPTEMBER",  "OKTOBER",     "NOVEMBER",  "DEZEMBER",
/* Italian	8 */  "GENNAIO",   "FEBBRAIO",   "MARZO",     "APRILE",   "MAGGIO",   "GIUGNIO",  "LUGLIO",   "AGOSTO",   "SETTEMBRE",  "OTTOBRE",     "NOVEMBRE",  "DICEMBRE",
/* Latin	      "JANUARIUS", "FEBRUARIUS", "MARTIUS",   "APRILIS",  "MAIUS",    "JUNIUS",   "JULIUS",   "AUGUSTUS", "SEPTEMBER",  "OCTOBER",     "NOVEMBER",  "DECEMBER", */
/* Korean	9 */  "IL-WOL",    "I-WOL",      "SAM-WOL",   "SA-WOL",   "O-WOL",    "YU-WOL",  "CH'IL-WOL", "P'AL-WOL", "KU-WOL",     "SI-WOL",      "SIP IL-WOL","SIP I-WOL",
/* Norwegian	10 */  "JANUAR",    "FEBRUAR",    "MARS",      "APRIL",    "MAI",      "JUNI",     "JULI",     "AUGUST",   "SEPTEMBER",  "OKTOBER",     "NOVEMBER",  "DESEMBER",
/* Polish	11 */  "STYCZEN",   "LUTY",       "MARZEC",    "KWIECIEN", "MAJ",      "CZERWIEC", "LIPIEC",   "SIERPIEN", "WRZESIEN",   "PAZDZIERNIK", "LISTOPAD",  "GRUDZIEN",
/* Portugese	12 */  "JANEIRO",   "FEVEREIRO",  "MARCO",     "ABRIL",    "MAIO",     "JUNHO",    "JULHO",    "AGOSTO",   "SETEMBRO",   "OUTUBRO",     "NOVEMBRO",  "DEZEMBRO",
/* Serbo-Croatian 13 */  "JANUAR",    "FEBRUAR",    "MART",      "APRIL",    "MAJ",      "JUN",      "JULI",     "AVGUST",   "SEPTEMBAR",  "OKTOBAR",     "NOVEMBAR",  "DECEMBAR",
/* Spanish	14 */  "ENERO",     "FEBRERO",    "MARZO",     "ABRIL",    "MAYO",     "JUNIO",    "JULIO",    "AGOSTO",   "SEPTIEMBRE", "OCTUBRE",     "NOVIEMBRE", "DICIEMBRE",
/* Swedish	15 */  "JANUARI",   "FEBRUARI",   "MARS",      "APRIL",    "MAJ",      "JUNI",     "JULI",     "AUGUSTI",  "SEPTEMBER",  "OKTOBER",     "NOVEMBER",  "DECEMBER",
/* Texan	16 */  "JANYEWARY", "FEBYEWARY",  "MAHCH",     "AYEPRIL",  "MEYE",     "CHEWN",    "CHEW-LIE", "AWGUST",   "SEPTEMBUH",  "AWKTOBUH",    "NOVEMBUH",  "DEESEMBUH"
      } ;

/* put names of days into array */
char *wkday[][7] =
       {
/* Afrikaans	0 */  "SONDAG",    "MAANDAG",       "DINSDAG",     "WOENSDAG",     "DONDERDAG",    "VRYDAG",      "SATERDAG",
/* Czech	1 */  "NEDELE",    "PONDELI",       "UTERY",       "STREDA",       "CTVRTEK",      "PATEK",       "SOBOTA",
/* Danish	2 */  "SONDAG",    "MANDAG",	"TIRSDAG",     "ONSDAG",       "TORSDAG",      "FREDAG",      "LORDAG",
/* Dutch	3 */  "ZONDAG",    "MAANDAG",       "DINSDAG",     "WOENSDAG",     "DONDERDAG",    "VRIJDAG",     "ZATERDAG",
/* English	4 */  "SUNDAY",    "MONDAY",	"TUESDAY",     "WEDNESDAY",    "THURSDAY",     "FRIDAY",      "SATURDAY",
/* Finnish	5 */  "SUNNUNTAI", "MAANANTAI",     "TIISTAI",     "KESKIVIIKKO",  "TORSTAI",      "PERJANTAI",   "LAUANTAI",
/* French	6 */  "DIMANCHE",  "LUNDI",	 "MARDI",       "MERCREDI",     "JEUDI",	"VENDREDI",    "SAMEDI",
/* German	7 */  "SONNTAG",   "MONTAG",	"DIENSTAG",    "MITTWOCH",     "DONNERSTAG",   "FREITAG",     "SONNABEND",
/* Italian	8 */  "DOMENICA",  "LUNEDI",	"MARTEDI",     "MERCOLEDI",    "GIOVEDI",      "VENERDI",     "SABATO",
/* Latin	      "DOMINICUS", "LUNES",	 "MIUS",	"",	     "",	     "",	    "", */
/* Korean	9 */  "IL-YO-IL",  "WOL-YO-IL",     "HWA-YO-IL",   "SU-YO-IL",     "MOK-YO-IL",    "KUM-YO-IL",   "T'O-YO-IL",
/* Norwegian	10 */  "SONDAG",    "MANDAG",	"TIRSDAG",     "ONSDAG",       "TORSDAG",      "FREDAG",      "LORDAG",
/* Polish	11 */  "NIEDZIELA", "PONIEDZIALEK",  "WTOREK",      "SRODA",	"CZWARTEK",     "PIATEK",      "SOBOTA",
/* Portugese	12 */  "DOMINGO",   "SEGUNDA-FEIRA", "TERCA-FIERA", "QUARTA-FIERA", "QUINTA-FIERA", "SEXTA-FIERA", "SABADO",
/* Serbo-Croatian 13 */  "NEDELJA",   "PONEDELJAK",    "UTORAK",      "SREDA",	"CETVRTAK",     "PETAK",       "SUBOTA",
/* Spanish	14 */  "DOMINGO",   "LUNES",	 "MARTES",      "MIERCOLES",    "JUEVES",       "VIERNES",     "SABADO",
/* Swedish	15 */  "SONDAG",    "MANDAG",	"TISDAG",      "ONSDAG",       "TORSDAG",      "FREDAG",      "LORDAG",
/* Texan	16 */  "SUNNY",     "MONEY",	"TEWSDI",      "WEDDSDI",      "THIRSTY",      "FRAWDDI",     "SATTADI"
      } ;

/* Hebrew - based on solar years and lunar months.
   shelema - 355 days - complete year, with Heshvan & Keslev having 30 days.
   sedur   - 354 days - normal year, Heshvan is defective (29 days).
   hasera  - 353 days - Heshvan & Kislev both defective (29 days).
   The 11 days by which the solar year (365 days) exceeds the lunar year
   (354 days) are accounted for by having a 13th month (Veadar of 30 days)
   on the 3rd, 6th, 8th, 11th, 14th, and 17th year of a 19 year cycle.

    "TISHRI", 30,	"HESHVAN", 29/30,	"KISLEV", 29/30,
    "TEBET", 29,	"SHEBAT", 30,		"ADAR", 30,	"VEADAR", 30
    "NISAN", 30,	"IYAR", 29,		"SIVIAN", 30,
    "TAMMUZ", 29,	"AB", 30,		"ELUL", 29
*/

int yr[3][12][6][7];	/* Array for 3 yrs, 4-D, 3 years, */
			/* 12 months/year, 6 weeks/month, 7 days/week */

char *new_file  = "DATAFILE.$$$";
char *file      = "DATAFILE.CAL";
FILE *fp;

void main(argc, argv)
char **argv;
int argc;
{
  int bd,	/* Beginning day passed to functions */
      bm,       /* Beginning month passed to functions */
      by,	/* Beginning year passed to functions */
      fy_bm,	/* Fiscal year beginning month */
      fy_bd,	/* Fiscal year beginning date */
      i,
      j,
      year;
  char c,
       outp;

  int get_int(),
      get_yn(),
      leap();
  void agg_printer(),
       cal_printer(),
       day_printer(),
       get_opts(),
       jul_printer(),
       prep_cal(),
       put_opts(),
       sched_printer(),
       setup_files(),
       signon();
  struct date today;	/* dos.h from Borland */
  get_opts(argv[0]);
  signon();

				/* DOS specific. . . */
    getdate(&today);		/* Get DOS's idea of date */
    fy_bd = today.da_day;	/* Make it today */
    fy_bm = today.da_mon;	/* Make it this month */
    year  = today.da_year;	/* It was a very good year. . . */

  for ( ;; )
  {
    fprintf(stderr,
    "\nSchedule period to be <A>nnual, <M>onthly, <W>eekly, or <D>aily (A/M/W/D)? %c\b",
			   Opt.sched_type ? Opt.sched_type : 'A' );
    for ( ;; )
    {
	c = toupper(getch());
	switch(c)
	{
	    case  'M'  : break;
	    case  'W'  : break;
	    case  'D'  : break;
	    case  'A'  : break;
	    case '\033': exit(0);
	    case '\r'  :
	    case '\n'  : c = (Opt.sched_type == FALSE) ? 'A': Opt.sched_type;
	    default    : ;
	}
	if( (c == 'M') ||
	    (c == 'W') ||
	    (c == 'D') ||
	    (c == 'A')  )
	{
	    Opt.sched_type = c;
	    putch(c);
	    if (Opt.sched_type == 'A')
		Opt.sched_type = FALSE;
	    break;
	}
	continue;
    }
    fputs("\nType year desired for calendar (1901-4000): ", stderr);
    year = get_int(year, 4, 1901, 4000);

    fprintf(stderr, "\nHow many successive %ss? (0-99): ",
		(Opt.sched_type == 'M') ? "month" :
		(Opt.sched_type == 'W') ? "week" :
		(Opt.sched_type == 'D') ? "day" :
				      "year");
    Opt.more_yrs = get_int(Opt.more_yrs, 2, 0, 99);

    if (! (Opt.sched_type) )
    {
	fputs("\nDo you want an Aggie Calendar", stderr);
	Opt.aggie = get_yn(Opt.aggie ? 'y' : 'n');
    }

    if (!Opt.aggie)
    {
	if (! (Opt.sched_type) )
	{
	    fputs("\nDo you want a 3-digit Julian Calendar", stderr);
	    Opt.julian = get_yn(Opt.julian ? 'y' : 'n');
	}

	if (! (Opt.julian | Opt.sched_type) )
	{
	    fputs("\nFiscal year calendar ", stderr);
	    Opt.fy = get_yn(Opt.fy ? 'y' : 'n');
	}

	if (Opt.fy | Opt.sched_type)
	{
	    fprintf(stderr, "\n%s beginning month (1-12): ",
		(Opt.sched_type == 'M') ? "Monthly calendar" :
		(Opt.sched_type == 'W') ? "Weekly schedule" :
		(Opt.sched_type == 'D') ? "Daily schedule" :
				      "Fiscal year");
	    fy_bm = get_int(fy_bm, 2, 1, 12);
			/* make month J=0, F=1, . . . D=11 */

	    if (Opt.sched_type != 'M')
	    {
		fprintf(stderr, "\n%s beginning date  (1-31): ",
		    (Opt.sched_type == 'W') ? "Weekly schedule" :
		    (Opt.sched_type == 'D') ? "Daily schedule" :
					  "Fiscal year");
		fy_bd = get_int(fy_bd, 2, 1, 31);
	    }
	    else fy_bd = 1;	/* make months start on first */
	}
    }

    if ( (Opt.sched_type) &&
	 (Opt.sched_type != 'M') )
    {
	fprintf(stderr, "\n%s schedule beginning hour  (0-23): ",
	    (Opt.sched_type == 'W') ? "Weekly" :
				  "Daily" );
	Opt.bh = get_int(Opt.bh, 2, 0, 23);

	fprintf(stderr, "\n%s schedule number of hours (1-24): ",
	    (Opt.sched_type == 'W') ? "Weekly" :
				  "Daily" );
	Opt.nh = get_int(Opt.nh, 2, 1, 24);

    }

    if (Opt.sched_type)
    {
fputs("\nYou may enter up to five custom title lines for the schedules.", stderr);
fputs("\nThe first three titles entered will be printed on the top three lines.", stderr);
fputs("\nThe last two titles will be printed below the date titles.", stderr);
	for (i = 0;
	     i < 8;
	     i++  )
	{
	    if (i == 3)
	    {
		fprintf(stderr, "\nTitles #4, #5, and #6 are used for dates");
		i = 6;	/* So skip to Title #7 */
	    }
	    fprintf(stderr,
      "\nTitle #%d currently is (RETURN to keep it the same, SPACE to erase):",
				     i + 1);
	    fprintf(stderr, "\nTitle #%d: %s", i + 1,
					  Opt.title[i][0] ?
					  Opt.title[i]    :
					  "\t(No title currently defined)" );
	    fprintf(stderr, "\nTitle #%d: ", i + 1);
	    gets(Opt.title[4]);
	    if (Opt.title[4][0] != '\0')
		strcpy(Opt.title[i], Opt.title[4]);
	}
    }

    fputs("\nProgrammer's Calendar", stderr);
    i = get_yn(Opt.base ? 'y' : 'n');
    if (!i)			/* Decimal numbers used */
	Opt.base = FALSE;
    else			/* Hexadecimal or Octal numbers */
    {
	fprintf(stderr,
	"\n<H>exadecimal or <O>ctal numbering (H/O)? %c\b",
			       (Opt.base == FALSE) ? 'H' : Opt.base);
	for ( ;; )
	{
	    c = toupper(getch());
	    switch(c)
	    {
		case  'O': break;
		case  'H': break;
		case '\r':
		case '\n': c = (Opt.base == FALSE) ? 'H' : Opt.base ; break;
		default  : c = '?';
	    }
	    if( (c == 'H') || (c == 'O') )
	    {
		putch(c);
		Opt.base = c;
		break;
	    }
	    continue;
	}
    }

    fputs("\nThe calendar(s) may be composed in several languages:\n", stderr);
    for (i = 0;
	 i < lingos;
	 i++)
    {
	if (!(i % 3))
	    fputs("\n", stderr);
	fprintf(stderr, "%2d = %-15s",
			 i +1, lingo[i]);
    }


    fprintf(stderr, "\nWhich language for this calendar? (1-%d): ", lingos);
    Opt.lang = get_int(Opt.lang, 2, 1, lingos);

    fputs("\n<V>iew, <P>rint, or write to <F>ile(s) (V/P/F)? V\b", stderr);
    for ( ;; )
    {
	c = toupper(getch());
	switch(c)
	{
	    case  'F'  : outp = 'F'; break;
	    case  'P'  : outp = 'P'; break;
	    case '\033': exit(0);
	    case '\r'  :
	    case '\n'  :
	    case  'V'  : outp = 'V'; break;
	    default    : outp = '?';
	}
	if( (outp == 'F') || (outp == 'V') || (outp == 'P') )
	{
	    putch(c);
	    break;
	}
	continue;
    }

    if (Opt.sched_type)		/* Page width/length useful only */
				/* in M/W/D schedules at this time */

    if ( (outp == 'P') ||	/* We want hardcopy */
/*       (outp == 'V') || */	/* He wants to look at it */
	 (outp == 'F')  )	/* He wants a file */
    {
	fputs("\nType # of lines for each page (51-132): ", stderr);
	Opt.pl = get_int(Opt.pl, 3, 51, 132);

	fputs("\nType # of characters for each line (40-270): ", stderr);
	Opt.pw = get_int(Opt.pw, 3, 40, 270);

	fputs("\nType # of spaces for left margin indent (0-40): ", stderr);
	Opt.indent = get_int(Opt.indent, 2, 0, 40);
    }

    if (outp == 'P')		/* We want hardcopy */
    {
	fprintf(stderr,
	"\nType # of copies desired of each %s (1-999): ",
		     (Opt.sched_type == 'M') ? "month" :
		     (Opt.sched_type == 'W') ? "week"  :
		     (Opt.sched_type == 'D') ? "day"   :
					   "year"  );
	Opt.copies = get_int(Opt.copies, 3, 1, 999);

	fputs("\nPause after each page", stderr);
	Opt.pause = get_yn(Opt.pause ? 'y' : 'n');
    }

    setup_files(outp);

    by = year;
    bm = fy_bm -1;
    bd = fy_bd;

    for (i = 0;
	 i < Opt.more_yrs + 1;
	 i++, (Opt.sched_type == 'M') ? bm++:
	      (Opt.sched_type == 'W') ? bd += 7:
	      (Opt.sched_type == 'D') ? bd++:
				    by++)
    {
	if (leap(by))		/* Tidy up month length array */
	    mon[1].length = 29; /* For this year by fixin' February. . . */
	else
	    mon[1].length = 28;

	if (bd > mon[bm].length)/* Past this End Of Month */
	{
	    bd -= mon[bm].length;
				/* So make it appropriate day. . . */
	    bm++;		/* Of following month. . . */
	}

       if (bm > 11)		/* Egad! We are past December. . . */
       {
	    bm = bm % 12;	/* Normalise it. . . */
	    if (bm == 0)	/* Now have January? J=0, F=1 D=11 */
		by++;		/* Ratchet the year. . . */
       }

	if (! (Opt.julian))		/* Prepare the calendar arrays */
	    prep_cal(by);

	for ( j = 0;
	      j < Opt.copies;
	      j++)
	    {
	       if (Opt.sched_type)
		   sched_printer(by, outp, bm, bd, Opt.bh, Opt.nh, Opt.pl, Opt.pw, Opt.sched_type, Opt.title, Opt.base, Opt.lang, Opt.pause, Opt.indent);
	       else
	       if (Opt.julian)
		   jul_printer(by, outp, Opt.base, Opt.lang, Opt.pause);
	       else
	       if (Opt.aggie)
		   agg_printer(by, outp, Opt.base, Opt.lang, Opt.pause);
	       else
	       if (Opt.fy)
		   cal_printer(by, outp, bm, Opt.fy, bd, Opt.base, Opt.lang, Opt.pause);
	       else
		   cal_printer(by, outp, 0,  Opt.fy,  1, Opt.base, Opt.lang, Opt.pause);
	    }
    }
    fputs("\nAnother calendar", stderr);
    if (!get_yn('n'))
    {
	if(outp != 'F')
	    fclose(fp);
/*      printf("\nClosed %s\n", file);*/
        fputs("\nRetain present options as defaults", stderr);
        if (get_yn('n'))
            put_opts(argv[0]);
	exit(0);
    }
  }
}



void setup_files(out)
char out;
{
    if (fp) fflush(fp);
    if(out == 'V')
    {
	if(!strcmp(file, "PRN"))
	{
	    fclose(fp);
/*	   printf("\nClosed %s\n", file);*/
	}
	if(strcmp(file, "CON"))
	{
	    strcpy(file, "CON");
	    fp = fopen(file, "w");
/*	  printf("\nOpened %s for write\n", file);*/
	}
    }
    else
    if(out == 'P')
    {
	if(!strcmp(file, "CON"))
	{
	    fclose(fp);
/*	  printf("\nClosed %s\n", file);*/
	}
	if(strcmp(file, "PRN"))
	{
	    strcpy(file, "PRN");
	    fp = fopen(file, "w");
/*	  printf("\nOpened %s for write\n", file);*/
	}
    }
}



void prep_cal(year)
int year;
{
    int count,		/* Count of the days */
	day,		/* Weekday Sun=0, Mon=2, ... Sat=6 */
	month,		/* Month Jan=0, Feb=1, ... Dec=11 */
	week,		/* Six weeks/month possible */
	yeer,		/* Year for calendar */
	y,		/* Year Last=0, This=1, Next=2 */
        leap(),
	weekday_month();
/*printf("\nIn Prep_cal\n"); */
    for (y = 0;		/* Start at last year y[0] */
	 y < 3;		/* Stop after next year y[2] */
	 y++)
    {
/*      printf("\nIn Prep_cal, y = [%d]\n", y); */
	yeer = (year + y - 1);
	if (leap(yeer))	/* Tidy up month length array */
	    mon[1].length = 29; /* For this year by fixin' February. . . */
	else
	    mon[1].length = 28;

	/* get weekday of first day of each month, put into array mon */
	/* Su = 0, Mo = 1, Tu = 2, We = 3, Th = 4, Fr = 5, Sa = 6   */
	for (month = 0;
	     month < 12;	/* 12 months/year */
	     month++)
	mon[month].first_day = weekday_month(month, yeer);

	/* form year array with 0 for unused dates, with dates */
	/* in their relative positions by weekday */
	for (month = 0;
	     month < 12;	/* 12 months/year */
	     month++)
	{
	    count = 0;		/* set day counter to 0 */
	    for (week = 0;
		 week < 6;	/* 6 weeks/month */
		 week++)
	    {
		for (day = 0;
		     day < 7;	/* 7 days/week */
		     day++)
		{
		    if( (week == 0) &&  /* first week of month */
			(day < mon[month].first_day))
					/* before first day */
			yr[y][month][week][day] = 0;
					/* Zero it */
		    else
		    if (++count <= mon[month].length)
					/* Not past End of Month */
			yr[y][month][week][day] = count;
		    else
			yr[y][month][week][day] = 0;
					/* Zero it */
		}
	    }
	}
    }
}



/* Function to get first day index (day of week of 1st of month) */
/* Jan = 0, Feb = 1, Mar = 2, Apr = 3, . . .  Dec = 11 */
/* Su = 0, Mo = 1, Tu = 2, We = 3, Th = 4, Fr = 5, Sa = 6 */
int weekday_month(Monat, Jahr) /* 1st day of any month after 1900 */
int Monat;
int Jahr;
{
    long int date,	/* date after 1901 */
	     numb_days; /* number of days to deal with */
    int      mt[12] = {   0,  31,  59,  90, 120, 151,
			181, 212, 243, 273, 304, 334  };
    date       = Jahr - 1901;	/* subtract 1901 from year */
    numb_days  = date * 365;	/* # of yrs times 365 days/year */
    numb_days += ( ( date / 4)	/* add number of leap years */
	      - (date / 100)	/* subtract non-leap centuries */
	      + (date / 400 ))	/* add number of leap centuries */
	      + mt[Monat];	/* add days offset for this year */
    if ( ( Jahr % 4) == 0 &&	/* if a conventional leap year */
	   Monat > 1 )		/* and after February */
	numb_days++;		/* add a day for Feb 29th */
    return((numb_days +2) %7);	/* divide the number of days by 7 */
				/* and return the remainder */
}



/* Function to decide if a year is a leap year */
int leap(years)
int years;
{			/* Leap year sieve */
    if (years & 0x0003) /* Mask year to see if not leap year */
       return FALSE;    /* If we look at the last two bits, and */
			/* one of them is on, then the year can't */
			/* be divisible by four.  Thus it is not */
			/* a leap year, so function returns FALSE. */
			/* Test for the leap century. */
    else		/* Leap century:   years that are: */
    if (!(years % 100) && /* evenly divisible by 100:  not leap */
	 (years % 400) )  /* years unless evenly divisible by 400 */
	return FALSE;	/* Gruenberger and Jaffray, "Problems for */
			/* Computer Solution", Wiley, 1965. */
			/* We have filtered out all years that are */
			/* not evenly divisible by four as well as */
			/* those evenly divisible by 100 that are */
			/* evenly divisible by 400. */
			/* It seems that all the rest are */
			/*   (a) evenly divisible by four */
			/*   (b) evenly divisible by 400  */
    return TRUE;	/*   (c) Thus, leap years. . . */
}



void cal_printer(year, out, bm, fy, bd, base, lang, pause)
int year;
char out;
int bm;
char fy;
int bd;
char base;	/* Number base used, 0 = Decimal, H = Hex, O = Octal */
int lang;	/* Language used 1=Danish, 2=Dutch, 3=English etc. */
char pause;	/* Page pause True/False */
{
    char hdr1[] = {" Su Mo Tu We Th Fr Sa"},
	 hdr2[] = {" -- -- -- -- -- -- --"};

    int y, month, week, day, count, stat;
    void hold();	/* Printing page pause function */
    char  linbuf[BUF],
	  temp[BUF],
	 *char_str(),
	 *center(),
	 *trim();
    char *off = (out == 'V') ? " " : "       ";
    y = 1;		/* Make year this year */

    sprintf(hdr1, " %2.2s %2.2s %2.2s %2.2s %2.2s %2.2s %2.2s",
	   wkday[lang -1][0], wkday[lang -1][1], wkday[lang -1][2],
	   wkday[lang -1][3], wkday[lang -1][4], wkday[lang -1][5],
	   wkday[lang -1][6] );

    if(out == 'F')		/* setup for file output */
    {
	sprintf(file, fy ?
		  "%dFY%c" :
		  "%d%c",
		   year, base);
	strcpy(new_file, file);
	strcat(file, ".$$$");
	sprintf(temp, ".%3.3s", lingo[lang -1]);
	strcat(new_file, temp);
	fp = fopen(file, "w");
/*      printf("\nOpened %s for write\n", file);*/
    }

    if ((pause)     &&		/* User wants time after each page */
	(out == 'P') )		/* gets printed to add paper etc... */
	hold();			/* Wait for keystroke */

    fputs("\n\n\n", fp);
    sprintf(temp, fy ? "%s%s%s Calendar for Fiscal Year %d-%d":
		       "%s%s%s Calendar for %d",
/*	       ( (base == 'O') && (lang != 10) ) ? "Classical " : "" ,*/
		 base ? "Programmer's " : "" ,
		 base ? ( (base == 'O') ? "Octal " : "Hexadecimal ") : "",
		 lingo[lang -1],
		 year, year +1);

    fputs(center(temp, (out == 'V') ? 75 : 89), fp);
    fputs("\n\n\n", fp);

    if (bd == 1)		/* If fy month starts on 1st, */
	fy = FALSE;		/* treat like ordinary month */

    for (month = bm;		/* Start at beginning Month for FY */
	 month < (12 + bm + fy);
	 month += 3)
    {				/* 4 sets of months  3 months across */
	sprintf(linbuf,
	     "%s%s    ", off, center(mnam[lang -1][month %12], 21));

	if (month != (bm +12))	/* NOT FY and 13th month */
	{
	    sprintf(temp,
		"%s    ", center(mnam[lang -1][(month +1) %12], 21));
	    strcat(linbuf, temp);
	    sprintf(temp,
		"%s"    , center(mnam[lang -1][(month +2) %12], 21));
	    strcat(linbuf, temp);
	}

	strcat(linbuf, "\n");	/* add a newline */
	fputs(linbuf, fp);	/* Names of three Months */
	linbuf[0] = '\0';

	if(month != (bm +12))	/* NOT FY and 13th month */
	{
	    fprintf(fp, "%s%s    %s    %s\n",
			off, hdr1, hdr1, hdr1);
	    fprintf(fp, "%s%s    %s    %s\n",
			off, hdr2, hdr2, hdr2);
	}
	else			/* FY and 13th month */
	if (bd != 1)		/* beginning after the 1st */
	{
	    fprintf(fp, "%s%s\n",	/* Names of weekdays */
			off, hdr1);
	    fprintf(fp, "%s%s\n",	/* Lines of dashes */
			off, hdr2);
	}

	for (week = 0;		/* One line per week */
	     week < 6;		/* 6 wks/month possible */
	     week++)
	{
	    sprintf(linbuf, off);

	    for (count = 0;	/* 3 months/line ordinarily */
		 count < ((month == (bm +12)) ?
					    1 : /* FY 13th month alone */
					    3); /* Ordinary line */
		 count++)
	    {
		for (day = 0;
		     day < 7;	/* 7 days/week */
		     day++)
		{
		    if (((month + count) == bm) &&  /* FY 1st month */
			(yr[y][month + count][week][day] < bd))
			 sprintf(temp, "   ");
		    else
		    if (((month + count) == (bm +12)) &&
			(yr[y][month + count][week][day] >= bd))
			 sprintf(temp, "   ");
		    else
		    sprintf(temp,
			yr[y][month + count][week][day] ?
			    (base ?
				((base == 'O') ? "%3o" : "%3X" ) :
						 "%3d" ) :
						 "   ",
			yr[y][month + count][week][day]);
		    strcat(linbuf, temp);
		}
		strcat(linbuf,
		count < ((month < (bm +12)) ? 2 : 0) ? "    ": "\n" );
	    }
	    fputs(linbuf, fp);
	    linbuf[0] = '\0';
	}
	fputs("\n\n", fp);
    }
    if (out != 'V') fputs("\f\r", fp);	/* no FF for screen */

    if (out == 'F')			/* setup for file output */
	if ((stat = fclose(fp)) == ERROR)
	    printf("\nCan't close %s.\n", file);
/*      else
	printf("\nClosed %s\n", file);*/

    if (out == 'F')
    {
	if ((stat = rename(file, new_file)) == ERROR)
	{
	    printf(
	    "\nCan't rename %s to %s.  %s may already exist.\n",
				 file, new_file, new_file);
	}
    }
}



void agg_printer(year, out, base, lang, pause)
int year;
char out;
char base;	/* Number base used, 0 = Decimal, H = Hex, O = Octal */
int lang;	/* Language used 1=Danish, 2=Dutch, 3=English etc. */
char pause;	/* Page pause True/False */
{
    int mf[12][2];	/* mf flag for aggie calendar */

    char agg1[] = {" Ja Fe Ma Ap Ma Jn Jl Au Se Oc No De"},
	 agg2[] = {" -- -- -- -- -- -- -- -- -- -- -- --"};

    int y, month, week, day, count, stat, i, j;
    void hold();	/* Printing page pause function */
    char  linbuf[BUF],
	  temp[BUF],
	 *char_str(),
	 *center(),
	 *trim();
    char *off = (out == 'V') ? " " : "    ";
    y = 1;		/* Make year this year */
    sprintf(agg1, " %2.2s %2.2s %2.2s %2.2s %2.2s %2.2s %2.2s %2.2s %2.2s %2.2s %2.2s %2.2s",
	   mnam[lang -1][0], mnam[lang -1][ 1], mnam[lang -1][ 2],
	   mnam[lang -1][3], mnam[lang -1][ 4], mnam[lang -1][ 5],
	   mnam[lang -1][6], mnam[lang -1][ 7], mnam[lang -1][ 8],
	   mnam[lang -1][9], mnam[lang -1][10], mnam[lang -1][11] );

    if (out == 'F')
    {
	sprintf(file, "%dAGG%c", year, base);
	strcpy(new_file, file);
	strcat(file, ".$$$");
	sprintf(temp, ".%3.3s", lingo[lang -1]);
	strcat(new_file, temp);
	fp = fopen(file, "w");
/*      printf("\nOpened %s for write\n", file);*/
    }

    if ((pause)     &&		/* User wants time after each page */
	(out == 'P') )		/* gets printed to add paper etc... */
	hold();			/* Wait for keystroke */
    fputs("\n\n\n", fp);
    sprintf(temp, "%sAggie %s%sGig'Em %s Calander fer %d",
		 (base == 'O') ? "Olde " : "" ,
		 base ? "Hacker's " : "" ,
		 base ? ( (base == 'O') ? "Octal " : "Hexadecimal ") : "",
		 lingo[lang -1],
		 year);
    fputs(center(temp, (out == 'V') ? 75 : 89), fp);
    fputs("\n\n\n", fp);
    for (day = 0;
	 day < 7;		/* 4 sets of days */
	 day += 2)		/* 2  days across */
    {
	 for (i = 0;
	      i < 12;		/* 12 months in a day */
	      i++)
	     for (j = 0;
		  j < 2;	/* 2  days across */
		  j++)
		      mf[i][j] = 0;
	if (day < 6)
	{
	    sprintf(linbuf, "%s%s    ", off, center(wkday[lang -1][day], 36));
	    sprintf(temp,   "%s\n"  , center(wkday[lang -1][day +1], 36));
	    strcat(linbuf, temp);
	    fputs(linbuf, fp);
	    linbuf[0] = '\0';
	    fprintf(fp, "%s%s   %s\n", off, agg1, agg1);
	    fprintf(fp, "%s%s   %s\n", off, agg2, agg2);
	}
	else
	{
	    fprintf(fp, "%s%s\n", off, trim(center(wkday[lang -1][day], 79) ) );
	    fprintf(fp, "%s%s\n", off, trim(center(agg1, 79) ) );
	    fprintf(fp, "%s%s\n", off, trim(center(agg2, 79) ) );
	}
	for (week = 0;
	     week < 5;		/* 5 weeks/day possible */
	     week++)
	{
	    sprintf(linbuf, off);
	    for (count = 0;	/* 2 days across except Saturday */
		 count < ( (day == 6) ? 1 : 2) ;
		 count++)
	    {
		if (day == 6)
		    strcat(linbuf, "		      ");
		for (month = 0;
		     month < 12;	/* 12 mnths/day */
		     month++)
		{
		    if (!yr[y][month][week][day + count])
			mf[month][count] = 1;
		    sprintf(temp,
		     yr[y][month][week +mf[month][count]][day +count] ?
			    (base ?
				((base == 'O') ? "%3o" : "%3X" ) :
						 "%3d" ) :
						 "   ",
		     yr[y][month][week +mf[month][count]][day + count]);
		    strcat(linbuf, temp);
		}
		strcat(linbuf, count < 1 ? "   ": "\n" );
		if (day == 6) strcat(linbuf, "\n");
	    }
	    fputs(linbuf, fp);
	    linbuf[0] = '\0';
	}
	fputs("\n\n", fp);
    }
    if (out != 'V') fputs("\f\r", fp);	/* no FF for screen */
    if(out == 'F')			/* setup for file output */
	if ((stat = fclose(fp)) == ERROR)
	    printf("\nCan't close %s.\n", file);
/*      else
	printf("\nClosed %s\n", file);*/
    if(out == 'F')
    {
	if ((stat = rename(file, new_file)) == ERROR)
	{
	    printf(
	    "\nCan't rename %s to %s.  %s may already exist.\n",
				 file, new_file, new_file);
	}
    }
}



void jul_printer(year, out, base, lang, pause)
int year;
char out;
char base;	/* Number base used, 0 = Decimal, H = Hex, O = Octal */
int lang;	/* Language used 1=Danish, 2=Dutch, 3=English etc. */
char pause;	/* Page pause True/False */
{
    int month, day, stat, count, leep;
    void hold();	/* Printing page pause function */
    char  temp[BUF],
	 *char_str(),
	 *center(),
	 *trim();
    int jul[12][31];	/* Array for Julian yr, 12 months/year */
			/* 31 days/month */
    char *off = (out == 'V') ? " " : "    ";
    int month_length[12] =
    {
	0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334
    };
    char jul1[] = {"Month:   Jan  Feb  Mar  Apr  May  Jun  Jul  Aug  Sep  Oct  Nov  Dec	   "};
    char jul2[] = {"	 ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---"};

    leep = leap(year);	/* Find out if leap year */
    sprintf(jul1, "	 %3.3s  %3.3s  %3.3s  %3.3s  %3.3s  %3.3s  %3.3s  %3.3s  %3.3s  %3.3s  %3.3s  %3.3s",
	   mnam[lang -1][0], mnam[lang -1][ 1], mnam[lang -1][ 2],
	   mnam[lang -1][3], mnam[lang -1][ 4], mnam[lang -1][ 5],
	   mnam[lang -1][6], mnam[lang -1][ 7], mnam[lang -1][ 8],
	   mnam[lang -1][9], mnam[lang -1][10], mnam[lang -1][11] );
    if (out == 'F')
    {
	sprintf(file, "%dJUL%c", year, base);
	strcpy(new_file, file);
	strcat(file, ".$$$");
	sprintf(temp, ".%3.3s", lingo[lang -1]);
	strcat(new_file, temp);
	fp = fopen(file, "w");
/*      printf("\nOpened %s for write\n", file);*/
    }

    if ((pause)     &&	/* User wants time after each page */
	(out == 'P') )	/* gets printed to add paper etc... */
	hold();		/* Wait for keystroke */
    fputs("\n\n\n", fp);
    sprintf(temp, "%s%s%s%s 3-Digit Julian Calendar for %d",
		 (base == 'O') ? "Olde " : "" ,
		 base ? "Hacker's " : "" ,
		 base ? ( (base == 'O') ? "Octal " : "Hexadecimal ") : "",
		 lingo[lang -1],
		 year);
    fputs(center(temp, (out == 'V') ? 75 : 89), fp);
    fputs("\n\n\n", fp);
    fprintf(fp, "%s%s\n", off, jul1);
    fprintf(fp, "%s%s\n", off, jul2);

    for (month = 0;
	 month < 12;	/* 12 months */
	 month++)
	for (day = 1;
	     day < 32;	/* 31 days possible */
	     day++)
	{
	    count = (day + month_length[month] + ((month > 1) ? leep : 0) );
	    if (count > (month_length[month +1] + ((month) ? leep : 0)) )
		count = 0;
/*	  fprintf(stderr, "%6d", count); */
	    jul[month][day -1] = count;
	}

    for (day = 0;
	 day < 31;
	 day++)
    {
	if ( (day)       &&
	     (!(day % 2)) )
		fprintf(fp, "\n");
	fprintf(fp, "%sDay #%2d", off, day +1);

	for (month = 0;
	     month < 12;
	     month++)
	{
	    if (jul[month][day])
	    {
		fprintf(fp,
		   (base ?
		       ((base == 'O') ?
			   "  %3o" :
			   "  %3X" ) :
			   "  %3d" ),
			  jul[month][day]);
	    }
	    else
		fprintf(fp, "     ");
	}
	fprintf(fp, "   Day #%2d\n", day +1);
    }

    if (out != 'V') fputs("\f\r", fp);	/* no FF for screen */
    if (out == 'F')			/* setup for file output */
	if ((stat = fclose(fp)) == ERROR)
	    printf("\nCan't close %s.\n", file);
/*      else
	printf("\nClosed %s\n", file);*/
    if (out == 'F')
    {
	if ((stat = rename(file, new_file)) == ERROR)
	{
	    printf(
	    "\nCan't rename %s to %s.  %s may already exist.\n",
				 file, new_file, new_file);
	}
    }
}



char *center(str, width)
char *str;
int width;
{
    static char buf[BUF];	/* location of finished string */
    static char copy[BUF];	/* copy of str to avoid side effects */
    char  filler,
	 *char_str(),
	 *strcpy(),
	 *strcat();
    int i;

    filler = ' ';
    strcpy(copy, str);          /* copy str to copy */
    if (strlen(copy) >= width)	/* Test for str longer than width */
	copy[width] = '\0';	/* Terminate it at width */
    strcpy(buf, char_str( (width - strlen(copy) ) >> 1, filler));
    strcat(buf, copy);
    i = strlen(buf);
    while (width - i)
	 buf[i++] = filler;
    buf[i] = '\0';
    return(buf);
}



char *trim(str)		/* trim spaces from end of string */
char *str;
{
    static char buf[BUF];	/* location of finished string */
    char  filler,
	 *char_str(),
	 *strcpy(),
	 *strcat();
    int i;
    filler = ' ';
    strcpy(buf, str);
    i = strlen(buf);
    while (buf[--i] == filler)
	buf[i] = '\0';
    return(buf);
}



char *char_str(length, c)
int length;
char c;
{
    static char loc[BUF];
    char *ptr = loc;

    while (length--)
	*ptr++ = c;
    *ptr = '\0';
    return(loc);
}



int get_int(defalt, digits, lowest, highest)
int defalt,	/* default value when C/R alone is struck */
    digits,	/* maximum number of digits accepted */
    lowest,	/* lowest value accepted */
    highest;	/* highest value accepted */
{
    char buffer[80];	/* Buffer to store input string */
    int ch;		/* All purpose character ch */
    int index = 0;	/* Character array index */
    int value = 0;	/* value to return */
    int done = FALSE;	/* Done/Not Done Flag */
    char dfalt[10];	/* Buffer to store default value as string */
    int i, ii;		/* length of default value as a string */

    sprintf(dfalt, "%d", defalt); /* Convert default to string */
    i = ii = strlen(dfalt);	/* Calculate its length */
    printf("%s", dfalt);	/* Display default value */
    while (ii--)
	putch('\b');		/* Backspace to beginning */

    while (!done)
    {
	while (index < digits)  /* while not enough digits, grab 'em */
	{
	    ch = getch();  	/* Get a char from console, no echo */

	    if (isdigit(ch))	/* Digit!! so . . .  */
	    {
		putch(ch);	/* Echo it to console, and */
		buffer[index++] = ch;	/* Stuff it into buffer */
		if (index == 1)	 /* Entered first digit, so. . . */
		{
		    for (ii = digits; --ii; )
			putch(' ');	/* Blank it out */
		    for (ii = digits; --ii; )
			putch('\b');	/* Backspace to beginning */
		}
	    }

	    else
	    if ((ch == '\b') && /* Backspace? */
		(index) )	/* if index not zero */
	    {
		--index;	/* Reduce index */
		putch(' ');	/* Blank it */
		putch('\b');	/* Backspace over blank */
		putch(ch);	/* Echo it to console */
	    }

	    else
	    if (ch == '\033')	/* Escape */
		exit(0);	/* says he's through */

	    else
	    if (ch == '\r')	/* Return? */
		break;		/* says he's through */
	}	/* End while not enough digits, grab 'em */

	buffer[index] = '\0';	/* Terminate string */

	if (index == 0)		/* no digits entered before return */
	{
	    value = defalt;	/* so use default */
	    done  = TRUE;	/* We're through */
	}

	else			/* some digits were entered */
	{
	    value = atoi(buffer);	/* Convert string to integer */

	    if ((value < lowest) ||	/* Range test value returned */
		(value > highest) )
	    {				/* Out of allowable range. . . */
		for (	    ;
		       index;
		     --index )
		    putch('\b');	/* Backspace to beginning */
		for (ii = digits;
		     ii;
		     --ii)
		    putch(' ');		/* Blank it out */
		for (ii = digits;
		     ii;
		     --ii)
		    putch('\b');	/* Backspace to beginning */
		printf("%s", dfalt);    /* Display default value */
		for (ii = i;
		     ii;
		      --ii)
		    putch('\b');	/* Backspace to beginning */
	    }

	    else
	    {
		done  = TRUE;		/* We're through */
	    }
	}
    }		/* end while !done */
    return(value);
}



int get_yn(defalt)
char defalt;
{
    char c;			/* all purpose character */
    fputs(" (y/n) ? ", stderr); /* Prompt for Yes or No */
    putch(defalt);		/* Display default */
    putch('\b');		/* Backspace to it */
    do
    {
	c = tolower(getch());	/* Get a character, make it lowercase */
    }   while ( (c != 'y') &&	/* Until we get a y, n, ESC, or return */
		(c != 'n') &&
		(c != '\033') &&
		(c != '\r') );

    if (c == '\033')	/* Escape */
	exit(0);	/* says he's through */
    else
    if (c == '\r')	/* If a return, make it the default */
	c = defalt;
    putch(c);		/* Display choice to user */
    return (c == 'y') ? TRUE : FALSE ;
}



void hold()
{
    fputs("\nPrinter ready?  Any key to continue. . . ", stderr);
    getch();
}



void signon()
{
    fputs(
    "\t\t\tYEARCAL v0.07T 3 Mar 1988\n\n\t\tCopyright 1987, 1988 by Paul M. Sittler\n\n", stderr);
    fputs(
    "\tYEARCAL makes calendars (A/M) or schedules (W/D) for years after 1901.\n\n", stderr);
    fputs(
    "\tAt the user's option, Normal, Fiscal Year, AGGIE, or 3-Digit Julian\n", stderr);
    fputs(
    "\tDate Calendars can be produced, in multiple copies, for a number of\n", stderr);
    fputs(
    "\tconsecutive years, or both.  Fiscal Year calendars may be produced for\n", stderr);
    fputs(
    "\tany arbitrary 12 or 13 month fiscal year.  The user may select a\n", stderr);
    fputs(
    "\tProgrammer's calendar, numbered in Hexadecimal or Octal numbers rather\n", stderr);
    fputs(
    "\tthan decimal numbers, and may choose one of several languages.\n\n", stderr);
    fputs(
    "\tThe calendars may be displayed on the screen, printed, or written to\n", stderr);
    fputs(
    "\ta file as desired.  If written to disk, files are named like YYYY.ENG\n", stderr);
    fputs(
    "\t(Normal ENGlish), YYYYFY.DUT (Fiscal-Year DUTch), YYYYAGG.TEX (Aggie-\n", stderr);
    fputs(
    "\ttype TEXan), or YYYYJUL.POL (3-Digit Julian Date POLish) calendars,\n", stderr);
    fputs(
    "\tetc.  Hex or Octal calendar files will have an \"H\" or \"O\" added to\n", stderr);
    fputs(
    "\ttheir names, such as YYYYH.SPA, YYYYFYO.SER or YYYYAGGH.FRE.\n\n", stderr);
    fputs(
    "\tYEARCAL may be freely distributed, and used for non-commercial\n", stderr);
    fputs(
    "\tpurposes, as long as the unmodified program source code and\n", stderr);
    fputs(
    "\tdocumentation files are distributed with it.\n", stderr);
}



void sched_printer(year, out, bm, bd, bh, nh, pl, pw, sched_type, title, base, lang, pause, indent)
int year;	/* Year for calendar */
char out;	/* Output goes where? View, Printer, File */
int bm;		/* Month (beginning) for schedule */
int bd;		/* Day (beginning) for schedule */
int bh;		/* Hour (beginning) for schedule */
int nh;		/* number of hours to schedule (1-24) */
int pl;		/* Page Length in lines */
int pw;		/* Page Width in characters or columns */
char sched_type;/* Schedule type M(onthly), W(eekly), D(aily) */
char title[][LIN];
char base;	/* Number base used, 0 = Decimal, H = Hex, O = Octal */
int lang;	/* Language used 1=Danish, 2=Dutch, 3=English etc. */
char pause;	/* Page pause flag True/False */
int indent;	/* Left margin indent (# of spaces) */
{
    int bw,	/* Box width */
	day,
	first_day,
	i, j,
	month,
	stat,
	w,	/* Week number for printing top calendars */
	wk_day,
	y;
    void hold();	/* Printing page pause function */
    char  linbuf  [BUF],
	  temp    [BUF],
	  temp1   [BUF],
	  temp2   [BUF],
	  bnd     [BUF],
	  sep     [BUF],
	  hoursw  [BUF],
	  spacerw [BUF],
	  dates   [BUF],
/*	bc, */
	 *char_str(),
	 *center(),
	 *trim();
    char *off = ( (out == 'V') ?
			   " " :	/* Offset for screen */
    "                                        ");
    /* 40 spaces-----------------------------| Max Offset for printing */
    off[indent] = '\0';		/* clip offset to proper length */
    y = 1;			/* Make year this year */
/*  printf(
"\nArgs: year=[%d], out=[%c], bm=[%d], bd=[%d], bh=[%d], nh=[%d], pl=[%d]\n",
       year,      out,      bm,      bd,      bh,      nh,      pl);
    printf(
"Args: pw=[%d], sched_type=[%c], base=[%c], lang=[%d]\n",
       pw,      sched_type,      base,      lang);
 */
    if (out == 'V')		/* Gonna View it, so. . . */
	  pw = 80  ;		/* pw is screen width */

    if (out == 'F')		/* setup for file output */
    {
	(sched_type == 'M') ?
	    sprintf(file, "%d%3.3s%c", year, mnam[lang -1][bm], base):
	    sprintf(file, "%d%02d%02d", year, bm + 1, bd);
	strcpy(new_file, file);
	strcat(file, ".$$$");
	sprintf(temp, ".%3.3s", lingo[lang -1]);
	strcat(new_file, temp);
	fp = fopen(file, "w");
/*      printf("\nOpened %s for write\n", file);*/
    }

    first_day = weekday_month(bm, year);	/* 1st day of month */
    if (sched_type == 'M')
	wk_day = 0;
    else
	wk_day = ( (first_day + bd -1) % 7);	/* Gives us weekday */

    sprintf(title[5], "%s",
		   (sched_type == 'M')?		/* Monthly calendar */
		   "Monthly Calendar" :
		   (sched_type == 'W')? /* if weekly instead of daily */
		    "Weekly Schedule" :
		    "Daily Schedule");

    sprintf(title[4], (sched_type == 'W') ?
		     "Week begins on %s" :
			 (sched_type == 'D') ?
					"%s" :
					  "" ,
		       wkday[lang -1][wk_day]);

    if (sched_type == 'M')
    {
	sprintf(title[3], "%s %d",
		   mnam[lang -1][bm], year );
    }
    else
	sprintf(title[3], "%d %s, %d",
		   bd,
		   mnam[lang -1][bm],
		   year );

    strcpy(hoursw, off);		/* Put offset into hoursw */
    strcpy(spacerw, off);		/* Put offset into spacerw */

    if (sched_type != 'M')		/* If Daily or Weekly */
    {
	sprintf(temp, "| %s00 |",
			    (base ?
		   ((base == 'O') ?
			    "%02o" :
			  "%02X" ) :
			  "%02d" ) );
	strcat(hoursw, temp);		/* Add header info to hoursw */
	strcat(spacerw, "|      |");	/* Add header info to spacerw */
    }
    else				/* Monthly Calendar, so. . . */
    {
	strcat(hoursw, "|");		/* Add header info to hoursw */
	strcat(spacerw, "|");		/* Add header info to spacerw */
    }

    bw = (((pw - strlen(hoursw) ) / 7) -1);  /* Calculate Box Width */
    for (i = 0;				/* Form strings for each day */
	 i < bw;
	 i++)
    {
	temp [i] = ' ';			/* Using spaces */
    }
    if (sched_type == 'M')		/* Monthly Calendar */
    {
	strcpy(dates, temp);		/* Copy temp to dates for length */
	dates[i -2] = '\0';		/* Cut dates to length */
    }

    temp[i++] =  (sched_type == 'D')?	/* If Daily */
				 ' ':	/* Add ' ' */
				 '|';	/* Otherwise add '|' */
    temp[i]   = '\0';			/* Terminate string */

    for (i = 0;				/* Adding multiple temps to */
	 i < 7;
	 i++)
    {
	if (sched_type != 'M')
	    strcat(hoursw, temp);	/* Add temp to hoursw */
	strcat(spacerw, temp);		/* Add temp to spacerw */
    }
    if(sched_type == 'D')
    {
	hoursw [strlen(hoursw) -1] = '|';
	spacerw[strlen(spacerw)-1] = '|';
    }

    strcat(spacerw, "\n");	/* Add '\n' */
    strcat(hoursw, "\n");	/* Add '\n' */

    strcpy(bnd, off);		/* Put offset into boundary */
    strcpy(sep, off);		/* Put offset into separator */

    for (i = 0;			/* Draw them lines. . . */
	 i < (strlen(spacerw) - strlen(off) -1);
	 i++)
    {
	temp[i] = '-';		/* Using Hyphens */
    }
    temp[i]     = '\n';		/* Add Newline */
    temp[i +1]  = '\0';		/* Terminate string */
    strcat(bnd, temp);		/* Add temp to bnd */
    temp[0]     = '|';
    temp[i -1]  = '|';
    strcat(sep, temp);		/* Add temp to sep */

    pw = strlen(bnd) -1;	/* Adjust Page Width */

    if ((pause)     &&	/* User wants time after each page */
	(out == 'P') )	/* gets printed to add paper etc... */
	hold();		/* Wait for keystroke */
    fputs("\n\n", fp);		/* Print two blank lines */
    /* Line One: Last and Next Month's names */
    sprintf(temp, "%s %d",	/* Last month's name and year */
		   mnam[lang -1]
		      [(sched_type == 'M') ?
			   (bm +11) % 12 :
				      bm ],
		       (sched_type == 'M') ?
				 (bm == 0) ?
				 year -1 :
				    year :
				    year );
    sprintf(temp2,"%s %d",	/* Next month's name and year */
		   mnam[lang -1][(bm +1) %12],
		   (bm == 11) ? year +1 : year );
    strcpy(linbuf, off);
    strcat(linbuf, center(temp, 21) );
    strcat(linbuf, center(title[0], (pw -42 -strlen(off)) ) );
    strcat(linbuf, center(temp2, 21) );
    strcat(linbuf, "\n");
    fputs(linbuf, fp);

    /* Line Two: Weekday headers */
    temp[0] = '\0';
    for (i = 0;
	 i < 7 ;
	 i++)
    {
	sprintf(temp2, " %2.2s",
			wkday[lang -1][i] );
	strcat(temp, temp2);
    }
    strcpy(linbuf, off);
    strcat(linbuf, center(temp, 21) );
    strcat(linbuf, center(title[1], (pw -42 -strlen(off)) ) );
    strcat(linbuf, center(temp, 21) );
    strcat(linbuf, "\n");
    fputs(linbuf, fp);

    /* Lines 3-8: Last and Next Month's first weeks */
    for (w = 0;
	 w < 6;
	 w++)
    {
	temp[0] = '\0';
	temp2[0] = '\0';
	for (day = 0;
	     day < 7;	/* 7 days/week */
	     day++)
	{
	    sprintf(linbuf,
		 yr[(sched_type == 'M') ? (bm == 0) ? y -1 : y : y]
		 [(sched_type == 'M') ? (bm +11) % 12 : bm][w][day] ?
				       (base ?
			      ((base == 'O') ?
				       "%3o" :
				     "%3X" ) :
				     "%3d" ) :
					"   ",
		 yr[(sched_type == 'M') ? (bm == 0) ? y -1 : y : y]
		 [(sched_type == 'M') ? (bm +11) % 12 : bm][w][day]);
	    sprintf(temp1,
		 yr[((bm == 11) ? y +1 : y)][(bm +1) %12][w][day] ?
				       (base ?
			      ((base == 'O') ?
				       "%3o" :
				     "%3X" ) :
				     "%3d" ) :
					"   ",
		 yr[((bm == 11) ? y +1 : y)][(bm +1) %12][w][day]);
	    strcat(temp, linbuf);
	    strcat(temp2, temp1);
	}
	strcpy(linbuf, off);
	strcat(linbuf, center(temp, 21) );
	strcat(linbuf, center(title[w + 2], (pw -42 -strlen(off)) ) );
	strcat(linbuf, center(temp2, 21) );
	strcat(linbuf, "\n");
	fputs(linbuf, fp);
    }

    fputs(bnd, fp);		/* print boundary */

  if (sched_type != 'M')	/* NOT Monthly calendar */
  {
    sprintf(linbuf, "%s|      |", off);
    for (i = day = bd, month = bm;
	 i < (bd + 7);
	 i++, day++)
    {
	if (day >  mon[month].length)	/* Past this month */
	{
	    day -= mon[month].length;
				/* So make it appropriate day. . . */
	    month++;		/* Of following month. . . */
	    month = month % 12; /* And then normalise it. . . */
	}
	sprintf(temp1, "%d %3.3s", day, mnam[lang -1][month]);
	sprintf(temp, "%s|", center(temp1, bw) );
	strcat(linbuf, temp);
    }
    strcat(linbuf, "\n");
    fputs(linbuf, fp);
  }

    sprintf(linbuf, (sched_type != 'M')?
			   "%s|      |":
			   "%s|", off);
    for (i = wk_day;
	 i < (wk_day + 7);
	 i++)
    {
	sprintf(temp1, "%3.9s", wkday[lang -1][i % 7]);
	sprintf(temp, "%s|", center(temp1, bw) );
	strcat(linbuf, temp);
    }
    strcat(linbuf, "\n");
    fputs(linbuf, fp);

    if (sched_type == 'M')	/* Monthly calendar */
    {				/* Months have 5 or 6 weeks */
	bh = 0;
	nh = (yr[y][bm][5][0])?	/* Sixth week, Sunday? */
			   6:	/* Six weeks/month */
			   5;	/* Five weeks/month */
    }

    for (i = bh;
	 i < (bh + nh);
	 i++)	/* print the hourly lines */
    {
	fputs(sep, fp);		/* print separators */
	if (sched_type == 'M')
	{
	    sprintf(hoursw, "%s|", off);/* Initialize hoursw */
	    for (j = 0;
		 j < 7;
		 j++)
	    {
		sprintf(temp,
			yr[y][bm][i][j] ?
			    (base ?
				((base == 'O') ? "%s%2o|" : "%s%2X|" ) :
						 "%s%2d|" ) :
						 "%s  |",
			dates,
			yr[y][bm][i][j]);
		strcat(hoursw, temp);
	    }
	    strcat(hoursw, "\n");
	}

	fprintf(fp, hoursw, (i % 24)); /* print hour/week line */

	for (j = 2;
	     j < ((pl - 18) / nh);
	     j++)
	    fputs(spacerw, fp);		/* print spacer line(s) */
    }
    fputs(bnd, fp);			/* print boundary */

    if(out != 'V') fputs("\f\r", fp);	/* no FF for screen */
    if(out == 'F')			/* setup for file output */
	if((stat = fclose(fp)) == ERROR)
	    printf("\nCan't close %s.\n", file);
/*  else
        printf("\nClosed %s\n", file);*/
    if(out == 'F')
    {
	if ((stat = rename(file, new_file)) == ERROR)
	{
	    printf(
	    "\nCan't rename %s to %s.  %s may already exist.\n",
				 file, new_file, new_file);
	}
    }
}



/************************************************************************/
void get_opts( name)
char *name;	/* filename from argv[0] */
{
	/* Various statistics (such as the last time the program was run)
	* are stored in a buffer at the end of the executable structure.
	* These stats and the settings for user selected options are all
	* stored in the "options" structure.  This function creates the
	* data area if it doesn't exist and initializes Opt as appropriate.
	* Alan Holub, C-Chest, DDJ Feb 1988.
	*/

    int fd;		/* file descriptor handle */
    int i, *p;		/* used to create checksum */
    void ferr();	/* fatal error handler */

    if( (fd = open( name, O_RDWR | O_BINARY ) ) == ERROR)
    {
	perror( name);
	exit(1);
    }

    lseek(fd, 0L - sizeof(Opt), SEEK_END);

    /* Check to see if all of the options area is still there */
    if( read(fd, (char *) &Opt, sizeof(Opt) ) != sizeof(Opt) )
	ferr( "Internal error:  Can't read options\n");

    /* Check to see if the signature is still as it should be */
    if(strcmp( DEF_SIG, Opt.signature) != 0)
    {
	memset( &Opt, 0, sizeof(Opt));
	/* The memset is for debugging.  It shows us that the record has
	 * been written correctly.
	 */

        strcpy( Opt.signature, DEF_SIG);
        /* Initialize other fields in the Opt structure here. */
    	Opt.aggie	= FALSE;	/* Not all are Aggies! */
	Opt.bh		=  6;		/* Hour defaults to 0600 */
	Opt.copies	=  1;		/* At least one copy. . . */
	Opt.fy		= FALSE;	/* Most are not Fiscal Year */
	Opt.indent      =  7;		/* Left Margin Indent in spaces */
	Opt.julian	= FALSE;	/* Regular calendar is default */
	Opt.lang	=  5;		/* Select English as default Language */
	Opt.more_yrs	=  0;		/* One period at a time. . . */
	Opt.nh		= 24;		/* Scheduled Hour defaults to 24 */
	Opt.pause	= FALSE;	/* No pause at end of each printed page */
	Opt.pl		= 66;		/* Page Length in lines */
	Opt.pw		= 80;		/* Page Width in characters or columns */
	Opt.base	= FALSE;	/* Decimal for most */
	Opt.sched_type 	= FALSE;	/* Annual calendar is default */
	for (i = 0;			/* Initialize title array */
             i < 8;
             i++)
	     Opt.title[i][0] = '\0';

        Opt.chksum = 0;
        for (p = (int *)(&Opt), i = sizeof(Opt) / 2;
             --i >= 0;
		      )
            Opt.chksum -= *p++;

        lseek(fd, 0L, SEEK_END);

        if(write(fd, (char *) &Opt, sizeof(Opt)) != sizeof(Opt) )
	    ferr("Internal error:  Can't initialize options\n");
    }
    close(fd);
}



void put_opts( name)
char *name;
{
    /* Update the options buffer at the end of the .EXE program (which
     * better exist).  If the options buffer doesn't exist, this function
     * will destroy the end of the file.  Alan Holub, C-Chest, DDJ Feb 1988.
     */

    int fd;			/* file descriptor handle */
    int i, *p, checksum;	/* used to create checksum */
    void ferr();		/* fatal error handler */

    if( (fd = open( name, O_RDWR | O_BINARY) ) == -1)
    {
	perror( name );
	exit(1);
    }

    /* Recompute the checksum */

    checksum = 0;
    for (p = (int *)(&Opt), i = sizeof(Opt) / 2;
       --i >= 0;
		  )
	checksum -= *p++;

    if(checksum != Opt.chksum)
	printf("\tDefaults have been changed.");

    Opt.chksum = checksum;

    lseek(fd, 0L - sizeof(Opt), SEEK_END);
    if(write(fd, (char *) &Opt, sizeof(Opt)) != sizeof(Opt) )
	ferr("Can't do final options update\n");
    close(fd);
}



void ferr( fmt)
char *fmt;
{
    /* ferr() is used for fatal error processing.  It is used just like
     * printf().  However, it exits the program with a status of 1
     * immediately after printing the message.  I'm using ANSI, not UNIX
     * variable argument conventions here.  (Alan Holub, C-Chest Book, p. 299)
     */

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    exit(1);
}


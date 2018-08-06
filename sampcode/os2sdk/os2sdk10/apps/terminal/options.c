/***
 *
 * TITLE 
 *
 *	options.c
 *	Copyright (C) Microsoft Corporation 1987
 *	March 1987
 *
 * DESCRIPTION
 *
 *	This module handles the user interface for setting the options for
 *      the COM port and the MODEM.
 *
 * 	If a filename was specified in the command line, the initialisation 
 *	of the COM port and the MODEM will be performed as indicated in 
 *	the file (creation of this file will be described later). If an error 
 *	is encountered in opening the file or, if no file was specified in the 
 *	command line, an option screen is displayed. The user is then required 
 *	to specify the appropriate options for initialisation of the COM port 
 *	and the MODEM.
 * 
 * 	The program will request the following information from the user via 
 *	the option screen:
 *
 *		Port        :
 *	        Baud Rate   :
 *		Data Bits   :
 *		Stop Bits   :
 *		Parity      :
 *		Connect     : 
 *		Dial Type   : 
 *		Wait Tone   :
 *		Wait Carrier:
 *		Phone Number:
 *		Setup File  :
 *
 *	Port:
 *		Specify com port. The available options are com1, com2.
 *	Baud Rate:
 *		The available options are 1200, 2400, 4800, 9600, 110,
 *		150, 300, 600.
 *	Data Bits:
 *		The available options are 7 and 8.
 *	Stop Bits:
 *		The available options are 1 and 2.
 *	Parity:
 *		The available options are even, none and odd.
 *	Connect: 
 *		The available options are computer and modem.
 *	Dial Type:
 *		The available options are tone and pulse.
 *	Wait Tone:
 *		The available options are 10, 15, 20, 25, 50, 75, 100, 
 *		125, 150, 175, 200, 225 and 250 seconds.
 *		Determines how long the modem waits after "picking up
 *		the telephone" before it dials the first digit of the
 *		telephone number. This delay allows time for the central
 *		office to detect the "off-hook" condition of the line and
 *		apply a dial tone.
 *	Wait Carrier: 
 *		The available options are 10, 15, 20, 25, and 30.
 *		Sets the time that the local modem waits for carrier from
 *		the remote modem before hanging up.
 *	Phone Number:
 *		Type the telephone number of the service. Type the entire
 *		number including 1 and the area code if necessary. 
 *		Commas instruct the modem to pause before dialing a number.
 *		For example, if you must dial 9 to get an outside line, type
 *		a comma after the 9. This allows time to wait for a dial
 *		tone before the modem dials the number. You can use hyphens 
 *		in the number for clarity; the modem ignores them.
 *	Setup File:
 *		This is the name of file in which the options will be saved.
 *		The default file name is terminal.ini . If you prefer
 *              a different name, delete the default name and type the
 *              name you prefer. You may include the path name.
 *
 */

#include	<stdio.h>
#include	<doscalls.h>
#include	<subcalls.h>
#include	<malloc.h>
#include	<string.h>
#include 	<memory.h>
#include 	<conio.h>
#include 	"term.h"

extern void far error (int, int);

extern char 	*ErrMsg[NUM_MSG];

#define Z_ALPHA     0x01
#define Z_DIGIT     0x02
#define Z_WHITE     0x04
#define Z_PATH	    0x08
#define Z_PHONE     0x10

#define     O_WIDTH	62
#define     O_HEIGHT	21

char optScreen[O_HEIGHT][O_WIDTH+1] = {
   /* 123456789 123456789 123456789 123456789 123456789 123456789 1 */
    "                    [ Terminal Parameters ]                   ", /*  0 */
    "                                                              ", /*  1 */
    "   Port        :     com1                                     ", /*  2 */
    "   Baud Rate   :     1200                                     ", /*  3 */
    "   Data Bits   :        7                                     ", /*  4 */
    "   Stop Bits   :        2                                     ", /*  5 */
    "   Parity      :     even                                     ", /*  6 */
    "   Connect     : computer                                     ", /*  7 */
    "   Dial Type   :     tone                                     ", /*  8 */
    "   Wait Tone   :       30                                     ", /*  9 */
    "   Wait Carrier:       10                                     ", /* 10 */
    "                                                              ", /* 11 */
    "   Phone Number: 9,555-1212                                   ", /* 12 */
    "                                                              ", /* 13 */
    "   Setup File  : terminal.ini                                 ", /* 14 */
    "                                                              ", /* 15 */
    "                                                              ", /* 16 */
    "       Use UP and DOWN cursor keys to move between fields.    ", /* 17 */
    "       Use LEFT and RIGHT cursor keys to change a field.      ", /* 18 */
    "                                                              ", /* 19 */
    "                   [ Press Enter to return ]                  "  /* 20 */
};

#define     rowPort	2
#define     colPort    21

#define     rowBaud	3
#define     colBaud    21

#define     rowData	4
#define     colData    24

#define     rowStop	5
#define     colStop    24

#define     rowParity	6
#define     colParity  21

#define	    rowConnect  7
#define     colConnect 17

#define     rowDial     8
#define     colDial    20

#define     rowTone     9
#define     colTone    22

#define     rowCarrier  10
#define     colCarrier  23

#define     rowPhone   12
#define     colPhone   17
#define     wPhone     20		/* width of phone string */
#define     sPhone  (Z_DIGIT | Z_PHONE)
			 /*012345678901234567890*/
char strPhone[wPhone+1] = "9,555-1212           ";
#define     lenPhone   12

#define     rowFile    14
#define     colFile    17
#define     wFile      40
#define     sFile   (Z_PATH | Z_ALPHA | Z_DIGIT)
		     /*01234567890123456789012345678901234567890*/
char strFile[wFile+1]="terminal.ini                             ";
#define     lenFile    13

int O_ROW;	/* Coordinates of top left corner of option menu */
int O_COL;

char *csBaud[] = {
    "1200",
    "2400",
    "4800",
    "9600",
    " 110",
    " 150",
    " 300",
    " 600"
};

#define nBaud	(sizeof(csBaud)/sizeof(char*))
#define wBaud   4	
int intBaud[nBaud] = {
   1200,
   2400,
   4800,
   9600,
    110,
    150,
    300,
    600
};

char *csPort[] = {
    "com1",
    "com2"
};
#define nPort	(sizeof(csPort)/sizeof(char*))
#define wPort	4

char *csData[] = {
    "7",
    "8"
};
#define nData	(sizeof(csData)/sizeof(char*))
#define wData	1

unsigned char chData[nData] = {
    7,
    8
};

char *csStop[] = {
    "1",
    "2"
};
#define nStop	(sizeof(csStop)/sizeof(char*))
#define wStop	1

unsigned char chStop[nStop] = {
    0,
    2
};

char *csParity[] = {
    "even",
    " odd",
    "none"
};
#define nParity   (sizeof(csParity)/sizeof(char*))
#define wParity   4

unsigned char chParity[nParity] = {
    EVEN,
    ODD,
    NONE
};

char *csConnect[] = {
    "computer",
    "   modem"
};
#define nConnect   (sizeof(csConnect)/sizeof(char*))
#define wConnect   8
unsigned char chConnect[nConnect] = {
    COMPUTER,
    MODEM
};

char *csDial[] = {
    " tone",
    "pulse"
};
#define nDial   (sizeof(csDial)/sizeof(char*))
#define wDial   5
unsigned char chDial[nConnect] = {
    TONE,
    PULSE
};

char *csTone[] = {
    " 10",
    " 15",
    " 20",
    " 25",
    " 50",
    " 75",
    "100",
    "125",
    "150",
    "175",
    "200",
    "225",
    "250"
};
/* register S6 in Hayes Modem */
#define nTone   (sizeof(csTone)/sizeof(char*))
#define wTone   3
int iTone[nTone] = {10,15,20,25,50,75,100,125,150,175,200,225,250};

char *csCarrier[] = {
    "30",
    "25",
    "20",
    "15",
    "10",
    " 5"
};
/* register S7 in Hayes Modem */
#define nCarrier   (sizeof(csCarrier)/sizeof(char*))
#define wCarrier   2
int iCarrier[nCarrier] = {30, 25, 20, 15, 10, 5};

#define OT_SET	    0		/* Option types */
#define OT_ASCIIZ   1

unsigned char cMap[256];

typedef struct {
    int sc_n;			/* number of different values */
    char **sc_name;		/* array of value names */
} setControl_s;

typedef struct {
    unsigned char az_Set;	/* flags of valid characters */
} azControl_s;

typedef struct {
    int ctl_type;		/* Type of control */
    int ctl_row;		/* row to display value name */
    int ctl_col;		/* column to display value name */
    int ctl_width;		/* width of value names */
    union {
	setControl_s  sc;
	azControl_s   az;
    } ctl_union;
} control_s;

typedef union {
    struct {
	char *oaz_str;
	int   oaz_cur;		/* cursor position */
    } oaz;
    int   osc;
} option_u;

control_s  control[] = {
    {OT_SET   ,rowPort	  ,colPort    ,wPort    ,{nPort    ,csPort    }},
    {OT_SET   ,rowBaud	  ,colBaud    ,wBaud    ,{nBaud    ,csBaud    }},
    {OT_SET   ,rowData	  ,colData    ,wData    ,{nData    ,csData    }},
    {OT_SET   ,rowStop	  ,colStop    ,wStop    ,{nStop    ,csStop    }},
    {OT_SET   ,rowParity  ,colParity  ,wParity  ,{nParity  ,csParity  }},
    {OT_SET   ,rowConnect ,colConnect ,wConnect ,{nConnect ,csConnect }},
    {OT_SET   ,rowDial    ,colDial    ,wDial    ,{nDial    ,csDial    }},
    {OT_SET   ,rowTone    ,colTone    ,wTone    ,{nTone    ,csTone    }},
    {OT_SET   ,rowCarrier ,colCarrier ,wCarrier ,{nCarrier ,csCarrier }},
    {OT_ASCIIZ,rowPhone   ,colPhone   ,wPhone   ,{sPhone	      }},
    {OT_ASCIIZ,rowFile	  ,colFile    ,wFile    ,{sFile	              }}
};

option_u option[] = {	 	/* Current option settings */
	{0},			/* port        */    
	{0},			/* baud        */    
	{0},			/* data bits   */    
	{0},			/* stop bits   */    
	{0},			/* parity      */    
	{0},			/* connect     */    
	{0},			/* dial type   */    
	{0},			/* wait tone   */    
	{0},			/* wait carrier*/    
	{strPhone,0},		/* phone       */    
	{strFile ,0}		/* file        */    
};
#define nOption    (sizeof(option)/sizeof(option_u))

static char 	AttrPanel  = COLOR_PANEL,
             	AttrOption = COLOR_OPTION,
              	AttrCursor = COLOR_CURSOR;
static int	N_of_Cols,		/* number of columns on screen */
		N_of_Rows;		/* number of rows on screen */




/*** 	get_options - get COM port settings and modem settings
 *
 *	This routine determines the type of display and sets the display
 *	attributes appropriately. If a filename was specified in the command
 *	line, it opens the file and displays the COM port and the MODEM options
 *	from the file. These options may be modified but the modifications will
 *	not be written out to the file. If the file open failed, or, if no file
 *	was specified in the command line, this routine wil display the default
 *	options on the screen. The user may modify the options and it will be
 *	writen out to the file specified in the options screen.
 *
 *	get_options(argc,argv)
 *
 *	ENTRY
 *		argc - number of command line arguments
 *		argv - pointer to an array of pointers (to command line args)
 *
 *	EXIT
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

void get_options(argc, argv)
int 	argc;
char 	*argv[];
{
	FILE		*fp;
	char		OptionsSet = FALSE; /* indicate if options are set  */
	unsigned	RetCode;
	static struct ConfigData DispConfigData = {sizeof(DispConfigData),};

	/* if the display is monochrome, change the attributes */
	if ((RetCode = VIOGETCONFIG(RESERVED,&DispConfigData,RESERVED)) != 0)
	  error(ERR_VIOGETCONFIG, RetCode);
	if ((DispConfigData.adapter_type == MONOCHROME) || 
	    (DispConfigData.display_type == MONOCHROME)) {
	  AttrPanel  = MONO_PANEL;
	  AttrOption = MONO_OPTION;
	  AttrCursor = MONO_CURSOR;
	}


	if (argc > 1) {  	/* get com/modem settings from the file */
	  if ((fp = fopen(argv[1], "r")) != NULL) {

	    /* clear the phone-number and file-name string buffers */
	    strnset(strPhone, ' ', wPhone+1);
	    strnset(strFile, ' ', sFile+1);

	    /* read the option settings from the file */
	    fscanf(fp, "%d %d %d %d %d %d %d %d %d \n",
                   &option[PORT_NAME].osc, &option[BAUD_RATE].osc, 
		   &option[DATA_BITS].osc, &option[STOP_BITS].osc, 
		   &option[PARITY].osc, &option[CONNECT].osc, 
		   &option[DIAL_TYPE].osc, &option[WAIT_TONE].osc, 
		   &option[WAIT_CARRIER].osc);
	    fscanf(fp, "%21s \n", option[PHONE_NUMBER].oaz.oaz_str);
	    fscanf(fp, "%41s \n", option[SETUP_FILE].oaz.oaz_str);
	    printf("file name = %s \n", option[SETUP_FILE].oaz.oaz_str);
	    option[PHONE_NUMBER].oaz.oaz_cur = 0;  /* init cursor position */
	    option[SETUP_FILE].oaz.oaz_cur = 0;    /* init cursor position */

	    /* show the options on the screen */
    	    screen_init();		/* init the physical screen */
    	    option_init();
    	    show_option();

	    OptionsSet = TRUE;
	    fclose(fp);
	  }
	}

	if (!OptionsSet) {		/* get options from the user */
    	  screen_init();			/* init the physical screen */
    	  option_init();
    	  show_option();
	  /* write option settings to the file specified. */
	  if ((fp = fopen(option[SETUP_FILE].oaz.oaz_str, "w")) != NULL) {
	    DOSHOLDSIGNAL(DISABLE_SIGNALS);
	    fprintf(fp, "%d %d %d %d %d %d %d %d %d \n",
                    option[PORT_NAME].osc, option[BAUD_RATE].osc, 
		    option[DATA_BITS].osc, option[STOP_BITS].osc, 
		    option[PARITY].osc, option[CONNECT].osc, 
		    option[DIAL_TYPE].osc, option[WAIT_TONE].osc, 
		    option[WAIT_CARRIER].osc);
	    fprintf(fp, "%21s \n", option[PHONE_NUMBER].oaz.oaz_str);
	    fprintf(fp, "%41s \n", option[SETUP_FILE].oaz.oaz_str);
	    fclose(fp);
	    DOSHOLDSIGNAL(ENABLE_SIGNALS);}
        } 
}




/***	get_com_options - get Com port options
 *
 *	This routine copies the current com port options into a structure
 *	provided by the caller. 
 *
 *	get_com_options(psComOptions)
 *
 *	ENTRY
 *		psComOptions = ptr to structure of com port options
 *
 *	EXIT
 *		the structure pointed by psComOptions is filled with com 
 *		port options
 *
 *	WARNING
 *
 *	EFFECTS
 *
 */

void get_com_options(psComOptions)
structComOptions *psComOptions;		/* ptr to struct of com port options */
{
	psComOptions->pPortName  = csPort[(option[PORT_NAME].osc)];
	psComOptions->iBaudRate  = intBaud[(option[BAUD_RATE].osc)];
	psComOptions->chDataBits = chData[(option[DATA_BITS].osc)];
        psComOptions->chStopBits = chStop[(option[STOP_BITS].osc)];
	psComOptions->chParity   = chParity[(option[PARITY].osc)];
}




/***	get_modem_options - get modem options 
 *
 *	This routine copies the current modem options into a structure
 *	provided by the caller. 
 *
 *	get_modem_options(psModemOptions)
 *
 *	ENTRY
 *		psModemOptions = ptr to structure of modem options
 *
 *	EXIT
 *		the structure pointed by psModemOptions is filled with 
 *		modem options
 *
 *	WARNING
 *
 *	EFFECTS
 *
 */

void get_modem_options(psModemOptions)
structModemOptions *psModemOptions;	/* ptr to struct of modem options */
{
	psModemOptions->chDialType   = chDial[(option[DIAL_TYPE].osc)];
	psModemOptions->iWaitTone    = iTone[(option[WAIT_TONE].osc)];
	psModemOptions->iWaitCarrier = iCarrier[(option[WAIT_CARRIER].osc)];
	psModemOptions->pPhoneNumber = option[PHONE_NUMBER].oaz.oaz_str;
}




/***	modem - determines if modem connection was requested
 *
 *	modem()
 *
 *	ENTRY
 *
 *	EXIT
 *		modem = TRUE if modem connection was requested
 *		        FALSE otherwise
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

modem()
{
	return (chConnect[(option[CONNECT].osc)] == MODEM);
}




/***	clear_screen - blank out the entire screen
 *
 *	clear_screen()
 *
 *	ENTRY
 *
 *	EXIT
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

clear_screen()
{
    Cell 		c;
    int			RetCode;

    c.ch = ' ';
    c.at = ATTR(WHITE,BLACK);
    if ((RetCode = VIOWRTNCELL((char *)(&c), N_of_Rows*N_of_Cols, 
                               0, 0, RESERVED)) != 0)
      error(ERR_VIOWRTNCELL, RetCode);
}   /* clear_screen */




/***	option_init - initialize option screen 
 *
 *	option_init()
 *
 *	ENTRY
 *
 *	EXIT
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

option_init()
{
    	int 		i;

    	O_ROW = (N_of_Rows - O_HEIGHT)/2;
    	O_COL = (N_of_Cols - O_WIDTH)/2;
    	for (i=0; i<nOption; i++) {   /* adjust to real screen coordinates */
          control[i].ctl_row += O_ROW;
          control[i].ctl_col += O_COL;
    	}
    	init_cmap();
}   	/* option_init */




/***	init_cmap - initialise valid character set
 *
 *	init_cmap()
 *
 *	ENTRY
 *
 *	EXIT
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

init_cmap()
{
    	register int 	i;

    	for (i=0; i<256; i++)
          cMap[i] = 0;
    	for (i='a'; i<='z'; i++)
          cMap[i] |= Z_ALPHA;
    	for (i='A'; i<='Z'; i++)
          cMap[i] |= Z_ALPHA;
    	for (i='0'; i<='9'; i++)
          cMap[i] |= Z_DIGIT;
    	cMap[':']  |= Z_PATH;
    	cMap['\\'] |= Z_PATH;
    	cMap['/']  |= Z_PATH;
    	cMap['.']  |= Z_PATH;
    	cMap['-']  |= Z_PATH;
    	cMap[' ']  |= Z_WHITE;
    	cMap['\t'] |= Z_WHITE;
    	cMap['-']  |= Z_PHONE;
    	cMap[',']  |= Z_PHONE;
}   	/* init_cmap */




/***	show_option - display option screen and record user modifications
 *
 *	show_option()
 *
 *	ENTRY
 *
 *	EXIT
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

show_option()
{
	int		RetCode;

    	do_option();
	clear_screen();
    	if ((RetCode = VIOSETCURPOS(0, 0, RESERVED)) != 0)
          error(ERR_VIOSETCURPOS, RetCode);
}   	/* show_option */




/***	do_option - show and update option screen
 *
 *	do_option()
 *
 *	ENTRY
 *
 *	EXIT
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

do_option()
{
    	unsigned	row, col;
    	char		a;
    	int 		RetCode,
			i,
    			key,
    			iOpt,lastOpt;

    	a = AttrPanel;
    	for (row=0; row<O_HEIGHT; row++)	    /* display option panel */
          if ((RetCode = VIOWRTCHARSTRATT(optScreen[row],O_WIDTH,row+O_ROW,
			                  O_COL, &a, RESERVED)) != 0)
            error(ERR_VIOWRTCHARSTRATT, RetCode);
    	for (i=0; i<nOption; i++)	   /* Fill in options */
	  show_opt(i, AttrOption);

    	key = 0;
    	iOpt = 0;	/* Start with first option */
    	lastOpt = iOpt;
    	while (key != ENTER_KEY) {
	  if (iOpt != lastOpt)	    /* user moved to different field */
	    show_opt(lastOpt, AttrOption); /* revert to normal color */
	  show_opt(iOpt, AttrCursor);	/* highlight current field */
	  lastOpt = iOpt;
	  key = get_key();
	  switch (key) {
	    case UP_KEY:
			if (--iOpt < 0)
		    	  iOpt = nOption - 1;
			break;
	    case DOWN_KEY:
			if (++iOpt > (nOption - 1))
		    	  iOpt = 0;
			break;
	    case RIGHT_KEY:
			mod_option(iOpt,  1);
			break;
	    case LEFT_KEY:
			mod_option(iOpt, -1);
			break;
	    case DEL_KEY:
			if (control[iOpt].ctl_type == OT_ASCIIZ)
		    	  do_right_del(iOpt);
			break;
	    case HOME_KEY:
			if (control[iOpt].ctl_type == OT_ASCIIZ)
		    	  option[iOpt].oaz.oaz_cur = 0;
			break;
	    case BKSP_KEY:
			if (control[iOpt].ctl_type == OT_ASCIIZ)
		    	  do_left_del(iOpt);
			break;
	    default:
			switch (control[iOpt].ctl_type) {
		          case OT_SET:    /* Ignore other key strokes */
					break;
		          case OT_ASCIIZ:
					do_insert(iOpt,key);
					break;
		    	  default:
					error(ERR_DO_OPT_INVOPT, NO_RETCODE);
			}
			break;
	  }   	/* switch */
    	}	/* while */
}   	/* do_option */




/***	do_insert - insert character in a string
 *
 *	do_insert(iOpt,key)
 *
 *	ENTRY
 *		iOpt = which option (setup File or Phone Number)
 *		key  = character to insert
 *
 *	EXIT
 *		char inserted into string (setup File name or Phone Number)
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

do_insert(iOpt,key)
int 	 iOpt;
unsigned key;
{
    	int   	ch;
    	int   	cur;
    	int   	i,n;
    	int   	width;
    	char 	*s,*d,*p;

    	ch = key >> 8;
    	if (ch > 0x7F)	/* Ignore */
	  return;
    	if ((cMap[ch] & control[iOpt].ctl_union.az.az_Set) == 0)
	  return; 	/* Not valid char */

    	/*	oaz_cur - points to insertion point
		We throw away characters at end of string.

		before: aaaaaaabbbbbbbbbbbbc
		               ^
		after:	aaaaaaaxbbbbbbbbbbbb
			        ^
    	*/
    	cur = option[iOpt].oaz.oaz_cur;
    	width = control[iOpt].ctl_width;
    	p = option[iOpt].oaz.oaz_str;

    	d = p+width-1;		/* End of string */
    	s = d-1;		/* End of string - 1 */
    	n = (width-cur)-1;	/* Number of chars to shift */
    	for (i=n; i>0; i--)
	  *d-- = *s--;		/* Shift string */
    	p[cur] = ch;		/* Insert character */
    	cur++;			/* Advance insertion point */
    	if (cur > (width-1))    /* cursor wrapped */
	  cur = 0;
    	option[iOpt].oaz.oaz_cur = cur;
}




/***	do_left_del - delete a character which is to the left of the cursor
 *
 *	do_left_del(iOpt)
 *
 *	ENTRY
 *		iOpt = which option (setup File or Phone Number)
 *
 *	EXIT
 *		char deleted from string (setup File name or Phone Number)
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

do_left_del(iOpt)
int 	iOpt;
{
    	if (option[iOpt].oaz.oaz_cur == 0)	/* Ignore at start of field */
	  return;
    	/*
		We delete the character to the left of the cursor and shift
		rest of characters left 1.  Ignore at beginning of field.

		before: aaaaaaaxbbbbbbbbbbbb
				^
		after:	aaaaaaabbbbbbbbbbbb
		       	       ^
    	*/
    	do_del(iOpt,0,-1);
}




/***	do_right_del - delete a character which is at the cursor
 *
 * 	do_right_del(iOpt)
 *
 *	ENTRY
 *		iOpt = which option (setup File or Phone Number)
 *
 *	EXIT
 *		char deleted from string (setup File name or Phone Number)
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

do_right_del(iOpt)
int 	iOpt;
{
    	/*
		We delete the character at the cursor and shift
		rest of characters left 1.

		before: aaaaaaaxbbbbbbbbbbbb
		       	       ^
		after:	aaaaaaabbbbbbbbbbbb
		               ^
    	*/
    	do_del(iOpt,1,0);
}




/***	do_del - delete a character
 *
 *	do_del(iOpt,iShift,iCur)
 *
 *	ENTRY
 *		iOpt = which option (setup File or Phone Number)
 *		iShift = amount by which string should be shifted
 *		iCur = amount by which cursor should be moved
 *	EXIT
 *		char deleted from string (setup File name or Phone Number)
 *		cursor position within the string updated
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

do_del(iOpt,iShift,iCur)
int 	iOpt,
        iShift,
        iCur;
{
    	int 	cur,width,i,n;
    	char 	*s,*d,*p;

    	cur = option[iOpt].oaz.oaz_cur;
    	width = control[iOpt].ctl_width;
    	p = option[iOpt].oaz.oaz_str;

    	s = p+cur+iShift;
    	d = s-1;
    	n = (width-(cur+iCur))-1;	/* Number of chars to shift */
    	for (i=n; i>0; i--)
	  *d++ = *s++;			/* Shift string */
    	cur += iCur;			/* Adjust cursor */
    	p[width-1] = ' ';		/* Put blank at end */
    	option[iOpt].oaz.oaz_cur = cur;
}




/***	mod_option - modify option
 *
 *	mod_option(iOpt, inc)
 *
 *	ENTRY
 *		iOpt = which option (Port, Baud Rate, ..., setup File)
 *		inc  =  1 ( -> key was pressed)
 *		       -1 ( <- key was pressed)
 *
 *	EXIT
 *		option array entry 'osc' updated for all iOpt except for 
 *		setupFile and PhoneNumber. In the case of setupFile and 
 *		PhoneNumber, the cursor position is updated (cursor moved 
 *		left if '<-' key was pressed; cursor moved right if '->' 
 *		key was pressed)
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

mod_option(iOpt, inc)
int 	iOpt,
	inc;
{
    	int 	col;
    	int 	width;

    switch (control[iOpt].ctl_type) {
	case OT_SET:	/* Ignore other key strokes */
	    if (inc > 0) {  /* Increment value */
		if (++option[iOpt].osc > (control[iOpt].ctl_union.sc.sc_n - 1))
		    option[iOpt].osc = 0;
	    }
	    else	    /* Decrement value */
		if (--option[iOpt].osc < 0 )
		    option[iOpt].osc = control[iOpt].ctl_union.sc.sc_n - 1;
	    break;
	case OT_ASCIIZ:
	    col = option[iOpt].oaz.oaz_cur;
	    width = control[iOpt].ctl_width;
	    if (inc > 0) {  /* move cursor right */
		if (++col > (width-1))
		    col = 0;
	    }
	    else	    /* move cursor left */
		if (--col < 0 )
		    col = (width-1);
	    option[iOpt].oaz.oaz_cur = col;
	    break;
	default:
	    error(ERR_MOD_OPT_INVOPT, NO_RETCODE);
    }
}




/***	show_opt - show option
 *
 *	show_opt(iOpt, color)
 *
 *	ENTRY
 *		iOpt = which option (Port, Baud Rate, . . ., Phone Number)
 *		color = color in which the option is to be displayed
 *
 *	EXIT
 *		option indicated by iOpt is displayed on the screen
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

show_opt(iOpt, color)
int	iOpt;
char	color;
{
    	int  		row,col,width;
    	int  		curCol;
    	char 		*name;
    	unsigned 	RetCode;

    	row = control[iOpt].ctl_row;
    	col = control[iOpt].ctl_col;
    	width = control[iOpt].ctl_width;
    	switch (control[iOpt].ctl_type) {
	  case OT_SET:
	    name = control[iOpt].ctl_union.sc.sc_name[option[iOpt].osc];
	    curCol = col+width-1;
	    break;
	  case OT_ASCIIZ:
	    name = option[iOpt].oaz.oaz_str;
	    curCol = col+option[iOpt].oaz.oaz_cur;
	    break;
	  default:
	    error(ERR_SHOW_OPT_INVOPT, NO_RETCODE);
    	};
    	if ((RetCode = VIOWRTCHARSTRATT(name, width, row, col, 
                                        &color, RESERVED)) != 0)
          error(ERR_VIOWRTCHARSTRATT, RetCode);
    	if ((RetCode = VIOSETCURPOS(row, curCol, RESERVED)) != 0)
          error(ERR_VIOSETCURPOS, RetCode);
}




/***	get_key - return two-byte key stroke; includes IBM extended ASCII codes
 *
 *	get_key()
 *
 *	ENTRY
 *
 *	EXIT
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

get_key()
{
    char 	c;
    unsigned 	key;

    if (c = getch())		    /* Get character */
      key = c << 8;		    /* Make scan code zero */
    else
      key = getch();		    /* Get scan code */
    return(key);
}   /* get_key */




/***	screen_init - Initialize the screen
 *
 *	screen_init()
 *
 *	ENTRY
 *
 *	EXIT
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

screen_init()
{
    	struct ModeData TheModeData; 	/* Mode data from VioGetMode */
    	int		RetCode;

    	if ((RetCode = VIOGETMODE(&TheModeData, RESERVED)) != 0)
          error(ERR_VIOGETMODE, RetCode);
    	N_of_Cols = TheModeData.col;
    	N_of_Rows = TheModeData.row;
	clear_screen();
}   	/* screen_init */

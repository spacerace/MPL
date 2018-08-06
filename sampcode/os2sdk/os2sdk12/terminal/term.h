/*  term.h 
 */

/* index definitions for error messages array */
#define ERR_VIOGETCONFIG	0
#define ERR_KBDSETSTATUS	1
#define ERR_OUTOFMEMORY		2
#define ERR_DOSCREATETHREAD	3
#define ERR_DOSCLOSECOMPORT	4
#define	ERR_IOCTLSETDCB		5
#define ERR_DOSREAD		6
#define ERR_VIOWRTTTY		7
#define ERR_DOSOPEN		8
#define ERR_IOCTLSETBAUD	9
#define	ERR_IOCTLSETLINECHAR   10
#define ERR_IOCTLGETDCB	       11
#define ERR_KBDCHARIN	       12
#define ERR_DOSWRITE	       13
#define ERR_VIOWRTCHARSTRATT   14
#define ERR_VIOWRTNCELL        15
#define ERR_DOSEXITLIST        16
#define ERR_VIOSETCURPOS       17
#define ERR_VIOGETMODE	       18
#define ERR_IOCTLFLUSHQUE      19	
#define ERR_IOCTLGETCOMERROR   20 
#define ERR_DOSSETSIGHANDLER   21
#define ERR_IOCTLSETBREAKON    22
#define ERR_IOCTLSETBREAKOFF   23
#define ERR_INVALIDSIGNAL      24
#define ERR_DO_OPT_INVOPT      25
#define ERR_MOD_OPT_INVOPT     26
#define ERR_SHOW_OPT_INVOPT    27

#define NUM_MSG		       28	/* total number of error messages */

#define NO_RETCODE	       -1	/* indicate no returncode */
/* keyboard definitions */
#define	ENTER_KEY	0x0D00
#define	UP_KEY		0x0048
#define	DOWN_KEY	0x0050
#define	LEFT_KEY	0x004B
#define	RIGHT_KEY	0x004D
#define	DEL_KEY		0x0053
#define	BKSP_KEY	0x0800
#define	HOME_KEY	0x0047
#define DEL_SCAN	83	/* scan code DELETE key */
#define DEL_ASCII	127     /* ascii code for DELETE */
#define ALT_F1		104	/* scan code for ALT F1 */

/* attribute definitions */
#define BLACK		0
#define BLUE    	1
#define GREEN		2
#define CYAN		3
#define RED		4
#define MAGENTA		5
#define YELLOW		6
#define WHITE		7
#define BRIGHT		8

/* attr - compute a character attribute
 * ENTRY:	f = foreground attribute
 *		b = backround attribute
 * EXIT:	returns combined  attribute
 */

#define ATTR(f,b)  ((b)<<4) + (f)

#define COLOR_PANEL  ATTR(BLUE,WHITE)
#define COLOR_OPTION ATTR(RED,WHITE)
#define COLOR_CURSOR ATTR(WHITE,RED)

#define MONO_PANEL   0x70
#define MONO_OPTION  0x70
#define MONO_CURSOR  0x07

/* index values for the option array */
#define	PORT_NAME	0
#define BAUD_RATE	1
#define	DATA_BITS	2
#define	STOP_BITS	3
#define	PARITY		4
#define	CONNECT		5
#define	DIAL_TYPE	6
#define	WAIT_TONE	7
#define	WAIT_CARRIER	8
#define	PHONE_NUMBER	9
#define	SETUP_FILE	10

/* IOCTL category and functions */
#define SERIAL	        1	/* category: serial device control */
#define SETBAUD		0x41	/* function: set baud rate */
#define SETLINECHAR     0x42    /* function: set line characteristics */
#define GETDCB		0x73    /* function: get device control block */
#define SETDCB		0x53	/* function: set device control block */
#define ENBL_XON_XOFF   0x03	/* enable auto transmit & receive flow cntl */
#define	GENERIC		11	/* category: generic device control commands */
#define GETCOMERROR     0x6d    /* function: retrieve & clear COM error info */
#define FLUSHINPUT	1	/* function: flush input buffer */
#define FLUSHOUTPUT	2	/* function: flush output buffer */
#define GETNUMCHARS     0x68	/* function: get number of chars in recv que */
#define FLUSH_CMDINFO   0	/* command information for FLUSH INPUT/OUTPUT*/
#define SETBREAKON      0x4b    /* function: set break on */
#define SETBREAKOFF     0x45    /* function: set break off */

/* miscellaneous definitions */
#define RESERVED	0	/* reserved word */
#define MONOCHROME	0	/* monochrome adapter, monochrome display */
#define STACKSIZE	4096	/* size of stack to allocate for threads */
#define	IOWAIT		0	/* wait */
#define KBD_BITMASK 	0x0006	/* bit mask for the KbdSetStatus call */
#define INBUFLENGTH	2048	/* length of input buffer */
#define BREAK		4	/* signal number for BREAK signal */
#define RECV_CTRL       2	/* signal handler should receive control */
#define READTIMEOUT     4000    /* time out for com port in .01 sec units */
#define DISABLE_SIGNALS 1	/* disable signals */
#define ENABLE_SIGNALS  0       /* enable signals */
#define ADD_ADDR        1       /* DOSEXITLIST function request code */
#define XFER		3	/* DOSEXITLIST function request code */

/* modem related definitions */

#define	NUM_RETRY	1	/* no. times to retry modem connection */

/* modem commands */
#define MC_INIT         0
#define MC_ATTN         1
#define MC_ESC          2
#define MC_ONHOOK       3
#define MC_OFFHOOK      4
#define MC_RESET        5
#define MC_DIAL       128       /* special command for dialing */

#define B_PAUSE       0x20      /* a pause in a modem command */

/* modem status values */
#define MS_OK           0       /* modem operation was successful */
#define MS_CONNECT      1       /* a connection was made */
#define MS_RING         2       /* a ring was detected */
#define MS_NOCARRIER    3       /* no carrier was detected */
#define MS_ERROR        4       /* a command error was detected */
#define MS_CONNECT1200  5   	/* a connection was established at 1200bps*/
#define MS_NODIALTONE   6	/*dialtone not detected within time in reg S7*/
#define MS_BUSY		7	/* a busy signal was detected */

#define MS_NOREPLY      8       /* no reply was recieved from the modem */
#define OT_FAILURE      9	/* other modem connection failure */

/* connection: modem/computer */
#define MODEM	  	1
#define COMPUTER  	2

/* dial type: tone/pulse */
#define TONE    	1
#define PULSE   	2

/* parity: even/odd/none */
#define EVEN    	2
#define ODD     	1
#define NONE    	0


/*** type definitions ***/

typedef struct {
  unsigned char   ch; 	/* character */
  unsigned char   at; 	/* attribute */
} Cell;

typedef struct LineChar {			/* line characteristics */
  unsigned char	DataBits;
  unsigned char	Parity;
  unsigned char	StopBits;
} structLineChar;

typedef	struct DCB {		/* device control block information */
  unsigned	WriteTimeOut;
  unsigned	ReadTimeOut;
  unsigned char Flags1;
  unsigned char	Flags2;
  unsigned char	Flags3;
  char		ErrRepChar;
  char		BrkRepChar;
  char		XONchar;
  char		XOFFchar;
} structDCB;

typedef	struct ComOptions {		/* structure to hold com port options */
  char		*pPortName;	/* port name char-string */
  int		iBaudRate;	/* baud rte */
  char	        chDataBits,	/* number of databits */
  		chStopBits,	/* number of stopbits */
		chParity;	/* parity */
} structComOptions;

typedef struct ModemOptions {		/* structure to hold modem options */
  unsigned char chDialType;	/* dial type */
  int		iWaitTone,	/* seconds modem should wait for dial tone */
  		iWaitCarrier;	/* seconds modem should wait for carrier */
  char		*pPhoneNumber;  /* phone number char-string */
} structModemOptions;

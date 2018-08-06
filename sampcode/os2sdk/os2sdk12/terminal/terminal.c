/***
 *
 * TITLE 
 *
 *	terminal.c
 *	Created by Microsoft Corporation 1987
 *
 * DESCRIPTION
 *
 * 	This program emulates an ANSI terminal device. 
 *
 * 	The device driver COM.SYS must be installed at IPL  (Initial Program
 * 	Load) by specifying the following in config.sys:
 *		device = COM01.SYS
 *
 *	To run this program, type the following to the MS OS/2 prompt:
 *
 *		terminal [filename]
 *
 * 	If a filename is specified, the initialisation of the COM port and
 * 	the MODEM will be performed as indicated in the file (creation of this
 * 	file is described in options.c). For more details on setting options
 *	pertaining to the COM port and the MODEM, see options.c
 *
 *	To exit the terminal emulator, type ALT F1.
 *
 ***/

#define INCL_DOSSIGNALS
#define INCL_SUB
#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS
#define INCL_DOSDEVICES

#include	<os2def.h>
#include	<bse.h>
#include	<stdio.h>
#include	<malloc.h>
#include	<string.h>
#include 	<memory.h>
#include 	<conio.h>
#include 	"term.h"

extern void get_options(int, char *[]);   /* get com port and modem options */
extern void get_com_options(structComOptions *); /* get com options */
extern make_modem_conn(void);		  /* make modem connection */
extern modem(void);       /* returns TRUE if modem connection was requested */
extern void discon_modem(void); 	  /* disconnect modem */

void init_com_port(void);		  /* initialise com port */
void far read_com_port(void);		  /* routine addr for a thread */
void write_com_port(void);		  /* routine addr for another thread */	
void APIENTRY handle_signals(USHORT,USHORT); /* BREAK signals handler */
void close_conn(void);			  /*close modem connection & com port*/
void far xit(void);			/* exit routine for this program */

char *ErrMsg[] = {
		"VIOGETCONFIG",
		"KBDSETSTATUS",
       		"OUT OF MEMORY",
		"DOSCREATETHREAD",
		"DOSCLOSE COMPORT",
		"DOSDEVIOCTL SETDCB",
		"DOSREAD",
		"VIOWRTTTY",
		"DOSOPEN: check if COM driver is installed",
		"DOSDEVIOCTL SETBAUD",
		"DOSDEVIOCTL SETLINECHAR",
		"DOSDEVIOCTL GETDCB",
		"KBDCHARIN",
		"DOSWRITE",
		"VIOWRTCHARSTRATT",
		"VIOWRTNCELL",
		"DOSEXITLIST ADD_ADDR",
		"VIOSETCURPOS",
		"VIOGETMODE",
		"DOSDEVIOCTL FLUSH_XMIT_RECV_QUEUE",
		"DOSDEVIOCTL GETCOM",
		"DOSSETSIGHANDLER",
		"DOSDEVIOCTL SETBREAKON",
		"DOSDEVIOCTL SETBREAKOFF",
       		"INVALID SIGNAL",
       		"do_option: invalid option type",
       		"mod_option: invalid option type",
       		"show_option: invalid option type"
};
HFILE		FileHndl = (HFILE) NULL;    /* COM port file handle */

static char	ExitPgm = FALSE;    /* indicate if program should terminate */
static int	ErrorNumber = -1,   /* used as index to ErrMsg array */
		ReturnCode  = -1;   /* retcode from system/subsystem call */




/***	main - entry point to TERMINAL program
 *
 *	This routine obtains the com port and the modem options from the file
 *      (specified in the command line) or directly from the user. It opens and
 *	initialises the com port. It sets up the modem connection (if one was
 *	requested). It creates a thread which loops continuously waiting to 
 *	receive data from the com port and writing it out to the display. It 
 *	loops continuosly waiting for an input from the keyboard and writing 
 *	it out to the com port. Both the loops terminate when the user presses 
 *	ALT-F1. It closes the modem connection (if one was made), and the com 
 *	port and then terminates.
 *
 *	main(argc, argv)
 *
 *	ENTRY	
 *		argc - number of command line arguments
 *		argv - pointer to an array of pointers (to command line args)
 *
 *	EXIT
 *		the program terminates
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/
 
main(argc,argv)
int 	argc;
char 	*argv[];
{
	UCHAR		*Stack1;	     /* stack for a thread */
	TID		ThreadID;
	unsigned	RetCode,	     /* return code */
			Result = TRUE;
	USHORT		NumBytes;	     /* number of bytes to be written */
	PFNSIGHANDLER	PrevAddress;
	USHORT		PrevAction;
	static KBDINFO OurKbdStatus =
                                   {sizeof(OurKbdStatus),KBD_BITMASK,};

	/* get COM port and modem options */
	get_options(argc, argv);	

	/* open and initialise COM port */
	init_com_port();			

	/* establish xit() as the exit routine */
	if ((RetCode = DosExitList(EXLST_ADD, (PFNEXITLIST)xit)) != 0)
	  error(ERR_DOSEXITLIST, RetCode);

	/* make modem connection if requested */
	if (modem())
	  Result = make_modem_conn();	

	if (Result) {
	  /* allocate memory for separate thread execution */
	  if (!(Stack1 = (char *) _nmalloc(STACKSIZE)))
	    error(ERR_OUTOFMEMORY, NO_RETCODE);

	  /* create a thread that will execute the read_com_port() */
	  Stack1 += STACKSIZE;
	  if ((RetCode = DosCreateThread(read_com_port, &ThreadID,
                                         Stack1)) != 0)
	    error(ERR_DOSCREATETHREAD, RetCode);

	  /* set the keyboard status */
	  if ((RetCode = KbdSetStatus(&OurKbdStatus,
                                      RESERVED)) != 0) 
	    error(ERR_KBDSETSTATUS, RetCode);

	  /* set signal handler for BREAK signal */
	  if ((RetCode = DosSetSigHandler(handle_signals,
			 &PrevAddress,
			 &PrevAction,
			 RECV_CTRL, BREAK)) != 0)
	    error(ERR_DOSSETSIGHANDLER, RetCode);

	  /* display "connected" message */
	  printf("connected... \n");

	  /* read chars from the keyboard and write to COM port */
	  write_com_port();
	}	/* if (Result) */
}




/***	init_com_port - open the COM port and initialise line characteristics
 *
 *	This routine opens the com port. It sets the com port options 
 *	BaudRate, DataBits, Parity and the StopBits. It sets the read timeout.
 *	It enables the automatic transmit and receive flow control.
 *
 *	init_com_port()
 *		
 *	ENTRY
 *		
 *	EXIT
 *		FileHndl = handle to com port
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

void init_com_port()
{
	USHORT		ActionTaken; /* action: file existed,created,replaced */
	unsigned	RetCode;
	structLineChar 	sLineChar;	/* line characteristics */
	structDCB	sDCB;		/* device control block information */
	structComOptions sComOptions;

	get_com_options(&sComOptions);

	/* open the com port */
	if ((RetCode = DosOpen(sComOptions.pPortName, &FileHndl,
                               &ActionTaken, 0L, 0, 0x0001, 0x0042, 0L)) != 0)
	  error(ERR_DOSOPEN, RetCode);

	/* set the baud rate */
	if ((RetCode = DosDevIOCtl(0L, &(sComOptions.iBaudRate),
			           SETBAUD, SERIAL, FileHndl)) != 0)
	  error(ERR_IOCTLSETBAUD, RetCode);

	/* set Data Bits, Stop Bits, Parity */
	sLineChar.DataBits = sComOptions.chDataBits; 
        sLineChar.Parity   = sComOptions.chParity; 
	sLineChar.StopBits = sComOptions.chStopBits;
	if ((RetCode = DosDevIOCtl(0L, &sLineChar, SETLINECHAR,
			           SERIAL, FileHndl)) != 0)
	  error(ERR_IOCTLSETLINECHAR, RetCode);

	/* get device control block info */
	if ((RetCode = DosDevIOCtl(&sDCB, 0L, GETDCB, SERIAL,
                                   FileHndl)) != 0)
	  error(ERR_IOCTLGETDCB, RetCode);

	sDCB.Flags2 |= 0x03;	/* enable auto Xmit and recv flow control */
	sDCB.Flags3 &= 0xf9;	/* clear read timeout flags */
	sDCB.Flags3 |= 0x04;	/* set wait for something read timeout */
	sDCB.ReadTimeOut = READTIMEOUT;	/* set read timout value */

	/* set device control block info */
	if ((RetCode = DosDevIOCtl(0L, &sDCB, SETDCB, SERIAL,
				    FileHndl)) != 0)
	  error(ERR_IOCTLSETDCB, RetCode);
}




/***	write_com_port	- read chars from the keyboard and write to COM port
 *
 *	This routine loops continuosly waiting for a keyboard input and
 *	writing it out to the com port. The loop terminates when the user
 *	presses the ALT-F1.
 *
 *	write_com_port()
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

void write_com_port()
{
	USHORT		NumBytes;	/* number of bytes actually written */
	unsigned	RetCode;
	char		OutBuffer;	/* output buffer */
	KBDKEYINFO	OurKeyData;	/* struc to read a char from kbd */


	while (!ExitPgm) {
	  /* read input from the keyboard */
	  if ((RetCode = KbdCharIn(&OurKeyData,
                                   IOWAIT, RESERVED)) != 0) 
	    error(ERR_KBDCHARIN, RetCode);
	  OutBuffer = OurKeyData.chChar;
	  if ((OutBuffer == 0) || (OutBuffer == 0xE0)) {
	    OutBuffer = OurKeyData.chScan;
	    switch (OutBuffer) {
	      case DEL_SCAN  : OutBuffer = DEL_ASCII;
                               break;
	      case ALT_F1    : ExitPgm = TRUE;
                               break;
              default        : break;
            };
            if (OutBuffer != ALT_F1) 
	      /* write the input from the keyboard to the com port */
	      if ((RetCode = DosWrite(FileHndl, &OutBuffer, 1,
                                      &NumBytes)) != 0)
		error(ERR_DOSWRITE, RetCode);
	  }
          else {
	    /* write the input from the keyboard to the com port */
	    if ((RetCode = DosWrite(FileHndl, &OutBuffer, 1,
                                    &NumBytes)) != 0)
	      error(ERR_DOSWRITE, RetCode);
	  };
	};
}




/*** 	read_com_port - read chars from com port and display on CRT screen 
 *
 * 	This routine is executed by a thread. It loops continuously waiting
 *      to receive data from the com port and writing it out to the display.
 *	The loop terminates when the user presses ALT-F1.
 *
 *	read_com_port()
 *
 *	ENTRY
 *		FileHndl (handle to com port) setup
 *
 *	EXIT
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/
 
void far read_com_port()
{
	USHORT		NumBytes;	/* number of bytes actually read */
	unsigned	RetCode;
	struct CharsInQue { 	/*data ret'ned by get num chars in que IOCTL*/
			unsigned 	NumCharsInQue;
			unsigned	SizeQue;
	} sCharsInQue;
	char		InBuffer[INBUFLENGTH];	/* input buffer  */

	while (!ExitPgm) {
	  /* get number of characters in receive queue */
	  if ((RetCode = DosDevIOCtl(&sCharsInQue, 0L, GETNUMCHARS,
                                     SERIAL, FileHndl)) != 0)
	    setup_error_msg(ERR_IOCTLSETDCB, RetCode);

	  if (sCharsInQue.NumCharsInQue == 0)
	     sCharsInQue.NumCharsInQue++;

	  if ((RetCode = DosRead(FileHndl, InBuffer, sCharsInQue.NumCharsInQue,
                                 &NumBytes)) != 0)
	    setup_error_msg(ERR_DOSREAD, RetCode);
	
	  if (NumBytes) {
	    /* write to the tty display */
	    if ((RetCode = VioWrtTTY(InBuffer, NumBytes, RESERVED)) != 0)
	      setup_error_msg(ERR_VIOWRTTTY, RetCode);
	  }
	}
}




/***	handle_signals  - handle BREAK signal
 *
 *	This routine is the handler for the BREAK signal. When the user presses
 *	the BREAK key, this routine sends a BREAK on the com line.
 *
 *	handle_signals(SigArg, SigNumber)
 *
 *	ENTRY
 *		SigArg - not used
 *		SigNumber - signal number being processed
 *
 *	EXIT
 *
 *	WARNING
 *
 *	EFFECTS
 *	    Since this is a signal handler it must be declared FAR.
 *
 ***/

void APIENTRY handle_signals(SigArg, SigNumber)
USHORT	SigArg,
	SigNumber;
{
	unsigned	RetCode,
			Status;
	char		OutBuffer;

	switch (SigNumber) {
	case BREAK :/* send BREAK to the com port */
		    if ((RetCode = DosDevIOCtl(&Status, 0L,
                                       SETBREAKON, SERIAL, FileHndl)) != 0)
	              error(ERR_IOCTLSETBREAKON, RetCode);
		    DosSleep(1L);
		    if ((RetCode = DosDevIOCtl(&Status, 0L,
                                       SETBREAKOFF, SERIAL, FileHndl)) != 0)
	              error(ERR_IOCTLSETBREAKOFF, RetCode);
		    break;
	default    :
		    error(ERR_INVALIDSIGNAL, NO_RETCODE);
		    break;
	}
}




/***	close_conn - close modem connection and the com port 
 *
 *	This routine closes the modem connection if one was made and then
 *	closes the com port.
 *
 *	close_conn()
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

void close_conn()
{
	int		RetCode;
	static char 	CloseMsg[] = "exiting terminal...";

	/* send closing message to the display */
	if ((RetCode = VioWrtTTY(CloseMsg, sizeof(CloseMsg), RESERVED)) != 0)
	  print_err_msg(ErrMsg[ERR_VIOWRTTTY], RetCode);

	/* if modem connection was made, close it */
	if (modem())
	  discon_modem();

	/* close the com port */
	if ((RetCode = DosClose(FileHndl)) != 0)
	  print_err_msg(ErrMsg[ERR_DOSCLOSECOMPORT], RetCode);
}




/***		ERROR HANDLING
 *
 *	There are two error handling routines:
 *		- error() which is invoked from the main thread
 *		- setup_err_msg() which is invoked from the thread executing 
 *		  the read_com_port() routine
 *	The two routines perform different functions (described below). This
 *	is done so that if an error is encountered "simultaneously" in both 
 *	the threads, there will be no race condition in error reporting.
 *	
 *	The following routines are also part of the error handling:
 *		- xit() is a DosExitList routine
 *		- print_err_msg() does the actual printing of the error message
 *	
 *	All the routines discussed above are defined below.
 *
 ***/



/***	error
 *
 *	This routine is invoked when there is an error. It prints an
 *	an error message and calls DosExit.
 *
 *	error(ErrNum, RetCode)
 *
 *	ENTRY
 *		ErrNum - error number (used as index to error message array)
 *		RetCode - return code from a DOS system/subsystem call
 *
 *	EXIT
 *	        global variables ErrorNumber & ReturnCode still contain the
 *		initial value of -1. At program termination (via DosExit call)
 *		the DosExitList routine xit() is invoked (described below).
 *		On return from xit(), this program will terminate.
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

error(ErrNum, RetCode)
int  ErrNum;
int  RetCode;
{
	print_err_msg(ErrMsg[ErrNum], RetCode);
	DosExit(EXIT_PROCESS, 1);
}




/***	setup_error_msg - setup error message
 *
 *	Sets up the global ErrorNumber and ReturnCode. It then calls DosExit.
 *
 *	setup_error_msg(ErrNum, RetCode)
 *
 *	ENTRY
 *		ErrNum - error number (used as index to error message array)
 *		RetCode - return code from a DOS system/subsystem call
 *
 *	EXIT
 *		global variables ErrorNumber & ReturnCode are setup. At 
 *		program termination (via DosExit call) the DosExitList
 *		routine xit() is invoked (described below). xit() will print
 *		an error message. On return from xit(), this program will 
 *		terminate.
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

setup_error_msg(ErrNum, RetCode)
int	ErrNum,
	RetCode;
{
	ErrorNumber = ErrNum;
	ReturnCode  = RetCode;
	DosExit(EXIT_PROCESS, 1);
}




/***	xit	- exit function executed at program termination
 *
 *	This is a DosExitList routine. It prints an error message if
 *	the global variable ErrorNumber is non-negative.
 *
 *	xit()
 *
 *	ENTRY
 *
 *	EXIT
 *		If ErrorNumber is non-negative, an error message is printed.
 *		The program will then terminate.
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

void far xit(void)
{
	if (ErrorNumber != -1) 
	  print_err_msg(ErrMsg[ErrorNumber], ReturnCode);
	close_conn(); 		/* close modem connection and the com port */
	DosExitList(XFER, 0L);
}




/***	print_err_msg - print error mesage
 *
 *	This routine prints an error message and the returncode.
 *
 *	print_err_msg(Msg, Retcode)
 *
 *	ENTRY
 *		Msg - error message string
 *		Retcode - returncode from DOS system/subsystem call
 *
 *	EXIT
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

print_err_msg(Msg, RetCode)
char	*Msg;
int	RetCode;

{
  	printf("*** ERROR %s *** ", Msg);
 	if (RetCode != -1)
	  printf("ReturnCode = %d ", RetCode);
	printf("\n");
}

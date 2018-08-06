/* monitor.c RHS 5/1/89
 *
 This program demonstrates the use of character device monitors on the
 keyboard. It creates a keyboard monitor which you can run from any session
 or the DETACHed session. Once you run the monitor with:

 MONITOR

 you can activate the monitor with Alt-F10. This will create a pop-up screen
 which will wait for any key press to end.

 You can terminate the monitor with Ctrl-F10.

 */

#define INCL_DOS
#define INCL_VIO
#define INCL_KBD
#define INCL_ERRORS

#include<os2.h>
#include<mt\stdio.h>
#include<mt\conio.h> 
#include<mt\process.h>

unsigned long semhandle = 0L;
unsigned char HotKey = 0x71;				/* HotKey = Alt-F10			*/
unsigned char TerminateKey = 0x67;		/* TerminateKey = Ctrl-F10	*/
unsigned program_active = TRUE;
unsigned popup_active = FALSE;

MONIN monInbuf;
MONOUT monOutbuf;

#define	THREADSTACK	400
char monitor_stack[THREADSTACK];

#define MONINBUFSIZE (sizeof(monInbuf)-sizeof(monInbuf.cb))
#define MONOUTBUFSIZE (sizeof(monOutbuf)-sizeof(monOutbuf.cb))
#define RELEASE 	0x40						/* bit mask to distinguish	*/
#define MAXMILLISECONDS		20000L
#define SLEEPTIME				1000L

typedef struct _keypacket					/* KBD monitor data record */
	{
	unsigned	   mnflags;
	KBDKEYINFO	   cp;
	unsigned	   ddflags;
	} KEYBUFF;


void error_exit(USHORT err, char *msg);
void main(void);
void keyboard_monitor(void);

void main(void)
	{
	USHORT popupwait = (VP_WAIT | VP_OPAQUE);
	KBDKEYINFO kbdkeyinfo;
	long milliseconds;

	DosSemSet(&semhandle);

	if(_beginthread(keyboard_monitor,monitor_stack,THREADSTACK,NULL) == -1)
		error_exit(-1,"_beginthread");

	while(TRUE)
		{
		DosSemRequest(&semhandle,SEM_INDEFINITE_WAIT);	/* wait for semaphore*/

		if(!program_active)								/* time to terminate?		*/
			break;											/* then get out				*/
		VioPopUp(&popupwait,0);
		printf("You made it...press a key");

								/* wait 20 seconds for keystroke, then break out	*/
		for( milliseconds = 0L; milliseconds < MAXMILLISECONDS;
				milliseconds += SLEEPTIME)
			{
			KbdCharIn(&kbdkeyinfo,IO_NOWAIT,0);		/* check for key press		*/
			if(kbdkeyinfo.fbStatus & FINAL_CHAR_IN)/* if pressed, break out	*/
				break;
			DosSleep(SLEEPTIME);							/* sleep for a while			*/
			}

		VioEndPopUp(0);									/* end the popup				*/
		popup_active = FALSE;
		}
	DosExit(EXIT_PROCESS,0); 	   					/* kill the process			*/
	}

void keyboard_monitor(void)
	{
	USHORT retval;
	SEL gdt_descriptor, ldt_descriptor;   /* infoseg descriptors */
	KEYBUFF keybuff;
	unsigned count;
	HKBD KBDHandle;
	PGINFOSEG gdt;
	PLINFOSEG ldt;

	monInbuf.cb = MONINBUFSIZE;
	monOutbuf.cb = MONINBUFSIZE;

		/* obtain a handle for registering buffers */
	if(retval = DosMonOpen ( "KBD$", &KBDHandle ))
		error_exit(retval,"DosMonOpen");

	if(DosGetInfoSeg(&gdt_descriptor, &ldt_descriptor))
		error_exit(retval,"DosGetInfoSeg");

	gdt = MAKEPGINFOSEG(gdt_descriptor);
	ldt = MAKEPLINFOSEG(ldt_descriptor);

	/* register the buffers to be used for monitoring */
	if(retval = DosMonReg ( KBDHandle, (PBYTE)&monInbuf, (PBYTE)&monOutbuf,
			MONITOR_BEGIN, gdt->sgCurrent))
		error_exit(retval,"DosMonReg");

												/* raise thread priority	*/
	DosSetPrty(PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, 0);

	for(keybuff.cp.chChar = 0; ; )				/* loop thru all keyboard in*/
		{
		count = sizeof(keybuff);
												/* read keyboard			*/
		if(retval = DosMonRead ((PBYTE)&monInbuf, IO_WAIT, (PBYTE)&keybuff, (PUSHORT)&count ))
			error_exit(retval,"DosMonRead");
												/* if make on our key		*/
		if(keybuff.cp.chChar == 0)
			if(!popup_active && (keybuff.cp.chScan == HotKey) &&
					!(keybuff.ddflags & RELEASE))
				{
				popup_active = TRUE;
				DosSemClear(&semhandle);
				continue;
				}
			else if((keybuff.cp.chScan == TerminateKey) &&
					!(keybuff.ddflags & RELEASE))
				{
				program_active = FALSE;
				break;
				}

		if(retval = DosMonWrite((PBYTE)&monOutbuf,(PBYTE)&keybuff,count))
			error_exit(retval,"DosMonWrite");
		}
	DosMonClose(KBDHandle);								/* close the monitor			*/
	DosSemClear(&semhandle);
	DosExit(EXIT_THREAD,0); 	   					/* kill the thread			*/
	}

void error_exit(USHORT err, char *msg)
	{
	printf("Error %u returned from %s\n",err,msg);
	DosExit(EXIT_PROCESS,0);
	}



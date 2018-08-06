/**********************************************************
 * SQLPOP - allows the user to hot-key into an SQL
 * Server query window
 *
 * Created by Marc Adler
 **********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <sqlfront.h>
#include <sqldb.h>
#include <netcons.h>
#include <server.h>
#include <os2def.h>
#include <doscalls.h>
#include <subcalls.h>
#include <infoseg.h>

typedef unsigned char BOOL;
typedef unsigned char BYTE;
typedef unsigned int  WORD;
typedef unsigned long DWORD;


/* Structure of the keyboard packet which the device monitor receives */

typedef struct kbdpacket
{
WORD		monitor_flags;
BYTE		ascii_code;
BYTE		scan_code;
WORD		nls;
WORD		shift_code;
DWORD	time;
WORD		devdrv_flags;
} KBDPACKET;

#define KEY_RELEASE		0x40
#define ALT			0x08

#define IDNO			FALSE
#define IDYES			TRUE
#define MB_YESNO		1

#define ERR_DBSQLEXEC_FAILED	-100
#define ERR_DBRESULTS_FAILED	-101

DBPROCESS *DBProc = NULL;	/* Our connection with the DataServer */

WORD		hMonitor;			/* Monitor handle */
BOOL		bIsResident = FALSE;	/* TRUE once we become resident */

struct server_info_1 ServerInfo[65]; /* server_info_1 buffer area */

/***********************************************************
 * Main()
 *
 * Logs into SQL Server, installs the monitor, and calls
 * DoSQLPopup() when the user presses the ALT-S key.
 **********************************************************/

main()
{
	KBDPACKET	packet;
	int		packet_len;
	char		inbuf[128], outbuf[128];
  
	/* Try to login */
	if (!InitDB())
		exit(1);

	bIsResident = TRUE;

	/* Open a keyboard monitor and register it */

	DOSMONOPEN((char far *) "KBD$", (unsigned far *) &hMonitor);
	inbuf[0] = sizeof(inbuf);    inbuf[1] = 0;
	outbuf[0] = sizeof(outbuf);  outbuf[1] = 0;
	DOSMONREG(hMonitor, (char far *) inbuf, (char far *)
			outbuf, 0, GetCurrScreenGroup());

	/* Monitor loop - watches for the ALT+S keystroke */
	for (;;)
	{
		packet_len = sizeof(packet);
		DOSMONREAD((char far *) inbuf, 0, (char far *) &
				 packet, (unsigned far *) &packet_len);
    
	if (!(packet.devdrv_flags & KEY_RELEASE))
	{
		/* Check for an ALT-S keystroke. */

		if ((packet.shift_code & ALT) && 
			packet.ascii_code == 0 && packet.scan_code == 31)
		{
			DoSQLPopup();
			continue;
		}
	}

	/* Pass all other keys through */
	DOSMONWRITE((char far *) outbuf, (char far *) &packet, packet_len);
	}
}


/**********************************************************
 * TerminateMonitor()
 * Logs off the server and terminates the monitor.
 *********************************************************/

TerminateMonitor()
{
	Logout();
	DOSMONCLOSE(hMonitor);
	DOSEXIT(1, 0);
}


/***********************************************************
 * DoSQLPopup()
 * Puts up a rudimentary one-line query box, gets the user's
 * query, sends it to SQL Server, and prints the results.
 * This is done when the user presses the proper hot-key
 * sequence.
 **********************************************************/

char *BoxStr[] =
{
"IMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM;",
":                                                                              :",
"HMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM<"
};

DoSQLPopup()
{
	unsigned	i;
	int		row;
	char		buf[82];
	struct	KbdStringInLength ksl;
	unsigned	waitflag;
	unsigned	attr = 0x07;
	int		nc, col, len;
	char		*more = "Press a key for more...";
 
	/* Take over the screen group for a while */
	waitflag = 0;
	if (VIOPOPUP((unsigned far *) &waitflag, 0))
    		return;

	/* Draw the query box */
	for (i = 0;  i <= 2;  i++)
	{
		VIOWRTCHARSTRATT((char far *) BoxStr[i],
					strlen(BoxStr[i]), i, 0, 
					(char far *) &attr, 0);
	}

	VIOSETCURPOS(1, 1, 0);

	/* Get the user input. Get rid of trailing carruage returns. */
	ksl.Length = sizeof(buf);
	KBDSTRINGIN((char far *) buf, (struct KbdStringInLength
				far *) &ksl, 0, 0);
	if (ksl.LengthB)
	{
		if (buf[ksl.LengthB] == '\r')
      		buf[ksl.LengthB] = '\0';
	}

	/* If the user wants to exit the program, logout and
	 deinstall the monitor */

	if (!strcmpi(buf, "quit") || !strcmpi(buf, "exit"))
	{
		VIOENDPOPUP(0);
		TerminateMonitor();
	}

	/* Ship the query to SQL Server */

	dbcmd(DBProc, buf);
	if (dbsqlexec(DBProc) != FAIL) 
	{
		while (dbresults(DBProc) != NO_MORE_RESULTS)
		{
			char results_buf[256], *rb;

			/* Find out how many columns each row has */
			nc = dbnumcols(DBProc);
			/* Insert the row data one column at a time
			 * into the buffer */
			for (rb = results_buf, col = 1;  col <= nc;  col++)
			{
				dbbind(DBProc, col, CHARBIND, 0, rb);
				len = dbcollen(DBProc, col);
				rb += len;
			}
			*rb = '\0';

			/* Print out the results */
			for (row = 3;  dbnextrow(DBProc) !=
				NO_MORE_ROWS;  row++)
			{
				VIOWRTCHARSTRATT((char far *)
					results_buf, strlen(results_buf),
					row, 0, (char far *) &attr, 0);
				if (row > 22)
				{
					VIOWRTCHARSTRATT((char far *) more,
						strlen(more), row, 0, 
						(char far *) &attr, 0);
					getch();
					row = 3;
				}
			}
		}
	}

	VIOWRTCHARSTRATT((char far *) more, strlen(more), 24,
					0, (char far *) &attr, 0);
	getch();

	VIOENDPOPUP(0);
}


/**********************************************************
 * InitDB()
 * Installs the error handler and tries to login to SQL
 * Server.
 **********************************************************/

InitDB()
{
	int DBErrorHandler();

	/* Install our error handler */
	dberrhandle(DBErrorHandler);

	/* Try to login to SQL Server */
	if (!DoLogin())
		return FALSE;
	return TRUE;
}


/**********************************************************
 * DoLogin()
 * Executed at startup and when user chooses LOGIN
 * from the menu.
 *
 * RETURNS
 *
 * TRUE if the login is successful, FALSE if not.
 **********************************************************/

DoLogin()
{
	LOGINREC	*login;	/* Our login information */
	int		iServer;

	/* Find all servers we are connected to who are running SQL SERVER */
	if ((iServer = LocateSQLServer()) == FALSE)
		return FALSE;

	/* Get a login structure and set the app name, the
	   login id, and the pwd. */
	login = dblogin();
	DBSETLAPP(login, "Adler");
	DBSETLUSER(login, "sa");
	DBSETLPWD(login, "");

	/* Open a new DBProc structure with the new login info.
	   If we fail, then DB_LIB will print out an message
	   through the error handler. */

	if ((DBProc = dbopen(login,
		ServerInfo[iServer].sv1_name)) == NULL)
		return FALSE;
	return TRUE;	/* Successful login ! */
}

/**********************************************************
 * Logout()
 * Closes the DBProc upon exit.  We call this when we end
 * our keyboard monitor.
 **********************************************************/

Logout()
{
	if(DBProc)
	{ 
		dbclose(DBProc);
		DBProc = NULL;
	}
}


/**********************************************************
 * DBErrorHandler()
 * Handles error conditions from SQL Server. Puts up a
 * message box and asks the user for an action.
 **********************************************************/

int DBErrorHandler(dbproc, severity, errno, oserr)
DBPROCESS	*dbproc;
int		severity,
		errno,
		oserr;
{
/* Print the SQL Server error message */

if (MessageBox("SQL Server Error!!!", 
		dberrstr(errno),
		"Continue the program?",
		MB_YESNO) == IDNO)
{
	TerminateMonitor();
	return INT_EXIT;
}

/* See if we have an operating system error */
if (oserr != DBNOERR)
	if (MessageBox("OS/2 Error!!!",
			dboserrstr(oserr),
			"Continue the program?",
			MB_YESNO) == IDNO)
	{
		TerminateMonitor();
		return INT_EXIT;
	}

/* If for some reason the DBProc structure is dead,
 * exit immediately */

if (DBDEAD(dbproc))
{
	TerminateMonitor();
	return INT_EXIT;
}

/* Tell DB-LIB to cancel the command and continue. */

return INT_CANCEL;
}


/**********************************************************
 * printmsgs()
 * Fetches any pending messages from SQL Server and 
 * inserts them into the results buffer.
 *
 * RETURNS - nothing
 **********************************************************/

void printmsgs(dbproc)
DBPROCESS *dbproc;

{
	char *msg;

	while ((msg = dbgetmsg(dbproc)) != NULL)
		fprintf(stderr, "%s\n", msg);
}


MessageBox(s1, s2, s3, action)
char *s1, *s2, *s3;
unsigned  action;

{
	int	c;

	if(s1)
	{
		fprintf(stderr, "%s", s1);
		if(s2)
		{
			fprintf(stderr, "\n%s", s2);
			if(s3)
			{
				fprintf(stderr, "\n%s", s3);
			}
		}
	}
  
	switch(action)
	{
		case MB_YESNO :
			if ((c = getch()) == 'Y' || c == 'y')
				return IDYES;
			return IDNO;
	}
}


/**********************************************************
 * GetCurrScreenGroup()
 *
 * Returns the id of the current screen group.
 **********************************************************/

#define	MAKEPGINFOSEG(sel)	(MAKEP(sel, 0))

GetCurrScreenGroup()
{
	unsigned ldt_seg, gdt_seg;
	struct InfoSegGDT far *pGdt;

	DOSGETINFOSEG((unsigned far *) &gdt_seg, 
				(unsigned far *) &ldt_seg);
	pGdt = MAKEPGINFOSEG(gdt_seg);	/* Get a pointer to the info */
	return pGdt->CurScrnGrp;		/* return the screen group   */
}


LocateSQLServer()
{
	unsigned entriesread;	/* number of entries read by
						   NetServerEnum */
	unsigned totalentries;	/* total number of entries
						   available */
	int i, rc;

	rc = NetServerEnum((char far *) NULL, 1,
				(char far *) ServerInfo,
				sizeof(ServerInfo),
				(unsigned far *) &entriesread,
				(unsigned far *) &totalentries);
	if (!rc)
	{
		for (i = 0;  i < entriesread;  i++)
		if (ServerInfo[i].sv1_type & SV_TYPE_SQLSERVER)
			return i + 1;	/* use offset of 1 so we
						   don't return a '0' */
	}
  
	fprintf(stderr, "Could not find an SQL Server\n");
	return FALSE;
}

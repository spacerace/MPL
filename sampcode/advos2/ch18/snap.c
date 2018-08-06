/*
        SNAP.C  

        A sample OS/2 device monitor that captures the current 
        display into the file SNAPxx.IMG, where xx is the 
        session number.  SNAP works in character mode only and
        and may not be used in a PM window.  The following keys
        are defined as defaults:

	Alt-F10   hot key to capture a screen
	Ctrl-F10  hot key to deinstall SNAP.EXE

        Compile with:  C> cl /F 2000 snap.c

        Usage is:  C> snap

        Copyright (C) 1988 Ray Duncan
*/

#include <stdio.h>
#include <string.h>

					/* hot key definitions */
#define SNAPKEY  0x71                   /* Alt-F10 to capture screen */
#define EXITKEY  0x67                   /* Ctrl-F10 to exit */

#define STKSIZE  2048                   /* stack size for threads */

#define WAIT     0                      /* parameters for DosMonRead */
#define NOWAIT   1                      /* and DosMonWrite */

#define API unsigned extern far pascal  /* API function prototypes */

API DosBeep(unsigned, unsigned);
API DosBufReset(unsigned);
API DosClose(unsigned); 
API DosCloseSem(unsigned long far *);
API DosCreateThread(void (far *)(), unsigned far *, void far *);
API DosCreateSem(unsigned, unsigned long far *, char far *);
API DosExecPgm(char far *, int, int, char far *, char far *, 
               int far *, char far *);
API DosExit(unsigned, unsigned);
API DosGetInfoSeg(unsigned far *, unsigned far *);
API DosMonClose(unsigned);
API DosMonOpen(char far *, unsigned far *);
API DosMonRead(void far *, unsigned, char far *, int far *);
API DosMonReg(unsigned, void far *, void far *, int, unsigned);
API DosMonWrite(void far *, char far *, int);
API DosOpen(char far *, unsigned far *, unsigned far *, unsigned long,
            unsigned, unsigned, unsigned, unsigned long);           
API DosOpenSem(unsigned long far *, char far *);
API DosSemClear(unsigned long far *);
API DosSemSet(unsigned long far *);
API DosSemWait(unsigned long far *, unsigned long);
API DosSetPrty(int, int, int, int);
API DosSleep(unsigned long);
API DosSuspendThread(unsigned);
API DosWrite(unsigned, void far *, int, unsigned far *);
API VioEndPopUp(unsigned);
API VioGetMode(void far *, unsigned);
API VioPopUp(unsigned far *, unsigned);
API VioReadCharStr(char far *, int far *, int, int, unsigned);
API VioWrtCharStr(char far *, int, int, int, unsigned);

void signon(void);                      /* local function prototypes */
void signoff(void);
void popup(unsigned);
void unpop(void);
void far snap(void);
void far watch(void);
void errexit(char *);

                                        /* RAM semaphores */
unsigned long exitsem = 0;		/* exit hot key semaphore */
unsigned long snapsem = 0;              /* screen snapshot semaphore */

char sname[20];                         /* system semaphore name */
unsigned long shandle = 0;              /* system semaphore handle */

char fname[20]; 			/* snapshot filename */
unsigned fhandle = 0;                   /* snapshot file handle */

char kname[] = "KBD$";                  /* keyboard device name */
unsigned khandle = 0;                   /* keyboard monitor handle */

struct _monbuf {                        /* monitor input and */
    int len;                            /* output buffers */
    char buf[128];
    } kbdin  = { sizeof(kbdin.buf)  } , 
      kbdout = { sizeof(kbdout.buf) } ;

struct _vioinfo {                       /* display mode info */
    int len;
    char type;
    char colors;
    int cols;
    int rows;
    } vioinfo;

char msg1[] = "SNAP utility installed!";
char msg2[] = "Alt-F10 to capture screen image into file SNAP.IMG,";
char msg3[] = "Ctrl-F10 to shut down SNAP.";
char msg4[] = "SNAP utility deactivated.";
char msg5[] = "Error detected during SNAP installation:";

main()
{
    char obuff[80];                     /* object name buffer */
    int retcode[2];                     /* receives child info */

    unsigned gseg, lseg;                /* receives selectors */
    char far *ginfo;			/* global info segment pointer */

    unsigned snapID, watchID;           /* receives thread IDs */
    char snapstk[STKSIZE];              /* snapshot thread stack */
    char watchstk[STKSIZE];             /* keyboard thread stack */

    DosGetInfoSeg(&gseg, &lseg);	/* get info segment selectors */
    (long) ginfo = (long) gseg << 16;   /* make far pointer */

					/* build semaphore and file names */
    sprintf(sname, "\\SEM\\SNAP%02d.LCK", ginfo[0x18]);
    sprintf(fname, "\\SNAP%02d.IMG",      ginfo[0x18]);

    if(DosOpenSem(&shandle, sname))     /* does \SEM\SNAPxx.LCK exist? */
    {
                                        /* no, we're parent SNAP */
                                        /* create system semaphore */
        if(DosCreateSem(1, &shandle, sname))
            errexit("Can't create SNAP system semaphore.");
        DosSemSet((unsigned long far *) shandle);

                                        /* start detached child SNAP */
        if(DosExecPgm(obuff, sizeof(obuff), 4, NULL, NULL, retcode, "snap.exe"))
            errexit("Can't start child copy of SNAP.");

                                        /* wait for child to load */
        DosSemWait((unsigned long far *) shandle, -1L);
        DosCloseSem((unsigned long far *) shandle);
    }
    else                                /* if SNAPxx.LCK exists, */
    {                                   /* we're the child SNAP */

                                        /* abort if already resident */
        if(! DosSemWait((unsigned long far *) shandle, 0L))
            errexit("SNAP is already loaded.");

        DosSemSet(&exitsem);            /* initialize exit and */
        DosSemSet(&snapsem);            /* snapshot semaphores */
    
        if(DosMonOpen(kname, &khandle)) /* open monitor connection */
            errexit("Can't open KBD$ monitor connection.");

                                        /* register at head of chain */
        if(DosMonReg(khandle, &kbdin, &kbdout, 1, ginfo[0x18]))
            errexit("Can't register as KBD$ monitor.");

                                        /* create keyboard thread */
        if(DosCreateThread(watch, &watchID, watchstk+STKSIZE))
            errexit("Can't create keyboard thread.");

        DosSetPrty(2, 3, 0, watchID);   /* promote keyboard thread */

                                        /* create snapshot thread */
        if(DosCreateThread(snap, &snapID, snapstk+STKSIZE))
            errexit("Can't create snapshot thread.");

        signon();                       /* announce installation */

                                        /* tell parent we're running */
        DosSemClear((unsigned long far *) shandle);

	DosSemWait(&exitsem, -1L);	/* wait for exit hot key */

        DosSuspendThread(snapID);       /* suspend snapshot thread */
        DosSuspendThread(watchID);      /* suspend keyboard thread */
        DosMonClose(khandle);           /* close monitor connection */
                                        /* close system semaphore */
        DosCloseSem((unsigned long far *) shandle);
        DosClose(fhandle);              /* close snapshot file */

	signoff();			/* announce deinstallation */
    }

    DosExit(1, 0);                      /* final exit */
}

/*
    The 'watch' thread is responsible for monitoring the keyboard
    data stream.  It clears the 'snapsem' semaphore when the 
    screen capture hot key is detected and clears the 'exitsem'
    semaphore when the deinstall hot key is detected.
*/
void far watch(void)
{
    char kbdpkt[128];                   /* monitor data packet */
    int kbdpktlen;                      /* data packet length */

    while(1)
    {
        kbdpktlen = sizeof(kbdpkt);     /* set buffer length */

                                        /* read monitor data */
        DosMonRead(&kbdin, WAIT, kbdpkt, &kbdpktlen);

					/* check for hot keys */
                                        /* ignore key breaks */
        if((kbdpkt[2] == 0) && (kbdpkt[3] == EXITKEY))
	{				/* exit hot key detected */
            if(kbdpkt[12] == 0) DosSemClear(&exitsem);
        }
        else if((kbdpkt[2] == 0) && (kbdpkt[3] == SNAPKEY))
	{				/* snapshot hot key detected */
            if(kbdpkt[12] == 0) DosSemClear(&snapsem);
	}				/* not hot key, pass it through */
        else DosMonWrite(&kbdout, kbdpkt, kbdpktlen);
    }
}

/*
    The 'snap' thread blocks on the 'snapsem' semaphore until it
    is cleared by the 'watch' thread, then captures the current
    screen contents into the snapshot file.
*/
void far snap(void)
{
    int i;                              /* scratch variable */
    unsigned action;                    /* receives DosOpen action */
    unsigned wlen;                      /* receives DosWrite length */
    char divider[81];                   /* snapshot divider line */
    char scrbuf[80];                    /* receives screen data */
    int slen;                           /* contains buffer size */

    memset(divider, '-', 79);           /* initialize divider line */
    divider[79] = 0x0d;
    divider[80] = 0x0a;

                                        /* create/replace snapshot file */
    if(DosOpen(fname, &fhandle, &action, 0L, 0, 0x12, 0x21, 0L))
        errexit("Can't create snapshot file.");

    while(1)
    {                                   /* write divider line */
        DosWrite(fhandle, divider, sizeof(divider), &wlen);
        DosBufReset(fhandle);           /* force file update */

	DosSemWait(&snapsem, -1L);	/* wait for hot key */

	popup(3);			/* pop-up in transparent mode */
        vioinfo.len = sizeof(vioinfo);  /* get screen dimensions */
        VioGetMode(&vioinfo, 0);

        for(i = 0; i < vioinfo.rows; i++)
        {
            slen = vioinfo.cols;        /* read line from screen */
            VioReadCharStr(scrbuf, &slen, i, 0, 0);

                                        /* discard trailing spaces */
            while((slen > 0) && (scrbuf[slen-1] == 0x20)) slen--;

                                        /* write line to file */
            DosWrite(fhandle, scrbuf, slen, &wlen); 
            DosWrite(fhandle, "\x0d\x0a", 2, &wlen);
        }

        DosBeep(440, 200);              /* reward the user */
        unpop();                        /* release screen */
        DosSemSet(&snapsem);            /* reset snapshot semaphore */
    }
}

/*
    Display the installation and help messages in popup window.
*/
void signon(void)
{
    popup(1);                           /* acquire popup screen */
    VioWrtCharStr(msg1, sizeof(msg1), 10, ((80-sizeof(msg1))/2), 0);
    VioWrtCharStr(msg2, sizeof(msg2), 13, ((80-sizeof(msg2))/2), 0);
    VioWrtCharStr(msg3, sizeof(msg3), 15, ((80-sizeof(msg3))/2), 0);
    DosSleep(4000L);			/* pause for 4 seconds */
    unpop();                            /* release popup screen */
}

/*
    Display exit message in popup window.
*/
void signoff(void)                      
{
    popup(1);                           /* acquire popup screen */
    VioWrtCharStr(msg4, sizeof(msg4), 12, ((80-sizeof(msg4))/2), 0);
    DosSleep(2000L);			/* pause for 2 seconds */
    unpop();                            /* release popup screen */
}

/*
    Get popup screen, using wait/no-wait and
    transparent/nontransparent flags supplied by caller.
*/
void popup(unsigned pflags)
{
    VioPopUp(&pflags, 0);   
}

/*
    Take down popup screen.
*/
void unpop(void)
{
    VioEndPopUp(0);
}

/*
    Common error exit routine.  Display error message on popup
    screen and terminate process.
*/
void errexit(char *errmsg)
{
    if(khandle != 0)                    /* close monitor handle */
        DosMonClose(khandle);           /* if monitor active */

    if(shandle != 0)                    /* clear and close the */
    {                                   /* SNAPxx.LCK semaphore */
        DosSemClear((unsigned long far *) shandle);
        DosCloseSem((unsigned long far *) shandle);
    }

    popup(1);                           /* get popup screen and */
                                        /* display error message */
    VioWrtCharStr(msg5, sizeof(msg5), 10, ((80-sizeof(msg5))/2), 0);
    VioWrtCharStr(errmsg, strlen(errmsg), 12, ((80-strlen(errmsg))/2), 0);
    DosSleep(3000L);			/* let user read message */
    unpop();                            /* release popup screen */
    DosExit(1, 1);			/* terminate, exitcode = 1 */
}                                       




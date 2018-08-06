/*
   comport.c -- This file contains the sources for COM port manipulation.
   Created by Microsoft Corporation, 1989
*/
#define  INCL_DOSFILEMGR
#define	 INCL_DOSDEVICES
#define	 INCL_DOSDEVIOCTL
#include <os2.h>
#include "global.h"
#include "comport.h"
/*
   Constants
*/
#define	XON	0x11	/* Ctrl Q */
#define	XOFF	0x13	/* Ctrl S */
char	CRLF[2] = { 0x0d, 0x0a };

/*
    Variables
*/
DCBINFO		dcbinfo;	/* Device control block for Ioctl 53H, 73H */
HFILE		hPort;
LINECONTROL	lnctlBuf;
int		rc;
USHORT		usErrWord;

int ComFlush(void) {
/*
    Flush the COM port with Category 11 functions
*/
    BYTE Data, Zero = 0;

    /* Call Category 11 Functions 1H, 2H  Flush Input, Output Buffers */
    if (rc = DosDevIOCtl(&Data, &Zero, 0x01, 11, hPort)) return rc;
    if (rc = DosDevIOCtl(&Data, &Zero, 0x02, 11, hPort)) return rc;
    return 0;
}

int ComInit(COM comTerm) {
/*
    Open the COM port according to the specifications
*/
    USHORT action;

    /* Get File Handle for COM port (shared read/write access) */
    if (rc = DosOpen(comTerm.szPort,&hPort, &action, 0L, 0, 0x0001, 0x0042, 0L))
	return rc;

    /* Call Category 1 Function 41H   Set Baud Rate */
    if (rc = DosDevIOCtl(NULL, &comTerm.usBaud, 0x41, 1, hPort)) return rc;

    /* Call Category 1 Function 42H   Set Line Characteristics */
    lnctlBuf.bDataBits	= comTerm.bData;
    lnctlBuf.bParity	= comTerm.bParity;
    lnctlBuf.bStopBits	= (BYTE) (comTerm.bStop - 20);	/* IDD_ONESTOP = 20 */
    if (rc = DosDevIOCtl(NULL, &lnctlBuf, 0x42, 1, hPort)) return rc;

    /* Call Category 1 Function 73H   Query Device Control Block */
    if (rc = DosDevIOCtl(&dcbinfo, 0L, 0x73, 1, hPort)) return rc;

    /*
	Do we want software handshaking?
    */
    dcbinfo.fbFlowReplace	&= ~(0x03);	/* Clear bits 0 and 1 */
    dcbinfo.fbFlowReplace	|=
	(comTerm.fSoftware)	? (MODE_AUTO_TRANSMIT | MODE_AUTO_RECEIVE) : 0;
    /*
	Do we want hardware handshaking?
    */
    /* Turn on DTR, if appropriate */
    dcbinfo.fbCtlHndShake	&= ~(0x03);	/* Clear bits 0 and 1 */
    dcbinfo.fbCtlHndShake	|= ((comTerm.fHardware) ? MODE_DTR_CONTROL : 0);

    /* Turn on RTS, if appropriate */
    dcbinfo.fbFlowReplace	&= ~(0xc0);	/* Clear bits 6 and 7 */
    dcbinfo.fbFlowReplace	|= ((comTerm.fHardware) ? MODE_RTS_CONTROL : 0);

    /* Adjust CTS output handshaking */
    dcbinfo.fbCtlHndShake	&= ~MODE_CTS_HANDSHAKE;     /* Clear bit 3 */
    dcbinfo.fbCtlHndShake	|= ((comTerm.fHardware)?MODE_CTS_HANDSHAKE:0);

    /* Adjust DSR output handshaking */
    dcbinfo.fbCtlHndShake	&= ~MODE_DSR_HANDSHAKE;     /* Clear bit 4 */
    dcbinfo.fbCtlHndShake	|= ((comTerm.fHardware)?MODE_DSR_HANDSHAKE:0);

    /* Turn off DCD output handshaking */
    dcbinfo.fbCtlHndShake	&= ~MODE_DCD_HANDSHAKE;     /* Clear bit 5 */

    /* Adjust DSR input sensitivity */
    dcbinfo.fbCtlHndShake	&= ~MODE_DSR_SENSITIVITY;   /* Clear bit 6 */
    dcbinfo.fbCtlHndShake	|= ((comTerm.fHardware)?MODE_DSR_SENSITIVITY:0);
    /*
	Set the line to Wait for Character, Read mode
    */
    dcbinfo.fbTimeout		&= ~(0x06);	/* Clear bits, then set */
    dcbinfo.fbTimeout		|= MODE_WAIT_READ_TIMEOUT;
    dcbinfo.usReadTimeout	= -1;		/* Never! */
 
    /* Call Category 1 Function 53H   Set Device Control Block */
    if (rc = DosDevIOCtl(0L, &dcbinfo, 0x53, 1, hPort)) return rc;

    /* Get ready to start */
    return ComFlush();
}

USHORT ComRead(Line pli) {
/*
    Reads all characters present
    Returns:	0 if successful
		nonzero (Dos Error or Com Error Word) if unsuccessful
*/
    /* Read from the port... And snatch as many as you can! (blocking read) */
    if (rc = DosRead(hPort, pli->szText, MAXLINELEN, &(pli->cch))) return rc;

    /* Check the COM Error Word */
    if (rc = DosDevIOCtl(&usErrWord, NULL, 0x6d, 1, hPort)) return rc;

    /* ...then return it */
    return usErrWord;
}

int ComWrite(char ch) {
/*
    Write a character at a time

    Okay as long as you don't type too fast
*/
    USHORT nCharsWritten;

    return DosWrite(hPort, &ch, 1, &nCharsWritten);
}

int ComClose(void) {
/*
    Close the COM port
*/
    if (rc = ComFlush()) return rc;
    return DosClose(hPort);
} 

int ComBreak(void) {
/*
    Set BREAK mode ON
*/
    USHORT ComErr;

    /* Call Category 1 Function 4BH -- Set Break On */
    return DosDevIOCtl(&ComErr, NULL, 0x4b, 1, hPort);
}

int ComUnbreak(void) {
/*
    Set BREAK mode OFF
*/
    USHORT ComErr;

    /* Call Category 1 Function 45H -- Set Break Off */
    return DosDevIOCtl(&ComErr, NULL, 0x45, 1, hPort);
}

int ComError(void) { return (int) usErrWord; }

/***	modem.c
 *
 * TITLE 
 *
 *	modem.c
 *	Copyright (C) Microsoft Corporation 1987
 *	March 1987
 *
 * DESCRIPTION
 *
 * 	This module contains routines that support Hayes compatible modem. 
 *
 ***/

#include	<stdio.h>	
#include	<doscalls.h>
#include	<subcalls.h>
#include	<malloc.h>
#include	<string.h>
#include 	<memory.h>
#include 	<conio.h>
#include 	"term.h"

#define min(a,b) (((a)<(b)) ? (a):(b))	/* This is pulled out of the old */
					/* v2tov3.h (c v2.0) header file */

extern char 	*ErrMsg[]; 
extern unsigned FileHndl;

make_dial_cmd(char *, int);
void flush_ques(int);
void clr_com_error(void);
void com_clr_flush(void);

static char Init[]    = {'A', 'T', 'V', '0', 0x0d};
static char Attn[]    = {'A', 'T', 0x0d};
static char Esc[]     = {'+', '+', '+'};
static char OnHook[]  = {'A', 'T', 'H', '0', 0X0d};
static char OffHook[] = {'A', 'T', 'H', '1', 0x0d};
static char Reset[]   = {'A', 'T', 'Z', 0x0d};

struct s_Cmd{
        char *pCmd;
        int   CmdLen;
} Cmds[] = {
    {Init, sizeof(Init)},        	/* Initialize modem */
    {Attn, sizeof(Attn)},        	/* Attention cmd */
    {Esc, sizeof(Esc)},          	/* Modem escape sequence*/
    {OnHook, sizeof(OnHook)},    	/* Put phone on hook */
    {OffHook, sizeof(OffHook)},  	/* Take phone off hook */
    {Reset, sizeof(Reset)}       	/* Reset command */
  };



/***	make_modem_conn - make modem connection
 *
 *	This routine tries to setup the modem connection, retrying if 
 *	appropriate.
 *
 *	make_modem_conn()
 *
 *	ENTRY
 *
 *	EXIT
 *		TRUE if modem connection was made 
 *		FALSE if modem connection did not go through
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

make_modem_conn()
{
	unsigned	NumBytes,	/* number of bytes to be written    */
			RetCode,	/* return code from system calls    */
			Result = FALSE; /* to be returned by this routine */
	char		OutBuffer,
			ModemRetry = TRUE,   /* retry for modem connection   */
			NumRetries = 0;      /* no. times connection retried */

	printf("trying modem connection...\n");

  	while (ModemRetry)
          switch (setup_modem_conn()) {
	    case MS_CONNECT    :
	    case MS_CONNECT1200: 
	      Result = TRUE;
	      ModemRetry = FALSE;
	      OutBuffer = '\r';
	      if ((RetCode = DOSWRITE(FileHndl, &OutBuffer, 1, 
                                      &NumBytes)) != 0)
	        error(ERR_DOSWRITE, RetCode);
              break;
	    case MS_NOCARRIER  :
	    case MS_NODIALTONE :
	      if (++NumRetries > NUM_RETRY) {
	        printf("modem connection failed\n");
	        ModemRetry = FALSE;
	      }
	      else
	        printf("modem connection failed; retrying...\n");
	      break;
	    case MS_NOREPLY    :
            case MS_ERROR      : 
	    case OT_FAILURE    :		/* OTher failure */
            case MS_BUSY       : 
	    default            : 
	      ModemRetry = FALSE;
              printf("modem connection failed\n");
              break;
	  }		/* switch (setup_modem_conn()) */

	return(Result);

}




/***	setup_modem_conn - setup modem connection
 *
 *	This is the helper routine for the make_modem_conn(). It initialises
 *	the modem connection and sends out the dial command to the modem.
 *
 *	setup_modem_conn()
 *
 *	ENTRY
 *
 *	EXIT
 *		returns one of the following: MS_CONNECT MS_CONNECT1200 
 *	    	MS_NOCARRIER MS_NODIALTONE MS_NOREPLY MS_ERROR MS_BUSY 
 *	    	OT_FAILURE
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

int	setup_modem_conn()
{
	int		Result = OT_FAILURE;

	/* initialise modem connection */
	if (!(send_modem_cmd(MC_INIT)))
	  return(Result);
	if (get_modem_reply() != MS_OK)
	  return(Result);
	DOSSLEEP((long) 100);

	/* write AT string to the modem. This will cause the modem to
	 * determine the communications rate of the terminal as well
	 * as its parity setting.
 	 */
	if (!(send_modem_cmd(MC_ATTN)))
	  return(Result);
	if (get_modem_reply() != MS_OK)
	  return(Result);
	DOSSLEEP((long) 100);

	/* send DIAL command to the modem */
	if (!(send_modem_cmd(MC_DIAL)))
	  return(Result);

	return(get_modem_reply());
};




/***	send_modem_cmd - send a command to modem
 *
 *	This routine sends a command to the modem and ensures that the echo 
 *	from the modem matches the input command.
 *
 *	send_modem_cmd(CmdType)
 *
 *	ENTRY
 *		CmdType - type of modem command: MC_INIT MC_ATTN MC_ESC 
 *		          	MC_ONHOOK MC_OFFHOOK MC_RESET MC_DIAL
 *
 *	EXIT
 *		TRUE if command was sent succesfully
 *		FALSE if command failed
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

send_modem_cmd(CmdType)
int 	CmdType;            	/* The command to send */
{
    	int  		Result = TRUE,
    			ReadCnt,
    			WriteCnt,
    			CmdLen,
			RetCode;
    	char 		Buf[80],
    			Cmd[80],
    			*pDst,
    			*pCmd;
	

	/* clear COM error and flush transmit/receive queues */
	com_clr_flush();

        if (CmdType == MC_INIT) {
	  send_modem_cmd(MC_RESET);	/* modem command: ATZ */
	  /* clear COM error and flush transmit/receive queues */
	  com_clr_flush();
	};

	/* set pCmd -> command string; CmdLen = length of command string */
    	if (CmdType == MC_DIAL) {
          CmdLen = make_dial_cmd(Cmd, sizeof(Cmd));
          pCmd = Cmd;
        }
    	else {
          pCmd = Cmds[CmdType].pCmd;
          CmdLen = Cmds[CmdType].CmdLen;
        };

	/* write the command out to the modem */
     	if (((RetCode = DOSWRITE(FileHndl, pCmd, CmdLen, &WriteCnt)) != 0)
	    || (WriteCnt != CmdLen))
	  return(FALSE);

        /* wait for the echo & wait for the command to clear */
        DOSSLEEP((long) 250); 
        if (CmdType == MC_DIAL || CmdType == MC_RESET)
          DOSSLEEP((long) 750); 

	/* read back the echo from the modem & check if it matches the input */
	RetCode = 
          DOSREAD(FileHndl, Buf, min(WriteCnt, sizeof(Buf)), &ReadCnt);
	if ((RetCode != 0) || (ReadCnt != WriteCnt))
	  Result = FALSE;
        else {
          pDst = Buf;
          while (--ReadCnt >= 0) {
            if (*pDst++ != *pCmd++) {
	      printf("send_modem_cmd: error in echo of modem input\n");
              Result = FALSE;
              break;
            }
          } /* while (--ReadCnt >= 0) */
        } 

    	return(Result);
}




/***	make_dial_cmd - create a modem command for dialing 
 *
 *	builds the dial command string and returns its length in bytes
 *
 *	make_dial_cmd(pBuf, BufLen)
 *
 *	ENTRY
 *		pBuf - ptr to buffer
 *	        BufLen - length of buffer in bytes
 *
 *	EXIT
 *		pBuf -> dial command string
 *	        make_dial_cmd = length of dial command string in bytes 
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

make_dial_cmd(pBuf, BufLen)
char 	*pBuf;
int  	BufLen;
{
    	char 		*pSrc,
    			*pDst,
    			Ch,
    			Cmd[80],  	/* allocate a huge command buffer */
			*bin_to_dec();
    	int   		Len;
	structModemOptions sModemOptions;		/* modem options */

	get_modem_options(&sModemOptions);
    	pDst = Cmd;
    	*pDst++ = 'A';          /* get Modem's attention */
    	*pDst++ = 'T';
    	*pDst++ = 'S';          /* set R6 for wait for dial tone */
    	*pDst++ = '6';
    	*pDst++ = '=';
    	pDst = bin_to_dec(pDst, sModemOptions.iWaitTone);
    	*pDst++ = B_PAUSE;
    	*pDst++ = 'S';          /* set R7 for wait for carrier time */
    	*pDst++ = '7';
    	*pDst++ = '=';
    	pDst = bin_to_dec(pDst, sModemOptions.iWaitCarrier); 
    	*pDst++ = B_PAUSE;
    	*pDst++ = 'D';
    	*pDst++ = (sModemOptions.chDialType == PULSE) ? 'P' : 'T';
    	pSrc = sModemOptions.pPhoneNumber;
    	while (Ch = *pSrc++) 
          if ((Ch >= '0' && Ch <= '9') || (Ch == ','))
            *pDst++ = Ch;
    	*pDst++ = 0x0d;
    	*pDst = 0;

    	/* copy the command string into caller's buffer */
    	pSrc = Cmd;
    	pDst = pBuf;
    	for (Len = 0; Len <= BufLen; ++Len)
          if (*pSrc==0)
            break;
          else
            *pDst++ = *pSrc++;
    	return(Len);
}




/***	bin_to_dec - convert a binary number to ASCII decimal 
 *
 *	bin_to_dec(pDst, i)
 *
 *	ENTRY
 *		pDst -> buffer to place the ASCII decimal
 *		i = integer to be converted to ASCII decimal
 *
 *	EXIT
 *		ASCII decimal placed in the buffer pointed by pDst
 *		bin_to_dec -> byte next to ASCII decimal in the buffer
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

char *bin_to_dec(pDst, i)
char 	*pDst;
int  	i;
{
    	if (i > 10)
          pDst = bin_to_dec (pDst, i/10);
    	*pDst++ = '0' + (i % 10);
    	return(pDst);
}




/***	get_modem_reply - get reply from modem
 *
 *	get_modem_reply()
 *
 *	ENTRY
 *
 *	EXIT
 *		get_modem_reply = MS_CONNECT MS_CONNECT1200 MS_NOCARRIER 
 *				  MS_NODIALTONE MS_NOREPLY MS_ERROR MS_BUSY
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

int get_modem_reply()
{
    	char 		Ch;
    	int  		Cnt,
    			Result,
	     		RetCode;

    	do  		/* skip CR, LF and get resultcode */
	  if ((RetCode = DOSREAD(FileHndl, &Ch, 1, &Cnt)) != 0) 
            Error(ERR_DOSREAD, RetCode);
        while ((Cnt == 1) && ((Ch == 0x0d) || (Ch == 0x0a)));

    	switch(Cnt) {
          case 0:  Result = MS_NOREPLY;
            	   break;
          case 1:  if ((Ch >= '0') && (Ch <= '7'))
                     Result = Ch - '0';
                   else
                     Result = MS_ERROR;
                   break;
          default: Result = MS_ERROR;
                   break;
        }

    	return(Result);
}




/***	discon_modem - disconnect the modem 
 *
 *	discon_modem()
 *
 *	ENTRY
 *
 *	EXIT
 *		modem disconnected
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

void discon_modem()
{
    	/* Wait for the last command to clear, then be quiet for 1.5 seconds */
    	DOSSLEEP((long) 1000);      	/* wait 1. second */
    	flush_ques(FLUSHOUTPUT);        /* flush the transmit que */
    	DOSSLEEP((long) 1500);          /* wait 1.5 seconds */

    	send_modem_cmd(MC_ESC);    	/* put modem in command mode */
    	DOSSLEEP((long) 2000);          /* wait 2 seconds */

    	send_modem_cmd(MC_ONHOOK);     	/* place phone back on hook */
    	DOSSLEEP((long) 200);          	/* wait for command to clear*/
}




/***	com_clr_flush - clear COM error and flush transmit/receive queues
 *
 *	com_clr_flush()
 *
 *	ENTRY
 *
 *	EXIT
 *		com error cleared
 *		transmit and receive queues of com port flushed out
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

void com_clr_flush()
{
	/* retrieve and clear the com error information */
	clr_com_error();
	/* flush transmit and receive queues */
	flush_ques(FLUSHINPUT);
	flush_ques(FLUSHOUTPUT);
}




/***	flush_ques - flush COM transmit/receive queue 
 *
 *	flush_ques(FuncId)
 *
 *	ENTRY
 *		FuncId - set to one of: FLUSHINPUT FLUSHOUTPUT
 *
 *	EXIT
 *		transmit or receive queue of com port flushed
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

void flush_ques(FuncId)
int	FuncId;			
{	
	char	FlushData,	/* data returned by flush IOCTL function */
		FlushParm = FLUSH_CMDINFO; /* param to flush IOCTL function */
	int	RetCode;
	
	/* flush transmit/receive queue */
	if ((RetCode = DOSDEVIOCTL(&FlushData, &FlushParm, 
			           FuncId, GENERIC, FileHndl)) != 0) 
	  Error(ERR_IOCTLFLUSHQUE, RetCode);
}




/***	clr_com_error - retrieve and clear COM error information
 *
 *	clr_com_error()
 *
 *	ENTRY
 *
 *	EXIT
 *		com error cleared
 *
 *	WARNING
 *
 *	EFFECTS
 *
 ***/

void clr_com_error()

{
	int  	ComError,
        	RetCode;

	if ((RetCode = DOSDEVIOCTL((char *) &ComError, 0L, GETCOMERROR, 
                                   SERIAL, FileHndl)) != 0)
	  Error(ERR_IOCTLGETCOMERROR, RetCode);
}

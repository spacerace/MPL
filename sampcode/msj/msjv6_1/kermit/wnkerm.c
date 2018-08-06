
/* WARNING -- This C source program generated by Wart preprocessor. */
/* Do not edit this file; edit the Wart-format source file instead, */
/* and then run it through Wart to produce a new C source file.     */

/* Wart Version Info: */
char *wartv = "Wart Version 1A(006) Jan 1989";

/*
 * WNKERM.W
 * Protocol module for Windows Kermit
 * 
 * Copyright (c) 1990 by
 * William S. Hall
 * 3665 Benton Street  #66
 * Santa Clara, CA 95051
 *
 * This module must be preprocessed by wart
 */

#define NOCOMM
#define NOKANJI
#define NOSOUND
#define NOATOM
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "wnkerm.h"

/* The state machine returns 0 to show completion or 1 if more input
 * is expected.  Currently, these return values are not used.
 */
#define RESUME return(0)
#define CONTINUE return(1)

/* local function prototypes */
static int near input(void);

/* protocol states */
#define rsinit 1
#define rsfile 2
#define rsdata 3
#define ssinit 4
#define ssfile 5
#define ssdata 6
#define sseof 7
#define sseot 8

/* Protocol description.
 */

#define BEGIN state =

int state = 0;

wart()
{
    int c,actno;
    extern short tbl[];
    while (1) {
	c = input();
	if ((actno = tbl[c + state*128]) != -1)
	    switch(actno) {
case 1:
    {		
    krm_tinit();			/* initialize */
    Kermit.start = 'w';			/* switch to wait */
}
    break;
case 2:
    {
    if (Kermit.delay) {			/* timer will reset this parameter */
	Kermit.start = 'w';		/* wait to send first packet */
        CONTINUE;
    } else {
        if (krm_sinit() < 0) {			/* transmit sendinit packet */
            krm_err(IDS_KRM_SENDINIT_ERROR);	/* error; inform remote */
            RESUME;				/* quit */
        }	
	krmFlushQue();		/* flush any pending input in local buffer */
        BEGIN ssinit;		/* prepare to open file and send file name */
    }	
}
    break;
case 3:
    {
    krm_tinit();			/* initialize */
    BEGIN rsinit;			/* prepare to receive init packet */
}
    break;
case 4:
    {
    krm_spar(krm_rcvpkt.data, krm_rcvpkt.len);	/* set init data */
    Kermit.bctu = Kermit.bctr;			/* select block check type */
    Kermit.pFile = krm_getnextfile(TRUE);	/* get file to send */
    if (krm_sfile() < 0) {			/* send file name packet */
        krm_err(IDS_KRM_SENDFILENAME_ERROR);	/* error; inform remote */
        RESUME;					/* quit */
    }
    BEGIN ssfile;		       /* prepare to send first data packet */
}
    break;
case 5:
    {
    int x;
    krm_savename();			/* post the remote's name for file */
    if ((x = krm_sdata()) == 0) {	/* no data in file */
        if (krm_seof("") < 0) {		/* send end of file */
           krm_err(IDS_KRM_SENDEOF_ERROR);   /* error; inform remote */
           RESUME;			/* quit */
        }
        BEGIN sseof;
    }
    else if (x < 0) {			/* error in sending file */
        krm_rclose(FALSE);		/* close the file */
        krm_err(IDS_KRM_SENDDATA_ERROR); /* post error to remote and quit */
        RESUME;
    }
    else
	BEGIN ssdata;			/* prepare to receive data */
}
    break;
case 6:
    {				
    krm_checkcnx();			/* check if transaction cancelled */
    if (Kermit.abort) {
	if (Kermit.abort == KRM_FILEABORT)  /* cancel file send */
	    Kermit.abort = 0;
	if (krm_seof("D") < 0) {	    /* tell remote about cancellation */
	   krm_err(IDS_KRM_SENDEOF_ERROR);  /* error; inform remote */
	   RESUME;			    /* quit */
	}
        BEGIN sseof;			/* prepare to send end of file */
    }
    else {
        int x;
	if ((x = krm_sdata()) == 0) {		/* no more data */
	    if (krm_seof("") < 0) {		/* send end of file */
	       krm_err(IDS_KRM_SENDEOF_ERROR);  /* error; inform remote */
	       RESUME;				/* quit */
	    }
            BEGIN sseof;		/* prepare to send end of file */
        }
	else if (x < 0) {			/* error */
	    krm_rclose(FALSE);			/* close the file */
	    krm_err(IDS_KRM_SENDDATA_ERROR);	/* inform remote */
	    RESUME;				/* quit */
        }
    }
}
    break;
case 7:
    {
    if (Kermit.pFile = krm_getnextfile(FALSE)) {  /* any more files? */
        if (krm_sfile() < 0) {			  /* send next file name */
            krm_err(IDS_KRM_SENDFILENAME_ERROR);  /* error, inform remote */
            RESUME;				  /* quit */
        }
        BEGIN ssfile;				/* prepare for next file */
    }	
    else {					/* no more files to send */
	if (krm_seot() < 0) {			/* send break */
	    krm_err(IDS_KRM_SENDEOT_ERROR);	/* error, inform remote */
	    RESUME;				/* quit */
	}
	BEGIN sseot;				/* prepare to exit file send */
    }
}
    break;
case 8:
    {
    krm_tend(IDS_KRM_TRANSACTION_DONE);		/* clean up */
    RESUME;					/* quit */
}
    break;
case 9:
    {
    BYTE data[KRM_MAXDATALEN + 1];
    krm_spar(krm_rcvpkt.data, krm_rcvpkt.len);	/* read send-init data */
    krm_ack(krm_rpar(data), data);		/* return init data in ACK */
    Kermit.bctu = Kermit.bctr;			/* set block check type */
    BEGIN rsfile;			        /* prepare for file name */
}
    break;
case 10:
    {
    if (krm_rcvfil()) {				/* try to open file */
	char buf[KRM_MAXDATALEN + 1];			
	int inlen, outlen;
	inlen = strlen(Kermit.pFile);		/* return our name to remote */
	outlen = krm_encode(buf, Kermit.pFile, sizeof(buf) - 1, &inlen);
        krm_ack(outlen, buf);			/* send the name in ACK */
        BEGIN rsdata;				/* prepare to receive data */
    }
    else {
	krm_err(IDS_KRM_FILE_OPEN_ERROR);	/* error, inform remote */
	RESUME;					/* quit */
    }
}
    break;
case 11:
    {
    if (krm_rcvdata()) {
	switch(Kermit.abort) {			/* see if abort flag is on */
	    case KRM_BATCHABORT:
		krm_ack(1, "Z");		/* cancel remaining files */
		Kermit.abort = 0;
		break;
	    case KRM_FILEABORT:
		krm_ack(1, "X");		/* cancel current file */
		Kermit.abort = 0;
		break;
	    default:
		krm_ack(0, "");				/* continue */
	}
    }
    else {
	krm_rclose(TRUE);			/* error in storing data */
	krm_err(IDS_KRM_FILE_WRITE_ERROR);	/* inform host */
	RESUME;					/* quit */
    }
}
    break;
case 12:
    {		/* see if remote wants us to delete file */
    if (krm_rclose(krm_rcvpkt.len && 
	(krm_rcvpkt.data[0] == 'D') ? TRUE : FALSE)) {
        krm_ack(0, "");				/* acknowledge */
        BEGIN rsfile;				/* prepare for next file */
    }
    else {
	krm_err(IDS_KRM_FILE_CLOSE_ERROR);	/* error; inform remote */
	RESUME;					/* quit */
    }
}
    break;
case 13:
    {
    krm_ack(0, "");			/* acknowledge */
    krm_tend(IDS_KRM_TRANSACTION_DONE);	/* clean up */
    RESUME;				/* quit */
}
    break;
case 14:
    {
    CONTINUE;			/* continue until packet is filled */
}
    break;
case 15:
    {
    krm_rclose(TRUE);		/* close any open files */
    krm_tend(KRM_ERROR_PACKET);	/* clean up */
    RESUME;			/* quit */
}
    break;
case 16:
    {
    krm_rclose(TRUE);			/* close any open files */
    krm_err(IDS_KRM_UNKNOWN_PACKET);	/* post error to remote */
    RESUME;				/* quit */
}
    break;

	    }
    }
}

short tbl[] = {
-1, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16,  1, 16, 16,  3,  2, 16, 16, 16, 16, 16, 16, 16, 16, 
-1, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16,  9, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16,  1, 16, 16,  3,  2, 16, 16, 16, 16, 16, 16, 16, 16, 
-1, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 13, 16, 16, 15, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16,  1, 16, 16,  3,  2, 16, 16, 16, 16, 16, 16, 16, 16, 
-1, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 11, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 12, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16,  1, 16, 16,  3,  2, 16, 16, 16, 16, 16, 16, 16, 16, 
-1, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16,  4, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16,  1, 16, 16,  3,  2, 16, 16, 16, 16, 16, 16, 16, 16, 
-1, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16,  5, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16,  1, 16, 16,  3,  2, 16, 16, 16, 16, 16, 16, 16, 16, 
-1, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16,  6, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16,  1, 16, 16,  3,  2, 16, 16, 16, 16, 16, 16, 16, 16, 
-1, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16,  7, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16,  1, 16, 16,  3,  2, 16, 16, 16, 16, 16, 16, 16, 16, 
 0, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16,  8, 16, 16, 16, 16, 16, 16, 
16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 16,  1, 16, 16,  3,  2, 16, 16, 16, 16, 16, 16, 16, 16, 
};


/* input
 *
 * provide packet type to state machine
 */
static int near input()
{

    register int type;

    if (Kermit.start) {
	type = Kermit.start;
	Kermit.start = 0;
    }
    else {
	type = krm_rpack();		/* read input */
	if ((type != '$') && (type != 'E'))  {
	  /* packet is complete but is not an error packet */
	    if ((Kermit.seq != krm_rcvpkt.seq) || strchr("NQT", type)) {
	      /* something is wrong with packet */
	        if (Kermit.retries >= KermParams.RetryLimit) {
		  /* pretend we have an error packet */
		    krmCreatePseudoPacket('E', PS_DONE, IDS_KRM_TOOMANYRETRIES);
		    type = 'E';
	        }
	        else if ((type == 'N') && 
		 	 (krm_rcvpkt.seq == ((Kermit.seq + 1) & 63))) {
		  /* this is OK since NAK for next packet is ACK for previous */
		    type = 'Y';
		    Kermit.retries = 0;
	        }
	        else {
		  /* resend previous packet and try again */
		    Kermit.retries += 1;
		    Kermit.totalretries += 1;
		    krm_resend();
		    type = '$';
	        }
	    }
	    else	/* packet is OK; reset retry count */
	        Kermit.retries = 0;
	}
    }
    return (type);
}

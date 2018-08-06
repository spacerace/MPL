/*
 * WNKERM.H
 *
 * Windows Kermit header file
 *
 * Copyright (c) 1990 by
 * William S. Hall
 * 3665 Benton Street, #66
 * Santa Clara, CA 95051
 *
 * This file should be included in one module in
 * the code for the terminal in the following 
 * way in order to declare the Kermit external variables:
 * #if defined(KERMIT)
 * #define KERMITEXTERN
 * #include "wnkerm.h"
 * #endif
 *
 */

/* This curious construction allows declaration of
 * Kermit variables only in the module in which
 * KERMITEXTERN is defined
 */
#if !defined(KERMITEXTERN)
#define KERMITEXTERN extern
#endif

/*
 * Kermit function declarations
 */
/* from Kermit protocol module */
int wart(void);

/* from Kermit function support module wnkfns.c */
void	NEAR	krm_tinit(void);	/* initialize session */
int	NEAR	krm_sinit(void);	/* send init packet */
void	NEAR	krm_err(int ref);	/* respond to error */
void	NEAR	krm_spar(BYTE *data, short len);/* read remote's init params */
char *	NEAR	krm_getnextfile(BOOL first);	/* build incoming packet */
int	NEAR	krm_sfile(void);	/* send file name */
int	NEAR	krm_seot(void);		/* send break packet */
int	NEAR	krm_seof(char *s);	/* send end of file packet */
int	NEAR	krm_sdata(void);	/* send file data */
BOOL	NEAR	krm_rclose(BOOL);	/* close a file */
void	NEAR	krm_tend(int code);	/* end session */
void	NEAR	krm_checkcnx(void);	/* check for session cancellation */
int	NEAR	krm_rpar(BYTE data[]);	/* prepare local init packet */
/* encode data before transmission */
int	NEAR	krm_encode(BYTE *outbuf, BYTE *inbuf, int maxlen, int *len);
void	NEAR	krm_ack(short len, BYTE * str); /* ack a packet */
BOOL	NEAR	krm_rcvfil(void);	/* receive a file name */
BOOL	NEAR	krm_rcvdata(void);	/* receive data */
int	NEAR	krm_rpack(void);	/* read a packet and return type */
void	NEAR	krm_resend(void);	/* resend a packet */
/* create a pseudo packet for handling internal errors */
void	NEAR	krmCreatePseudoPacket(char type, short state, int msgnum);
void	FAR	krmShowMessage(int msgnum);	/* show a message to user */
BOOL	FAR	krmWndCommand(HWND hWnd, WORD mode); /* Kermit menu function */
void	FAR	krmPaint(HWND hWnd, HDC hDC);	/* paint terminal's icon */
void	NEAR	krm_savename(void);	/* show remote's name for file */
void	NEAR	krmFlushQue(void);	/* flush the input and output queues */
int	FAR	krmShutdown(void);	/* respond to terminal shutdown */
/* handle packet timeouts */
void FAR PASCAL krmDoTimeout(HWND hWnd,unsigned message,short event,DWORD time);

/* from Kermit dialog box routines module wnkmsc.c */
/* open a dialog box */
int  FAR	krmOpenDlgBox(HWND hWnd, FARPROC fpProc, WORD boxnum);
/* show progress of transfers */
BOOL FAR PASCAL krmXferDlgBox(HWND, unsigned, WORD, LONG);
/* cancel session */
BOOL FAR PASCAL krmCancelBoxProc(HWND, unsigned, WORD, LONG);
/* select files for sending */
BOOL FAR PASCAL krmSendFileProc(HWND, unsigned, WORD, LONG);
/* modify protocol parameters */
BOOL FAR PASCAL krmProtocol(HWND hDlg,unsigned message,WORD wParam,LONG lParam);
/* modify inbound and outbound packet parameters */
BOOL FAR PASCAL krmPackets(HWND hDlg,unsigned message,WORD wParam,LONG lParam);

/* from Kermit initialization module wnkint.c */
/* start up initialization of Kermit */
BOOL	FAR	krmInit(HWND hWnd, BYTE *bufptr, int *buflen, int *cid);

/* macros */
#define tochar(ch)	((ch) + ' ')	/* convert int to printable char */
#define unchar(ch)	((ch) - ' ')	/* convert printable char to int */
#define ctl(ch)		((ch) ^ 64)	/* 'controllify' a char */

/* constants and defaults */
#define KRM_MINTIMEOUT		2	/* packet timeout parameters */
#define KRM_MAXTIMEOUT		60
#define KRM_DEFTIMEOUT		5
#define KRM_MINPACKETSIZE	10	/* packet sizes */
#define KRM_MAXPACKETSIZE	94
#define KRM_DEFPACKETSIZE	80
#define KRM_MAXDATALEN		(KRM_MAXPACKETSIZE - 3) /* max data size */
#define KRM_FILEABORT		1	/* file transfer abort flags */
#define KRM_BATCHABORT		2
#define KRM_ERROR_PACKET	0	/* id of error packet */
#define KRM_MAXPADCOUNT		94	/* packet padding paramters */
#define KRM_DEFPADCOUNT		0
#define KRM_DEFPADCHAR		'\000'
#define KRM_DEFTIMER		TRUE	/* default to timer on */
#define KRM_DEFVERBOSE		TRUE	/* show transfer details */
#define KRM_DEFFILEWARN		TRUE	/* warn about duplicate files */
#define KRM_DEFDISCARD		TRUE	/* discard partial files */
#define KRM_DEFBELL		TRUE	/* ring bell after transfer */
#define KRM_DEFBLOCKCHECK	1	/* default block check type */
#define KRM_DEFSENDDELAY	0	/* default delay before first packet */
#define KRM_MAXSENDDELAY	60	/* maximum delay */
#define KRM_DEFRETRYLIMIT	10	/* retry limits */
#define KRM_MAXRETRYLIMIT	63
#define KRM_MINRETRYLIMIT	1
#define KRM_DEFMARK		'\001'	/* default start of packet char */
#define KRM_DEFEOL		'\015'	/* default end of packet char */
#define KRM_DEFQUOTE		'#'	/* default control char quote */
#define KRM_DEFEBQUOTE		'&'	/* default 8-bit char quote */
#define KRM_DEFRPTQUOTE		'~'	/* default repeat char quote */

/* menu items */
#define KRM_MENUBASE		10000	/* adjust to avoid menu id conflicts */
#define IDM_KRM_RECEIVE		KRM_MENUBASE
#define IDM_KRM_SEND		KRM_MENUBASE+1
#define IDM_KRM_CANCEL		KRM_MENUBASE+2
#define IDM_KRM_FILEABORT	KRM_MENUBASE+3
#define IDM_KRM_BATCHABORT	KRM_MENUBASE+4
#define IDM_KRM_ERRORABORT	KRM_MENUBASE+5
#define IDM_KRM_PROTOCOL	KRM_MENUBASE+6
#define IDM_KRM_PACKETS		KRM_MENUBASE+7

/* strings */
#define KRM_STRINGBASE		 10100 /* adjust to avoid string id conflicts */
#define IDS_KRM_TRANSACTION_DONE	KRM_STRINGBASE
#define IDS_KRM_UNKNOWN_PACKET		KRM_STRINGBASE+1
#define IDS_KRM_CANCELLED		KRM_STRINGBASE+2
#define IDS_KRM_TOOMANYRETRIES		KRM_STRINGBASE+3
#define IDS_KRM_FILE_OPEN_ERROR		KRM_STRINGBASE+4
#define IDS_KRM_FILE_READ_ERROR		KRM_STRINGBASE+5
#define IDS_KRM_FILE_WRITE_ERROR	KRM_STRINGBASE+6
#define IDS_KRM_FILE_CLOSE_ERROR	KRM_STRINGBASE+7
#define IDS_KRM_SENDING			KRM_STRINGBASE+8
#define IDS_KRM_SENDINIT_ERROR		KRM_STRINGBASE+9
#define IDS_KRM_SENDFILENAME_ERROR	KRM_STRINGBASE+10
#define IDS_KRM_SENDDATA_ERROR		KRM_STRINGBASE+11
#define IDS_KRM_SENDEOF_ERROR		KRM_STRINGBASE+12
#define IDS_KRM_SENDEOT_ERROR		KRM_STRINGBASE+13
#define IDS_KRM_CANNOTINIT		KRM_STRINGBASE+14
#define IDS_KRM_KERMITMENU		KRM_STRINGBASE+15
#define IDS_KRM_KERMIT			KRM_STRINGBASE+16
#define IDS_KRM_TIMER			KRM_STRINGBASE+17
#define IDS_KRM_FILEWARN		KRM_STRINGBASE+18
#define IDS_KRM_VERBOSE			KRM_STRINGBASE+19
#define IDS_KRM_DISCARD			KRM_STRINGBASE+20
#define IDS_KRM_BELL			KRM_STRINGBASE+21
#define IDS_KRM_BLOCKCHECKTYPE		KRM_STRINGBASE+22
#define IDS_KRM_SENDDELAY		KRM_STRINGBASE+23
#define IDS_KRM_RETRYLIMIT		KRM_STRINGBASE+24
#define IDS_KRM_SENDMARK		KRM_STRINGBASE+25
#define IDS_KRM_RCVMARK			KRM_STRINGBASE+26
#define IDS_KRM_SENDMAXLEN		KRM_STRINGBASE+27
#define IDS_KRM_RCVMAXLEN		KRM_STRINGBASE+28
#define IDS_KRM_SENDTIMEOUT		KRM_STRINGBASE+29
#define IDS_KRM_RCVTIMEOUT		KRM_STRINGBASE+30
#define IDS_KRM_SENDPADCOUNT		KRM_STRINGBASE+31
#define IDS_KRM_RCVPADCOUNT		KRM_STRINGBASE+32
#define IDS_KRM_SENDPADCHAR		KRM_STRINGBASE+33
#define IDS_KRM_RCVPADCHAR		KRM_STRINGBASE+34
#define IDS_KRM_SENDEOL			KRM_STRINGBASE+35
#define IDS_KRM_RCVEOL			KRM_STRINGBASE+36
#define IDS_KRM_SENDQUOTE		KRM_STRINGBASE+37
#define IDS_KRM_RCVQUOTE		KRM_STRINGBASE+38
#define IDS_KRM_QUIT			KRM_STRINGBASE+39

/* timer values */
#define KRM_TIMERBASE	10200	/* adjust as needed for timer conflicts */
#define KRM_WAITPACKET	KRM_TIMERBASE	/* SetTimer parameter for timeout */
#define KRM_WAITSEND	KRM_TIMERBASE+1	/* SetTimer parameter for send delay */

/* this enumeration type is used to plot the
 * progress of a packet as it is filled
 */
enum PACKETSTATE {PS_START,	/* start accumulating packet data */
		  PS_SYNCH,	/* waiting for start of header */
		  PS_LEN,	/* waiting for length byte */
		  PS_NUM,	/* waiting for packet number */
		  PS_TYPE,	/* waiting for packet type */
		  PS_DATA,	/* reading data */
		  PS_CHK,	/* reading checksum */
		  PS_DONE,	/* packet is complete */
};

/*
 * KERMITPARAMS
 *
 * This data structure is associated with the Kermit protocol
 * settings dialog box.
 *
 */
typedef struct _KERMITPARAMS {
    BOOL Bell;				/* sound warning bell */
    BOOL Timer;				/* use timer */
    BOOL DiscardPartialFile;		/* discard partial file */
    BOOL FileWarning;			/* rename incoming files if conflict */
    short BlockCheckType;		/* char, word, or CRC block check */
    short SendDelay;			/* delay before sending first packet */
    short RetryLimit;			/* retry attempts before timing out */
    BYTE ebquote;			/* eight bit quote character */
    BYTE rpquote;			/* repeat quote character */
    BOOL verbose;			/* show progress of transfer */
} KERMITPARAMS;

/*
 * KRMPACKETPARAMS
 *
 * This data structure is associated with the Kermit packet
 * settings dialog box.
 *
 */
typedef struct _KRMPACKETPARAMS {
    BYTE mark;			/* start of header character */
    short maxpktsize;		/* maximum packet size */
    short timeout;		/* timeout interval */
    short padcount;		/* padding character count */
    BYTE padchar;		/* padding character */
    BYTE eol;			/* end of packet character */
    BYTE quote;			/* control character quote */
} KRMPACKETPARAMS;

/* 
 * KERMITDATA
 *
 * This structure contains most of the additional parameters
 * needed by Kermit.
 */
typedef struct _KERMITDATA {
    HWND hWnd;				/* main window handle */
    HANDLE hInst;			/* instance handle */
    BOOL start;				/* start transfer */
    BOOL InTransfer;			/* in transfer */
    WORD mode;				/* mode of transfer - send, receive */
    short bctu;				/* block check type used */
    short bctr;				/* block check type requested */
    short seq;				/* packet sequence number */
    BOOL delay;				/* waiting to send first packet */
    FARPROC fpTimer;			/* farproc pointer to Kermit timer */
    short retries;			/* retries per packet */
    short totalretries;			/* total retries */
    OFSTRUCT fstruct;			/* file structure */
    DWORD bytesmoved;			/* bytes transferred */
    DWORD packetcount;			/* packets exchanged */
    DWORD filesize;			/* size of send file */
    int percentage;			/* percentage of file sent */
    int maxsenddatalen;			/* max send packet data length */
    int hFile;				/* file handle */
    HANDLE hFilelist;			/* handle to send file list */
    char *pFilelist;			/* pointer to send file list */
    char *pFile;			/* pointer to single file */
    FARPROC fpXfer;			/* transfer dialog box farproc */
    HWND hWndXfer;			/* handle to transfer dialog box */
    short abort;			/* abort type; file or group */
    BOOL ebqflag;			/* using eight-bit quoting */
    BOOL rptflag;			/* using run-length encoding */
} KERMITDATA;

/*
 * KRMPACKET structure
 *
 * This data structure is used to hold the various fields
 * of a packet received from the remote Kermit.
 *
 */
typedef struct _KRMPACKET {
    short len;				/* length of packet */
    short seq;				/* packet sequence number */
    BYTE type;				/* packet type */
    BYTE data[KRM_MAXDATALEN + 1];	/* packet data */
    char rchksum[3];			/* received checksum bytes */
    short data_count;			/* current data count */
    short chk_count;			/* current checksum data count */
    enum KRMPACKETSTATE state;		/* state of packet decomposition */
} KRMPACKET;

/*
 * KRMSENDINIT structure
 * 
 * This structure is used to hold the parameters required
 * to compose outbound packets and to decompose inbound packets.
 *
 */
typedef struct _KRMSENDINIT {
    BYTE mark;			/* start of packet header */
    short maxpktsize;		/* maximum packet size */
    short timeout;		/* how many seconds to wait before timing out */
    short padcount;		/* how much padding to add to a packet */
    BYTE padchar;		/* character used for padding */
    BYTE eol;			/* character denoting end of packet */
    BYTE quote;			/* control code quote character */
    BYTE ebquote;		/* eight bit quote character */
    BYTE rpquote;		/* run length quote character */
} KRMSENDINIT;

/* External variables needed by Kermit */
KERMITEXTERN KERMITPARAMS KermParams;   /* protocol dialog box values */
KERMITEXTERN KRMPACKETPARAMS sndparams;	/* outbound packet dialog box values */
KERMITEXTERN KRMPACKETPARAMS rcvparams;	/* inbound packet dialog box values */

KERMITEXTERN KRMSENDINIT krm_sndinit;   /* data for making outbound packets */
KERMITEXTERN KRMSENDINIT krm_rcvinit;   /* data for making inbound packets */
KERMITEXTERN KERMITDATA Kermit;		/* most other Kermit data */
KERMITEXTERN KRMPACKET krm_rcvpkt;	/* holds data from incoming packet */

KERMITEXTERN BYTE krm_sndpkt[100]; /* Holds an assembled packet to be sent */
KERMITEXTERN BYTE *krmBufptr;	   /* Buffer holding data read from comm port */
KERMITEXTERN int *krmBuflen;	   /* Length of data in buffer */
KERMITEXTERN int *krmcid;	   /* Comm port identifier */
KERMITEXTERN HICON krmIcon;	   /* Kermit icon */

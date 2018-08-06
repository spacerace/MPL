/*
**	litecomm.h
**		Communications support for Datalight (tm) C
**		Copyright (c) 1987 - Information Technology, Ltd.,
**							 All Rights Reserved
*/

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define ERR (-1)

#ifndef LITECOMM_H				/* if not already included		*/

#define COM1BASE	0x3f8		/* base for com port 1 */
#define COM2BASE	0x2f8		/* base for com port 2 */
#define IRQ1		0x10		/* int req mask for port 1 - irq4 */
#define IRQ2		0x08		/* int req mask for port 2 - irq3 */
#define COM1VEC		0x0c		/* vector for port 1 */
#define COM2VEC		0x0b		/* vector for port 2 */

#ifdef KRNL
unsigned COM3BASE = 0x3e8;		/* user changeable com port 3 */
unsigned COM4BASE = 0x2e8;		/* user changeable com port 4 */
char IRQ3 = IRQ1;				/* ditto for int levels */
char IRQ4 = IRQ2;
unsigned COM3VEC = 0x0c;
unsigned COM4VEC = 0x0b;
#else
extern unsigned COM3BASE;
extern unsigned COM4BASE;
extern char IRQ3;
extern char IRQ4;
extern unsigned COM3VEC;
extern unsigned COM4VEC;
#endif

/*
**	special control characters
*/
#define	XON		0x11
#define	XOFF	0x13

/*
**	8250 register defines (offsets from base)
*/
#define	THREG	0				/* transmit hold register */
#define RDREG	0				/* input data register */
#define LSBDIV	0				/* least sig. byte of baud rate divisor */
#define MSBDIV	1				/* most sig, only when line ctl bit 7 = 1 */
#define IEREG	1				/* interrupt enable register */
#define IIDREG	2				/* interrupt ID register */
#define LCREG	3				/* line control register */
#define MCREG	4				/* modem control register */
#define LSREG	5				/* line status register */
#define MSREG	6				/* modem status register */

/*
**	PARITY SETTINGS used in conjunction with above
*/
#define	NPARITY	0x00			/* no parity */
#define OPARITY	0x08			/* odd parity */
#define EPARITY	0x18			/* even parity */
#define	MPARITY	0x28			/* mark parity */
#define SPARITY	0x38			/* space parity */

/*
**	DATA and STOP BIT settings
*/
#define BIT5	0x00			/* 5 data bits */
#define BIT6	0x01			/* 6 data bits */
#define BIT7	0x02			/* 7 data bits */
#define BIT8	0x03			/* 8 data bits */
#define STOP1	0x00			/* 1 stop bit */
#define STOP2	0x04			/* 2 stop bit */

/*
**	Baud Rate Settings
*/
#define B110	0x417
#define	B300	0x180
#define	B600	0x0C0
#define B1200	0x060
#define	B2400	0x030
#define	B4800	0x018
#define	B9600	0x00C
#define	B19200	0x006

/*
** SPECIAL PURPOSE SETTINGS - internal use only
*/
#define	BREAK_ON 0x40			/* enable transmitter break */
#define	SETBAUD	0x80			/* enable access to baud rate divisor */

#define	DTR		0x01			/* turn on DTR modem signal */
#define RTS		0x02			/* turn on RTS modem signal */
#define	OUT2	0x08			/* enable OUT2 for interrupts */
#define	LOOPBACK 0x10			/* enable loopback mode */

#define	RDINT	0x01			/* enable receive data int */
#define	THREINT	0x02			/* enable transmit hold reg empty int */
#define	BRKINT	0x04			/* enable break/error int */
#define	MSTINT	0x08			/* enable modem change int */

#define NOPEND	0x01			/* no interrupts pending */
#define INTMST	0x00			/* modem change int */
#define	INTTHRE	0x02			/* THRE int */
#define	INTRD	0x04			/* receive data int */
#define	INTBRK	0x06			/* break/error int */

/*
**	line status register values
*/
#define	RDRDY	0x01			/* receive data ready */
#define ORUNERR	0x02			/* over-run error */
#define	PARERR	0x04			/* parity error */
#define FRMERR	0x08			/* framing error */
#define	BRKDET	0x10			/* break detect */
#define THREMP	0x20			/* transmit hold reg empty */
#define TSREMP	0x40			/* transmit shift reg empty */
#define TIMEOUT	0x80			/* timeout */

/*
**	modem status register values
**		Bits 0 - 3 define which signal(s) have changed
**				   with a value of 1 indicating a change has occurred
**		Bits 4 - 7 indicate the current state of each of the 4 signals
*/
#define	CTSCHG	0x01			/* delta CTS */
#define DSRCHG	0x02			/* delta DSR */
#define	RICHG	0x04			/* delta RI */
#define	DCDCHG	0x08			/* delta DCD */
#define	CTS		0x10			/* Clear To Send */
#define DSR		0x20			/* Data Set Ready */
#define RI		0x40			/* Ring Indicator */
#define DCD		0x80			/* Data Carrier Detect */

typedef	struct
{
	void interrupt	(*orgvect)();		/* original vector */
	unsigned	baseport;		/* baseline port */
	unsigned	pvector;		/* int vector for port */
	unsigned	pirq;			/* port int req mask */
	volatile unsigned mdmstat;	/* current modem status */
	unsigned	mdmctlr;		/* current modem control values */
	volatile unsigned lasterr;	/* last error return */
	unsigned	intmask;		/* current active ints */
	char		*inbuff;		/* input ring buffer */
	int 		inbuflen;		/* size of buffer */
	volatile int inbufcnt;		/* chars in buffer now */
	int			inbufmrk;		/* trigger point for xoff */
	volatile char *inhead;		/* pointer to insert */
	volatile char *intail;		/* pointer to remove */
	int			xstate;			/* TRUE if XON/XOFF active */
	volatile int inxrecd;		/* TRUE if XOFF rec'd */
	volatile int outxsent;		/* TRUE if XOFF sent */
	char		*outbuff;		/* output ring buffer */
	int			outbuflen;		/* size of buffer */
	volatile int outbufcnt;		/* chars in buffer now */
	volatile char *outhead;		/* pointer to insert */
	volatile char *outtail;		/* pointer to remove */
} COMM;

#define LITECOMM_H	1
#endif

/*
   SIO.H - CTask - Serial I/O interface routine definitions.

   Public Domain Software written by
      Thomas Wagner
      Patschkauer Weg 31
      D-1000 Berlin 33
      West Germany
*/


#define XON       0x11
#define XOFF      0x13

/* Parity values for "v24_change_parity" */

#define PAR_NONE  0x00
#define PAR_EVEN  0x18
#define PAR_ODD   0x08
#define PAR_MARK  0x28
#define PAR_SPACE 0x38

/* Modem control bits for "v24_watch_modem" */

#define CTS       0x10
#define DSR       0x20
#define RI        0x40
#define CD        0x80

/* Values for "v24_protocol" */

#define XONXOFF   0x01
#define RTSCTS    0x02

/* Internal structures */

typedef struct s_port_data far *portptr;
typedef struct s_sio_datarec far *sioptr;

typedef struct s_port_data {
                           portptr     next;    /* Next defined port */
                           int         pnum;    /* Internal Port ID */
                           sioptr      sio;     /* SIO control block */
                           int         base;    /* Port base */
                           byte        irq;     /* IRQ level */
                           byte        vector;  /* Interrupt vector number */
                           } port_data;


typedef struct s_sio_datarec {
               sioptr   next;          /* Next link for shared IRQs */
               farptr   savvect;       /* Interrupt vector save location */
               portptr  port;          /* Port descriptor pointer */
               int      port_base;     /* Port base I/O address */
               int      r_xoff;        /* Receive disable (XOFF sent) */
               int      t_xoff;        /* Transmit disable (XOFF received) */
					int		xoff_threshold; /* Pipe free threshold for XOFF */
					int		xon_threshold; /* Pipe free threshold for XON */
               byte     clcontrol;     /* Current line control reg */
               byte     cmodcontrol;   /* Current modem control reg */
               byte     irqbit;        /* IRQ-Bit for this port */
               byte     civect;        /* Interrupt Vector for this port */
               byte     modstat;       /* Current modem status */
               byte     wait_xmit;     /* Transmit delayed */
               byte     xmit_pending;  /* Transmit in progress */
               byte     rtsoff;        /* RTS turned off by protocol */
               byte     overrun;       /* Pipe full on receive */
               byte     modem_flags;   /* Transmit enable modem flags */
               byte     flags;         /* Protocol flags */

               byte     save_lcon;     /* Previous line control value */
               byte     save_mcon;     /* Previous modem control value */
               byte     save_inten;    /* Previous interrupt control value */
               byte     save_irq;      /* Previous interrupt bit value */
               byte     save_bd1;      /* Previous baud rate lower byte */
               byte     save_bd2;      /* Previous baud rate upper byte */

               wpipe    rcv_pipe;      /* Received characters */
               pipe     xmit_pipe;     /* Transmit pipe */

               } sio_datarec;


/* function prototypes */

extern int far v24_define_port (int base, byte irq, byte vector);

extern sioptr far v24_install (int port, int init,
                               farptr rcvbuf, word rcvsize, 
                               farptr xmitbuf, word xmitsize);
extern void far v24_remove (sioptr sio, int restore);
extern void far v24_remove_all (void);

extern void far v24_change_rts (sioptr sio, int on);
extern void far v24_change_dtr (sioptr sio, int on);
extern void far v24_change_baud (sioptr sio, long rate);
extern void far v24_change_parity (sioptr sio, int par);
extern void far v24_change_wordlength (sioptr sio, int len);
extern void far v24_change_stopbits (sioptr sio, int n);
extern void far v24_watch_modem (sioptr sio, byte flags);
extern void far v24_protocol (sioptr sio, int prot, 
                              word offthresh, word onthresh);

extern int far v24_send (sioptr sio, byte ch, dword timeout);
extern int far v24_receive (sioptr sio, dword timeout);
extern int far v24_overrun (sioptr sio);
extern int far v24_check (sioptr sio);
extern int far v24_modem_status (sioptr sio);
extern int far v24_complete (sioptr sio);
extern int far v24_wait_complete (sioptr sio, dword timeout);


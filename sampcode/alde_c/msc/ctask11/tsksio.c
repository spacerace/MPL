/*
   TSKSIO.C - CTask - Serial I/O interface routines.

   Public Domain Software written by
      Thomas Wagner
      Patschkauer Weg 31
      D-1000 Berlin 33
      West Germany

   With version 1.1, support for shared IRQ lines and dynamically
   defined ports was added. The tables used are

      port_list   The pointer to the list of defined ports. Newly
                  defined ports are added to the end of the list.
                  Ports can never be deleted. Each port descriptor
                  contains the hardware info for the port, plus a
                  pointer to the associated sio-control-block if the
                  port was initialized.
                  
      port_last   The pointer to the last element in port_list.

      irq_array   Is an array of pointers to sio-control-blocks. For
                  each possible IRQ-line the entry in this table points
                  to the first block active for this line. If the IRQ
                  is shared, sio-blocks are chained into this list via 
                  their "next" pointer.

      irq_procs   Contains the pointer to the interrupt handler function
                  for the corresponding IRQ-line.

      sio_data    Contains the statically defined sio control blocks.
      port_descr  Contains the statically defined port descriptor blocks.

   NOTE:    You can not dynamically define ports for IRQ-lines that
            have no interrupt handler function defined. To be completely
            flexible, you would have to define sioint-handlers for all
            possible IRQs and enter the addresses into the irq_procs array.

   CAUTION: Please restrict the installation and removal of v24-
            ports to *one* task. The manipulation of the lists is
            not protected, so simultaneous install/remove calls
            may cause trouble. Since ports are normally installed and
            removed in the main task, the protection of critical regions
            seemed unnecessary.

   CAUTION: Shared interrupt logic and IRQ-lines above 4 were not
            tested due to the lack of suitable hardware. Changes may
            be necessary in the handling of the modem control register
            OUT2 line that normally controls interrupt enable, depending
            on the multiport-hardware installed in the target system.
*/

#include "tsk.h"
#include "sio.h"

#define MAX_IRQ   16    /* Maximum number of interrupt lines, 
                           16 for AT, 8 for XT. Can be left at 16 */

#define CHAIN_IRQBIT    0x04  /* Chained int controller IRQ bit */

#define RESTORE_DEFAULT 1     /* Restore parameter for remove_all */

typedef void (interrupt far * intprocptr)(void);

#define RTS       0x02
#define DTR       0x01
#define OUT2      0x08

#define ERR_MASK  0x1e

#define inta00    0x20   /* 8259 interrupt controller control-port */
#define inta01    0x21   /* 8259 interrupt controller mask-port */

#define inta10    0xa0   /* secondary 8259 control-port for IRQ 8-15 */
#define inta11    0xa1   /* secondary 8259 mask-port for IRQ 8-15 */

#define rdserv    0x0b   /* read service register control value */
#define eoi       0x20   /* end of interrupt signal for 8259 */

#define intdata   0x0b   /* Enable Interrupts except Line status */

#define rxreadybit 0x01
#define txemptybit 0x40
#define txreadybit 0x20
#define framingbit 0x08
#define breakbit   0x10

/* Note: In version 1.1, port offsets start at 0, not 8 */

#define linecontrol  0x03
#define linestatus   0x05
#define intid        0x02
#define intenable    0x01
#define modemcontrol 0x04
#define modemstatus  0x06
#define receivedata  0x00
#define transmitdata 0x00

#define baudreg_dll  0x00     /* baud rate least significant byte */
#define baudreg_dlm  0x01     /* baud rate most significant byte */

/*
   Default values for initialising the ports.
   Change to your liking (but remember that OUT2 must be set in the
   modem control register to allow interrupts to come through with 
   normal controllers).
*/

#define dflt_modcon 0x0b   /* Modem Control: Activate DTR, RTS, OUT2 */
#define dflt_baud   96     /* Baud Rate Divisor: 1200 Baud */
#define dflt_lcon   0x03   /* Line Control: No Parity, 1 Stop, 8 Data */

/*
   Defined baud rates. You may expand this table with non-standard
   rates if desired.
*/

local long baud_table [] = {
                               50L, 2304L,
                               75L, 1536L,
                              110L, 1047L,
                              134L,  857L,
                              150L,  768L,
                              300L,  384L,
                              600L,  192L,
                             1200L,   96L,
                             1800L,   64L,
                             2000L,   58L,
                             2400L,   48L,
                             3600L,   32L,
                             4800L,   24L,
                             7200L,   16L,
                             9600L,   12L,
                            19200L,    6L,
                            38400L,    3L,
                                0L,    0L };

/*-------------------------------------------------------------------------*/

/*
   To add static support for other COM-Ports, define
      - Port base
      - IRQ-Line
      - Interrupt vector
   here, and add the necessary data to the port_descr array.
   If the port does *not* share an IRQ with the predefined ports,
   define the corresponding interrupt function by duplicating siointx,
   and place the entry for this function into the irq_procs array.

   Note that ports may also be defined on-line if TSK_DYNAMIC is enabled.
*/

#define STATIC_PORTS  2       /* Number of statically defined ports */

/* Note: In version 1.1, port offsets start at 0, not 8 */

#define com1_base    0x3f8    /* COM1 port base */
#define com2_base    0x2f8    /* COM2 port base */

#define com1_irq     4        /* IRQ-Line for COM1 */
#define com2_irq     3        /* IRQ-Line for COM2 */

#define com1_vect    0x0c     /* Interrupt vector for COM1 */
#define com2_vect    0x0b     /* Interrupt vector for COM2 */

/*-------------------------------------------------------------------------*/


local void interrupt far sioint3 (void);
local void interrupt far sioint4 (void);

/* 
   Table of Interrupt handler functions for each IRQ line.
*/

local intprocptr irq_procs [MAX_IRQ] = {  NULL,    /* IRQ 0 */
                                          NULL,    /* IRQ 1 */
                                          NULL,    /* IRQ 2 */
                                          sioint3, /* IRQ 3 */
                                          sioint4, /* IRQ 4 */
                                          NULL,    /* IRQ 5 */
                                          NULL,    /* IRQ 6 */
                                          NULL,    /* IRQ 7 */
                                          NULL,    /* IRQ 8 */
                                          NULL,    /* IRQ 9 */
                                          NULL,    /* IRQ 10 */
                                          NULL,    /* IRQ 11 */
                                          NULL,    /* IRQ 12 */
                                          NULL,    /* IRQ 13 */
                                          NULL,    /* IRQ 14 */
                                          NULL };  /* IRQ 15 */


local sio_datarec sio_data [STATIC_PORTS];

/* When adding entries to port_descr, be sure to chain the 
   elements in ascending order via the first field, and to
   increase the internal port number in the second field. */

local port_data port_descr [STATIC_PORTS] = {
     { &port_descr[1], 0, NULL, com1_base, com1_irq, com1_vect },
     { NULL,           1, NULL, com2_base, com2_irq, com2_vect }
                                            };

local sioptr  irq_array [MAX_IRQ] = { NULL };

local portptr port_list = &port_descr [0];
local portptr port_last = &port_descr [1];

local int ports = STATIC_PORTS;

extern funcptr v24_remove_func;

/*-------------------------------------------------------------------------*/


local void near change_rts (sioptr data, int on)
{
   data->rtsoff = (byte)(!on);
   data->cmodcontrol = (data->cmodcontrol & ~RTS) | ((on) ? RTS : 0);
   tsk_outp (data->port_base + modemcontrol, data->cmodcontrol);
}


local void near transmit_ready (sioptr data)
{
   int i;
#if (MSC)
   int temp;
#endif

   if ((i = data->r_xoff) < 0)
      {
#if (MSC)
      temp = (i == -1) ? XOFF : XON;
      tsk_outp (data->port_base + transmitdata, temp);
#else
      /* NOTE: Microsoft C 5.0 generates an "Internal Compiler Error"
               when compiling the following statement.
      */
      tsk_outp (data->port_base + transmitdata, (i == -1) ? XOFF : XON);
#endif
      data->r_xoff = (i == -1) ? 1 : 0;
      data->xmit_pending = 1;
      return;
      }

   data->xmit_pending = 0;

   if (!(data->wait_xmit = (byte)(check_pipe (&data->xmit_pipe) != -1)))
      return;

   if ((data->modem_flags & data->modstat) ^ data->modem_flags)
      return;

   if (data->flags & XONXOFF && data->t_xoff)
      return;

   data->wait_xmit = 0;

   if ((i = c_read_pipe (&data->xmit_pipe)) < 0)
      return;

   tsk_outp (data->port_base + transmitdata, (byte)i);
   data->xmit_pending = 1;
}


local void near modem_status_int (sioptr data)
{
   data->modstat = tsk_inp (data->port_base + modemstatus);

   if (data->wait_xmit)
      transmit_ready (data);
}


local void near receive_ready (sioptr data)
{
   word status;
   word ch;

   while ((status = tsk_inp (data->port_base + linestatus)) & rxreadybit)
      {
      tsk_nop ();
      ch = tsk_inp (data->port_base + receivedata);

      if (data->flags & XONXOFF)
         {
         if (ch == XON)
            {
            data->t_xoff = 0;
            if (data->wait_xmit)
               transmit_ready (data);
            continue;
            }
         else if (ch == XOFF)
            {
            data->t_xoff = 1;
            continue;
            }
         if (!data->r_xoff && 
             wpipe_free (&data->rcv_pipe) < data->xoff_threshold)
            {
            data->r_xoff = -1;
            if (!data->xmit_pending)
               transmit_ready (data);
            }
         }

      if (data->flags & RTSCTS && !data->rtsoff)
         if (wpipe_free (&data->rcv_pipe) < data->xoff_threshold)
            change_rts (data, 0);

      status = (status & ERR_MASK) << 8;
      if (c_write_wpipe (&data->rcv_pipe, ch | status) < 0)
         data->overrun = 1;
      }
}


/*-------------------------------------------------------------------------*/


local void near sioint (sioptr data)
{
   int id;

   while (!((id = tsk_inp (data->port_base + intid)) & 1))
      switch (id & 0x07)
         {
         case 0x00:  modem_status_int (data);
                     break;

         case 0x02:  transmit_ready (data);
                     break;

         case 0x04:  receive_ready (data);
                     break;

/*       case 0x06:  line_status_int (data); (currently not used)
                     break;
*/
         }
}


local void interrupt far sioint3 (void)
{
   sioptr curr;

   tsk_sti ();
   for (curr = irq_array [3]; curr != NULL; curr = curr->next)
      sioint (curr);

   tsk_cli ();
   /* NOTE: for IRQ's 8-15, add the following here:
      tsk_outp (inta10, eoi);
   */
   tsk_outp (inta00, eoi);
}


local void interrupt far sioint4 (void)
{
   sioptr curr;

   tsk_sti ();
   for (curr = irq_array [4]; curr != NULL; curr = curr->next)
      sioint (curr);

   tsk_cli ();
   /* NOTE: for IRQ's 8-15, add the following here:
      tsk_outp (inta10, eoi);
   */
   tsk_outp (inta00, eoi);
}

/*-------------------------------------------------------------------------*/

int far v24_define_port (int base, byte irq, byte vector)
{
#if (TSK_DYNAMIC)
   portptr portp;

   if (irq >= MAX_IRQ)
      return -1; 
   if (irq_procs [irq] == NULL)
      return -1; 

   if ((portp = tsk_alloc (sizeof (port_data))) == NULL)
      return -1;
   portp->pnum = ports;
   portp->base = base;
   portp->irq = irq;
   portp->vector = vector;
   portp->next = NULL;
   portp->sio = NULL;

   if (port_list == NULL)
      port_list = portp;
   else
      port_last->next = portp;

   port_last = portp;
   ports++;

   return portp->pnum;

#else
   return -1;
#endif
}


local sioptr ret_error (sioptr sio)
{
   sio->port->sio = NULL;
#if (TSK_DYNAMIC)
   if (sio->port->pnum >= STATIC_PORTS)
      tsk_free (sio);
#endif
   return NULL; 
}

sioptr far v24_install (int port, int init,
                        farptr rcvbuf, word rcvsize,
                        farptr xmitbuf, word xmitsize)
{
   sioptr sio;
   portptr portp;
   int pbase;
   intprocptr far *intptr;
   int i, inta;

#if (TSK_NAMEPAR)
   static char xname [] = "SIOnXMIT", rname [] = "SIOnRCV";

   xname [3] = rname [3] = (char)(port & 0x7f) + '0';
#endif

   if (port < 0 || !rcvsize || !xmitsize)
      return NULL;

   portp = port_list;

   if (port & 0x80)     /* Relative port number */
      {
      port &= 0x7f;
      if (port > 4)
         return NULL; 
      pbase = *((wordptr)(MK_FP (0x40, port * 2)));
      if (!pbase)
         return NULL;

      for (port = 0; port < ports; port++, portp = portp->next)
         if (portp->base == pbase)
            break;

      if (port >= ports)
         return NULL;
      }
   else 
      {
      if (port > ports)
         return NULL;
      for (port = 0; port < ports; port++)
         portp = portp->next;
      }

   if (portp->sio != NULL) /* Port already in use ? */
      return NULL;

   if (port < STATIC_PORTS)
      portp->sio = sio = &sio_data [port];
   else
#if (TSK_DYNAMIC)
      if ((portp->sio = sio = tsk_alloc (sizeof (sio_datarec))) == NULL)
         return NULL;
#else
      return NULL;
#endif

   pbase = sio->port_base = portp->base;
   sio->port = portp;

   /* Check if port accessible by modifying the modem control register */

   i = sio->cmodcontrol = sio->save_mcon = tsk_inp (pbase + modemcontrol);
   if (i & 0xe0)
      return ret_error (sio);
   tsk_nop ();
   tsk_outp (pbase + modemcontrol, 0xe0 | i);
   tsk_nop ();
   if (tsk_inp (pbase + modemcontrol) != (byte) i)
      return ret_error (sio);

   /* Port seems OK */

   if (create_pipe (&sio->xmit_pipe, xmitbuf, xmitsize
#if (TSK_NAMEPAR)
                , xname
#endif
                ) == NULL)
      return ret_error (sio);

   if (create_wpipe (&sio->rcv_pipe, rcvbuf, rcvsize
#if (TSK_NAMEPAR)
                , rname
#endif
                ) == NULL)
      {
      delete_pipe (&sio->xmit_pipe);
      return ret_error (sio);
      }

   sio->civect = portp->vector;
   sio->irqbit = (byte)(1 << (portp->irq & 0x07));

   sio->wait_xmit = sio->xmit_pending = 0;
   sio->overrun = 0;
   sio->flags = 0;
   sio->modem_flags = 0;
   sio->r_xoff = sio->t_xoff = 0;
   sio->rtsoff = 0;

   sio->clcontrol = sio->save_lcon = tsk_inp (pbase + linecontrol);
   tsk_nop ();
   sio->save_inten = tsk_inp (pbase + intenable);
   tsk_nop ();

   if (init)
      {
      sio->clcontrol = dflt_lcon;
      sio->cmodcontrol = dflt_modcon;
      }

   tsk_outp (pbase + linecontrol, sio->clcontrol | 0x80);
   tsk_nop ();
   sio->save_bd1 = tsk_inp (pbase + baudreg_dll);
   tsk_nop ();
   sio->save_bd2 = tsk_inp (pbase + baudreg_dlm);
   tsk_nop ();
   tsk_outp (pbase + linecontrol, sio->clcontrol);
   tsk_nop ();

   tsk_outp (pbase + intenable, 0);

   if (irq_array [portp->irq] == NULL)
      {
      intptr = (intprocptr far *)MK_FP (0, sio->civect * 4);
      tsk_cli ();
      sio->savvect = *intptr;
      *intptr = irq_procs [portp->irq];
      tsk_sti ();
      }

   if (init)
      {
      tsk_outp (pbase + linecontrol, dflt_lcon | 0x80);
      tsk_nop ();
      tsk_outp (pbase + baudreg_dll, dflt_baud);
      tsk_nop ();
      tsk_outp (pbase + baudreg_dlm, dflt_baud >> 8);
      tsk_nop ();
      tsk_outp (pbase + linecontrol, dflt_lcon);
      tsk_nop ();
      tsk_outp (pbase + modemcontrol, dflt_modcon);
      tsk_nop ();
      }
   else
      {
      i = tsk_inp (pbase + modemcontrol) | OUT2;
      tsk_nop ();
      tsk_outp (pbase + modemcontrol, i);
      tsk_nop ();
      }

   while (tsk_inp (pbase + linestatus) & rxreadybit)
      {
      tsk_nop ();
      tsk_inp (pbase + receivedata);
      tsk_nop ();
      }
   tsk_nop ();

   tsk_inp (pbase + linestatus);
   tsk_nop ();
   sio->modstat = tsk_inp (pbase + modemstatus);
   tsk_nop ();
   tsk_inp (pbase + intid);
   tsk_nop ();

   inta = (portp->irq > 7) ? inta11 : inta01;

   if (irq_array [portp->irq] == NULL)
      {
      if (portp->irq > 7)
         {
         i = tsk_inp (inta01) & ~CHAIN_IRQBIT;
         tsk_nop ();
         tsk_outp (inta01, i);
         }

      sio->save_irq = (byte)((i = tsk_inp (inta)) & sio->irqbit);
      tsk_nop ();
      tsk_outp (inta, i & ~sio->irqbit);
      }
   else
      sio->save_irq = (irq_array [portp->irq])->save_irq;

   tsk_cli ();
   sio->next = irq_array [portp->irq];
   irq_array [portp->irq] = sio;
   tsk_sti ();

   v24_remove_func = v24_remove_all;

   tsk_outp (pbase + intenable, intdata);

	return sio;
   }


void far v24_remove (sioptr sio, int restore)
   {
   intprocptr far *intptr;
   int pbase, i, inta;
   portptr portp;
   sioptr curr, last;

   pbase = sio->port_base;
   portp = sio->port;

   last = NULL;
   curr = irq_array [portp->irq];
   while (curr != sio && curr != NULL)
      {
      last = curr;
      curr = curr->next;
      }
   if (curr == NULL)
      return;

   tsk_outp (pbase + intenable, 0);
   tsk_cli ();
   if (last == NULL)
      irq_array [portp->irq] = sio->next;
   else
      last->next = sio->next;
   tsk_sti ();

   inta = (portp->irq > 7) ? inta11 : inta01;

   if (restore)
      {
      tsk_outp (pbase + modemcontrol, sio->save_mcon);
      tsk_nop ();
      tsk_outp (pbase + linecontrol, sio->save_lcon | 0x80);
      tsk_nop ();
      tsk_outp (pbase + baudreg_dll, sio->save_bd1);
      tsk_nop ();
      tsk_outp (pbase + baudreg_dlm, sio->save_bd2);
      tsk_nop ();
      tsk_outp (pbase + linecontrol, sio->save_lcon);
      tsk_nop ();
      if (irq_array [portp->irq] == NULL)
         {
         tsk_cli ();
         tsk_outp (pbase + intenable, sio->save_inten);
         i = tsk_inp (inta) & ~sio->irqbit;
         tsk_nop ();
         tsk_outp (inta, i | sio->save_irq);
         }
      }
   else if (irq_array [portp->irq] == NULL)
      {
      tsk_cli ();
      i = tsk_inp (inta) | sio->irqbit;
      tsk_nop ();
      tsk_outp (inta, i);
      }

   if (irq_array [portp->irq] == NULL)
      {
      tsk_cli ();
      intptr = (intprocptr far *)MK_FP (0, sio->civect * 4);
      *intptr = sio->savvect;
      }
   tsk_sti ();

   portp->sio = NULL;
   delete_pipe (&sio->xmit_pipe);
   delete_wpipe (&sio->rcv_pipe);

#if (TSK_DYNAMIC)
   if (portp->pnum >= STATIC_PORTS)
      tsk_free (sio);
#endif
   }


void far v24_remove_all (void)
{
   int i;
   sioptr sio;

   for (i = 0; i < MAX_IRQ; i++)
      {
      while ((sio = irq_array [i]) != NULL)
         v24_remove (sio, RESTORE_DEFAULT);
      }
}

/*-------------------------------------------------------------------------*/

/*
   void v24_change_rts (sioptr sio, int on)
*/

void far v24_change_rts (sioptr sio, int on)
{
   sio->cmodcontrol = (sio->cmodcontrol & ~RTS) | ((on) ? RTS : 0);
   tsk_outp (sio->port_base + modemcontrol, sio->cmodcontrol);
}

/*
   void v24_change_dtr (sioptr sio, int on)
*/

void far v24_change_dtr (sioptr sio, int on)
{
   sio->cmodcontrol = (sio->cmodcontrol & ~DTR) | ((on) ? DTR : 0);
   tsk_outp (sio->port_base + modemcontrol, sio->cmodcontrol);
}


/*
   void far v24_change_baud (sioptr sio, int rate)
*/

void far v24_change_baud (sioptr sio, long rate)
{
   int i;

   for (i = 0; baud_table [i]; i += 2)
      if (baud_table [i] == rate)
         break;
   if (!(i = (int)baud_table [i + 1]))
      return;

   tsk_outp (sio->port_base + linecontrol, sio->clcontrol | (byte)0x80);
   tsk_nop ();
   tsk_outp (sio->port_base + baudreg_dll, (byte)i);
   tsk_nop ();
   tsk_outp (sio->port_base + baudreg_dlm, (byte)(i >> 8));
   tsk_nop ();
   tsk_outp (sio->port_base + linecontrol, sio->clcontrol);
}


void far v24_change_parity (sioptr sio, int par)
{
   sio->clcontrol = (sio->clcontrol & 0xc7) | par;
   tsk_outp (sio->port_base + linecontrol, sio->clcontrol);
}


void far v24_change_wordlength (sioptr sio, int len)
{
   int i;

   switch (len)
      {
      case 5:  i = 0x00; break;
      case 6:  i = 0x01; break;
      case 7:  i = 0x02; break;
      case 8:  i = 0x03; break;
      default: return;
      }
   sio->clcontrol = (sio->clcontrol & 0xfc) | i;
   tsk_outp (sio->port_base + linecontrol, sio->clcontrol);
}


void far v24_change_stopbits (sioptr sio, int n)
{
   int i;

   switch (n)
      {
      case 1:  i = 0x00; break;
      case 2:  i = 0x04; break;
      default: return;
      }
   sio->clcontrol = (sio->clcontrol & 0xfb) | i;
   tsk_outp (sio->port_base + linecontrol, sio->clcontrol);
}


void far v24_watch_modem (sioptr sio, byte flags)
{
   sio->modem_flags = flags & (CTS | DSR | RI | CD);
}


void far v24_protocol (sioptr sio, int prot, word offthresh, word onthresh)
{
   byte old;
   
   old = sio->flags;
   sio->flags = (byte)prot;
   if (prot)
      {
      if (!offthresh)
         offthresh = 10;
      sio->xoff_threshold = offthresh;
      if (onthresh <= offthresh)
         onthresh = offthresh + 10;
      sio->xon_threshold = onthresh;
      }

   if ((old & RTSCTS) != ((byte)prot & RTSCTS))
      {
      change_rts (sio, 1);
      sio->modem_flags = (sio->modem_flags & ~CTS) |
                         ((prot & RTSCTS) ? CTS : 0);
      }

   if (!(prot & XONXOFF))
      {
      if (sio->r_xoff)
         sio->r_xoff = -2;
      sio->t_xoff = 0;
      }

   if (!sio->xmit_pending)
      transmit_ready (sio);
}


/*-------------------------------------------------------------------------*/


int far v24_send (sioptr sio, byte ch, dword timeout)
{
   int res;

   if ((res = write_pipe (&sio->xmit_pipe, ch, timeout)) < 0)
      return res;
   tsk_cli ();
   if (!sio->xmit_pending)
      transmit_ready (sio);
   tsk_sti ();
   return 0;
}


int far v24_receive (sioptr sio, dword timeout)
{
   int res;
   
   if ((res = (int)read_wpipe (&sio->rcv_pipe, timeout)) < 0)
      return res;

   if (!sio->flags)
      return res;

   if (wpipe_free (&sio->rcv_pipe) > sio->xon_threshold)
      {
      tsk_cli ();
      if (sio->r_xoff)
         {
         sio->r_xoff = -2;
         if (!sio->xmit_pending)
            transmit_ready (sio);
         }
      tsk_sti ();

      if (sio->rtsoff)
         change_rts (sio, 1);
      }
   return res;
}


int far v24_overrun (sioptr sio)
{
   int res;

   res = sio->overrun;
   sio->overrun = 0;
   return res;
}


int far v24_check (sioptr sio)
{
   return check_wpipe (&sio->rcv_pipe);
}


int far v24_modem_status (sioptr sio)
{
   return sio->modstat;
}


int far v24_complete (sioptr sio)
{
   return (check_pipe (&sio->xmit_pipe) == -1);
}


int far v24_wait_complete (sioptr sio, dword timeout)
{
   return wait_pipe_empty (&sio->xmit_pipe, timeout);
}



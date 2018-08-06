/*

 commsupp.c    - communications support for DeSmet C

 Tom Poindexter- March, 1984

 routines:

  init_com     - initialize comm port, interrupts, baud, data, parity, stop
  uninit_com   - disable interrupts, flushes buffer
  send_brk     - send break signal
  set_xoff     - enable XON, XOFF protocol
  recd_xoff    - return true if XOFF received
  sent_xoff    - return true if XOFF sent
  inp_cnt      - returns the number of unread bytes in receive buffer
  inp_char     - return one byte from buffer
  inp_strn     - transfer n bytes from buffer to string
  outp_char    - send one byte
  outp_strn    - send n bytes of a string, up to end of string
  com_stat     - return line status (high byte) and modem status (low byte)
  on_com       - specify a routine to execute when a byte is received

 internal routines:
  -in commsupp.c-
  port_chk     - validate the port number (C88)

  -in commasm.a-
  int_c_handlr - interrupt driven port read, com1:, store in buffer (ASM88)
  int_b_handlr - interrupt driven port read, com2:, store in buffer (ASM88)
  sysint       - call a system routine (ASM88)
  inb          - input byte from hardware port (ASM88)
  outb         - output byte to hardware port (ASM88)
  cli          - clear interrupts (ASM88)
  sti          - set interrupts   (ASM88)

 standard DeSmet C library functions called:

  _lmove     - block move outside C data
  _move      - block move within C data
  _showcs    - get code segment
  _showds    - get data segment
  malloc     - allocate memory


*/

/* global parm data for two com ports */

struct
 {
  int  port_base;
  char *buff_addr;
  int  buff_len;
  char *buff_head;
  char *buff_tail;
  int  buff_cnt;
  int  x_state;
  int  x_recd;
  int  x_sent;
  int  org_int_off;
  int  org_int_seg;
  int  (*on_com_rtn)();
 }
 _com_parms[2];


/* global defines */
#define FALSE     0
#define TRUE      1
#define ERROR     -1

/* defines for 8250 serial controller */
#define DATAPORT  0
#define INT_REG   1
#define INT_ID    2
#define LCTL_REG  3
#define MDM_REG   4
#define LCTL_STAT 5
#define MDM_STAT  6
#define LSB_BAUD  0
#define MSB_BAUD  1
#define DLAB      0x80
#define DLAB_OFF  0x7f
#define MDM_BRK   0x40
#define PAR_NONE  0x00
#define PAR_EVEN  0x18
#define PAR_ODD   0x08
#define PAR_STICK 0x20
#define DATA_RDY  0x01
#define OUT2_DR   0x0b
#define BREAK_ON  0x40
#define THRE      0x20
/* count to wait for THRE */
#define TIME      0xffff

/* defines for break request */
#define TIME_OF_DAY 0x1a
#define HALF_SEC  9

/* baud rate divisor constants */
#define B110      1047
#define B150      768
#define B300      384
#define B450      256
#define B600      192
#define B1200     96
#define B2400     48
#define B4800     24
#define B9600     12
/* defines to mask baud divisors MSB and LSB */
#define MASK_LSB  0xff00
#define MASK_MSB  0x00ff

/* defines for 8259 interrupt controller */
#define C8259_1   0x21
#define C8259_3   0x20
#define ENBL_IRQ4 0xef
#define DIS_IRQ4  0x10
#define ENBL_IRQ3 0xf7
#define DIS_IRQ3  0x08
#define EOI_IRQ4  0x64
#define EOI_IRQ3  0x63
#define IRQ4_VEC  0x0c
#define IRQ3_VEC  0x0b

/* defines for data areas */
#define SYS_VEC   0x0
#define ROM_DATA  0x40
#define INT_DSEG  2


/****************************************************************************/

/* init_com

      parms:
            port: 1 = com1:, 2 = com2:
            baud: 110, 150, 300, 450, 600, 1200, 2400, 4800, 9600
          parity: 0 = none, 1 = odd, 2 = even, 3 = mark, 4 = space
            data: 5, 6, 7, 8
            stop: 1, 2
        buff_len: length desired for receive buffer ( 0 < buff_len < 32767 )

   returns:
      ERROR = com port not available or invalid parm value
       TRUE = ok

*/

init_com (port, baud, parity, data, stop, buff_len)

int port;
int baud;
int parity;
int data;
int stop;
int buff_len;

{

  int divisor;
  int buff_addr;
  int line_reg;
  int data_seg;
  int code_seg;
  int int_c_handlr();
  int int_b_handlr();
  int int_handlr_addr;

  /*  get C program segment registers */

  data_seg = _showds ();
  code_seg = _showcs ();

  /* validate port; store port address */

  if (port < 1 || port > 2)
    return (ERROR);
  else
   {
    port--;
    _lmove (2,port*2,ROM_DATA,&_com_parms[port].port_base,data_seg);
    if (_com_parms[port].port_base == 0)
      return (ERROR);
   }


  /* validate baud; set baud divisors */

  switch (baud)
   {
    case  110:
      divisor =  B110;
      break;
    case  150:
      divisor =  B150;
      break;
    case  300:
      divisor =  B300;
      break;
    case  450:
      divisor =  B450;
      break;
    case  600:
      divisor =  B600;
      break;
    case 1200:
      divisor = B1200;
      break;
    case 2400:
      divisor = B2400;
      break;
    case 4800:
      divisor = B4800;
      break;
    case 9600:
      divisor = B9600;
      break;
    default:
      return (ERROR);
   }


  /* validate parity; set line control register parity */

  switch (parity)
   {
     case 0:  /* none  */
       line_reg = PAR_NONE;
       break;
     case 1:  /* odd   */
       line_reg = PAR_ODD;
       break;
     case 2:  /* even  */
       line_reg = PAR_EVEN;
       break;
     case 3:  /* mark  */
       line_reg = PAR_ODD | PAR_STICK;
       break;
     case 4:  /* space */
       line_reg = PAR_EVEN | PAR_STICK;
       break;
     default:
       return (ERROR);
    }


  /* validate data len; set line control register data length */

  if (data < 5 || data > 8)
    return (ERROR);
  else
    line_reg |= data - 5;


  /* validate stop bits; set line control register stop bits */

  if (stop < 1 || stop > 2)
    return (ERROR);
  else
    line_reg |=  (stop - 1) << 2;


  /* validate buffer len; allocate memory, set initial settings */

  if (buff_len < 1 || buff_len > 32767)
    return (ERROR);
  else
   {
    _com_parms[port].buff_addr  = malloc (buff_len);
    if (_com_parms[port].buff_addr == 0)
      return (ERROR);
    _com_parms[port].buff_len   = buff_len;
    _com_parms[port].buff_head  = _com_parms[port].buff_addr;
    _com_parms[port].buff_tail  = _com_parms[port].buff_addr;
    _com_parms[port].buff_cnt   = 0;
    _com_parms[port].x_state    = FALSE;
    _com_parms[port].x_recd     = FALSE;
    _com_parms[port].x_sent     = FALSE;
    _com_parms[port].on_com_rtn = FALSE;
   }

  /* set 8250 baud divisor */

  outb (_com_parms[port].port_base + LCTL_REG, DLAB);
  outb (_com_parms[port].port_base + LSB_BAUD, divisor & MASK_MSB);
  outb (_com_parms[port].port_base + MSB_BAUD, (divisor & MASK_LSB) >> 8);


  /* set 8250 line control register */

  outb (_com_parms[port].port_base + LCTL_REG, line_reg);


  /* set IRQx interrupt to point to interrupt handler ;
         enable 8259 IRQx interrupts;
         enable 8250 data ready interrupts;
         enable 8250 OUT2 ,DTR, RTS */

  cli ();

  if (port == 0)
   {
    /* IRQ4, com1: */
    _lmove (2,IRQ4_VEC*4,  SYS_VEC,&_com_parms[port].org_int_off,data_seg);
    _lmove (2,IRQ4_VEC*4+2,SYS_VEC,&_com_parms[port].org_int_seg,data_seg);

    int_handlr_addr = (unsigned) &int_c_handlr;
    _lmove (2,&int_handlr_addr,data_seg,IRQ4_VEC*4,  SYS_VEC);
    _lmove (2,&code_seg,       data_seg,IRQ4_VEC*4+2,SYS_VEC);

    outb (C8259_1, (inb (C8259_1) & ENBL_IRQ4));

   }

  else
   {
    /* IRQ3, com2: */
    _lmove (2,IRQ3_VEC*4,  SYS_VEC,&_com_parms[port].org_int_off,data_seg);
    _lmove (2,IRQ3_VEC*4+2,SYS_VEC,&_com_parms[port].org_int_seg,data_seg);

    int_handlr_addr = (unsigned) &int_b_handlr;
    _lmove (2,&int_handlr_addr,data_seg,IRQ3_VEC*4,  SYS_VEC);
    _lmove (2,&code_seg,       data_seg,IRQ3_VEC*4+2,SYS_VEC);

    outb (C8259_1, (inb (C8259_1) & ENBL_IRQ3));

   }

  /* save C data segment in interrupt handlr routine */
  int_handlr_addr = (unsigned) &int_c_handlr + INT_DSEG;
  _lmove (2,&data_seg,data_seg,int_handlr_addr,code_seg);


  outb (_com_parms[port].port_base + INT_REG, DATA_RDY);

  outb (_com_parms[port].port_base + MDM_REG, OUT2_DR);

  sti ();

  return (TRUE);
  /* end of init_com */

}


/****************************************************************************/

/* port_chk

      parms:
            port: 1 = com1:, 2 = com2:

   returns:
      ERROR = com port never initialized, invalid port number
      TRUE = ok

*/

port_chk(port)

int port;

{
  if (port < 1 || port > 2)
    return (ERROR);

  if (_com_parms[--port].port_base == FALSE)
    return (ERROR);

  return (TRUE);

}


/****************************************************************************/

/* uninit_com

      parms:
            port: 1 = com1:, 2 = com2:
         lv_open: TRUE = leave comm open (DTR true), FALSE = turn off

   returns:
      ERROR = com port never initialized
      TRUE = ok

*/


uninit_com (port,lv_open)

int port;
int lv_open;

{
  int data_seg;
  int code_seg;

  /*  get C program segment registers */

  data_seg = _showds ();
  code_seg = _showcs ();


  /* validate port; reset interrupts ; disable data ready, out2 */

  if (port_chk(port) == ERROR)
    return (ERROR);
  else
    port--;

  cli ();

  /* restore original interrupt vectors, mask 8259 interrupt handling */
  if (port == 0)
   {
    /* IRQ4, com1: */
    _lmove (2,&_com_parms[port].org_int_off,data_seg,IRQ4_VEC*4,  SYS_VEC);
    _lmove (2,&_com_parms[port].org_int_seg,data_seg,IRQ4_VEC*4+2,SYS_VEC);

    outb (C8259_1, inb (C8259_1) | DIS_IRQ4);
   }
  else
   {
    /* IRQ3, com2: */
    _lmove (2,&_com_parms[port].org_int_off,data_seg,IRQ3_VEC*4,  SYS_VEC);
    _lmove (2,&_com_parms[port].org_int_seg,data_seg,IRQ3_VEC*4+2,SYS_VEC);

    outb (C8259_1, inb (C8259_1) | DIS_IRQ3);
   }

  /* disable 8250 interrupt generation */
  outb (_com_parms[port].port_base + LCTL_REG,
         inb (_com_parms[port].port_base + LCTL_REG) & DLAB_OFF);
  outb (_com_parms[port].port_base + INT_REG, FALSE);

  /* leave serial port enabled if requested, else drop DTR */
  if (!lv_open)
    outb (_com_parms[port].port_base + MDM_REG, FALSE);

  sti ();

  /* free buffer storage */
  free (_com_parms[port].buff_addr);
  _com_parms[port].port_base = FALSE;

  return (TRUE);
  /* end of uninit_com */

}


/****************************************************************************/


/* send_brk

      parms:
            port: 1 = com1:, 2 = com2:

   returns:
      ERROR = com port never initialized or bad parm
      TRUE = ok

*/


send_brk (port)

int port;

{

  int line_reg;
  int ticks;
  int count;

  struct
   {int ax,bx,cx,dx,si,di,ds,es;} r;

  r.ax = r.bx = r.cx = r.dx = r.si = r.di = r.ds = r.es = 0;

  /* validate port; send break (1/2 second) */

  if (port_chk(port) == ERROR)
    return (ERROR);
  else
    port--;

  /* get current line reg */
  line_reg = inb (_com_parms[port].port_base + LCTL_REG);

  /* set break */
  outb (_com_parms[port].port_base + LCTL_REG, line_reg | BREAK_ON);

  /* delay for 1/2 second using time-of-day counter */
  /* dx returned is the lower 16 bits of the counter */
  for (count = 0; count < HALF_SEC; count++)
   {
    sysint (TIME_OF_DAY,&r,&r);
    ticks = r.dx;
    while (ticks == r.dx)
      sysint (TIME_OF_DAY,&r,&r);
   }

  /* reset line reg */
  outb (_com_parms[port].port_base + LCTL_REG, line_reg);


  return (TRUE);
  /* end of send_brk */

}


/****************************************************************************/

/* set_xoff

      parms:
            port: 1 = com1:, 2 = com2:
           state: TRUE = XON/XOFF enabled, FALSE = XON/XOFF disabled

   returns:
      ERROR = com port never initialized
      TRUE = ok

*/


set_xoff (port,state)

int port;
int state;

{

  /* validate port */
  if (port_chk(port) == ERROR)
    return (ERROR);
  else
    port--;

  /* set XON/XOFF state */
  _com_parms[port].x_state = state;

  /* reset flags */
  _com_parms[port].x_recd = FALSE;
  _com_parms[port].x_sent = FALSE;

  return (TRUE);
  /* end of set_xoff */

}


/****************************************************************************/

/* recd_xoff

      parms:
            port: 1 = com1:, 2 = com2:

   returns:
      ERROR = com port never initialized
      FALSE = XOFF never received
      TRUE = XOFF received

*/


recd_xoff (port)

int port;

{
  int state;

  /* validate port */
  if (port_chk(port) == ERROR)
    return (ERROR);
  else
    port--;


  state = _com_parms[port].x_recd;

  _com_parms[port].x_recd = FALSE;

  return (state);
  /* end of recd_xoff */

}


/****************************************************************************/

/* sent_xoff

      parms:
            port: 1 = com1:, 2 = com2:

   returns:
      ERROR = com port never initialized
      FALSE = XOFF never sent
      TRUE = XOFF sent

*/


sent_xoff (port)

int port;

{
  int state;

  /* validate port */
  if (port_chk(port) == ERROR)
    return (ERROR);
  else
    port--;


  state = _com_parms[port].x_sent;

  _com_parms[port].x_sent = FALSE;

  return (state);
  /* end of sent_xoff */

}


/****************************************************************************/

/* inp_cnt

      parms:
            port: 1 = com1:, 2 = com2:

   returns:
      ERROR = com port never initialized, bad parm
      other = number of bytes in buffer

*/


inp_cnt (port)

int port;

{

  /* validate port */
  if (port_chk(port) == ERROR)
    return (ERROR);
  else
    port--;


  return (_com_parms[port].buff_cnt);
  /* end of inp_cnt */

}



/****************************************************************************/

/* inp_char

      parms:
            port: 1 = com1:, 2 = com2:

   returns:
      ERROR = com port never initialized, bad parm, nothing to get
      other = next (FIFO) char in buffer

*/


inp_char (port)

int port;

{
  char c;

  /* validate port */
  if (port_chk(port) == ERROR)
    return (ERROR);
  else
    port--;


  if (_com_parms[port].buff_cnt)
   {
    _com_parms[port].buff_cnt--;
    c = *_com_parms[port].buff_tail;

    if ( (unsigned int) ++_com_parms[port].buff_tail >=
         (unsigned int)   _com_parms[port].buff_addr +
         (unsigned int)   _com_parms[port].buff_len   )
      _com_parms[port].buff_tail = _com_parms[port].buff_addr;

    return (c);
   }
  else
   return (ERROR);

  /* end of inp_char */

}



/****************************************************************************/

/* inp_strn

      parms:
            port: 1 = com1:, 2 = com2:
            addr: address of string to put incoming chars
             cnt: number of bytes to transfer (up to actual number in buffer)

   returns:
      ERROR = com port never initialized, bad parm, nothing to get
      other = the actual number of chars transferred

*/


inp_strn (port,addr,cnt)

int port;
char *addr;
int cnt;

{
  int rest_buff;

  /* validate port */
  if (port_chk(port) == ERROR)
    return (ERROR);
  else
    port--;

  /* max transfer = buff count */
  if (cnt > _com_parms[port].buff_cnt)
    cnt = _com_parms[port].buff_cnt;

  if (cnt)
   {
    rest_buff = ( (unsigned int) _com_parms[port].buff_addr +
                  (unsigned int) _com_parms[port].buff_len ) -
                (unsigned int) _com_parms[port].buff_tail;

    if (cnt > rest_buff)
     {
       /* circle around buffer */
       _move(rest_buff,_com_parms[port].buff_tail,addr);
       _move(cnt - rest_buff,_com_parms[port].buff_addr,addr + rest_buff);
       _com_parms[port].buff_tail =
             (unsigned int) _com_parms[port].buff_addr + (cnt - rest_buff);
      }
    else
      {
       _move(cnt,_com_parms[port].buff_tail,addr);
       _com_parms[port].buff_tail += cnt;
       if (cnt == rest_buff)
         _com_parms[port].buff_tail = _com_parms[port].buff_addr;
      }

    _com_parms[port].buff_cnt -= cnt;
    return (cnt);

   }

  else
   return (ERROR);

  /* end of inp_strn */

}



/****************************************************************************/

/* outp_char

      parms:
            port: 1 = com1:, 2 = com2:
               c: the character to send

   returns:
      TRUE = OK
      ERROR = timeout condition


*/


outp_char (port,c)

int  port;
char c;

{

  unsigned int time_out;

  /* validate port; return state */
  if (port_chk(port) == ERROR)
    return (ERROR);
  else
    port--;

  /* wait for THRE (transmitter holding register empty) */

  for (time_out = TIME;
       (inb (_com_parms[port].port_base + LCTL_STAT) & THRE) == FALSE
          && time_out > 0;
       time_out--)
    ;

  if (time_out == 0)
    /* THRE timeout error */
    return (ERROR);


  /* everything ok, send it */

  outb (_com_parms[port].port_base + DATAPORT, c);

  return (TRUE);
  /* end of outp_char */

}


/****************************************************************************/

/* outp_strn

      parms:
            port: 1 = com1:, 2 = com2:
            addr: the string to send
             cnt: the maximum number of characters to send

   returns:
      TRUE = OK
      ERROR = timeout condition


*/


outp_strn (port,addr,cnt)

int  port;
char addr[];
int cnt;

{
  int n;

  for (n = 0; n <= cnt && addr[n] != '\0'; n++)
    if (outp_char(port,addr[n]) == ERROR)
      return (ERROR);


  return (TRUE);
  /* end of outp_strn */

}


/****************************************************************************/

/* com_stat

      parms:
            port: 1 = com1:, 2 = com2:

   returns:
      line status (MSB) and modem status (LSB)
      ERROR = invalid com port, never initialized


*/


com_stat (port)

int  port;

{
  int status;

  /* validate port */
  if (port_chk(port) == ERROR)
    return (ERROR);
  else
    port--;

  status = inb (_com_parms[port].port_base + LCTL_STAT) << 8;

  status |= inb (_com_parms[port].port_base + MDM_STAT);

  return (status);
  /* end of com_stat */

}


/****************************************************************************/

/* on_com

      parms:
            port: 1 = com1:, 2 = com2:
             rtn: pointer to a C routine

   returns:
      TRUE = ok
      ERROR = invalid com port, never initialized


*/


on_com (port,rtn)

int  port;
int  (*rtn)();

{

  /* validate port */
  if (port_chk(port) == ERROR)
    return (ERROR);
  else
    port--;

  _com_parms[port].on_com_rtn = rtn;

  return (TRUE);
  /* end of on_com */

}


/****************************************************************************/


/* end of commsupp.c */

                                                                                                                                                                                                  
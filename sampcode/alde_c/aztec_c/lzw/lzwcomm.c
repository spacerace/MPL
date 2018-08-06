/* COMMLZW - ROUTINES COMMON TO LZWCOM AND LZWUNC                       */
#include "stdio.h"
#include "debug.h"
#define FALSE    0
#define TRUE     !FALSE
#define TABSIZE  4096
#define NO_PRED  0xFFFF
#define EMPTY    0xFFFF
#define NOT_FND  0xFFFF
#define SECTSIZE 128
#define CTRLZ '\032' /* ascii end of file */
extern struct entry {
  char used;
  unsigned int next;                    /* index of next in collision list */
  unsigned int predecessor;             /* 12 bit code                  */
  unsigned char follower;
} string_tab[TABSIZE];

/* h uses the 'mid-square' algorithm. I.E. for a hash val of n bits     */
/* hash = middle binary digits of (key * key).  Upon collision, hash    */
/* searches down linked list of keys that hashed to that key already.   */
/* It will NOT notice if the table is full. This must be handled        */
/* elsewhere                                                            */

unsigned h(pred,foll)
/* returns the mid square of pred + foll                                */
	unsigned int pred; unsigned char foll;
{
  long temp, local;             /* 32 bit receiving field for local^2   */
  local = (pred + foll) | 0x0800;
  temp = local * local;
  local = (temp >> 6) & 0x0FFF;
  return local;                 /* middle 12 bits of result             */
}

unsigned eolist(index)
/* return last element in a collision list                              */
unsigned int index;
{
  register int temp;
  while ( 0 != (temp = string_tab[index].next) )
    index = temp;
  return index;
}

unsigned hash(pred,foll,update)
unsigned int pred; unsigned char foll; int update;
{
  register unsigned int local, tempnext;
  register struct entry *ep;
  local = h(pred,foll);
  if ( !string_tab[local].used )
    return local;
  else {
  /* if collision has occured                                           */
    local = eolist(local);

  /* search for free entry from local + 101 */
    tempnext = (local + 101) & 0x0FFF; 
    ep = &string_tab[tempnext];                 /* initialize pointer   */
    while ( ep->used ) {
      ++tempnext;
      if ( tempnext == TABSIZE ) {
        tempnext = 0;           /* handle wrap to beginning of table    */
        ep = string_tab;        /* address of first element of table    */
      } else
        ++ep;                   /* point to next element in table       */
    }
    
    /* put new tempnext into last element in collision list             */ 
    if ( update )               /* if update requested                  */
      string_tab[local].next = tempnext;
    return tempnext;
  } 
}

/* unhash uses the 'next' field to go down the collision tree to find   */
/* the entry corresponding to the passed key                            */
/* passed key and returns either the matching entry # or NOT_FND        */

unsigned unhash(pred,foll)
unsigned int pred; unsigned char foll;
{
  register unsigned int local, offset;
  register struct entry *ep;    /* pointer to current entry             */

  local = h(pred,foll);         /* initial hash                         */
  loop:
    ep = &string_tab[local];
    if ( (ep->predecessor == pred) && (ep->follower == foll) ) 
      return local;
    if ( ep->next == 0 )
      return NOT_FND;
    local = ep->next;
  goto loop;
}

init_tab() {
  register unsigned int i;
  setmem((char *)string_tab,sizeof(string_tab),0);
  for (i = 0; i <= 255; i++) {
    upd_tab(NO_PRED,i);
  }
}

#define UPDATE TRUE

upd_tab(pred,foll)
  unsigned int pred, foll;
{
  register struct entry *ep;    /* pointer to current entry     */
  /* calculate offset just once */
  ep = &string_tab[ hash(pred,foll,UPDATE) ];
  ep->used = TRUE;
  ep->next = 0;
  ep->predecessor = pred;
  ep->follower = foll;
}


unsigned int inbuf = EMPTY;
unsigned int outbuf = EMPTY;

/* getcode and putcode 'gallop' through input and output - they either  */
/* output two bytes or one depending on the contents of the buffer.     */

getcode(fd) 
FILE *fd;     {
  register unsigned int localbuf, returnval;
  if (EMPTY == inbuf) {         /* On code boundary                     */
    if (EOF == (localbuf = getch(fd)) ) {       
                                /* H L1 byte - on code boundary         */
      return EOF;
    }
    localbuf &= 0xFF;
	if (EOF == (inbuf = getch(fd)) ) {  /* L0 Hnext                     */
      return EOF;       /* The file should always end on code boundary  */
    }
    inbuf &= 0xFF;
	DEBUGGER(fprintf(stderr,"%x and %x read\n",localbuf,inbuf);)
    returnval = ((localbuf << 4) & 0xFF0) + ((inbuf >> 4) & 0x00F);
    DEBUGGER(fprintf(stderr,"returnval = %x\n",returnval);)
    inbuf &= 0x000F;
  } else {                      /* buffer contains nibble H             */
    if (EOF == (localbuf = getch(fd)) )
      return EOF;
    localbuf &= 0xFF;
    DEBUGGER(fprintf(stderr,"%x read, inbuf = %x\n",localbuf,inbuf);)
    returnval = localbuf + ((inbuf << 8) & 0xF00);
    inbuf = EMPTY;
    DEBUGGER(fprintf(stderr,"returnval = %x\n",returnval);)
  }
  return returnval;
}

putcode(fd,code)
FILE *fd;
unsigned int code;     {
  register int localbuf;
  if (EMPTY == outbuf) {
    putch( ((code >> 4) & 0xFF),fd);    /* H L1                        */
    outbuf = code & 0x000F;     /* L0                                   */
  } else {
    putch( ( ((outbuf << 4) & 0xFF0) + ((code >> 8) & 0x00F) ),fd );
                                /* L0prev H                             */
    putch( (code & 0x00FF),fd);        /* L1 L0                        */
    outbuf = EMPTY;
  }
}
static int incount = 0, outcount = 0;
static int ink = 0, outk = 0;
putch(c,fp)
	char c;
	FILE *fp;
{
	if (outcount == 0)
	{
		reset_crc();
	}
	update_crc(c);
	putc(c,fp);
	outcount++;
	if (outcount == 1024)
	{
		fprintf(stderr,"%dk written\r",++outk);
		outcount = 0;
		putw(get_crc(),fp);
	}
}

getch(fp)
	FILE *fp;
{
	extern FILE *op;
	register int returnc;
	unsigned int crc;
	if (incount == 0)
	{
		reset_crc();
	}
	if (EOF == (returnc = getc(fp)))
		return EOF;
	update_crc(returnc);
	++incount;
	if (incount == 1024)
	{
		if (!isatty(fileno(op)))
		{
			fprintf(stderr,"%dk read\r",++ink);
		}
		incount = 0;
		if (EOF == (crc = getw(fp)))
			return EOF;
		update_crc((crc & 0xFF));	/* lo byte */
		update_crc(((crc >> 8) & 0xFF));	/* hi byte */
		if (get_crc())
		{
			fprintf(stderr,"crc error - aborting");
			exit(1);
		}
	}
	return returnc;
}

flushout(fd)
FILE *fd;
{
	if ( EMPTY != outbuf)
		putc(((outbuf << 4) & 0xFF0),fd);
}

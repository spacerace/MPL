/*
** Peek/Poke for Microsoft C
**
** MSC /Ze peek
**
** Copyright (c) 1986 - Philip A. Mongelluzzo
** All rights reserved.
**
** Placed in public domain 5/8/86 by P.A. Mongelluzzo
**
*/

unsigned peek(ioff,iseg)
unsigned iseg,ioff;
{
int far *adr;
unsigned long seg,off;
  seg=iseg;
  off=ioff;
  adr = (seg << 16) | off;
  return (unsigned)(*adr);
}

void poke(ioff,iseg,val)
unsigned iseg,ioff;
unsigned char val;
{
int far *adr;
unsigned long seg,off;
  seg=iseg;
  off=ioff;
  adr = (seg << 16) | off;
  *adr = val;
}

/* End */

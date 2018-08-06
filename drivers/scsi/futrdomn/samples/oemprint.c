/*********************** OEMPRINT.C **********************/

  #include <string.h>
  #include <dos.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <fcntl.h>
  #include <io.h>


  struct oemtable
    {
    unsigned char (far *cmdblock)[];
    unsigned char (far *datablock)[];
    long expectlen;
    long actuallen;
    int timeout;
    int blkfactora;
    int blkfactorb;
    unsigned char scsistatus;
    unsigned char scsimsg;
    unsigned char scsibits;
    unsigned char scsiaddress;
    unsigned char scsiparity;
    int adaptererr;
    } ;

oemprint(oemtbl)
  struct oemtable *oemtbl ;
  {
  int i,ierr;
  int j,k;
  long sect,x;
  int unit;
  unsigned char cc,ic;
  struct SREGS segregs ;
  union
    {
    unsigned char (far *point)[];
    long longval;
    int intval[2];
    unsigned char ch [4];
    } cnvtr ;
  segread(&segregs) ;
  for(i=0;i<25;i++)printf("\n");  /* clear screen */
  printf("\nCurrent segments  CS=%04X  DS=%04X  ES=%04X  SS=%04X\n",
    segregs.cs,segregs.ds,segregs.es,segregs.ss);
  cnvtr.point=oemtbl->cmdblock;
  printf("Command Buffer @  %04X:%04X  ",cnvtr.intval[1],cnvtr.intval[0]);
  for(i=0;i<10;i++)printf(" %02X",(*cnvtr.point)[i]&255);
  printf("\n");
  cnvtr.point=oemtbl->datablock;
  printf("Data Buffer @  %04X:%04X\n",cnvtr.intval[1],cnvtr.intval[0]);
  printf("Expected length = %lu\n",oemtbl->expectlen);
  printf("Actual length = %lu\n",oemtbl->actuallen);
  printf("Timeout factor is %u seconds.\n",oemtbl->timeout);
  printf("Blocking factors A and B are %u  &  %u\n",oemtbl->blkfactora,
    oemtbl->blkfactorb);
  printf("SCSI Status byte is %02X\n",oemtbl->scsistatus&255);
  printf("SCSI Message byte is %02X\n",oemtbl->scsimsg&255);
  ic=oemtbl->scsibits;
  printf("Adapter status byte is %02X\n",ic&255);
  if(ic!=0)
    {
    if((ic&0x1)!=0)
      printf("  SCSI Busy status asserted\n");
    if((ic&0x2)!=0)
      printf("  SCSI Message status asserted\n");
    if((ic&0x4)!=0)
      printf("  SCSI I/O status asserted\n");
    if((ic&0x8)!=0)
      printf("  SCSI C/D status asserted\n");
    if((ic&0x10)!=0)
      printf("  SCSI Request status asserted\n");
    if((ic&0x20)!=0)
      printf("  SCSI Select status asserted\n");
    if((ic&0x40)!=0)
      printf("  Adapter Parity Error status asserted\n");
    }
  printf("SCSI Address is %u\n",oemtbl->scsiaddress&255);
  if(oemtbl->scsiparity==0)printf("Adapter parity checking disabled.\n");
  else printf("Adapter parity checking enabled.\n");
  i=oemtbl->adaptererr;
  printf("Request error status is %d\n",i);
  switch (i)
    {
    case -1:
      printf("  Timeout waiting for bus free state.\n");
      break;
    case -2:
      printf("  Timeout during selection phase.\n");
      break;
    case -3:
      printf("  Timeout during command phase.\n");
      break;
    case -4:
      printf("  Timeout during data phase.\n");
      break;
    case -5:
      printf("  Timeout during status phase.\n");
      break;
    case -6:
      printf("  Timeout during message phase.\n");
      break;
    case -7:
      printf("  Parity error detected by adapter.\n");
      break;
    case -16:
      printf("  Unable to locate the adapter in the system.\n");
      break;
    case 2:
      printf("  Buffer underflow.  Data phase shorter than expected length.\n");
      break;
    case 1:
      printf("  Buffer overflow.  Data phase longer than expected length.\n");
      break;
    }
  printf("\n");
  return(0);
  }

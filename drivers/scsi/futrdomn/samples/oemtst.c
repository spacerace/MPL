/*********************** OEMTST.C **********************/

  #include <string.h>
  #include <dos.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <fcntl.h>
  #include <io.h>

  #define esc 0x1B

  unsigned char buf [4096];
  unsigned char p [32];

  struct SREGS segregs ;
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

  struct oemtable oem ;
  unsigned char cmds [12];    /* build command block */

main ()
  {
  int i,ierr;
  int j,k;
  long sect,x;
  int unit;
  unsigned char cc;
  union
    {
    unsigned char (far *point)[];
    unsigned char *ptnear;
    long longval;
    int intval[2];
    unsigned char ch [4];
    } cnvtr ;
  segread(&segregs) ;
  cnvtr.intval[1]=segregs.ds;
  cnvtr.ptnear=&cmds[0];
  oem.cmdblock=cnvtr.point;
  cnvtr.ptnear=&buf[0];
  oem.datablock=cnvtr.point;
  oem.expectlen=512l;
  oem.timeout=5;
  oem.blkfactora=512;
  oem.blkfactorb=512;
  oem.scsiaddress=0;
  oem.scsiparity=0;
  cmds[0]=8;
  cmds[1]=0;
  cmds[5]=0;
  do
    {
    sect = 0l;
    printf("\nSector number - ");
    scanf("%lX",&sect);
    printf("\nSector entered - %lX\n",sect);
    cnvtr.longval=sect;
    cmds[1]=cnvtr.ch[2];
    cmds[2]=cnvtr.ch[1];
    cmds[3]=cnvtr.ch[0];
    cmds[4]=1;	       /* read a single sector */
    for(i=0;i<sizeof buf;i++)buf[i]=-1;
    ierr=oemscsi(&oem);
    if(ierr<0)oemreset();
    if(ierr!=0)oemprint(&oem);
    else
      {
      for (i=0;i<512;i+=16)
	{
	printf ("%04X  ",i);
	k=0 ;
	for(j=i;j<i+16;j++)
	  {
	  cc=buf[j]&255;
	  printf("%02X ",cc);
	  if ((cc<0x20)||(cc>0x7e)) cc=0x20;
	  p[k]=cc;
	  k++ ;
	  }
	p[k]=0;
	printf("%s\n",p);
	}
      }
    printf("\nHit any key to continue, ESC to Exit.");
    if(getch()==esc)exit(0);
    } while (1);
  exit (0);
  }

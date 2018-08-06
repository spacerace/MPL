/**************************** TEST.C *************************/

  #include <string.h>
  #include <dos.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <fcntl.h>
  #include <io.h>

  #define esc 0x1b
  unsigned char buf [512];
  unsigned char p [32];

main ()
  {
  int i,ierr;
  int j,k;
  long sect;
  unsigned char cc;
  drive_ck(0);
  do
    {
    printf("\nSector number - ");
    scanf("%lX",&sect);
    ierr=read13(buf,(long)sect,1,0);
    if(ierr==0)
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
    printf("\nHit any key to continue, ESC to exit.");
    if(getch()==esc)exit(0);
    } while (1);
  exit (0);
  }

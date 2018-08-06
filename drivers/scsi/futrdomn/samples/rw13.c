/**************************** RW13.C ********************************/

  #include <string.h>
  #include <dos.h>
  #include <stdio.h>

  /* Structure for the disk definition table */
  /* This format matchs the table in the ROM BIOS */

  struct
    {
    unsigned char bits	     ;
    int cylinders	     ;	 /* number of cylinders on the disk */
    unsigned char heads      ;	 /* number of heads */
    unsigned char sectors    ;	 /* sectors per track */
    unsigned char select     ;	 /* 0-2 = LUN, 3-5 = CONTROLLER */
    unsigned char ready      ;	 /* slot for drive ready signal */
    unsigned char save_sense[4]; /* request sense data */
    unsigned char err_cmd[10];	 /* erroring command for sense data */
    unsigned long capacity   ;	 /* drive capacity */
    unsigned char drivename[24]; /* drive name from inquiry command */
    unsigned char unitno     ;
    } romtbls [8] ;

  unsigned int physmax	 ;	  /* maximum allowed unit number */
  unsigned int physcur	 ;	  /* current selected unit */
  unsigned int physbase  ;	  /* base address for drives, ie 80H */
  unsigned char scratchbuf [512] ;  /* scratch buffer */
  #define esc 0x1b		  /* escape character */

  union REGS regs ;
  struct SREGS segregs;

  char dosmsg [16] [25] =
   {
     {"Write Protect Error"},   /* 00 */
     {"Unknown Unit"},          /* 01 */
     {"Device Not Ready"},      /* 02 */
     {"Unknown Command"},       /* 03 */
     {"CRC Error"},             /* 04 */
     {"Bad Req Struct Len"},    /* 05 */
     {"Seek Error"},            /* 06 */
     {"Unknown Media"},         /* 07 */
     {"Sector Not Found"},      /* 08 */
     {"Printer out of Paper"},  /* 09 */
     {"Write Fault"},           /* 0A */
     {"Read Fault"},            /* 0B */
     {"General Failure"},       /* 0C */
     {"Reserved"},              /* 0D */
     {"Reserved"},              /* 0E */
     {"Media Changed"},         /* 0F */
   }	;

/* RESPONSE DEFINITIONS */
  char resp13 [] = "Abort, Retry, Ignore?" ;

/* int wrt13 (buffer,sector,length,unit)				     */
/* int read13 (buffer,sector,length,unit)				     */
/*     buffer - pointer to buffer for input/output of data		     */
/*     sector - absolute disk sector number for input/output		     */
/*     length - length of transfer in sectors				     */
/*     unit   - disk unit number, ie 0, 1, ...				     */
/*									     */
/* These procedures read/write disk using the int13 call to the ROM BIOS.    */
/*   If there is an error of any kind, the user is asked for a prompt, ie    */
/*   Abort, Retry, Ignore.  The retry is done internally, the abort returns  */
/*   a value of 2 to the caller, and the ignore is completed the same as     */
/*   a successful I/O request with a 0. 				     */
/*									     */

read13 (buf,sect,len,unit) /* do int 13 call */
  int buf, len, unit ;
  unsigned long sect ;
  {
  int i,j;
  do
    {
    if(do_13_parms(buf,sect,len,unit)!=0)return(2);  /* setup int13 parms */
    regs.h.ah=2 ;  /* read request code */
    int86x (0x13,&regs,&regs,&segregs) ;
    i=err_13 (unit,0) ;    /* get error response */
    } while (i==1) ;  /* retry check */
  return (i)   ;      /* back to caller */
  }

wrt13 (buf,sect,len,unit) /* do int 13 call */
  int buf, len, unit ;
  unsigned long sect ;
  {
  int i,j;
  do
    {
    if(do_13_parms(buf,sect,len,unit)!=0)return(2);  /* setup int13 parms */
    regs.h.ah=3 ;  /* read request code */
    int86x (0x13,&regs,&regs,&segregs) ;
    i=err_13 (unit,1) ;    /* get error response */
    } while (i==1) ;  /* retry check */
  return (i)   ;      /* back to caller */
  }

rw13reset(unit) 	 /* reset the hard disk system */
  {
  int i;
  regs.h.ah=0;		 /* reset command */
  regs.h.dl=romtbls[unit].unitno;
  int86x (0x13,&regs,&regs,&segregs) ;
  i=regs.h.ah&255 ;	 /* get error response */
  return(i);
  }

do_13_parms (buf,sect,len,unit)  /* setup the int13 parameters */
  unsigned int buf, len, unit ;
  unsigned long sect ;
  {
  unsigned int i, j ;
  unsigned int dskheads, dsksects ;
  union
    {
    unsigned char ch[4] ;
    unsigned int ii[2] ;
    unsigned long longval ;
    }  temp   ;
  segread(&segregs) ;
  segregs.es=segregs.ds ;    /* setup segments for int13 call */
  regs.h.al=len ;	   /* number of sectors */
  temp.longval=0l;
  dskheads=romtbls[unit].heads&255;
  dsksects=romtbls[unit].sectors&255;
  i=temp.ii[0]=dskheads*dsksects ;
  if(i==0)return(2);	     /* no valid disk unit */
  temp.longval=sect/temp.longval ;
  regs.h.ch=temp.ch[0] ; /* lsb of cylinder number */
  regs.h.cl=temp.ch[1]<<6 ; /* msb of cylinder number */
  temp.longval=0l;
  temp.ii[0]=i;
  temp.longval=sect%temp.longval ; /* get remainder */
  j=temp.ii[0];
  temp.ii[0]=j/dsksects; /* head number */
  regs.h.dl=romtbls[unit].unitno;
  regs.h.dh=temp.ch[0] ;
  temp.ii[0]=j%dsksects+1;
  regs.h.cl|=temp.ch[0] ;  /* put in sector number */
  regs.x.bx=buf ;
  return (0);
  }

doformat (unit,interleave)   /* issue the format command to the drive */
  int unit,interleave ;
  {
  int i ;
  do
    {
    regs.x.ax=0x700+interleave;
    regs.h.dl=romtbls[unit].unitno ; /* set up the unit number */
    int86x (0x13,&regs,&regs,&segregs) ;
    i=err_13(unit,2) ; /* get error message */
    } while (i==1) ;
  return (i) ;
  }

domodesel (buf,unit)   /* issue the mode select command to drive */
  int unit ;
  {
  int i ;
  do
    {
    regs.x.ax=0x500 ;
    regs.h.dl=romtbls[unit].unitno ; /* set up the unit number */
    segread(&segregs) ;
    segregs.es=segregs.ds ;    /* setup segments for int13 call */
    regs.x.bx=buf ;
    int86x (0x13,&regs,&regs,&segregs) ;
    i=err_13(unit,4) ; /* get error message */
    } while (i==1) ;
  return (i) ;
  }

dofmtcyl (buf,unit,interleave,cd) /* issue the format command to drive */
  int unit ;
  int buf;
  int interleave;
  char cd ;	     /* code for format type */
  {
  int i ;
  do
    {
    regs.x.ax=0x600+interleave ;
    regs.h.dl=romtbls[unit].unitno ; /* set up the unit number */
    regs.h.dh=cd;
    segread(&segregs) ;
    segregs.es=segregs.ds ;    /* setup segments for int13 call */
    regs.x.bx=buf ;
    int86x (0x13,&regs,&regs,&segregs) ;
    i=err_13(unit,3) ; /* get error message */
    } while (i==1) ;
  return (i) ;
  }

long rdcaptot (unit)   /* get total capacity read again with mode info */
  int unit ;
  {
  int i ;
  union
    {
    unsigned int ii[2] ;
    unsigned long longval ;
    }  temp   ;
  do
    {
    regs.x.ax=0x1900 ;
    regs.h.dl=romtbls[unit].unitno ; /* set up the unit number */
    int86x (0x13,&regs,&regs,&segregs) ;
    if(regs.h.al!=0)i=err_13(unit,5) ;	  /* get error response */
    else i=0;
    temp.ii[0]=regs.x.dx ;
    temp.ii[1]=regs.x.cx ;
    } while (i==1) ;
  if(i!=0)temp.longval=0l; /* on error, return length of 0 */
  return (temp.longval) ;
  }

err_13 (unit,rwcode)  /* get operator response for int 13 request */
		      /* return 0=no error
			 1=repeat
			 2=abort */
  int unit,rwcode ;
  {
  int row,col ;
  int i,k,j;
  char msgline [70] ; /* build message line */
  int ierr ;
/******************************************************************************
*THE ROM BIOS ERROR TYPE CODES ARE CONVERTED TO DOS ERROR CODES.  THE CONVER-
*  SION IS AS FOLLOWS:
*		03 - WRITE PROTECT (00)
*		20 - UNKNOWN UNIT (01)
*		FF - NOT READY (02)
*		80 - NOT READY (02)
*		01 - UNKNOWN COMMAND (03)
*		10 - CRC ERROR (04)
*		BB - BAD DRIVE REQUEST (05)
*		40 - SEEK ERROR (06)
*		04 - SECTOR NOT FOUND (08)
*		0B - WRITE FAULT (0A)
*		02 - READ FAULT (0B)
*		07 - GENERAL FAILURE (0C)
*		11 - DATA CORRECTED NOT REPORTED AS AN ERROR
*		06 - MEDIA CHANGE (0F)
******************************************************************************/
  switch (regs.h.ah) /* decode ROM error to DOS error */
    {
    case 0x03:	   /* write protect */
      ierr = 0 ;
      break ;
    case 0x20:	   /* unknown unit */
      ierr = 1 ;
      break ;
    case 0xFF:	   /* not ready */
      ierr = 2 ;
      break ;
    case 0x80:	   /* not ready */
      ierr = 2 ;
      break ;
    case 0x01:	   /* unknown command */
      ierr = 3 ;
      break ;
    case 0x10:	   /* CRC Error */
      ierr = 4 ;
      break ;
    case 0xBB:	   /* bad drive request */
      ierr = 5 ;
      break ;
    case 0x40:	   /* seek error */
      ierr = 6 ;
      break ;
    case 0x04:	   /* sector not found */
      ierr = 8 ;
      break ;
    case 0x0B:	   /* write fault */
      ierr = 10;
      break ;
    case 0x02:	   /* read fault */
      ierr = 11;
      break ;
    case 0x07:	   /* general failure */
      ierr = 12;
      break ;
    case 0x06:	   /* media change error */
      ierr = 15;
      break;
    default  :
      ierr = -1 ;
    }
  if (ierr==-1) return (0) ;   /* no error */
  strcpy (msgline,dosmsg[ierr]) ;
  if (rwcode==0) strcat (msgline," Reading") ;
  if (rwcode==1) strcat (msgline," Writing" ) ;
  if (rwcode==2) strcat (msgline," Formatting") ;
  if (rwcode==3) strcat (msgline," Formatting") ;
  if (rwcode==4) strcat (msgline," Mode Select") ;
  if (rwcode==5) strcat (msgline," Drive Capacity") ;
  strcat (msgline," Drive ") ;
  col=strlen(msgline) ;
  msgline[col]=(unit&255)+'1';
  msgline[col+1]=0;
  row=20;
  col=39-col/2 ;
  printf("%s\n",msgline) ;
  j=-1 ;
  do
    {
    printf ("%s",resp13);
    i = bdos (8,0,0) & 255  ;  /* input character from keyboard */
    if(i==0)bdos(8,0,0);
    msgline[0]=i ;
    msgline[1]=8 ;
    msgline[2]=0;
    if(msgline[0]>0x1f)printf("%s\n",msgline) ; /* response character */
    if(i==esc) j=2 ;
    i|=0x20 ; /* eliminate case */
    if (i==0x61) j=2 ; /* return 2 on abort */
    if (i==0x69) j=0 ; /* return 0 on ignore */
    if (i==0x72) j=1 ; /* return 1 on retry */
    } while (j==-1) ;
  return (j) ;
  }

drive_ck ()	   /* check for presence of SCSI drives in the system */
  {
  union
    {
    unsigned char (far *pointc) [] ;
    int (far *point) [] ;
    int ivalue[2]	;
    long value		;
    } low_mem;
  union
    {
    int intval [2] ;
    unsigned char charval [4] ;
    long longval ;
    } cnvtr ;
  int num,i,ok;
  char icx ;
  if ((bdos(0x30,0,0)&255)<3)
    {
    printf ("\nInvalid DOS Version.\n") ;
    printf ("SCSI disk System Requires DOS 3.x or higher\n");
    exit (0) ;
    }
  segread(&segregs) ;	  /* setup segment registers */
  segregs.es=segregs.ds ;
  low_mem.value=0l     ;  /* initialize pointer */
  icx=(*low_mem.pointc)[0x475] ; /* base drive number */
  ok=0;
  do
    {
    regs.h.dl=icx+0x80; /* base drive number */
    regs.h.dh=0 ;
    regs.x.ax=0x1800	;  /* get drive id and number of them */
    int86x (0x13,&regs,&regs,&segregs) ;
    if((regs.x.ax==0x4321)&&(regs.h.cl==10))ok=1;
    icx--;
    } while ((icx>=0)&&(ok==0));
  if (ok==0)
    {
    printf ("\nNo SCSI host adapter in the system!\n");
    exit (0) ;
    }
  if (regs.h.bh==0)   /* check for any units reported */
    {
    printf ("\nNo SCSI drives reported in system!\n");
    exit (0) ;
    }
  cnvtr.charval[0]=regs.h.bh ;
  cnvtr.charval[1]=0 ;
  physmax=cnvtr.intval[0] ;
  cnvtr.charval[0]=regs.h.dl ;
  cnvtr.charval[0]=cnvtr.charval[0]-regs.h.bl ;
  physbase=cnvtr.intval[0];
  for(i=0;i<physmax;i++)drivetbl(physbase+i,i); /* move all tables to local */
  return (0) ;
  }

drivetbl (romunit,physunit) /* update the internal structure with drive table */
			 /* romunit is the desired unit 80-8n */
			 /* physunit is the internal physical unit number */
			 /* the table is moved into the correct slot in
			    romtbls.					  */
  int romunit, physunit ;
  {
  int i ;
  union
    {
    unsigned char (far *pointc) [] ;
    int (far *point) [] ;
    long value		;
    int ivalue [2]	;
    } rom_ptr ;
  regs.x.dx=romunit ;  /* unit selection */
  regs.x.ax=0x1B00    ;        /* get rom table pointer */
  int86x (0x13,&regs,&regs,&segregs) ;
  rom_ptr.ivalue[0]=regs.x.bx ;
  rom_ptr.ivalue[1]=segregs.es ;
  for (i=0;i<sizeof romtbls;i++) scratchbuf[i]=(*rom_ptr.pointc)[i];
  mvstruct (&romtbls[physunit],&scratchbuf[0],sizeof romtbls);
  romtbls[physunit].unitno=romunit ;  /* assign the unit number */
  return (0) ;
  }

mvstruct (ots, ins, size)   /* move whole structure */
  unsigned char (*ots)[], (*ins)[];
  int size ;
  {
  int i;
  for (i=0; i<size; i++) (*ots)[i]=(*ins)[i] ;
  return (0);
  }

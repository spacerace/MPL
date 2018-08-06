/**************************  PROGRAM OEMLIST.C  **************************/
/*    AUTHOR               KJC: APPLICATIONS ENGINEERING                 */
/*    DATE LAST REVISED    08/16/90                                      */
/*    VERSION              1.01                                          */
/*    FUNCTION             TO PROVIDE A PROGRAMING EXAMPLE USING THE     */
/*                         THE OEMSCSI INTERFACE.                        */
/*    DESCRIPTION          THIS EXAMPLE USES THE TEST UNIT READY AND THE */
/*			   INQUIRY COMMAND TO OBTAIN AND DISPLAY DATA    */
/*                         FOR EACH SCSI ADDRESS ON EACH HOST ADAPTER    */
/*                         FOUND.					 */
/*************************************************************************/

/**********************
 *  REQUIRED INCLUDES *
 **********************/

  #include <string.h>
  #include <dos.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <conio.h>

/*******************************
 *  REQUIRED MACRO DEFINITIONS *
 *******************************/

  #define MAKE_FP(seg,ofs)   ((void far *) \
			     (((unsigned long)(seg) << 16) | (unsigned)(ofs)))

/*********************************
 * REQUIRED FUNCTION DEFINITIONS *
 *********************************/

  void display_screen(void);
  void display_device_data(void);
  int check_host_adapter(char adapter_id);
  int  test_unit_ready(int adapter_id,int scsi_id);
  int inquiry_command(int adapter_id,int scsi_id);

/*******************************
 * REQUIRED GLOBAL DEFINITIONS *
 *******************************/

  unsigned char buf[512];		/* Buffer Space               */
  unsigned char cmds[12];    		/* Command Block Space        */
  int	i,				/* Counters & varibles 	      */
	i2,
	ha,
	ierr;
  char	id_string[23];			/* Space for Device ID string */
  struct SREGS segregs ;

/******************************
 * DEFINE OEM TABLE STRUCTURE *
 ******************************/

  struct oem_cmd_table
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
    } oem;

/*****************************
 * MAIN PROGRAM STARTS HERE  *
 *****************************/

main()
{

/*  SET UP OEM TABLE   */

   segread(&segregs) ;
   oem.cmdblock = MAKE_FP(segregs.ds,&cmds[0]);  /* Set pointer to CMDS */
   oem.datablock = MAKE_FP(segregs.ds,&buf[0]);  /* Set pointer to DATA */
   oem.expectlen=1;			/* Set expeded length to 1 */
   oem.timeout=5;			/* Set time out value in seconds */
   oem.blkfactora=1;			/* Set blocking factor A */
   oem.blkfactorb=1;			/* Set blocking factor B */
   oem.scsiaddress=0;			/* Test SCSI address 0 */
   oem.scsiparity=1;			/* Enable SCSI Parity Checking */

  display_screen();
  for(ha=0;ha<8;ha++)
   {


      if(check_host_adapter(ha) == -16)
	continue;

      printf("\nHBA %d\n",ha);
      for (i = 0;i <= 7;i++)
	{
	   ierr = test_unit_ready(ha,i);
	   if(ierr != 0)
	     {
	       if(ierr == -2)  		/* Timeout durring selection  */
		 continue;             	/* skip address device not there */
	       else            		/* Display error and continue */
		 printf(" %i   Error Code :%i\n",i,oem.adaptererr);
	      }
	   for (i2 = 0;i2 <= sizeof(buf);i2++)buf[i2] = 0;
	   ierr = inquiry_command(ha,i);/* Issue Inquiry command to id*/
	   display_device_data();	/* Display data		      */
	 };
     }
  exit (0);
}

int check_host_adapter(char adapter_id)
{

  int	rtn_err;


  /* SET UP COMMAND BLOCK FOR TEST UNIT READY  */

   cmds[0]= 0;
   cmds[1]= 0;
   cmds[4]= 0;
   cmds[5]= 0;

   /* SET OEM TABLE FOR TEST UNIT READY COMMAND  */

   segread(&segregs) ;
   oem.cmdblock = MAKE_FP(segregs.ds,&cmds[0]);  /* Set pointer to CMDS */
   oem.datablock = MAKE_FP(segregs.ds,&buf[0]);  /* Set pointer to DATA */
   oem.expectlen=1;				/* Set expeded length to 1 */
   oem.timeout=5;				/* Set time out value */
   oem.blkfactora=1;				/* Set blocking factor A */
   oem.blkfactorb=1;			        /* Set blocking factor B */
   oem.scsiaddress= (adapter_id << 4) & 0xf0;  /* Test SCSI address 0 */


   /* CALL OEM TOOLKIT FOR TRANSFER  */

   rtn_err = oemscsi(&oem);
   if(rtn_err != 0)
   {
     if (oem.adaptererr == -7)		/* Parity error detected */
       {
	oem.scsiparity = 0;		/* Disable parity checking*/
	check_host_adapter(adapter_id);	/* Send call again	  */
       }
     if (oem.adaptererr == -16 && ha == 0)
	{
	  printf("\nNo Host Adapter Found Address %d",adapter_id);
	  exit(1);
       }
     if (oem.adaptererr == -16)		/* No host adapter found */
	  return(-16);
   }
}

void display_screen(void)
{
   clrscr();
   printf("                                Future Domain\n");
   printf("                             SCSI Inquiry Utility\n\n");
   printf("\n\n\n");
   printf("SCSI ID     Inquiry String     Revision  Level             Type\n");
   for(i = 0;i <= 79;i++)printf("²");
   printf("\n");
}

void display_device_data(void)
{
  for(i2 =0;i2 <= sizeof(id_string);i2++)id_string[i2] = 0;
  for(i2 =0;i2 <= sizeof(id_string)-1;i2++)id_string[i2] = buf[i2 + 8];
  printf("   %i    %.16s  	    %c%c%c%c                 "
	,i
	,&id_string
	,buf[32]
	,buf[33]
	,buf[34]
	,buf[35]);
  if(buf[1] & 0x80)
    printf(" Removable\n");
  else
    printf(" Fixed\n");
}

int  test_unit_ready(int adapter_id,int scsi_id)
{
   int	ierr;

/* SET UP COMMAND BLOCK FOR TEST UNIT READY  */

     cmds[0]= 0;
     cmds[1]= 0;
     cmds[4]= 0;
     cmds[5]= 0;

  /* SET OEM TABLE FOR TEST UNIT READY COMMAND  */

     segread(&segregs) ;
     oem.cmdblock = MAKE_FP(segregs.ds,&cmds[0]);
     oem.datablock = MAKE_FP(segregs.ds,&buf[0]);
     oem.expectlen=1;
     oem.timeout=5;
     oem.blkfactora=1;
     oem.blkfactorb=1;
     oem.scsiaddress=(adapter_id << 4 &0xf0) | scsi_id;
     oem.expectlen = 1;

  /* CALL OEM TOOLKIT FOR TRANSFER  */

     ierr = oemscsi(&oem);

     return(ierr);
}

int inquiry_command(int adapter_id,int scsi_id)
{
  cmds[0]= 0x12;                 /* Set command block for Inquiry */
  cmds[1]= 0;			 /* command  			  */
  cmds[4]= 0x27;                 /* Expected length is 39 bytes   */
  cmds[5]= 0;
  segread(&segregs) ;
  oem.cmdblock = MAKE_FP(segregs.ds,&cmds[0]);
  oem.datablock = MAKE_FP(segregs.ds,&buf[0]);
  oem.expectlen=1;
  oem.timeout=5;
  oem.blkfactora=1;
  oem.blkfactorb=1;
  oem.scsiaddress=(adapter_id << 4 &0xf0) | scsi_id;
  oem.expectlen = 39;
  ierr = oemscsi(&oem);		/* Call OEM TOOLKIT 		 */

}
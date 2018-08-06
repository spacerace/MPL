/***************************************************************************/
	SAMPLE ROUTINES TO ACCESS THE OEM TOOLKIT
		Note: This is a sample program which gives you a general idea
				on how to use the OEM Toolkit.  This program is not
				meant to be compiled but may be modified to create
				a working program with little effort.
/***************************************************************************/

#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#define PASS 0
#define FAIL 1
#define ESC 0x1B
#define CMDS_LENGTH 10        /* length of command block */

	/* structure passed to the oem toolkit */
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
   struct SREGS segregs ;

   unsigned char *scsi_buf;   /* character pointer to data buffer */
   int *scsi_bufw;            /* integer pointer to data buffer */

   unsigned char cmds[CMDS_LENGTH]; /* command block */


/***************************************************************************

	(IssueCommand) - This routine issues the command block to the
						  specified scsi device.

	Arguments: expected transfer length (long value)
     		     timeout in seconds (int value)
					
	Returns:   0 - completed succesfully
	           1 - adapter or device error

****************************************************************************/
int IssueCommand (long explen, int timeout)
{
   int ierr, prm_stat = FAIL;

   union
      {
      unsigned char (far *point)[];
      unsigned char *ptnear;
      long longval;
      int intval[2];
      unsigned char ch [4];
      } cnvtr ;

   /* set block factor to 512 for read & writes */
   if (cmds[0] == 0x08 || cmds[0] == 0x0a)
      {
      oem.blkfactora = 512;
      oem.blkfactorb = 512;
      }
   else
      {
      oem.blkfactora = 1;
      oem.blkfactorb = 1;
      }
   
   segread (&segregs);         /* get the system registers */
   cnvtr.intval[1] = segregs.ds;
   cnvtr.ptnear = &cmds[0];
   oem.cmdblock = cnvtr.point; /* set pointer to command block */
   cnvtr.ptnear = &scsi_buf[0];
   oem.datablock = cnvtr.point;/* set pointer to transfer buffer */
   oem.expectlen = explen;     /* set expected length */

	/* issue command to device - call to oem toolkit */
	ierr = oemscsi (&oem);
   
   oem.scsistatus &= ~drive_lun; /* remove lun bits from status byte */

   /* do not display error message for buffer overflow or underflow */
   if (oem.adaptererr == 2 && oem.scsistatus != 2)
      ierr = oem.adaptererr = 0;

   if ( ierr == -2 )
      return (FAIL);    /* do not reset bus on selection timeout */
   if ( ierr < 0 )
      oemclear (adpt_nbr); /* must reset bus if timeout in any other phase */

	if ( (ierr == 0) && (oem.scsistatus == 0) )
      return (PASS);
   else
      return (FAIL);
}

void GetInquiry (void)
{
   cmds[0] = 0x12;            /* inquiry command */
   cmds[4] = 0x24;            /* inquiry length */

   if (IssueCommand (0x24l, 2, 1))
      {
      ShowResult ("GET INQUIRY", "ERROR");
      return ;
      }

   ShowInquiry ();            /* display inquiry information window */
}

void ShowInquiry (void)
{
   char si_idn [25];				/* store product indentification */
	char si_rev [5];				/* store product revision level */

   char pq, dt, rmb, ansi_ver;
   int i, j, si_dmy;

	/* get product identification */
   strncpy (si_idn, &scsi_buf[8], 24);
	si_idn[24] = NULL;  			/* make sure we have a null at the end */
	printf ("Product Identification: %s\n", si_idn);
	
	/* get product revision level */
	strncpy (si_rev, &scsi_buf[32], 4);
	si_rev[4] = NULL;				/* make sure we have a null at the end */
	printf ("Product revision level: %s\n", si_rev);
	
   rmb = scsi_buf[1]&0x80;
   if (rmb)
		printf ("Physical medium removable\n");
   else
		printf ("Physical medium fixed/not removable\n");
}

void ReadCapacity (void)
{
   union
      {
      unsigned char byteval[4];
      int integerval[2];
      long intlongval;
      } number;
   
   long drivecap, blklength;

   cmds[0] = 0x25;				/* read capacity command */

   if (IssueCommand (8l, 6, 1))
      {
      ShowResult ("DISPLAY CAPACITY", "ERROR");
      return ;
      }

   number.byteval[0] = scsi_buf[3];
   number.byteval[1] = scsi_buf[2];
   number.byteval[2] = scsi_buf[1];
   number.byteval[3] = scsi_buf[0];
   if (number.intlongval)
      drivecap=number.intlongval+1;
   else
      drivecap = 0l;
   number.byteval[0] = scsi_buf[0];
   number.byteval[1] = scsi_buf[6];
   blklength=number.integerval[0];

	printf ("Number of sectors: %ld\n", drivecap);
	printf ("Block length: %ld\n", blklength);
	printf ("Drive capacity: %ld\n", drivecap*blklength);

}

void RequestSense (void)
{
   unsigned char cc;
   int i;

   cmds[0] = 0x03;				/* request sense command */
   cmds[4] = 16;              /* transfer length */

   if (IssueCommand (16l, 2, 1))
      {
      ShowResult ("REQUEST SENSE", "ERROR");
      return ;
      }

	/* display resense sense bytes */
   for (i=0;i<16;i++)
      printf ("Byte %2i->%02X",i, scsi_buf[i]&0xff);
}

void RezRewUnit (void)
{
   int rru_timeout;
   
   cmds[0] = 0x01;
   if (IssueCommand (0x05l, rru_timeout, 1))
      {
      ShowResult ("REZERO/REWIND", "ERROR");
      return ;
      }
   
   ShowResult ("REZERO/REWIND", "Command Completed Succesfully");
}

void TestUnitRdy (void)
{
   cmds[0] = 0x00;

   if (IssueCommand (0x05l, 30, 1))
      {
      ShowResult ("TEST UNIT READY", "ERROR");
      return ;
      }

   ShowResult ("TEST UNIT READY", "Drive is Ready");
}

void RezRewUnit (void)
{
   int rru_timeout;
   
   cmds[0] = 0x01;
   if (IssueCommand (0x05l, rru_timeout, 1))
      {
      ShowResult ("REZERO/REWIND", "ERROR");
      return ;
      }
   
   ShowResult ("REZERO/REWIND", "Command Completed Succesfully");
}

void ResetBus (void)
{
   oemclear (adpt_nbr);       /* reset the scsi bus */
   ShowStatus ();             /* clear the status window */

   ShowResult ("RESET SCSI BUS",  "SCSI Bus has been reset");
}


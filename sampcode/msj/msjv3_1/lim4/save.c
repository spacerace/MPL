/*

Figure 3
========

*/

/******************************************************************/
/* This function checks to see if an expanded memory manager is   */
/* and then checks to see if the version number is >= 4.0. It     */
/* uses emm function GET VERSION NUMBER.                          */
/******************************************************************/

int check_emm_version_number()
{  
   char *emm_device_name_ptr ;
   
   /***************************************************************/
   /* Use the dos get interrupt function (0x35) to get pointer    */
   /* at interrupt vector 0x67, and check for emm device name.    */
   /***************************************************************/

   inregs.h.ah = 0x35;
   inregs.h.al =EMM_INT;
   intdosx(&inregs, &outregs, &segregs);
   emm_device_name_ptr = (segregs.es * 65536) + 10;
   if (memcmp(emm_device_name, emm_device_name_ptr,8) !=0)
     {
       printf("Expanded memory manager not present\n");
       exit(1);
     }

   /***************************************************************/
   /* Now check for version number >= 4.0                         */    
   /***************************************************************/

   inregs.h.ah = GET_VERSION ;  /* set funciton code */
   int86(EMM_INT,&inregs,&outregs);
   if (outregs.h.ah != 0) exit(1);
   if ((outregs.h.ah == 0) & (outregs.h.al < 0x40))
   {
     printf("Expanded memory manager does not support LIM 4.0");
     exit(1) ;
   }  
}
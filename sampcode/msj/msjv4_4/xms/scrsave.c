/****************************************************************************/
/*									    */
/*  SCRSAVE.C - 					    Chip Anderson   */
/*									    */
/*	XMS Demonstration Program.  Saves the contents of the text screen   */
/*	in extended memory, writes a test pattern on the screen, then	    */
/*	restores the screen's original contents.  11/1/88		    */
/*									    */
/****************************************************************************/

#include "stdio.h"

#define WORD	    unsigned int

WORD XMM_Installed(void);
WORD XMM_Version(void);
long XMM_AllocateExtended(WORD);
long XMM_FreeExtended(WORD);
long XMM_MoveExtended(struct XMM_Move *);

struct XMM_Move
  {
    unsigned long   Length;
    unsigned int    SourceHandle;
    unsigned long   SourceOffset;
    unsigned int    DestHandle;
    unsigned long   DestOffset;
  };

#define XMSERROR(x)	(unsigned char)((x)>>24)

/*--------------------------------------------------------------------------*/
/*									    */
/*  main() -								    */
/*									    */
/*--------------------------------------------------------------------------*/

main()

{
  int		    i,j;
  long		    lret;
  WORD		    xHandle = 0;
  struct XMM_Move   MoveStruct;

  /* Display the credits. */
  printf("SCRSAVE - XMS Demonstration Program\n");

  /* Is an XMS Driver installed? */
  if (!XMM_Installed())
    {
      printf("No XMS Driver was found.\n");
      exit(-1);
    }

  /* Is it a version 2.00 driver (or higher)? */
  if (XMM_Version() < 0x200)
    {
      printf("This program requires an XMS driver version 2.00 or higher.\n");
      exit(-1);
    }

  /* We've found a legal XMS driver.  Now allocate 8K of extended memory for
     the screen. */

  lret = XMM_AllocateExtended(8);
  xHandle = (WORD)lret;

  if (!xHandle)
    {
      printf("Allocation Error: %X\n", XMSERROR(lret));
      exit(-1);
    }

  /* Copy the contents of the screen video buffer into the extended memory
     block we just allocated. */

  MoveStruct.SourceHandle = 0;		/* Source is in conventional memory */
  MoveStruct.SourceOffset = 0xB8000000L;/* Text screen data is at B800:0000 */
  MoveStruct.DestHandle   = xHandle;
  MoveStruct.DestOffset   = 0L;
  MoveStruct.Length	  = 8000L;	/* 80 x 25 x 4 */

  lret = XMM_MoveExtended(&MoveStruct);

  if (!(WORD)lret)
    {
      printf("Memory Move Error: %X\n", XMSERROR(lret));
      exit(-1);
    }

  /* Fill the screen with garbage. */
  for (i=0; i < 22; i++)
      for (j=64; j < (64+81); j++)
	  printf("%c",j);

  /* Prompt the user. */
  printf("Press Enter to restore the screen:");
  getchar();

  /* Restore the screen buffer. */
  MoveStruct.DestHandle   = 0;
  MoveStruct.DestOffset   = 0xB8000000L;
  MoveStruct.SourceHandle = xHandle;
  MoveStruct.SourceOffset = 0L;
  MoveStruct.Length	  = 8000L;

  lret = XMM_MoveExtended(&MoveStruct);

  if (!(WORD)lret)
    {
      printf("Memory Move Error: %X\n", XMSERROR(lret));
      exit(-1);
    }

  /* Free the extended memory buffer. */
  XMM_FreeExtended(xHandle);

  printf("Program terminated normally.");
  exit(0);
}

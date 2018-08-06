/*-------------------------------------------------------
   WE.C -- A Program that Obtains an Anchor Block Handle
  -------------------------------------------------------*/

#include <os2.h>

int main (void)
     {
     HAB  hab ;

     hab = WinInitialize (0) ;

     WinTerminate (hab) ;
     return 0 ;
     }

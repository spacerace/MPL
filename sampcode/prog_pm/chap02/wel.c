/*-------------------------------------------------
   WEL.C -- A Program that Creates a Message Queue
  -------------------------------------------------*/

#include <os2.h>

int main (void)
     {
     HAB  hab ;
     HMQ  hmq ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

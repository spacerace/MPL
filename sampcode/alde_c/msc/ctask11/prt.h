/*
   PRT.H - CTask - Printer driver routine definitions.

   Public Domain Software written by
      Thomas Wagner
      Patschkauer Weg 31
      D-1000 Berlin 33
      West Germany
*/


#define AUTOFEED  0x02
#define INIT      0x04
#define SELECT    0x08

#define BUSY      0x80
#define ACK       0x40
#define PEND      0x20
#define SELIN     0x10
#define ERROR     0x08

extern int far prt_install (int port, byte polling, 
                            word prior, 
                            farptr xmitbuf, word xmitsize);
extern void far prt_remove (int port);
extern void far prt_remove_all (void);
extern void far prt_change_control (int port, byte control);
extern int far prt_write (int port, byte ch, dword timeout);
extern int far prt_status (int port);
extern int far prt_complete (int port);
extern int far prt_wait_complete (int port, dword timeout);


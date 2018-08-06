/*Figure 6. tsrbios.h -- function declarations for the tsrbios.c routines. */

   extern  unsigned int kbd_read_ch(void);
   extern  unsigned int kbd_stat(void);
   extern  int scr_swapmsg(unsigned char *msg,unsigned char *savbuf,
	   int row,int col);
   extern  unsigned scr_read_ch(int row,int col);
   extern  void scr_write_ch(unsigned int ch,int row,int col);
   extern  int far *scr_get_curs_addr(void);

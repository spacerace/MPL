/* scrn.h -- header file for BIOS video I/O functions */
/*           contained in scrfun.c and scrfun.lib     */
#define VIDEO 0x10
#define SETMODE 0
#define SETCURSOR 2
#define GETCURSOR 3
#define SETPAGE 5
#define SCROLL 6
#define READCHATR 8
#define WRITECHATR 9
#define GETMODE 15
#define NORMAL 0x7
#define VIDREV 0x70
#define INTENSE 0x8
#define BLINK  0x80
#define COLS 80
#define ROWS 25
#define TEXTBW80 2
#define TEXTC80 3
#define TEXTMONO 7


void Clearscr(void),
     Setvmode(unsigned char),
     Setpage(unsigned char),
     Setcurs(unsigned char, unsigned char,
             unsigned char),
     Read_ch_atr(unsigned char *, unsigned char *,
                 unsigned char),
     Write_ch_atr(unsigned char, unsigned char,
                  unsigned char, unsigned int),
     Rewrite(unsigned char, unsigned char),
     Getcurs(unsigned char *, unsigned char *,
             unsigned char);

unsigned char Getvmode(void),
              Getpage(void),
              Curslt_lim(unsigned char),
              Cursrt_lim(unsigned char),
              Cursup_lim(unsigned char),
              Cursdn_lim(unsigned char);

/* macro definitions */

#define Home()        Setcurs(0, 0, Getpage())
/* the next four macros set cursor limits to the      */
/* full screen                                        */
#define Curslt()      Curslt_lim(0)
#define Cursrt()      Cursrt_lim(COLS - 1)
#define Cursdn()      Cursdn_lim(ROWS - 1)
#define Cursup()      Cursup_lim(0)

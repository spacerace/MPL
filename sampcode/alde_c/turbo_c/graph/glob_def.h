/*--------------------------------------------------------------------------
 *                                glob_def.h
 *
 *                      copyright 1987 by Michael Allen
 *
 *	   This contains the global includes and defines thay you will use
 *	   in all your program modules that used functions from graph.lib.
 *	      add: #include "glob_def.h" to all such files.
 *
 *-------------------------------------------------------------------------*/


#define max(a,b)	(((a) > (b)) ? (a) : (b))
#define min(a,b)	(((a) < (b)) ? (a) : (b))
#define textmode() init_graphics(TEXTMODE)
#define hires()    init_graphics(HIGHRES)


#define false  0
#define true   1

/* function key definitions */

#define UP    'H'
#define DOWN  'P'
#define LEFT  'K'
#define RIGHT 'M'
#define SPACE ' '
#define ESC   '\x1b'
#define CR    '\x0d'
#define BS    '\x08'
#define BEL   '\x07'
#define INS   'R'
#define HOME  'G'
#define AEND  'O'
#define CTRLEND 'u'
#define PGUP  'I'
#define PGDN  'Q'
#define DEL   'S'
#define F1    ';'
#define F2    '<'
#define F3    '='
#define F4    '>'
#define F5    '?'
#define F6    '@'
#define F7    'A'
#define F8    'B'
#define F9    'C'
#define F10   'D'
#define CTLEFT  's'
#define CTRIGNT 't'
#define CTLPGUP '„'
#define CTLPGDN 'v'
#define CTLF1   '^'
#define CTLF9   'f'
#define ATTC    '.'
#define ALTP    ''
#define ALTW    ''
#define ALTS    ''

#define HIGHRES        6
#define TEXTMODE       3
#define BLACK          0x00
#define BLUE           0x01
#define GREEN          0x02
#define CYAN           0x03
#define RED            0x04
#define MAGENTA        0x05
#define BROWN          0x06
#define LIGHTGREY      0x07
#define DARKGREY       0x08
#define LIGHTBLUE      0x09
#define LIGHTGREEN     0x0A
#define LIGHTCYAN      0x0B
#define LIGHTRED       0x0C
#define LIGHTMAGENTA   0x0D
#define YELLOW         0x0E
#define WHITE          0x0F

#define INT_10        0x10   /* Function call to the BIOS console driver */
#define MOUSE         0x33   /* microsoft mouse interface */

#define NORMAL        0
#define XOR_MODE      1
#define SCREENADDR    0xb8000000         /* IBM SPECIFIC SCREEN BUFFER */
#define BYTE	      unsigned char

typedef unsigned char far * T_SCREEN;      /* --> 16K SCREEN BUFFER */


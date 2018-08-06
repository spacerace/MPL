/***************************************************************************
 *                                                                         *
 *                             ucvideo.h                                   *
 *                                                                         *
 *    This header supplies all the definitions required to deal with  the  *
 *    the vidoe state of the machine.                                      *
 *                                                                         *
 ***************************************************************************/

/*  Storage for current information  */

extern  short  vmode;    /* current mode   */
extern  short  vwidth;   /* current width  */
extern  short  vpage ;   /* current page   */

#define MAXMODE  16      /* maximum video mode available */

/*  Set limits on video screen size */

extern short maxrow[MAXMODE];
extern short maxcol[MAXMODE];
extern short maxpage[MAXMODE];

/* possible display types */

#define MONO_D   1
#define COLOR_D  2

/* Possible cursor modes */

#define CURSOR_OFF  0
#define CURSOR_ON   1

/* Possible display adapter types */

#define  MDA  1
#define  CGA  2
#define  EGA  4

/*  Possible video modes  */

/*  CGA */
#define M40    0
#define C40    1
#define M80    2
#define C80    3
#define CMRES  4
#define MMRES  5
#define MHRES  6

/* MDA */
#define MM80   7

/* Tandy & PCjr */
#define TCLRES 8
#define TMRES  9
#define TCHRES 10

/* EGA */
#define ECMRES 13
#define ECHRES 14
#define EMHRES 15
#define EEHRES 16

/*  Video byte masks */
#define CMASK  0x00FF   /* Character mask */
#define AMASK  0xFF00   /* Attribute mask */

/* Modifiers for attribute bytes */
#define BRIGHT  8
#define BLINK   128

/*  Colors  */
#define BLACK     0
#define BLUE      1
#define GREEN     2
#define CYAN      3
#define RED       4
#define MAGENTA   5
#define BROWN     6
#define WHITE     7
#define GRAY      8
#define LTBLUE    9
#define LTGREEN   10
#define LTCYAN    11
#define LTRED     12
#define LTMAGENTA 13
#define YELLOW    14
#define LTBROWN   14     /*  alternate */
#define LTWHITE   15

#define NORMAL    WHITE
#define REVERSE   0X70

/* Characters used in drawing various box designs.  */

/* Single line boxes */

#define VLINE1   179
#define HLINE1   196
#define ULC11    218  /* First number is horizontal commponent */
#define URC11    191
#define LLC11    192
#define LRC11    217
#define TEEL11   195
#define TEER11   180
#define TEET11   194
#define TEEB11   193
#define CROSS11  197


/* Double line boxes */

#define VLINE2   186
#define HLINE2   205
#define ULC22    201  /* First number is horizontal commponent */
#define URC22    187
#define LLC22    200
#define LRC22    188
#define TEEL22   204
#define TEER22   185
#define TEET22   203
#define TEEB22   202
#define CROSS22  206


/* Double line Horizontal & single line vertical boxes */

#define ULC21    213  /* First number is horizontal commponent */
#define URC21    184
#define LLC21    212
#define LRC21    190
#define TEEL21   198
#define TEER21   181
#define TEET21   209
#define TEEB21   207
#define CROSS21  216


/* Single line Horizontal & double line vertical boxes */

#define ULC12    214  /* First number is horizontal commponent */
#define URC12    183
#define LLC12    211
#define LRC12    189
#define TEEL12   199
#define TEER12   182
#define TEET12   210
#define TEEB12   208
#define CROSS12  215


/* Special block graphic characters */

#define LT_BLOCK  144    /*  Light block like BLOCK below            */
#define MED_BLOCK 145
#define DRK_BLOCK 146
#define BLOCK     219    /*  Solid Block filling character position */
#define HBARB     220
#define VBARL     221    /* Solid vertical bar to left side of char cell */
#define VBARR     222
#define HBART     223

/* Special characters */
#define HAP_FAC     1
#define RHAP_FAC    2
#define HEART       3
#define DIAMOND     4
#define CLUB        5
#define SPADE       6
#define UP_ARROW    24
#define DN_ARROW    25
#define RT_ARROW    26
#define LF_ARROW    27




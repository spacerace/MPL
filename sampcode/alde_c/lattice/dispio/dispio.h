
/*                        Listing 4
                    C to Assembly Interface
      Computer Language, Vol. 2, No. 2 (February, 1985), pp. 49-59
                         Include file
*/
/* See IBM Personal Computer Reference Manual.
   For August, 1981 edition, pp. 43-45.
   For April, 1983 edition, pp. 45-47. */
/* Modified by Lew Paper.  Upper cases and additions */

#define PAGE 0

 /* video modes */
#define S40X25_BW 0
#define S40X25_COLOR 1
#define S80X25_BW 2
#define S80X25_COLOR 3
#define MED_COLOR 4
#define MED_BW 5
#define HIGH_BW 6

 /* Lower right hand corners of screen */ /* L.P. */
#define LRH80 0x244f
#define LRH40 0x2427

 /* Character attributes */ /* L.P. */
#define STD_CHAR 0x07         /* White on black */
#define UND_CHAR 0x01         /* Underline white on black */
#define HI_CHAR 0x0f          /* High intensity white on black */
#define UND_HI_CHAR 0x09      /* Underline high intensity white on black */
#define REV_STD_CHAR 0x70     /* Black on white */

 /* video functions */
#define SET_TYPE 256                  /* AH = 1, AL = 0 */
#define SET_CUR 512                   /* AH = 2, AL = 0 */
#define READ_POSITION 768             /* AH = 3, AL = 0 */
#define READ_LIGHT_PEN_POSITION 1024  /* AH = 4, AL = 0 */
#define SELECT_PAGE 1280              /* AH = 5, AL = 0 */
#define SCROLL_UP 1536                /* AH = 6, AL = 0 */
#define SCROLL_DN 1792                /* AH = 7, AL = 0 */
#define READ_ATTRIBUTE_CHAR 2048      /* AH = 8, AL = 0 */
#define WRITE_ATTRIBUTE_CHAR 2304     /* AH = 9, AL = 0 */
#define WRITE_CHAR 2560               /* AH = 10, AL = 0 */
#define SET_PALETTE 2816              /* AH = 11, AL = 0 */
#define WRITE_DOT 3072                /* AH = 12, AL = 0 */
#define READ_DOT 3328                 /* AH = 13, AL = 0 */
#define WRITE_TELETYPE 3584           /* AH = 14, AL = 0 */
#define GET_STATE 3840                /* AH = 15, 1L = 0 */

#define READ_TYPE 0x1000              /* L.P. */
#define READ_LOC 0x1100               /* L.P. */

int curset();
int dispio(), read_loc(), read_typ(), write_ac(), write_ch(), writ_tty();
int c40_disp(), c80_disp(), mono_dsp();

 /* macros */
#define CURPOS(ROW,COL) dispio(SET_CUR,PAGE,(((int)ROW<<8)+COL)
#define WRTCHAR(CH) write_ch(CH,PAGE,1)


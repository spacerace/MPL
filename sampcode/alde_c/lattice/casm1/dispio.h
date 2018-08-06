
/*                        Listing 4
                    C to Assembly Interface
      Computer Language, Vol. 2, No. 2 (February, 1985), pp. 49-59
                         Include file
*/
/* See IBM Personal Computer Reference Manual.
   For August, 1981 edition, pp. 43-45.
   For April, 1983 edition, pp. 45-47. */
#define PAGE 0
#define PAGE_1 1
 /* video modes */
#define s40x25_bw 0
#define s40x25_color 1
#define s80x25_bw 2
#define s80x25_color 3
#define med_color 4
#define med_bw 5
#define high_bw 6
#define mono 7       /*L.P.*/
 /* video functions */
#define set_type 256                  /* AH = 1, AL = 0 */
#define set_cur 512                   /* AH = 2, AL = 0 */
#define read_position 768             /* AH = 3, AL = 0 */
#define read_light_pen_position 1024  /* AH = 4, AL = 0 */
#define select_page 1280              /* AH = 5, AL = 0 */
#define scroll_up 1536                /* AH = 6, AL = 0 */
#define scroll_dn 1792                /* AH = 7, AL = 0 */
#define read_attribute_char 2048      /* AH = 8, AL = 0 */
#define write_attribute_char 2304     /* AH = 9, AL = 0 */
#define write_char 2560               /* AH = 10, AL = 0 */
#define set_palette 2816              /* AH = 11, AL = 0 */
#define write_dot 3072                /* AH = 12, AL = 0 */
#define read_dot 3328                 /* AH = 13, AL = 0 */
#define write_teletype 3584           /* AH = 14, AL = 0 */
#define get_state 3840                /* AH = 15, 1L = 0 */
 /* macros */
#define curpos(row,col) dispio(set_cur,PAGE,(((int)row)<<8)+col)
#define SET_MODE dispio(s80x25_bw)
#define horline(ch,width) dispio(write_char+ch,PAGE,width) 
#define hline(row,col,ch,width) curpos(row,col) ; horline(ch,width)
#define wrtchar(ch) dispio(write_char+ch,PAGE,1)
#define horline_1(ch,width) dispio(write_char+ch,PAGE_1,width) 
#define hline_1(row,col,ch,width) curpos(row,col) ; horline(ch,width)
#define wrtchar_1(ch) dispio(write_char+ch,PAGE_1,1)

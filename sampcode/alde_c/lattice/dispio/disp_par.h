/* Global parameters defined in the module dispiof.c.  To set all of them 
   except d_attr to the current full screen, use set_dfull(). */

extern int d_mode, d_cols, d_page;  /* From GET_STATE */
extern int d_top, d_bottom;         /* Rows 0-24 */
extern int d_left, d_right;         /* Columns 0-39 or 0-79 */
extern int d_ulc;                   /* Upper left corner, 
                                         (d_left << 8) + d_top */
extern int d_lrc;                   /* Lower right corner, 
                                         (d_right << 8) + d_bottom */
extern int d_attr;                  /* Attribute. */

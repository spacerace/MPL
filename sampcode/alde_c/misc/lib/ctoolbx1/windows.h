/****************************************************************************
 *                                windows.h                                 *
 *                                                                          *
 *   Basic window header for the C Tool Box.   This header contains the     *
 *   box type definition structure.                                         *
 *                                                                          *
 *                                                                          *
 *                           Copyright (c) 1987                             *
 *                            David A.  Miller                              *
 *                            Unicorn Software                              *
 *                                                                          *
 ****************************************************************************/



typedef struct
   {
      int hlchar;
      int vlchar;
      int ulchar;
      int urchar;
      int llchar;
      int lrchar;
      int bfore;       /*  border foreground color */
      int bback;       /*  border background color */
      int ffore;       /*  foreground fill color  */
      int fback;       /*  background fill color  */
      int row;         /*  upper left corner row  */
      int col;         /*  upper left corner column */
      int height;      /*  height of box in rows  */
      int width;       /*  width of box in characters */
      unsigned *crt;   /*  location of saved widow background */
   } BOX ;

BOX *makebox(), *copybox();





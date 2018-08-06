/****************************************************************************
 *                               border.h                                   *
 *                                                                          *
 *   Basic border header for the C Tool Box.   This header contains the     *
 *   border type definition structure.                                      *
 *                                                                          *
 *                                                                          *
 *                           Copyright (c) 1987                             *
 *                            David A.  Miller                              *
 *                            Unicorn Software                              *
 *                                                                          *
 ****************************************************************************/


typedef struct
   {
      int hlchar;      /*  character for horizontal line  */
      int vlchar;      /*  character for vertical line */
      int ulchar;      /*  character for upper left corner  */
      int urchar;      /*  character for upper right corner */
      int llchar;      /*  character for lower left corner  */
      int lrchar;      /*  character for lower right corner */
   }  BORDER ;


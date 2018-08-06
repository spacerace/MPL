/* dots.c -- illustrates the _setcolor(), _setpixel(), */
/*           and _selectpalette() functions from the   */
/*           QuickC graphics library                   */
/* If you load graphics.qlb, no program list is needed */

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#define ESC '\033'
#define BKCOLS 8      /* number of background colors */
#define PALNUM 4      /* number of palettes */
long Bkcolors[BKCOLS] = {_BLACK, _BLUE, _GREEN, _CYAN, _RED,
                         _MAGENTA, _BROWN, _WHITE};
main (argc, argv)
int argc;
char *argv[];
{
     struct videoconfig vc;
     unsigned int col, row;
     short color = 0;
     int bkc_index = 1;  /* blue background */
     short palette = 0;  /* red, green, brown */
     int firstcol, firstrow, lastrow, lastcol;
     int mode = _MRES4COLOR;
     int ch;

     if (argc > 1)
          mode = atoi(argv[1]);

     if (_setvideomode(mode) == 0)
     {
          printf("Can't do that mode.\n");
          exit(1);
     }
     _getvideoconfig(&vc);
     firstcol = vc.numxpixels / 5;
     firstrow = vc.numypixels / 5;
     lastcol = 4 * vc.numxpixels / 5;
     lastrow = 4 * vc.numypixels / 5;
     _selectpalette(palette);
     _setbkcolor (Bkcolors[bkc_index]);
     for (col = firstcol; col <= lastcol; ++col)
     {
          _setcolor((++color / 3) % vc.numcolors);
          for (row = firstrow; row <= lastrow; ++row)
               _setpixel(col, row);
     }
     while ((ch = getch()) != ESC)
     {
          if (ch == 'p')
                _selectpalette(++palette % PALNUM);
          else if (ch == 'b')
                _setbkcolor(Bkcolors[++bkc_index % BKCOLS]);
     }
     _setvideomode(_DEFAULTMODE);  /* reset orig. mode */
}

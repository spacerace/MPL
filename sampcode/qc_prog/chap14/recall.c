/* recall.c -- displays previously stored screen,     */
/*             including attributes.  Uses DMA.       */
/* Program list: recall.c, scrfun.c                   */
/* User include files: scrn.h, grafchar.h             */
/* Note: activate Screen Swapping On in Debug menu    */

#include <stdio.h>
#include <conio.h>
#include "scrn.h"
#include "grafchar.h"

main(ac, ar)
int ac;
char *ar[];
{
     unsigned char mode;
     unsigned short char_attr;
     FILE *save;
     unsigned int offset;
     char filename[81];
     VIDMEM screen;

     if (ac < 2)
          {
          fprintf(stderr, "Usage: %s filename\n", ar[0]);
          exit(1);
          }

     if ((save = fopen(ar[1],"rb")) == NULL)
          {
          fprintf(stderr, "Can't open %s\n", ar[1]);
          exit(1);
          }

     if ((mode = Getvmode()) == TEXTMONO)
          screen = MONMEM;
     else if (mode == TEXTC80 || mode == TEXTBW80)
          screen = CGAMEM;
     else
        exit(1);

     Clearscr();
     for (offset = 0; offset < CHARS; offset++)
          {
          fread(&char_attr, 2, 1, save);
          screen[offset] = char_attr;
          }
     fclose(save);
     Setcurs(23, 0, PAGE);
     getch();   /* anti scrolling for QC environment */
}

/* savescrn.c -- saves screen, including attribute    */
/*               values, in a file                    */
/*               Uses direct memory access.           */

#include <stdio.h>  /* for file-handling */
#include "scrn.h"
#include "grafchar.h"
void Save_screen()
{
    FILE *save;
    char filename[80];
    unsigned char mode;
    unsigned short char_attr;  /* character,attribute */
    int offset;
    VIDMEM screen;

     if ((mode = Getvmode()) == TEXTMONO)
          screen = MONMEM;
     else if (mode == TEXTC80 || mode == TEXTBW80)
          screen = CGAMEM;
     else
        exit(1);
    Setcurs(BOTLINE + 1, 0, PAGE);
    printf("Please enter name for save file: ");
    scanf("%s", filename);
    if ((save = fopen(filename,"wb")) == NULL)
        {
        fprintf(stderr,"Can't open %s\n", filename);
        exit(1);
        }
   for (offset = 0; offset < CHARS; offset++)
        {
        char_attr = screen[offset];
        fwrite(&char_attr, 2, 1, save);
        }
   fclose(save);
}

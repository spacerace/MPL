/* allcolor.c -- shows _ERESCOLOR 64-color palette     */
/* If you load graphics.qlb, no program list is needed.*/

/*  Hit <g> to advance left palette, <G> to go back.   */
/*  Hit <h> to advance right palette, <H> to go back.  */
/*  Hit <Esc> to quit                                  */
#include <stdio.h>
#include <graph.h>
#include <conio.h>
#define MAXCOLORS 64
#define ESC '\033'
long Ega_to_vga(int);   /* color value conversion */

main(argc, argv)
int argc;
char *argv[];
{
    struct videoconfig vc;
    int mode = _ERESCOLOR;
    int xmax, ymax;
    int c1 = 1;
    int c2 = 4;
    char left[11];
    char right[11];
    int lpos, rpos;
    char ch;

    if (argc > 1)
        mode = atoi(argv[1]);
    if (_setvideomode(mode) == 0)
    {
        fprintf(stderr,"%d mode not supported\n", mode);
        exit(1);
    }
    _getvideoconfig(&vc);
    _setlogorg(vc.numxpixels/2, vc.numypixels/2);

    xmax = vc.numxpixels / 2 - 1;
    ymax = vc.numypixels / 2 - 1;
    lpos = vc.numxpixels/32 - 5;
    rpos = lpos + vc.numxpixels / 16;
    _setcolor(1);
    _rectangle(_GFILLINTERIOR, -xmax, -ymax, 0, ymax);
    _setcolor(4);
    _rectangle(_GFILLINTERIOR, 1, -ymax, xmax, ymax);
    sprintf(left,"<-G %2d g->", c1);
    sprintf(right,"<-H %2d h->", c2);
    _settextcolor(6);
    _settextposition(0, 0);
    _outtext("Press Esc to quit");
    _settextposition(24, lpos);
    _outtext(left);
    _settextposition(24, rpos);
    _outtext(right);
    while ((ch = getch()) != ESC)
    {
        switch (ch)
        {
            case 'g': c1 = (c1 + 1) % MAXCOLORS;
                      _remappalette(1, Ega_to_vga(c1));
                      break;
            case 'G': c1 = (c1 - 1) % MAXCOLORS;
                      _remappalette(1, Ega_to_vga(c1));
                      break;
            case 'h': c2 = (c2 + 1) % MAXCOLORS;
                      _remappalette(4, Ega_to_vga(c2));
                      break;
            case 'H': c2 = (c2 - 1) % MAXCOLORS;
                      _remappalette(4, Ega_to_vga(c2));
                      break;
        }
        sprintf(left,"<-G %2d ->g", c1);
        sprintf(right,"<-H %2d ->h", c2);
        _settextposition(0, 0);
        _outtext("Press Esc to quit");
        _settextposition(24, lpos);
        _outtext(left);
        _settextposition(24, rpos);
        _outtext(right);
    }
    _setvideomode(_DEFAULTMODE);
}
long Ega_to_vga(egacolor)
int egacolor;       /* ega color value */
{
    static long vgavals[6] = {0x2A0000L, 0x002A00L, 0x00002AL,
                              0x150000L, 0x001500L, 0x000015L};
    long vgacolor = 0L; /* vga color value */
    int bit;

    for (bit = 0; bit < 6; bit++)
        vgacolor += ((egacolor >> bit) &1) * vgavals[bit];
    return (vgacolor);
}

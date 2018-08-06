/* allvga.c -- shows _MRES256COLOR 256K colors         */
/* If you load graphics.qlb, no program list is needed.*/

#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#include <conio.h>
#define FULLBRIGHT 64
#define ESC '\033'
char label[2][7] = {"ACTIVE", "      "};

main(argc, argv)
int argc;
char *argv[];
{
    struct videoconfig vc;
    int mode = _MRES256COLOR;
    int xmax, ymax;
    static long colors[2] = {_BLUE, _RED};
    char left[11];
    char right[11];
    int lpos, rpos;
    char ch;
    unsigned long blue = _BLUE >> 16;
    unsigned long green = 0L;
    unsigned long red = 0L;
    long color;
    short palnum = 0;

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
    lpos = vc.numxpixels / 32 - 5;
    rpos = lpos + vc.numxpixels/16;
    _remappalette(2, _RED);
    _setcolor(1);
    _rectangle(_GFILLINTERIOR, -xmax, -ymax, 0, ymax);
    _setcolor(2);
    _rectangle(_GFILLINTERIOR, 1, -ymax, xmax, ymax);
    sprintf(left," %6lxH ", colors[0]);
    sprintf(right," %6lxH ", colors[1]);
    _settextcolor(6);
    _settextposition(1, 1);
    _outtext("Press Tab to toggle panels, Esc to quit.");
    _settextposition(2, 1);
    _outtext("B increases blue level, b decreases it. ");
    _settextposition(3, 1);
    _outtext("G and g control green, R and r red.     ");
    _settextposition(24, lpos);
    _outtext(left);
    _settextposition(24, rpos);
    _outtext(right);
    _settextposition(5, 7);
    _outtext(label[0]);
    _settextposition(5, 27);
    _outtext(label[1]);
    while ((ch = getch()) != ESC)
        {
        switch (ch)
            {
            case '\t': _settextposition(5, 27);
                       _outtext(label[palnum]);
                       palnum ^= 1;
                       blue = (colors[palnum] << 16) & 0x3F;
                       green = (colors[palnum] << 8) & 0x3F;
                       red = colors[palnum] & 0x3F;
                       _settextposition(5, 7);
                       _outtext(label[palnum]);
                       break;
            case 'B':  blue = (blue + 1) % FULLBRIGHT;
                       colors[palnum] = blue << 16 |
                               green << 8 | red;
                       _remappalette(palnum + 1, colors[palnum]);

                       break;
            case 'b':  blue = (blue - 1) % FULLBRIGHT;
                       colors[palnum] = blue << 16 |
                               green << 8 | red;
                       _remappalette(palnum + 1, colors[palnum]);

                       break;
            case 'G':  green = (green + 1) % FULLBRIGHT;
                       colors[palnum] = blue << 16 |
                               green << 8 | red;
                       _remappalette(palnum + 1, colors[palnum]);

                       break;
            case 'g':  green = (green - 1) % FULLBRIGHT;
                       colors[palnum] = blue << 16 |
                               green << 8 | red;
                       _remappalette(palnum + 1, colors[palnum]);

                       break;
            case 'R':  red = (red + 1) % FULLBRIGHT;
                       colors[palnum] = blue << 16 |
                               green << 8 | red;
                       _remappalette(palnum + 1, colors[palnum]);

                       break;
            case 'r':  red = (red - 1) % FULLBRIGHT;
                       colors[palnum] = blue << 16 |
                               green << 8 | red;
                       _remappalette(palnum + 1, colors[palnum]);

                       break;

            }
        sprintf(left," %6lxH ", colors[0]);
        sprintf(right," %6lxH ", colors[1]);
        _settextposition(24, lpos);
        _outtext(left);
        _settextposition(24, rpos);
        _outtext(right);
        }
    _setvideomode(_DEFAULTMODE);
}

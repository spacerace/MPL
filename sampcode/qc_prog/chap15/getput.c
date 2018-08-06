/*  getput.c -- illustrates _getimage(), _putimage(), */
/*              the image-background interaction, and */
/*              the aspect ratio                      */
/* If you load graphics.qlb, no program list is needed*/

#include <stdio.h>
#include <stdlib.h>  /* declares malloc()  */
#include <graph.h>
#include <conio.h>
#define ESC '\033'

/* The following variables describe various          */
/* coordinates and sizes.                            */
/* They are declared externally so that they can be  */
/* shared easily by several functions.               */
int X1, Yb1, X2, Y2, Xdelta, Xside, Yside; /* image  */
int Xmid, Xmax, Ymid, Ymax;           /* background  */
int Xps, Xpr, Xand, Xor, Xxor, Ytop, Ybot; /* copies */
int X[3], Y[3];
float Ar;    /* aspect ratio */

struct videoconfig Vc;
char Mask[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0};
void Initialize(void), Drawfig(void),
      Drawbackground(void), Drawcopies(void);

main(argc, argv)
int argc;
char *argv[];
{
    int mode = _MRES4COLOR;

    if (argc > 1)
        mode = atoi(argv[1]);
    if (_setvideomode(mode) == 0)
    {
        fprintf(stderr, "Can't handle mode %d\n", mode);
        exit(1);
    }
    Initialize();
    Drawfig();
    Drawbackground();
    Drawcopies();
    _settextposition(1, 1);
    _outtext("Press a key to end");
    _settextposition(3, 1);
    _outtext("_GPSET _GPRESET _GAND");
    _settextposition(11, 5);
    _outtext("_GOR _GXOR");
    getch();
    _setvideomode(_DEFAULTMODE);
}

void Initialize()
{
    _getvideoconfig(&Vc);
    Ar = (float) (10 * Vc.numypixels)/
          (6.5 * Vc.numxpixels);
    _setlogorg(0, 0);
    Xmid = Vc.numxpixels / 2;
    Ymid = Vc.numypixels / 2;
    Xmax = Vc.numxpixels - 1;
    Ymax = Vc.numypixels - 1;
    /* locate three background rectangles */
    X[0] = Xmid;
    Y[0] = 0;
    X[1] = Xmid;
    Y[1] = Ymid;
    X[2] = 0;
    Y[2] = Ymid;
    X1 = 0.2 * Vc.numxpixels;
    Yb1 = 0.2 * Vc.numypixels;
    Xdelta = 0.033 * Vc.numxpixels;
    Xside = 3 * Xdelta;
    Yside = 3 * Ar * Xdelta;
    X2 = X1 + Xside;
    Y2 = Yb1 + Yside;
    /* offsets for _putimage() */
    Xps = .05 * Vc.numxpixels;
    Xpr = .20 * Vc.numxpixels;
    Xand = 0.35 * Vc.numxpixels;
    Xor = .10 * Vc.numxpixels;
    Xxor = .30 * Vc.numxpixels;
    Ytop = .05 * Vc.numypixels;
    Ybot = 2 * Ytop + Yside;
    _selectpalette(0);
}

void Drawfig()
{
    _setcolor(1);
    _rectangle(_GFILLINTERIOR, X1, Yb1,
               X1 + Xdelta , Y2);
    _setcolor(2);
    _rectangle(_GFILLINTERIOR,X1 + Xdelta + 1, Yb1,
               X1 + 2 * Xdelta, Y2);
    _setcolor(3);
    _rectangle(_GFILLINTERIOR,X1 +  2 * Xdelta + 1,
               Yb1, X2, Y2);

}

void Drawbackground()
{
    _setfillmask(Mask);
    _setcolor(1);
    _rectangle(_GFILLINTERIOR, Xmid, 0, Xmax - 1, Ymid - 1);
    _setcolor(2);
    _rectangle(_GFILLINTERIOR, Xmid, Ymid, Xmax, Ymax);
    _setcolor (3);
    _rectangle(_GFILLINTERIOR, 0, Ymid, Xmid - 1, Ymax);

}

void Drawcopies()
{
    int quad;   /* quadrant used */
    char far *storage;

    storage = (char far *) malloc((unsigned)_imagesize(
               X1, Yb1, X2, Y2));
    _getimage(X1, Yb1, X2, Y2, storage);

    for (quad = 0; quad < 3; quad++)
        {
        _putimage(X[quad] + Xps, Y[quad] + Ytop,
                  storage, _GPSET);
        _putimage(X[quad] + Xpr, Y[quad] + Ytop,
                  storage, _GPRESET);
        _putimage (X[quad] + Xand, Y[quad] + Ytop,
                   storage, _GAND);
        _putimage (X[quad] + Xor, Y[quad] + Ybot,
                   storage, _GOR);
        _putimage(X[quad] + Xxor, Y[quad] + Ybot,
                  storage, _GXOR);
        }
}
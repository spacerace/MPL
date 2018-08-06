/* race.c -- race of the patterned circles             */
/*    Illustrates animation with _getimage() and       */
/*    _putimage(), random number use with srand() and  */
/*    rand(), and system clock use with time() and     */
/*    ftime().
/* Program list: race.c (for srand(), rand(), and      */
/*                       ftime())                      */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graph.h>
#include <time.h>
#include <sys\types.h>
#include <sys\timeb.h>

#define END 25
#define FIGNUM 3
typedef char far *PTFRCHAR;
PTFRCHAR Bufs[FIGNUM];
unsigned char Masks[FIGNUM][8] = {
            {0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00},
            {0xF0,0xF0,0xF0,0xF0,0x0F,0x0F,0x0F,0x0F},
            {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA}};
short Xul[FIGNUM], Yul[FIGNUM];  /* figure locations */
short Xsize, Ysize;              /* figure size      */
struct videoconfig Vc;
void Initialize(void);
void Draw_n_store(void);
void Move_figs(void);
void Wait(double);

main(argc, argv)
int argc;
char *argv[];
{
    int mode = _MRES4COLOR;

    if (argc > 1)
        mode = atoi(argv[1]);
    if (_setvideomode(mode) == 0)
    {
        fprintf(stderr,"mode %d not supported\n",mode);
        exit(1);
    }
    Initialize();
    Draw_n_store();
    _settextcolor(2);
    _settextposition(1, 1);
    _outtext("Place your bets and type a key");
    _settextposition(25, 1);
    _outtext("Type a key again when done");
    getch();
    Move_figs();
    getch();
    _setvideomode(_DEFAULTMODE);
}

void Initialize()
{
    int i;
    float ar;  /* aspect ratio */

    _getvideoconfig(&Vc);
    ar = (float)(10 * Vc.numypixels) / (6.5 * Vc.numxpixels);
      /* set size, initial positions */
    Xsize = Vc.numxpixels / 30;
    Ysize = ar * Xsize;
    for(i = 0; i < FIGNUM; i++)
    {
        Xul[i] = 0;
        Yul[i] = (i + 1) * Vc.numypixels /
                      (FIGNUM + 1);
    }
    _selectpalette(0);
    _setcolor(1);
        /*  draw finish line */
    _moveto(END * Xsize, 0);
    _lineto(END * Xsize, Vc.numypixels - 1);
}

void Draw_n_store() /* draw images, save them */
{
    int i;

    for (i = 0; i < FIGNUM; i++)
        {
        _setcolor(i + 1);
        _setfillmask(Masks[i]);
        _ellipse(_GFILLINTERIOR ,Xul[i], Yul[i],
                 Xul[i] + Xsize, Yul[i] + Ysize);
        _ellipse(_GBORDER ,Xul[i], Yul[i],
                 Xul[i] + Xsize, Yul[i] + Ysize);
        Bufs[i] = (PTFRCHAR) malloc((unsigned int)
                   _imagesize(0,Yul[i], Xul[i] +
                   Xsize, Yul[i] + Ysize));
        _getimage(Xul[i],Yul[i], Xul[i] + Xsize, Yul[i] +
                  Ysize, Bufs[i]);

        }
}
void Move_figs()
{
    int i, j;
    static int dx[FIGNUM] = {0, 0, 0}; /* displacements */
    time_t tval;

    time(&tval);    /*   use the current time value   */
    srand(tval);    /*   to initialize rand()         */
    while (dx[0] < END && dx[1] < END && dx[2] < END)
        {
        for (i = 0; i < FIGNUM; i++)
            {
            /* advance the figure one position if  */
            /* rand() returns an even number       */
            if (rand() % 2 == 0)
                {
                /* erase old image */
                _putimage(dx[i] * Xsize, Yul[i],
                          Bufs[i], _GXOR);
                /* redraw in new position */
                _putimage((1 + dx[i]) * Xsize, Yul[i],
                          Bufs[i], _GPSET);
                dx[i]++;
                }
            }
        Wait(0.15);
        }
    for (j = 0; j < 5; j++)
        {
        for(i = 0; i < FIGNUM; i++)
            {
            /* flash winning figure */
            if (dx[i] >= END)
                {
                Wait(0.2);
                _putimage(dx[i] * Xsize,Yul[i],
                          Bufs[i], _GPRESET);
                Wait(0.2);
                _putimage(dx[i] * Xsize,Yul[i],
                          Bufs[i], _GPSET);
                }
            }
        }
}

void Wait(pause) /* wait for pause seconds */
double pause;
{
    struct timeb start, end;
    long delay;

    delay = 1000 * pause;  /* convert to milliseconds */
    ftime(&start);
    ftime(&end);
    while ((1000 * (end.time - start.time) +
            + end.millitm - start.millitm) < delay)
        ftime(&end);
}


/* modeinfo.c -- set modes and obtain information      */
/* Demonstrates _setvideomode() and _getvideoconfig()  */
/* If you load graphics.qlb, no program list is needed.*/

#include <conio.h>
#include <graph.h>
struct videoconfig vc;
int modes[15] ={_TEXTBW40, _TEXTC40, _TEXTBW80, _TEXTC80,
        _MRES4COLOR, _MRESNOCOLOR, _HRESBW, _TEXTMONO,
        _MRES16COLOR, _HRES16COLOR, _ERESNOCOLOR, _ERESCOLOR,
        _VRES2COLOR, _VRES16COLOR, _MRES256COLOR};
char *Adapt(short), *Display(short);
main()
{
    int i;

    for (i = 0; i < 15; i++)
    {
        if (_setvideomode(modes[i]))
        {
            _getvideoconfig(&vc);
            printf("video mode is %d\n", vc.mode);
            printf("number of columns is %d\n", vc.numtextcols);
            printf("number of colors is %d\n", vc.numcolors);
            printf("number of pages is %d\n", vc.numvideopages);
            printf("adapter is %s\n", Adapt(vc.adapter));
            printf("display is %s\n", Display(vc.monitor));
            printf("the adapter has %dK of memory\n",
                    vc.memory);
        }
        else
            printf("mode %d not supported\n", modes[i]);
        printf("strike a key for next mode\n");
        getch();
    }
    _setvideomode (_DEFAULTMODE);
}

/* Adapt() returns a pointer to a string describing   */
/* the adapter characterized by adapt_num.            */
char *Adapt(adapt_num)
short adapt_num; /* videoconfig.adapter value         */
{
    static char *anames[6] = {"Monochrome", "CGA", "EGA",
                              "MCGA", "VGA", "Not known"};
    char *point;

    switch (adapt_num)
    {
        case _MDPA : point = anames[0];
                     break;
        case _CGA  : point = anames[1];
                     break;
        case _EGA  : point = anames[2];
                     break;
        case _MCGA : point = anames[3];
                     break;
        case _VGA  : point = anames[4];
                     break;
        default    : point = anames[5];
    }
    return point;
}

/* Display() returns a pointer to a string describing  */
/* the monitor characterized by disp.                  */
char *Display(disp)
short disp;  /* videoconfig.monitor value              */
{
    static char *types[5] = {"monochrome", "color",
                             "enhanced color", "analog",
                             "unknown"};
    char *point;

    if (disp & _MONO)
        point = types[0];
    else if (disp & _COLOR)
        point = types[1];
    else if (disp & _ENHCOLOR)
        point = types[2];
    else if (disp & _ANALOG)
        point = types[3];
    else
        point = types[4];
    return point;
}

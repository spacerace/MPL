/*
** POPUP - Popup Menu/Window Driver
**
** Copyright (C) 1985 - Philip A. Mongelluzzo
** All rights reserved.
**
*/

#include "windows.h"                    /* window header */

#define UARROW  0x48                    /* define scan codes */
#define DARROW  0x50
#define LARROW  0x4b
#define RARROW  0x4d
#define BS      0x0e
#define DEL     0x53
#define RET     0x1c
#define ESC     0x01
#define SPACE   0x39

#define TRUE    1
#define FALSE   0

/*
** Popup structure templates
*/

  struct mitem {                        /* menu item template */
    int r;                              /* row */
    int c;                              /* col */
    char *t;                            /* text */
    int rv;                             /* return value */
  };

  struct pmenu {                        /* popup menu structure */
    int fm;                             /* first menu item index */
    int lm;                             /* last menu item index */
    struct mitem scrn[25];              /* a bunch of menu items */
  };

/*
*********
* popup *
*********
*/

popup(page,row,col,width,height,atrib,batrib,mx,cflag)
int page;                               /* video page */
int row, col;                           /* window - upper row/col */
int width, height;                      /* window - height & width */
struct pmenu *mx;                       /* pointer to popup menu struct */
int cflag;                              /* close on return strike flag */
int atrib, batrib;                      /* attributes - window & border */
{
int i;                                  /* scratch integer */
WINDOWPTR w;                            /* window pointer */
static WINDOWPTR wpsave;                /* a place to remember it */
int c;                                  /* key scan code,,char */
static winopn = FALSE;                  /* window currently open flag */
static lndx = 0;                        /* last open window item index */

  if(winopn) {                          /* window is still open */
    goto d0;                            /* enter processing loop */
  }
  lndx = -1;                            /* set index out of range */
  w = wn_open(page, row, col, width, height, atrib, batrib);
  wn_sync(w,TRUE);                      /* sync cursor */
  wpsave = w;                           /* save pointer */
  if (w == NULL) return(99);            /* out of mem or just fubar */
  winopn = TRUE;                        /* say window is open */

  i = 0;                                /* init index */
  while(mx->scrn[i].r) {                /* for as long as we have data */
    wn_puts(w, mx->scrn[i].r, mx->scrn[i].c, mx->scrn[i].t);
    i++;
  }

d0:
  w = wpsave;                           /* restore pointer */
  i = lndx;                             /* BLINDLY assume that we're back */
  if(i < mx->fm) i = mx->fm;            /* and reset if "i" is now out of */
  if(i > mx->lm) i = mx->fm;            /* range - (dumb, but it works) */
  while(TRUE) {                         /* till we exit */
    wn_locate(w, mx->scrn[i].r, mx->scrn[i].c);
    c = v_getch() >> 8;                 /* see whats goin' on */
    if(c == ESC) break;                 /* ESC (user wants out) - swk */
    if(c == RET) {                      /* swk RETURN */
      if(cflag) {                       /* close window on return strike ? */
        wn_close(w);                    /* close the window */
        winopn = FALSE;                 /* say window is closed */
      }
      lndx = i;                         /* remember last indx */
      return(mx->scrn[i].rv);           /* return with rv */
    }
    if(c == DARROW) c = SPACE;          /* swk conversion */
    if(c == RARROW) c = SPACE;          /* swk conversion */
    if(c == LARROW) c = BS;             /* swk conversion */
    if(c == UARROW) c = BS;             /* swk conversion */
    if(c == SPACE || c == DEL || c == BS) {
      if(c == SPACE)                    /* foreward ?? */
        i++;                            /* bump index */
      else
        i--;                            /* decrement */
      if(i < mx->fm) i = mx->lm;        /* wrap on either */
      if(i > mx->lm) i = mx->fm;        /* end */
    }                                   /* endif */
  }                                     /* end while */
  wn_close(w);                          /* bye bye */
  winopn = FALSE;                       /* say window is closed */
  return(99);                           /* nothing selected */
}

#ifdef COMMENTS                                                  
/*
**********                              /* quick popup... */
* qpopup *                              /* this is a hack of popup */
**********                              /* but the code fragment */
*/                                      /* could be of value */
#endif

WINDOWPTR qpopup(page,row,col,width,height,atrib,batrib,mx)
int page;                               /* video page */
int row, col;                           /* window - upper row/col */
int width, height;                      /* window - height & width */
int atrib, batrib;                      /* atributes - window & border */
struct pmenu *mx;                       /* pointer to text struct */
{
int i;                                  /* scratch integer */
WINDOWPTR w;                            /* window pointer */

  w = wn_open(page, row, col, width, height, atrib, batrib);

  i = 0;                                /* init index */
  while(mx->scrn[i].r) {                /* for as long as we have data */
    wn_puts(w, mx->scrn[i].r, mx->scrn[i].c, mx->scrn[i].t);
    i++;
  }
  return(w);
}

/* End */

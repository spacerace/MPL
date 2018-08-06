/*
** Demo for The Window Boss
**
** Copyright (c) 1985 - Philip A. Mongelluzzo
** All rights reserved.
**
*/

#include "windows.h"                    /* windows header file */

#if DLC
int _stack = 8192;                      /* Datalight C */
#endif

#define TRUE 1                          /* a few equates */
#define FALSE 0

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

main(argc,argv)
int argc;
char *argv[];
{
WINDOWPTR w0, w2, w3;                   /* a few windows */
WINDOWPTR tw1, tw2, tw3;                /* and a few more */

WINDOWPTR qpopup();                     /* function returns WP */
int i,j,k;                              /* scratch integers */
int watrib,batrib;                      /* scratch atributes */
int rv;                                 /* for popup */

  static struct pmenu m1 = {
    00, 00, {
    01, 02, "Presenting", 0,
    00, 00, "",99 }
  };

  static struct pmenu m2 = {
    00, 00, {
    01, 02, "The", 0,
    03, 02, "W i n d o w   B O S S", 0,
    05, 02, "Fast - Easy - Windows", 0,
    07, 02, "for the C Language", 0,
    00, 00, "",99 }
  };

  static struct pmenu intelc = {
    02, 06, {
    01, 02, "         Intellicom", 0,
    02, 02, "        Quick - Help", 0,
    04, 05, "_ General Information", 1,
    05, 05, "_ Terminal Mode Options", 2,
    06, 05, "_ XMODEM File Send", 3,
    07, 05, "_ XMODEM File Receive", 4,
#if MSC | DLC
     8, 05, "_ CompuServe Exec Mode", 5,
#else
    08, 05, "_ CompuServe Exec Mode", 5,
#endif
    10, 02, "Press: ESC to quit or Cursor", 0,
    11, 02, " Keys to Position Cursor then", 0,
    12, 02, " press RETURN for MORE info.", 0,
    00, 00, "",99 }
  };

  if(argc > 1) wn_dmaflg=FALSE;         /* for testing bios access */
  w0 = wn_save(0,0,0,78,23);            /* save entry screen */
  if(!w0) exit();                       /* less than graceful exit */

  batrib = v_setatr(WHITE,BLACK,0,BOLD);/* set border atrib */
  watrib = v_setatr(WHITE,RED,0,0);     /* make it american */
  w2 = qpopup(0,0,0,16,3,watrib,batrib,&m1);
  for(i=1; i<11; i++)                   /* move it into place */
    w2 = wn_move(w2, i, i*2); 

  hiber(1);                             /* wait 1 second (aprx) */

  batrib = v_setatr(RED,WHITE,0,0);     /* set border atrib */
  watrib = v_setatr(BLUE,WHITE,0,0);    /* make this american too */
  w3 = qpopup(0,0,0,30,9,watrib,batrib,&m2);
  for(i=1; i<8; i++)                    /* move into place */
    w3 = wn_move(w3,i,6*i);             /* move into place */

  hiber(7);                             /* reading time.. */

  wn_close(w3);                         /* close a couple */
  hiber(1);
  wn_close(w2);
  hiber(1);

  watrib = v_setatr(WHITE,BLACK,0,BOLD);
  for(i=0; i<25; i++) {                 /* build the back drop */
    v_locate(0,i,0);                    /* position cursor */
    v_wca(0, 0xb0, watrib, 80);         /* the fast way */
  }
  v_hidec();                            /* hide the cursor */

  watrib = v_setatr(WHITE,BLACK,0,0);   /* window attribute */
  batrib = v_setatr(BLUE,WHITE,0,0);    /* border attribute */
  w3 = wn_open(0,2,9,60,19,watrib,batrib);
  w3->bstyle |= BOLD;                   /* toggle bold on then off */
  wn_title(w3," The Window BOSS for C ");
  w3->bstyle ^= BOLD;
  wn_puts(w3,1,1,"The Window BOSS puts YOU in control of the most important");
  wn_puts(w3,2,1,"sophisticated state-of-the-art screen handing techniques ");
  wn_puts(w3,3,1,"available today.  Pop-up windows, pull-down menus, status ");
  wn_puts(w3,4,1,"lines and context sensitive help functions are a breeze ");
  wn_puts(w3,5,1,"to implement!  Your program automatically senses the ");
  wn_puts(w3,6,1,"video card installed and ALL your video output takes ");
  wn_puts(w3,7,1,"place with no snow, no flicker, and no delay. But best of");
  wn_puts(w3,8,1,"all, The Window BOSS is SHAREWARE software!  ");
  wn_puts(w3,9,1," ");
  wn_puts(w3,10,1,"The Window BOSS is available for the Lattice, Microsoft,  ");
  wn_puts(w3,11,1,"Datalight, and Computer Innovations CI86 Compilers.");
  wn_puts(w3,12,1,"Registered users receive complete documentation, all");
  wn_puts(w3,13,1,"source code, and complete libaries for all memory models.");
  wn_puts(w3,14,1," ");
  wn_puts(w3,15,1,"I challenge you to compare: The Window BOSS is faster");
  wn_puts(w3,16,1,"and easier to use, than most commercial products on the");
  wn_puts(w3,17,1,"market today!");
  hiber(30);

  watrib = v_setatr(WHITE,BLUE,0,0);    /* window attribute */
  batrib = v_setatr(RED,WHITE,0,BOLD);  /* border attribute */
  w2 = wn_open(0,5,5,50,10,watrib,batrib);
  if(!w2) exit();                       /* yes.. it should be better */
  wn_puts(w2,0,0," Simple text output is very fast");
  wn_puts(w2,1,0," and snow free.");
  wn_puts(w2,3,0," Watch this....");
  hiber(3);
  wn_clr(w2);                           /* clear it then fill it */
  for(i=0; i<10; i++)
    wn_puts(w2,i,0," Very fast screen output - no snow!");
  hiber(5);
  wn_clr(w2);                           /* do it again */
  for(i=0; i<10; i++)
    wn_puts(w2,i,0," Another window full of fast text.");
  hiber(5);

  wn_clr(w2);
  wn_printf(w2," The BOSS also supports a printf function!\n");
  wn_printf(w2," Its as simple as:\n\n");
  wn_printf(w2," \twn_printf(wn,\"%%d\\n\",i);\n\n");
  wn_printf(w2," to print a value in a window.\n\n");
  wn_printf(w2," Scrolling works too\n");
  wn_printf(w2," Lets count (slowly) to 10...\n");
  wn_sync(w2, TRUE);
  hiber(10);
  for(i=1;i<11;i++) {
    wn_printf(w2," The count is: %d\n", i);
    nap(5);
  }
  wn_printf(w2," Ok, lets see what it looks like at\n");
  wn_printf(w2," full speed.. (counting to 25)\n");
  hiber(5);
  for(i=1;i<26;i++)
    wn_printf(w2," The count is: %d\n", i);
  hiber(5);                             /* reading time */
  wn_sync(w2, FALSE);
  v_hidec();

  wn_clr(w2);
  wn_putsa(w2,0,0," Full color support is provided.",WHITE<<4);
  for(i=1; i<6; i++)
    wn_putsa(w2,i,0, " Add a little color to your life.",WHITE<<4|i);
  wn_putsa(w2,6,0," As is window movement....watch...",WHITE<<4|BLACK);
  hiber(5);

  for(i=0; i<13; i++)                   /* move the window around */
    w2 = wn_move(w2, i, i*2); 
  hiber(5);
  wn_printf(w2,"\n Lets put it back where we started...\n");
  hiber(3);
  w2 = wn_move(w2,5,5); 
  wn_printf(w2," Now, lets add a title to our window!\n");
  hiber(3);
  w2->bstyle |= BOLD;
  wn_title(w2," Pretty Powerful Stuff ");
  w2->bstyle ^= BOLD;
  hiber(5);

/* ============================= Tiled Window Stuff ===================== */

  wn_printf(w2," Tiled windows are supported\n");
  wn_printf(w2," Lets add three.\n");
  hiber(5);

  wn_sbit=FAST;
  wn_blank=FALSE;
  tw1 = wn_open(0,3,5,40,10, RED<<4|YELLOW|BOLD, RED<<4|WHITE|BOLD);
  wn_sync(tw1,TRUE);
  wn_printf(tw1,"ONE\n");
  tw2 = wn_open(0,6,8,40,10, BLUE<<4|WHITE|BOLD, BLUE<<4|GREEN|BOLD);
  wn_sync(tw2,TRUE);
  wn_printf(tw2,"TWO\n");
  tw3 = wn_open(0,9,11,40,10,GREEN<<4|WHITE|BOLD, GREEN<<4|WHITE|BOLD);
  wn_sync(tw3,TRUE);
  wn_printf(tw3,"THREE\n");
  wn_printf(tw3," Press a key and watch TWO move\n");
  wn_printf(tw3," To the top!!\n");
  v_getch();

  wn_activate(tw2);
  wn_printf(tw2," Press a key and watch THREE move\n");
  wn_printf(tw2," To the top!!\n");
  v_getch();
  wn_activate(tw3);
  wn_printf(tw3," Press a key and watch ONE move\n");
  wn_printf(tw3," To the top!!\n");
  v_getch();
  wn_activate(tw1);

  wn_printf(tw1, " Ok..Press a key\n");
  wn_printf(tw1, " and we will continue with the show!");
  v_getch();

  wn_close(tw1);
  wn_close(tw3);
  wn_close(tw2);
  wn_sbit=SLOW;
  wn_blank=TRUE;

/* ========================= End Tiled Stuff ============================ */

  wn_clr(w2);
  wn_printf(w2," Popup Menus are a breeze.\n\n");
  wn_printf(w2," Watch....");
  hiber(2);

  help_init("intelc");                  /* init help function */
  do {
    rv = popup(0,0,0,33,14, WHITE<<4|BLACK, BLUE<<4|WHITE, &intelc,FALSE);

    switch (rv) {                       /* dispatch */
      case 1:
        help("%general information\r\n");
        break;
      case 2:
        help("%terminal\r\n");
        break;
      case 3:
        help("%checksum xmit1\r\n");
        break;
      case 4:
        help("%checksum recv1\r\n");
        break;
      case 5:
        help("%cistty1\r\n");
        break;
      case 99:
      default:
        break;
    }
  } while(rv !=99);

  wn_clr(w2);                           /* clear the window */
  w2->bstyle |= BOLD;                   /* toggle bold on then off */
  wn_title(w2," The Window BOSS for C ");
  w2->bstyle ^= BOLD;
  wn_printf(w2,"\n\n  The following is a quick summary\n");
  wn_printf(w2,"  of SOME of the functions available...");
  hiber(4);

  help("%bossinfo\r\n");                /* provide some details */

  wn_clr(w2);                           /* and now the credits */
  wn_printf(w2,"\n\n\n");
  wn_printf(w2,"                  The Window BOSS\n");
  wn_printf(w2,"                Copyright (c) 1985\n");
  wn_printf(w2,"              Philip  A. Mongelluzzo\n");
  wn_printf(w2,"                 273 Windy Drive\n");
  wn_printf(w2,"              Waterbury, Conn  06705\n");

  wn_printf(w2,"\n\t\t\t\tThats all folks...\n\n");

  hiber(10);                            /* all done ! */
  wn_close(w2);
  wn_close(w3);
  wn_restore(w0);
}

nap(ticks)                              /* sleep a few clock ticks */
int ticks;
{
int tc;
int ts, te;

#if CI86 | MSC | MWC                    /* MS , CI86, or MWC */
  ts = peek(0x6c,0x40);
  for(tc=0; tc<ticks; ) {
    if(chkesc()) return;
    te = peek(0x6c,0x40);
    if(ts == te) continue;
    tc++;
    ts = te;
  }
#else                                   /* lattice */
  peek(0x40, 0x6c, &ts, 2);
  for(tc=0; tc<ticks; ) {
    if(chkesc()) return;
    peek(0x40, 0x6c, &te, 2);
    if(ts == te) continue;
    tc++;
    ts = te;
  }
#endif
}

hiber(seconds)
int seconds;
{
  nap(seconds * 18);
}

chkesc()                                /* check for ESC while napping */
{
int c;

  if(v_kstat()) {
    c=v_getch() & 0x7f;
    switch(c) {
      case ESC:
        return(TRUE);
      case ETX:
        v_cls(7);
        v_locate(0,0,0);
        exit(0);
      default:
        break;
    }
  }
  return(FALSE);
}

/* End */


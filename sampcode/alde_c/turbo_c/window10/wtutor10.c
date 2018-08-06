#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dos.h>
#include <conio.h>
#include <w1.h>
#include <windprot.h>
#include <color.h>

#define RETKEY       13
#define ESCKEY       27
#define TRUE         1
#define FALSE        0

typedef char str80[80];
extern wndwstattype wndwstat[MAXWNDW];/*  window stats  */
extern byteptr      wndwptr[MAXWNDW];/*  pointer to window on heap  */
extern byte         li, licurrent;/*  level index, level index for swapping windows  */

void         show30windows(void);
void         showinitialscreen(void);
void         showgeneral(void);
void         showlist(void);
void         showinitwindow(void);
void         showborders(void);
void         showremovewindow(void);
void         showmakewindow(void);
void         showflicker(void);
void         showscrollwindow(void);
void         showtitlewindow(void);
void         showspecialeffects(void);
void         showmaxwndw(void);
void         showqwik21(void);
void         showprogramming(void);
void         showeditor(void);
void         readkb(int *);
void         getkey(void);
void         findnextstep(void);
void         showwindows(void);
int          random(int);


    int          i, j, k, m;
    int          oldcursor, battr, step ;
    int          key;

    static str80        stra[16] = {
      "",
      "The windowing functions in  the file  WINDOW10.LIB",
      "are  a  set of public domain routines  that  allow",
      "Turbo C  to  create  incredibly fast multi - level",
      "windows.   Created  under  the  Teamware  concept,",
      "WINDOW10.LIB makes use of the quick screen writing",
      "utilities  of Jim H. LeMay -- which were converted",
      "from Turbo Pascal inline to external assembly. The",
      "windows may  be  of any  size  (from 2x2 to screen",
      "limits)  and  color  and  may  have one of several",
      "different  border  styles and colors including  no",
      "border.  They  also   work   in any   column  mode",
      "40/80/etc.  These routines automatically configure",
      "to your video card and  video  mode. All of  these",
      "routines were converted from Turbo Pascal to Turbo",
      "C, (SMALL MODEL) with the authors permission.     "};

    static str80        strb[11] = {
      "The window function package",
      "consists of five functions;",
      "",
      "      initwindow()",
      "      makewindow()",
      "      scrollwindow()",
      "      removewindow()",
      "      titlewindow()",
      "",
      "The functions are described",
      "in the following windows:"};

    static str80        strc[8] = {
      "initwindow",
      "",
      "FORMAT:   void initwindow (int windowattribute);",
      "", 
      "Initwindow initializes  several variables required  by  the",
      "windowing package,  as well as selecting the foreground and",
      "background colors of the initial screen display. Initwindow",
      "must be called before using any of the other functions.    "}; 

    static str80        strd[17] = {
      "makewindow", 
      "",
      "FORMAT:  void makewindow (int row,col,rows,cols,wattr,battr;",
      "                                            BORDERS brdrsel);",
      "",
      "Makewindow puts a new blank window on the display.  The window starts", 
      "at the upper left corner (row,col)  and extends for a number of  rows",
      "and columns (rows,cols).   If a border exists, the actual  dimensions",
      "of the Turbo C window will be 2 less than indicated in the makewindow",
      "statement.  The border can be one of the following: (SEE w1.h)",
      "",
      "   nobrdr     - just window       evensolidbrdr - evenly solid",
      "   blankbrdr  - blank spaces      thinsolidbrdr - thin solid line",
      "   singlebrdr - single line       lhatchbrdr - light hatch",
      "   doublebrdr - double line       mhatchbrdr - medium hatch",
      "   mixedbrdr  - single/double     hhatchbrdr - heavy hatch",
      "   solidbrdr  - solid             userbrdr   - user defined border"};

    static str80        stre[6] = {
      "removewindow",
      "FORMAT:   void removewindow(void);",
      "",
      "Removewindow removes the last window",
      "remaining  on  the screen  from  the",
      "makewindow function."};

    static str80        strf[2] = {
      "See if your BIOS gives you flicker",
      "when your screen rolls down next ..."};

    static str80        strg[19] = {
      "",
      "    scrollwindow",
      "",
      "",
      "FORMAT:  void scrollwindow (byte rowbegin,rowend byte; DIRTYPE dir);",
      "",
      "The BIOS scroll that was used for this call may have worked just fine",
      "for this window.  However, if  your screen just had some bad flicker ",
      "as it was scrolling down, your BIOS is not flicker-free.  To keep the",
      "display flicker-free,  to work on other  video pages  or an EGA, then",
      "you will need this function.  The upward  scroll, used scrollwindow, ",
      "so no flicker was seen then.  It also scrolls partial windows.",
      "",
      "The direction of the scroll can be any of the following:(SEE w1.h)",
      "",
      "    'up'   - to scroll up",
      "    'down' - to scroll down",
      "",
      ""};

    static str80        strh[6] = {
      "titlewindow",
      "FORMAT:   void titlewindow (justify;title);",
      "                   int justify; title str80",
      "Titlewindow places a title in the top border",
      "of the current window.  Justify permits left,",
      "center or right justification of the title."};

    static str80        stri[18] = {
      "There are now two special effects that can",
      "enhance the window display:",
      "   zoomeffect   - emulates the MACINTOSH.",
      "   shadoweffect - places a left or right",
      "              shadow underneath the menu.",
      "These global variables can be placed anywhere",
      "in your program.  zoomEffect is INT while",
      "shadoweffect is of DIRtYPE.",
      "                             press RETURN ...",
      "CGA:",
      "The CGA is self-regulating and controls the",
      "zoom rate.",
      "MDA and EGA:",
      "These video cards are quite fast and need a",
      "delay for the effect.  A default value of 11",
      "milliseconds is used in a global INT",
      "named 'zoomdelay' and shouldn't need any",
      "adjustment."};

    static str80        strj[5] = {
      " The maximum  number  of windows that",
      " may be on the screen at any one time",
      " is   specified   by   the   constant",
      " \"MAXWNDW\".The compiled library uses",
      " this value so do NOT change it......"};

    static str80        strk[9] = {
      " In addition to windows, there are 15 powerful",
      " Q screen write functions  you can use:",
      "      qwritelv   qfillc     qstore",
      "      qwritev    qfill      qrestore",
      "      qwrite     qattr      qpage",
      "      qwritec    qattrc     qwritepage",
      "      qwritecv   gotorc     cursorchange ",
      " In WINDOW10.ARC, compile and run QDEMO.C to see",
      " all of these features."};

    static str80        strl[20] = {
      " The functions are used as follows:",
      "",
      "  ....Your include files....",
      "  #include <w1.h> ",
      "  #include <windprot.h> ",
      "  #include <color.h> ",
      "  ...Your variables and functions...",
      "  main(argc,argv)",
      "    {",
      "    initwindow (windowattribute);",
      "    makewindow (row,col,rows,cols,wattr,battr,",
      "                                     brdrsel);",
      "    titlewindow (justify,'the window title');",
      "    removewindow();",
      "    }",
      "  end.",
      "",
      " { Use one removewindow for each makewindow. }",
      " { IMPORTANT: Remember to link in the proper }",
      " {            WINDOW1x.lib library .         }"};

    static str80        strm[7] = {
      "If you have any questions or comments,",
      "please write to, or drop a note (CIS:)",
      "",
      "        Michael G. Mlachak",
      "        4318 Stewart Court.",
      "        East Chicago, IN 46312",
      "        CIS - 76327,1410"};

    static str160       title = " TURBO-C WINDOWS (Continued) ";
    static int          hidecursor = 0x2020; /*  Value to hide cursor  */


void main(void)
{
   int dummy_li;

   initwindow(wiattr(BLUE, CYAN));
   oldcursor = cursorchange(hidecursor);
   gotorc(1, 1);
   show30windows();
   showinitialscreen();
   showwindows();
   /*  -- Use the following statment to return to the original screen.--  */
   dummy_li = li;
   for (i = 1; i <= dummy_li; i++)
      removewindow();
   qfill(1, 1, 25, 80, 112, ' ');
   qwritec(12, 1, 80, 112, "This concludes the windows tutorial...");
   sleep(2);
   oldcursor = cursorchange(oldcursor);
}  /* of main */

void         show30windows()
{
   /*  -- Throw out 30 windows 4 times --  */
   int hold_li;

   for (m = 1; m <= 4; m++) {
     for (i = 1; i <= 28; i++) {
       j = random(70);
       k = random(21);
       makewindow(k + 1, j + 1, 5, 11, wiattr(YELLOW, RED), wiattr(YELLOW, RED), lhatchbrdr);
       qwrite(k + 3, j + 3, -1, "WINDOWS");
     }
     hold_li = li;
     if (m == 4)
        sleep(3);
     else
       sleep(1);
     for (i = 1; i <= hold_li; i++) {
       removewindow();
     }
   } /* for m= 1; */
} /* of show30windows */

void         showinitialscreen()
{
   /*  -- Create initial screen --  */
   qwritec(1, 1, 80, -1, "TURBO C - Windows Tutorial");
   qwritec(2, 1, 80, -1, "Version 1.0 -- (C) COPYRIGHT 1987 Michael G. Mlachak");
   qwritec(3, 1, 80, -1, "Original TEAMWARE concept (C) COPYRIGHT Jim H. LeMay, Michael Burton");
   qwritec(5, 1, 80, 112, "For each of the following displays:");
   qwrite(6, 26, 112, "1. Press RETURN to continue.");
   qwrite(7, 26, 112, "2. Press ESC to back up.");
   qwritec(12, 1, 80, 112, "This is the original screen without windows.");
   step = 0;
}  /* of showinitialscreen() */

void         showgeneral()
{
   /*  -- General description --  */
   shadoweffect = right;
   makewindow(4, 15, 19, 52, wiattr(BLACK, GREEN), wiattr(BLACK, GREEN), mixedbrdr);
   titlewindow(center, " TURBO-C WINDOWS ");
   for (j = 1; j <= 16; j++)
     qwritev(wndwstat[li].wsrow + j, wndwstat[li].wscol + 1, -1, stra[j-1]);
   shadoweffect = nodir;
}  /* showgeneral */

void         showlist() 
{ 
   /*  -- List of Procedures --  */
   makewindow(5, 2, 15, 33, wiattr(BLACK, CYAN), wiattr(WHITE, BLUE), mhatchbrdr); 
   titlewindow(center, " Five Functions ");
   for (j = 1; j <= 2; j++)
      qwritev(wndwstat[li].wsrow + 1 + j, wndwstat[li].wscol + 2, -1, strb[j-1]);
   for (j = 3; j <= 9; j++)
      qwritev(wndwstat[li].wsrow + 1 + j, wndwstat[li].wscol + 2, wiattr(WHITE, CYAN), strb[j-1]);
   for (j = 10; j <= 11; j++)
      qwritev(wndwstat[li].wsrow + 1 + j, wndwstat[li].wscol + 2, -1, strb[j-1]);
}   /* showlist() */

void         showinitwindow() 
{ 
   /*  -- InitWindow description --  */ 
   makewindow(11, 17, 12, 63, wiattr(YELLOW, RED), wiattr(YELLOW, RED), singlebrdr);
   titlewindow(left, title); 
   qwritev(wndwstat[li].wsrow + 2, wndwstat[li].wscol + 2, wiattr(WHITE, BLUE), strc[0]);
   for (j = 2; j <= 4; j++)
      qwritev(wndwstat[li].wsrow + 1 + j, wndwstat[li].wscol + 2, wiattr(CYAN, RED), strc[j-1]);
   for (j = 5; j <= 8; j++)
      qwritev(wndwstat[li].wsrow + 1 + j, wndwstat[li].wscol + 2, wiattr(WHITE, RED), strc[j-1]);
} /* showinitwindow() */

void         showmakewindow()
{ 
   /*  -- MakeWindow description --  */ 
   makewindow(4, 7, 21, 71, wiattr(LIGHTMAGENTA, BLUE), wiattr(LIGHTMAGENTA, BLUE), solidbrdr);
   titlewindow(left, title);
   qwritev(wndwstat[li].wsrow + 2, wndwstat[li].wscol + 5, wiattr(YELLOW, RED), strd[0]);
   for (j = 2; j <= 4; j++)
      qwritev(wndwstat[li].wsrow + 1 + j, wndwstat[li].wscol + 1, wiattr(LIGHTRED, BLUE), strd[j-1]);
   for (j = 5; j <= 17; j++)
      qwritev(wndwstat[li].wsrow + 1 + j, wndwstat[li].wscol + 1, wiattr(YELLOW, BLUE), strd[j-1]);
} /* showmakewindow () */

void         showborders()
{
   /*  -- Show different borders --  */
   for (i = 0; i <= 5; i++)  {
     if (i == 1)
        battr = wiattr(YELLOW, YELLOW);
     else
        battr = wiattr(YELLOW, GREEN);
     makewindow(2 + 2 * i, 13 + 3 * i, 5, 10, wiattr(YELLOW, GREEN), battr, i);
   }
   /* with Brdr[UserBrdr] DO  { **/
/*   {
     TL = '\017';
     TH = '\017';
     TR = '\017';
     LV = 'Þ';
     RV = 'Ý';
     BL = '\017';
     BH = '\017';
     BR = '\017';
   }
    }  END WITH */
   for (i = 6; i <= 11; i++) {
     makewindow(24 - 2 * i, 24 + 3 * i, 5, 10, wiattr(YELLOW, GREEN), battr, i);
   }
}  /* showborders () */

void         showremovewindow()
{
   /*  -- RemoveWindow description --  */
   makewindow(11, 20, 11, 40, wiattr(BLACK, BROWN), wiattr(BLACK, BROWN), doublebrdr);
   titlewindow(left, title);
   qwritev(wndwstat[li].wsrow + 2, wndwstat[li].wscol + 5, wiattr(WHITE, MAGENTA), stre[0]);
   qwritev(wndwstat[li].wsrow + 4, wndwstat[li].wscol + 2, wiattr(YELLOW, BROWN), stre[1]);
   for (j = 3; j <= 6; j++)
      qwritev(wndwstat[li].wsrow + 2 + j, wndwstat[li].wscol + 2, wiattr(WHITE, BROWN), stre[j-1]);
} /* showremoveeindow() */

void         showflicker()
{
   /*  -- Flicker Note --  */
   makewindow(11, 22, 4, 40, wiattr(BLACK, BROWN), wiattr(BLACK, BROWN), mixedbrdr);
   for (j = 1; j <= 2; j++)
     qwritev(wndwstat[li].wsrow + j, wndwstat[li].wscol + 2, -1, strf[j-1]);
}  /* showflicker() */

void         showscrollwindow()
{
   /*  -- ScrollWindow description --  */
     gotorc(wndwstat[li].wsrow + 1, wndwstat[li].wscol + 1);
     for (j = 1; j <= 19; j++)
        window(8,5,76,22,wiattr(LIGHTMAGENTA, BLUE));
     for (j = 1; j <= 19; j++) {
        scrollwindow(1, 19, up);
        qwritev(wndwstat[li].wsrow + 19, wndwstat[li].wscol + 1, -1, strg[j-1]);
     }
}  /* showscrollwindow() */

void         showtitlewindow()
{
   /*  -- TitleWindow description --  */
   makewindow(12, 16, 11, 49, wiattr(RED, LIGHTGRAY), wiattr(RED, LIGHTGRAY), evensolidbrdr);
   titlewindow(left, title); 
   qwritev(wndwstat[li].wsrow + 2, wndwstat[li].wscol + 5, wiattr(LIGHTGREEN, BLUE), strh[0]);
   qwritev(wndwstat[li].wsrow + 4, wndwstat[li].wscol + 2, wiattr(BLACK, LIGHTGRAY), strh[1]);
   for (j = 3; j <= 6; j++)
      qwritev(wndwstat[li].wsrow + 2 + j, wndwstat[li].wscol + 2, wiattr(GREEN, LIGHTGRAY), strh[j-1]);
} /* showtitlewindow() */

void         showspecialeffects() 
{ 
   /*  -- Special effects description --  */ 
   if (qwait)
      shadoweffect = right; 
   makewindow(6, 25, 14, 49, wiattr(BLACK, GREEN), wiattr(WHITE, GREEN), doublebrdr); 
   titlewindow(center, " Special Effects "); 
   for (j = 1; j <= 2; j++)
      qwritev(wndwstat[li].wsrow + 1 + j, wndwstat[li].wscol + 2, -1, stri[j-1]);
   for (j = 3; j <= 5; j++) 
      qwritev(wndwstat[li].wsrow + 2 + j, wndwstat[li].wscol + 2, wiattr(WHITE, GREEN), stri[j-1]);
   for (j = 6; j <= 9; j++)
      qwritev(wndwstat[li].wsrow + 3 + j, wndwstat[li].wscol + 2, -1, stri[j-1]);
   /**scanf("\n");  **/
   qfill(wndwstat[li].wsrow + 2, wndwstat[li].wscol + 2, 11, 45, wiattr(BLACK, GREEN), ' ');
   qwritev(wndwstat[li].wsrow + 2, wndwstat[li].wscol + 2, wiattr(WHITE, GREEN), stri[9]);
   for (j = 11; j <= 12; j++)
      qwritev(wndwstat[li].wsrow + j - 8, wndwstat[li].wscol + 2, -1, stri[j-1]);
   qwritev(wndwstat[li].wsrow + 6, wndwstat[li].wscol + 2, wiattr(WHITE, GREEN), stri[12]);
   for (j = 14; j <= 18; j++)
      qwritev(wndwstat[li].wsrow + j - 7, wndwstat[li].wscol + 2, -1, stri[j-1]);
   shadoweffect = nodir; 
} /* showspecialeffects() */

void         showmaxwndw() 
{ 
   /*  -- MaxWndw constant --  */ 
   makewindow(17, 9, 7, 38, wiattr(BLACK, BROWN), wiattr(BLACK, BROWN), nobrdr); 
   titlewindow(center, "- WINDOW LIMITS -"); 
   for (j = 1; j <= 5; j++)
      qwritev(wndwstat[li].wsrow + j, wndwstat[li].wscol, -1, strj[j-1]);
} /* showmaxwndw() */

void         showqwik21() 
{ 
   /*  -- QWIK21.INC procedures --  */
   makewindow(8, 20, 15, 51, wiattr(YELLOW, RED), wiattr(YELLOW, RED), evensolidbrdr); 
   titlewindow(center, " Utility Functions ");
   for (j = 1; j <= 2; j++)
      qwritev(wndwstat[li].wsrow + 1 + j, wndwstat[li].wscol + 1, -1, strk[j-1]);
   for (j = 3; j <= 7; j++)
      qwritev(wndwstat[li].wsrow + 2 + j, wndwstat[li].wscol + 1, wiattr(WHITE, RED), strk[j-1]);
   for (j = 8; j <= 9; j++) 
      qwritev(wndwstat[li].wsrow + 3 + j, wndwstat[li].wscol + 1, -1, strk[j-1]);
} /* showqwik21() */

void         showprogramming() 
{ 
   /*  -- Programming for WINDOW33.INC --  */ 
   makewindow(2, 25, 23, 50, wiattr(YELLOW, MAGENTA), wiattr(YELLOW, MAGENTA), mhatchbrdr); 
   for (j = 1; j <= 20; j++)
      qwritev(wndwstat[li].wsrow + 1 + j, wndwstat[li].wscol + 1, -1, strl[j-1]);
} /* showprogramming() */

void         showeditor() 
{ 
   /*  -- Editor for WINDOW33.INC --  */ 
   if (qwait)
      shadoweffect = right; 
   makewindow(8, 20, 11, 42, wiattr(BLACK, BROWN), wiattr(BLACK, BROWN), hhatchbrdr); 
   titlewindow(center, " Teamware Editor: "); 
   for (j = 1; j <= 7; j++)
      qwritev(wndwstat[li].wsrow + 1 + j, wndwstat[li].wscol + 2, wiattr(WHITE, BROWN), strm[j-1]);
   shadoweffect = nodir; 
}  /* showeditor */

void        readkb(key)
int         *key; 
{ 

  int t_key;

  t_key = getch();
  if (t_key ==0)     /* extended character hit */
     t_key = getch();
   *key = t_key;
} /* readkb() */

void         getkey() 
{ 
  do { 
     readkb(&key);
  }  while (!((key == RETKEY) || (key == ESCKEY)));
}  /* getkey() */

void         findnextstep()
{
  switch (key) {
      case ESCKEY:
         if (step > 0) {
             removewindow();
             switch (step) {
                case 5:
                        for (j = 1; j <= 11; j++)
                           removewindow();
                break;
                case 7:
                       showborders();
                       showremovewindow();
                break;
                case 8:
                       step = 4;
                break;
             } /*  switch step  */
            step = step - 1;
         } /* if step > 0 */
      break;

      case RETKEY:
         step = step + 1;
         if ((step >=9) && (step <=13))
            zoomeffect = TRUE;
         else
            zoomeffect = FALSE;
         switch (step) {
            case 7:
                   for (i = 0; i <= 12; i++)
                      removewindow();
            break;
            case 8:
                   removewindow();
            break;
         } /* switch step */
      break;
  } /*  switch key  */
}  /* findnextstep() */

void         showwindows(void)
{
  do {
    getkey();
    findnextstep();
    if (key == RETKEY) {
       switch (step) {
          case 1:
                 showgeneral();
          break;
          case 2:
                 showlist();
          break;
          case 3:
                 showinitwindow();
          break;
          case 4:
                 showmakewindow();
          break;
          case 5:
                 showborders();
          break;
          case 6:
                 showremovewindow();
          break;
          case 7:
                 showflicker();
          break;
          case 8:
                 showscrollwindow();
          break;
          case 9:
                 showtitlewindow();
          break;
          case 10:
                 showspecialeffects();
          break;
          case 11:
                 showmaxwndw();
          break;
          case 12:
                 showqwik21();
          break;
          case 13:
                  showprogramming();
          break;
          case 14:
                  showeditor();
          break;
      } /* switch step */
    } /* if key ==RETKEY */
   }  while (step != 15) ;
}  /* showindows */

int random(seed)
{
  int hold;

     hold= rand();
     if (hold > 127)
       hold = hold % 127;
     hold = hold % seed;
   return(hold);
}

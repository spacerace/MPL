/*

        TCXLDEMO.C  - A demonstration program of the TCXL function library.

                        (c) 1987, 1988  by Mike Smedley

        To compile:

            from the command line type:  tcc tcxldemo tcxl.lib
                                              or:
                                         make -ftcxldemo

                                    or:

            from the environment, specify 'TCXLDEMO.PRJ' as the Project Name

*/

#include <conio.h>
#include <dos.h>
#include <process.h>
#include <stdio.h>
#include "tcxldef.h"
#include "tcxlkey.h"
#include "tcxlvid.h"
#include "tcxlwin.h"

void interrupt exit_prog(void);
void error_exit(void);
void pulldown_demo(void);

int row,col,*sbuf1;                                    /*  global variables  */

main() 
{
    register int i,j;
    int ch;
    int whandle[10];
    int *sbuf2;
    char *fname="        ";
    char *mi=" ";
    char *lname="              ";
    char *phone="          ";
    char *pressakey="Press a key";
    char *spaces=  "           ";


    videoinit();                    /*  initialize video  */
    sbuf1=ssave();                  /*  save current screen contents  */
    if(!sbuf1) error_exit();
    readcur(&row,&col);             /*  save current cursor position  */
    setcursz(32,0);                 /*  turn off cursor  */
    setvect(0x23,exit_prog);        /*  redirect Ctrl-Break interrupt  */
    setcbrk(ON);                    /*  set Ctrl-Break checking on  */
    for(;;) {

        /*  open up opening screen windows  */

        for(i=0;i<6;i++) {
            whandle[i]=wopen(i,i+10,i+13,i+47,3,LCYAN|_BLUE);
            if(!whandle[i]) error_exit();

            /*  sound tones of increasing pitch  */

            sound_(15000-((i+2)*1500),1);
            sound_(12000-((i+1)*1500),1);
        }

        wprints(0,16,LRED,"TCXL");
        wprints(1,2,LGREEN,"An Extended Library of Functions");
        wprints(2,12,LGREEN,"for Turbo C");
        wprints(4,10,YELLOW,"by Mike Smedley");
        wprints(6,0,LMAGENTA,"(c) 1987, 1988 - All Rights Reserved");
        wprints(8,7,WHITE,"Demonstration Program");
        wprints(10,7,LCYAN|_BLUE,"Press any key to begin");

        /*  change colors of prompt until a key is pressed  */

        i=LBLUE;
        for(;;) {
            gotoxy_(16,22);
            setattr(i|_BLUE,23);
            delay_(2);
            if(kbhit()) {
                ch=getch();
                break;
            }
            i++;
            if(i>WHITE) i=LBLUE;
        }
        if(ch==ESC) exit_prog();             /*  see if Esc key was pressed  */
        wprints(10,7,_BLUE,"                       ");

        /*  scroll the window's text up and out of the window  */

        for(i=0;i<10;i++) {
            wscroll(1,UP);
            delay_(2);
        }

        /*  close all open windows */

        for(i=5;i>=0;i--) {
            wclose();
            delay_(2);
        }


        /*  open demo window 0 and demonstrate text wraparound and
            scrolling from within a window                          */

        whandle[0]=wopen(1,1,11,41,0,LMAGENTA|_RED);
        if(!whandle[0]) error_exit();
        wtextattr(LCYAN|_RED);
        wprintf("\n TCXL's window display functions like"
                "\n wprintf() & wputs() allow wrap-around"
                "\n and scrolling from within windows.");
        wprints(7,14,YELLOW|BLINK|_RED,pressakey);
        if(waitkeyt(273)==ESC) exit_prog();
        wprints(7,14,_RED,spaces);
        wgotoxy(8,0);
          for(i=0;i<15;i++) {
            if(kbhit()) {
                if(getch()==ESC) exit_prog();
                break;
            }
            for(j=DGREY;j<=WHITE;j++) {
                wtextattr(j|_RED);
                wputs("TCXL   ");
                delay_(1);
            }
        }


        /*  open demo window 1 and demonstrate changing of window attribute  */

        whandle[1]=wopen(14,35,23,65,2,YELLOW|_BLUE);
        if(!whandle[1]) error_exit();
        wtextattr(LCYAN|_BLUE);
        wputs("\n  The wchgattr() function"
              "\n  allows you to change the"
              "\n  attribute of the active"
              "\n  window.");
        wprints(6,9,YELLOW|BLINK|_BLUE,pressakey);
        if(waitkeyt(273)==ESC) exit_prog();
        wprints(6,9,_BLUE,spaces);
        wchgattr(WHITE|_GREEN);
        delay_(12);
        wchgattr(LMAGENTA|_RED);
        delay_(12);
        wchgattr(LCYAN|_BLUE);
        if(kbhit()) {                               /*  check for keypress  */
            if(getch()==ESC) exit_prog();
        }
        delay_(12);
        wchgattr(YELLOW|_MAGENTA);
        delay_(12);
        wchgattr(WHITE|_LGREY);
        delay_(12);

        /*  open demo window 2 and demonstrate window titles
            and resizing of windows                           */

        whandle[2]=wopen(5,20,19,45,3,LCYAN|_GREEN);
        if(!whandle[2]) error_exit();
        wtextattr(WHITE|_GREEN);
        wputs("\n  The wtitle() function"
              "\n  allows you to give a"
              "\n  window a title.");
        wprints(5,6,YELLOW|BLINK|_GREEN,pressakey);
        if(waitkeyt(273)==ESC) exit_prog();
        wprints(5,6,_GREEN,spaces);
        wtitle("[ My Title ]",TLEFT);
        delay_(18);
        wtitle("[ My Title ]",TRIGHT);
        delay_(18);
        wtitle("[ My Title ]",TCENTER);
        wtextattr(LGREEN|_GREEN);
        wputs("\n\n\n  The wsize() function"
                  "\n  allows you to resize"
                  "\n  the active window.");
        wprints(10,6,YELLOW|BLINK|_GREEN,pressakey);
        if(waitkeyt(273)==ESC) exit_prog();
        wprints(10,6,_GREEN,spaces);
        wsize(19,60);
        delay_(12);;
        wsize(19,75);
        delay_(12);;
        wsize(24,75);
        delay_(12);;
        wsize(24,60);
        delay_(12);;
        wsize(24,45);
        delay_(12);;
        wsize(19,45);
        delay_(12);;


        /*  open demo window 3 and demonstrate multi-field keyboard input  */

        whandle[3]=wopen(12,10,23,50,1,LCYAN|_BLUE);
        if(!whandle[3]) error_exit();
        wtextattr(LMAGENTA|_BLUE);
        wputs("\n    The winpdef() and winpread()"
              "\n    functions allow multi-field"
              "\n    keyboard input.");
        wprints(3,21,YELLOW|BLINK|_BLUE,pressakey);
        if(waitkeyt(273)==ESC) exit_prog();
        wprints(3,21,_BLUE,spaces);
        wprints(5,2,LCYAN|_BLUE,"First name?");
        if(winpdef(5,15,fname,'M',BLACK|_LGREY)) error_exit();
        wprints(5,26,LCYAN|_BLUE,"MI?");
        if(winpdef(5,31,mi,'U',BLACK|_LGREY)) error_exit();
        wprints(7,2,LCYAN|_BLUE,"Last name?");
        if(winpdef(7,15,lname,'M',BLACK|_LGREY)) error_exit();
        wtextattr(LGREEN|_BLUE);
        setcursz(6,7);                          /*  turn cursor on  */
        i=winpread();
        setcursz(32,0);                         /*  turn cursor back off  */
        if(i) exit_prog();                      /*  check for Esc press  */


        /*  open demo window 4 and demonstrate controlled keyboard input  */

        whandle[4]=wopen(8,41,16,78,3,WHITE|_MAGENTA);
        if(!whandle[4]) error_exit();
        wtextattr(LGREEN|_MAGENTA);
        wputs("\n  For complete control of keyboard"
              "\n  input, there is the winputsf()"
              "\n  function.  ");
        wprints(3,13,YELLOW|BLINK|_MAGENTA,pressakey);
        if(waitkeyt(273)==ESC) exit_prog();
        wprints(3,13,_MAGENTA,spaces);
        wtextattr(WHITE|_MAGENTA);
        setcursz(6,7);                                   /*  turn cursor on  */
        i=winputsf(phone," '\n\n  Phone number?  ' !R-! "
            " !-! '(' !+! ### !-! ') ' !+! ### !-! '-' !+! #### ");
        setcursz(32,0);                            /*  turn cursor back off  */
        if(i) exit_prog();                      /*  check for Esc press  */

        /*  clear window 4 and demonstrate text line drawing  */

        wclear();
        wsize(22,78);
        wtextattr(LGREEN|_MAGENTA);
        wputs("\n The whline() & wvline() functions"
              "\n are used for drawing 'smart' text"
              "\n lines inside the active window.");
        wprints(5,12,YELLOW|BLINK|_MAGENTA,pressakey);
        if(waitkeyt(273)==ESC) exit_prog();
        wprints(5,12,_MAGENTA,spaces);
        wtextattr(LCYAN|_MAGENTA);
        whline(6,2,23,3);
        delay_(18);
        wvline(5,5,7,3);
        delay_(18);
        whline(10,1,25,3);
        delay_(18);
        wtextattr(LBLUE|_MAGENTA);
        if(kbhit()) {                               /*  check for keypress  */
            if(getch()==ESC) exit_prog();
        }
        wvline(6,25,5,3);
        delay_(18);
        whline(8,5,21,3);
        delay_(18);
        wvline(5,16,6,3);
        delay_(18);
        wtextattr(LGREEN|_MAGENTA);
        whline(7,2,15,3);
        delay_(18);
        whline(5,9,15,3);
        if(waitkeyt(108)==ESC) exit_prog();

        /*  clear window 4 and demonstrate pull-down & bar-selection menus  */

        wclear();
        wtextattr(LGREEN|_MAGENTA);
        wputs("\n  With the wmbardef() & wmbarget()"
              "\n  functions, you can make pull-down"
              "\n  and bar-select menus.");
        wprints(5,12,YELLOW|BLINK|_MAGENTA,pressakey);
        if(waitkeyt(273)==ESC) exit_prog();
        pulldown_demo();
        if(waitkeyt(273)==ESC) exit_prog();
        wprints(5,12,_MAGENTA,spaces);
        wprints(3,24,YELLOW|_MAGENTA,"Select one:");
        wmbardef(5,3,LCYAN|_MAGENTA,"Add record",'A');
        wmbardef(5,18,LCYAN|_MAGENTA,"Show record",'S');
        wmbardef(7,3,LCYAN|_MAGENTA,"Delete record",'D');
        wmbardef(7,18,LCYAN|_MAGENTA,"Update record",'U');
        wmbardef(9,3,LCYAN|_MAGENTA,"Print record",'P');
        wmbardef(9,18,LCYAN|_MAGENTA,"Quit program",'Q');
        i=wmbarget(LCYAN|_LGREY,'A',NO);
        wprints(3,24,_MAGENTA,spaces);
        if(!i) exit_prog();
        wgotoxy(11,10);
        wtextattr(WHITE|_MAGENTA);
        wprintf("You selected %c!",i);
        delay_(12);

        /*  open demo window 5 and demonstrate screen swapping  */

        whandle[5]=wopen(3,3,12,33,0,YELLOW|_BLACK);
        if(!whandle[5]) error_exit();
        wtextattr(LCYAN|_BLACK);
        wputs("\n  The ssave() and srestore()"
              "\n  functions allow screen"
              "\n  swapping to/from memory.");
        wprints(5,8,LRED|BLINK|_BLACK,pressakey);
        if(waitkeyt(273)==ESC) exit_prog();
        wprints(5,8,_BLACK,spaces);
        sbuf2=ssave();
        if(!sbuf2) error_exit();
        gotoxy_(24,79);
        clrscrn();
        prints(10,27,LMAGENTA,"Press a key to return");
        if(waitkeyt(273)==ESC) exit_prog();
        srestore(sbuf2);


        /*  open demo window 6 and demonstrate activating of windows  */

        whandle[6]=wopen(11,48,21,77,1,LCYAN|_CYAN);
        wtextattr(WHITE|_CYAN);
        wputs("\n TCXL's wactiv() is used to"
              "\n activate a window, making"
              "\n it available for windowing"
              "\n operations.");
        wprints(4,14,YELLOW|BLINK|_CYAN,pressakey);
        if(waitkeyt(273)==ESC) exit_prog();
        wprints(4,14,_CYAN,spaces);
        for(i=0;i<=6;i++) {
            if(kbhit()) {                            /*  check for keypress  */
                if(getch()==ESC) exit_prog();
            }
            if(wactiv(whandle[i])) error_exit();
            delay_(15);
        }


        /*  open demo window 7 and demonstrate moving of active window  */

        whandle[7]=wopen(4,2,16,35,2,LGREEN|_BROWN);
        if(!whandle[7]) error_exit();
        wtextattr(WHITE|_BROWN);
        wputs("\n To move the active window,"
              "\n the wmove() function is"
              "\n used.");
        wprints(3,8,YELLOW|BLINK|_BROWN,pressakey);
        if(waitkeyt(273)==ESC) exit_prog();
        wprints(3,8,_BROWN,spaces);
        if(wmove(0,0)) error_exit();
        delay_(12);
        if(wmove(10,4)) error_exit();
        delay_(12);
        if(wmove(6,19)) error_exit();
        delay_(12);
        if(kbhit()) {                               /*  check for keypress  */
            if(getch()==ESC) exit_prog();
        }
        if(wmove(1,13)) error_exit();
        delay_(12);
        if(wmove(2,35)) error_exit();
        delay_(12);
        if(wmove(11,35)) error_exit();
        delay_(12);


        /*  demonstrate making copies of the same window  */

        wtextattr(LRED|_BROWN);
        wputs("\n\n The wcopy() function will"
                "\n create a duplicate of the"
                "\n active window.");
        wprints(7,17,YELLOW|BLINK|_BROWN,pressakey);
        if(waitkeyt(273)==ESC) exit_prog();
        wprints(7,17,_BROWN,spaces);
        whandle[8]=wcopy(0,0);
        if(!whandle[8]) error_exit();
        delay_(12);
        whandle[9]=wcopy(1,37);
        if(!whandle[9]) error_exit();
        if(kbhit()) {                               /*  check for keypress  */
            if(getch()==ESC) exit_prog();
        }
        delay_(18);
        wclose();                                      /*  close copies  */
        delay_(12);
        wclose();
        delay_(12);


        /*  close all windows except window 0  */

        while(wnopen()>1) {
            if(kbhit()) {
                if(getch()==ESC) exit_prog();
            }
            wclose();
            delay_(6);
        }


        /*  list other features of TCXL  */

        wclear();
        wputs("  Other features of TCXL include:\n");
        wtextattr(LGREEN|_RED);
        wputs("\n  - EGA 43 & VGA 50 line modes"
              "\n  - expanded memory usage"
              "\n  - screen/window swapping to disk"
              "\n  - advanced string manipulation"
              "\n  - equipment detection"
              "\n  - mouse functions"
              "\n  - printer functions");
        wprints(8,26,YELLOW|BLINK|_RED,pressakey);
        if(waitkeyt(273)==ESC) exit_prog();
        wprints(8,26,_RED,spaces);


        /*  get response from the keyboard, controlling input  */

        wclear();
        if(wmove(7,19)) error_exit();
        wgotoxy(3,11);
        wtextattr(WHITE|_RED);
        wputs("View demo again?  ");
        setcursz(7,1);                           /*  turn cursor on  */
        clearkeys();                             /*  clear keyboard buffer  */
        ch=wgetchf("YyNn");                      /*  get keyboard response  */
        setcursz(32,0);                          /*  turn cursor back off  */
        if(ch==ESC) exit_prog();                 /*  see if Esc was pressed  */
        wclose();
        if(ch=='Y'||ch=='y') {
        }
        else {
            exit_prog();
        }
    }
}

/*  This function is the demonstration of pull-down windows   */

void pulldown_demo(void)
{
    char m1,m2;

    if(!wopen(0,0,24,79,2,CYAN)) error_exit();
    wprints(0,0,_BLUE,"                                        "
                      "                                      ");
    wprints(1,0,CYAN,"컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴"
                      "컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴");
    wgotoxy(2,0);
    wtextattr(YELLOW);
    wputs("This is a sample editor interface to demonstrate how TCXL's "
          "wmbardef()\n"
          "and wmbarget() functions can be used for pull-down menus.  "
          "Use arrow keys\n"
          "to move selection bar around and use the Esc key to exit the "
          "menu you are\n"
          "currently in.  Any selection from a pull-down menu will return "
          "you to demo.");
    m1='F';                         /* initialize selection bar position  */
    m2=PDMAIN;
    top:
    wmbardef(0,2,YELLOW|_BLUE,"File",'F');      /* define main menu */
    wmbardef(0,15,YELLOW|_BLUE,"Editing",'E');
    wmbardef(0,31,YELLOW|_BLUE,"Defaults",'D');
    wmbardef(0,47,YELLOW|_BLUE,"Help",'H');
    wmbardef(0,59,YELLOW|_BLUE,"Options",'O');
    if(wmbardef(0,73,YELLOW|_BLUE,"Quit",'Q')) error_exit();
    m1=wmbarget(YELLOW|_MAGENTA,m1,m2);
    switch(m1) {          /*  test return value from main (horizontal) menu  */
        case 'F':                    /* define pull-down menu for option 'F' */
            if(!wopen(2,1,11,14,0,LGREY)) error_exit();
            wmbardef(0,0,YELLOW|_BLUE," Load       ",'L');
            wmbardef(1,0,YELLOW|_BLUE," Save       ",'S');
            wmbardef(2,0,YELLOW|_BLUE," Rename     ",'R');
            wmbardef(3,0,YELLOW|_BLUE," New        ",'N');
            wmbardef(4,0,YELLOW|_BLUE," Directory  ",'D');
            wmbardef(5,0,YELLOW|_BLUE," Change dir ",'C');
            wmbardef(6,0,YELLOW|_BLUE," OS Shell   ",'O');
            if(wmbardef(7,0,YELLOW|_BLUE," Quit       ",'Q')) error_exit();
            m2=wmbarget(YELLOW|_MAGENTA,'L',PDMENU);
            wclose();
            if(m2<=PDMENU) goto top;        /* test for Esc or Lt/Rt Arrow   */

/*          This is where you would test the
            return value from the pull-down menu        */

            break;
        case 'E':                    /* define pull-down menu for option 'E' */
            if(!wopen(2,14,10,28,0,LGREY)) error_exit();
            wmbardef(0,0,YELLOW|_BLUE," Cut         ",'C');
            wmbardef(1,0,YELLOW|_BLUE," Paste       ",'P');
            wmbardef(2,0,YELLOW|_BLUE," cOpy        ",'O');
            wmbardef(3,0,YELLOW|_BLUE," block Begin ",'B');
            wmbardef(4,0,YELLOW|_BLUE," block End   ",'E');
            wmbardef(5,0,YELLOW|_BLUE," Insert line ",'I');
            if(wmbardef(6,0,YELLOW|_BLUE," Delete line ",'D')) error_exit();
            m2=wmbarget(YELLOW|_MAGENTA,'C',PDMENU);
            wclose();
            if(m2<=PDMENU) goto top;        /* test for Esc or Lt/Rt Arrow   */

/*          This is where you would test the
            return value from the pull-down menu        */

            break;
        case 'D':                    /* define pull-down menu for option 'D' */
            if(!wopen(2,30,11,52,0,LGREY)) error_exit();
            wmbardef(0,0,YELLOW|_BLUE," Left margin       0 ",'L');
            wmbardef(1,0,YELLOW|_BLUE," Right margin     72 ",'R');
            wmbardef(2,0,YELLOW|_BLUE," Tab width         4 ",'T');
            wmbardef(3,0,YELLOW|_BLUE," tab eXpansion   yes ",'X');
            wmbardef(4,0,YELLOW|_BLUE," Insert mode     yes ",'I');
            wmbardef(5,0,YELLOW|_BLUE," iNdent mode     yes ",'N');
            wmbardef(6,0,YELLOW|_BLUE," Word wrap mode   no ",'W');
            if(wmbardef(7,0,YELLOW|_BLUE," Save defaults       ",'S')) {
                error_exit();
            }
            m2=wmbarget(YELLOW|_MAGENTA,'L',PDMENU);
            wclose();
            if(m2<=PDMENU) goto top;        /* test for Esc or Lt/Rt Arrow   */

/*          This is where you would test the
            return value from the pull-down menu        */

            break;
        case 'H':                    /* define pull-down menu for option 'H' */
            if(!wopen(2,46,7,65,0,LGREY)) error_exit();
            wmbardef(0,0,YELLOW|_BLUE," help on Help     ",'H');
            wmbardef(1,0,YELLOW|_BLUE," help on Editing  ",'E');
            wmbardef(2,0,YELLOW|_BLUE," help on Defaults ",'D');
            if(wmbardef(3,0,YELLOW|_BLUE," help on Options  ",'O')) {
                error_exit();
            }
            m2=wmbarget(YELLOW|_MAGENTA,'H',PDMENU);
            wclose();
            if(m2<=PDMENU) goto top;        /* test for Esc or Lt/Rt Arrow   */

/*          This is where you would test the
            return value from the pull-down menu        */

            break;
        case 'O':                    /* define pull-down menu for option 'O' */
            if(!wopen(2,56,8,77,0,LGREY)) error_exit();
            wmbardef(0,0,YELLOW|_BLUE," screen siZe     43 ",'Z');
            wmbardef(1,0,YELLOW|_BLUE," Backup files   yes ",'B');
            wmbardef(2,0,YELLOW|_BLUE," bOxed display   no ",'O');
            wmbardef(3,0,YELLOW|_BLUE," Load options       ",'L');
            if(wmbardef(4,0,YELLOW|_BLUE," Save options       ",'S')) {
                error_exit();
            }
            m2=wmbarget(YELLOW|_MAGENTA,'Z',PDMENU);
            wclose();
            if(m2<=PDMENU) goto top;        /* test for Esc or Lt/Rt Arrow   */

/*          This is where you would test the
            return value from the pull-down menu        */

            break;
        case 'Q':                    /* define pull-down menu for option 'Q' */
            if(!wopen(2,60,7,78,0,LGREY)) error_exit();
            wcclear(BLUE|_BLUE);
            wprints(0,2,WHITE|_BLUE,"Are you sure?");
            wmbardef(2,0,YELLOW|_BLUE,"       No        ",'N');
            if(wmbardef(3,0,YELLOW|_BLUE,"       Yes       ",'Y')) {
                error_exit();
            }
            m2=wmbarget(YELLOW|_MAGENTA,'N',PDMENU);
            wclose();
            if(m2<=PDMENU) goto top;        /* test for Esc or Lt/Rt Arrow   */

/*          This is where you would test the
            return value from the pull-down menu        */

            break;
        default:
    }
    wclose();
}


/*  if the Esc key or Ctrl-Break is pressed, this function will be called  */ 

void interrupt exit_prog(void)
{
    clearkeys();                       /*  clear keyboard buffer  */
    setcursz(6,7);                     /*  set cursor size back to normal  */
    srestore(sbuf1);                   /*  restore original screen  */
    gotoxy_(row,col);                  /*  restore original cursor position  */
    exit(0);
}

/*  if an error occurs, this function will be called  */

void error_exit(void)
{
    setcursz(6,7);                       /*  set cursor size back to normal  */
    if(_werrno) {
        printf("\nError:  %s      ",werrmsg());    /*  display error message */
    }
    else {
        abort();
    }
    exit(1);
}

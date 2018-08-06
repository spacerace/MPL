/*
**  C S R P R O M O . C
**
**  Promotional program to demonstrate what the C Spot Run
**  library does and how to use it. 
**
**  Uses the C Spot Run C Add-On Library and was compiled with
**  Microsoft C V3.0.
**
**  The C Spot Run C Add-On Library is always available at
**  the listed BBS, and the author may be contacted as below.
**
**    C Spot Run
**    New Dimension Software
**    23 Pawtucket Dr.
**    Cherry Hill, NJ 08003
**    Voice: (609) 424-2595
**     Data: (609) 354-9259  300-2400B  24hrs  (FidoNet 107/414)
**
**  Copyright 1986, 1987 Bob Pritchett
**
**  History in reverse order:
**
**  01/10/87  Final modifications for version 2.0.
**
**  09/17/86  Changing some colors, testing multi color in a window.
**
**  08/18/86  Some more small modifications.
**
**  08/06/86  Small modifications to work with new version.
**
**  05/24/86  Created using version 1.0 and a few routines
**            from the not yet released version 1.1.  (V1.1a)
**
*/

#include <stdio.h>
#include <color.h>
#include <csrsound.h>

static char *menu[] =
 {
  "  General Information  ",
  "      Window Demo      ",
  "       Directory       ",
  "    Other Functions    ",
  "   Programming Tools   ",
  "      Exit to DOS      "
 };

static char *tools[] =
 {
  "    CheckC    ",
  "     FLine    ",
  " Header Files "
 };

static char *logo[] =
 {
  "   CCCC",
  "  CC  CC",
  " CC",
  " CC   S P O T   R U N",
  " CC",
  "  CC  CC",
  "   CCCC"
 };

main()
 {
  int w;
  int x;
  int i;
  dma(0);
  save_cursor();
  save_screen();
  cls();
  wcolor(LGRY_F+BLU_B,BLU_B);
  w = wopen(4,26,19,53,1);
  wcenter(w,1,"Introducing");
  for ( x = 0; x < 7; ++x )
      wputat(w,3+x,2,logo[x]);
  wcenter(w,11,"A User-Supported");
  wcenter(w,12,"C Add-On Library");
  wmessage(w,"[ Any Key ]",0);
  sound(C,30);
  sound(D,30);
  sound(E,30);
  sound(F,30);
  sound(G,30);
  sound(A,30);
  sound(B,30);
  getch();
  wclose(w);
  while ( 1 )
   {
    mcolor(BLK_F+RED_B,WHT_F+BLU_B);
    color(RED_F+WHT_B);
    x = pop_menu(9,27,6,menu," C Spot Run ",3);
    if ( x == -1 )
       break;
    else if ( x == 5 )
       break;
    else if ( x == 4 )
       utilities();
    else if ( x == 3 )
       other();
    else if ( x == 2 )
       directory();
    else if ( x == 1 )
       wndwdemo();
    else if ( x == 0 )
       info();
   }
  wcloseall();
  restore_screen();
  restore_cursor();
  exit(0);
 }

other()
 {
  int w;
  int w1;
  int h;
  int m;
  int s;
  int hn;
  int dy;
  int mn;
  int yr;
  int n;
  color(BLK_F+GRN_B);
  w = wopen(10,10,20,70,1);
  wtitle(w,"< Other Functions >",0);
  wk(w);
  wcenter(w,1,"Why not add some sound to life?");
  for ( s = 25; s < 1000; s += 5 )
      sound(s,40);
  dosver(&m,&n);
  wcenterf(w,2,"You are running V%d.%d of DOS.\n",m,n);
  wcenter(w,4,"And there are many more functions handling");
  wcenter(w,5,"time, data, DOS and BIOS calls.");
  w1 = wopen(2,61,5,78,3);
  while ( ! kbhit() )
   {
    get_date(&dy,&mn,&yr);
    get_time(&h,&m,&s,&hn);
    whome(w1);
    wprintf(w1," Date: %02d/%02d/%d\n",mn,dy,(yr-1900));
    wprintf(w1," Time: %02d:%02d:%02d",h,m,s);
    gotoxy(30,0);			/* Cursor Goes Away */
    for ( s = 0; s < 20000; ++s )
        ;
   }
  getch();
  wcloseall();
 }

info()
 {
  int w;
  color(BOLD+RED_F+BLK_B);
  w = wopen(4,9,19,71,3);
  wcenter(w,1,"C Spot Run");
  wcenter(w,3,"Version 1.1a  Copyright 1986 Bob Pritchett");
  whline(w,5);
  wcenter(w,7,"C Spot Run is a collection of routines and programming");
  wcenter(w,8,"utilities for C programmers.  The purpose of this");
  wcenter(w,9,"library is to provide low cost tools for programmers");
  wcenter(w,10,"under the user-supported distribution concept.");
  wcenter(w,11,"For more information, contact the author at the following");
  wcenter(w,12,"address, or consult the C Spot Run manual.");
  wk(w);
  getch();
  wclose(w);
  w = wopen(7,28,17,52,3);
  wtitle(w,"< Address >",0);
  wcenter(w,1,"C Spot Run");
  wcenter(w,3,"Bob Pritchett");
  wcenter(w,4,"23 Pawtucket Dr.");
  wcenter(w,5,"Cherry Hill, NJ 08003");
  wcenter(w,7,"FidoNet 107/414");
  wk(w);
  getch();
  wclose(w);
 }

wk(num)
 int num;
 {
  wmessage(num,"[ Press a Key ]",0);
 }

wndwdemo()
 {
  int w;
  int w2;
  int w3;
  wcolor(WHT_F+BLK_B,BOLD+GRN_F);
  w = wopen(1,5,22,75,177);
  wtitle(w," Do you do windows?... ",1);
  wk(w);
  wcenter(w,1,"With the C Spot Run window functions library, you have");
  wcenter(w,2,"access to one of the most powerful windowing packages");
  wcenter(w,3,"available, and by far one of the best priced.");
  wcenter(w,4,"-- It's Free!! --");
  wcenter(w,5,"That's right.  The object code, libraries, and utilities");
  wcenter(w,6,"included in this package are absolutely free, and you may");
  wcenter(w,7,"use them for any non-commercial purpose as long as credit");
  wcenter(w,8,"is given.  For use in commercial applications, or to");
  wcenter(w,9,"obtain the surce code, check in the included manual");
  wcenter(w,10,"and contact the author.");
  wcenter(w,12,"Speaking of credit, I'd like to thank Phillip A. Mongelluzzo");
  wcenter(w,13,"whose source code taught me many of the techniques in this");
  wcenter(w,14,"library, and who answered many questions for me.");
  wcenter(w,16,"What follows is a small demonstration of some of the routines");
  wcenter(w,17,"in the library, and a short explanation of each.  For a");
  wcenter(w,18,"complete description, please consult your manual.");
  getch();
  wclose(w);
  wcolor(BOLD+CYN_F+WHT_B,MAG_F);
  w = wopen(5,7,22,70,4);
  wprint(w,"Title anywhere with wtitle()...\n");
  getch();
  wtitle(w,"[ Left ]",1);
  wtitle(w,"< Right ]",2);
  wtitle(w,"< Center a Message >",0);
  getch();
  wprint(w,"Send a message with wmessage()...\n");
  getch();
  wmessage(w," Here... ",1);
  wmessage(w," ...or here. ",2);
  wk(w);
  getch();
  wprint(w,"We can skip around...");
  wscolor(w,RED_F+WHT_B,MAG_F);
  wgotoxy(w,6,6);
  wprint(w,"+ To here at 6,6 with wgotoxy()...");
  getch();
  wputat(w,7,4,"+ Or to 7,4 with wpuat()...");
  getch();
  wcenter(w,8,"Or center a line with wcenter().");
  wgotoxy(w,10,0);
  wprint(w,"Of course all output is done through an assembly routine,\n");
  wprint(w,"just as cursor movement is...  We do stick with C for this:\n");
  wprint(w,"(Go on, hit a key...)\n");
  getch();
  wjump(w,1,1);
  getch();
  wprint(w,"(Hit a key...)");
  getch();
  wprint(w,"\f The windows are all in a linked format, and can be moved\n");
  wprint(w,"from fore to background easily.  Any call that involves a non\n");
  wprint(w,"active window will activate that window before performing\n");
  wprint(w,"it's action.  You may also close the windows in any order\n");
  wprint(w,"you wish.\n");
  wprint(w," The wactivate(window) function will activate the window\n");
  wprint(w,"specified by window, and place it on 'top' of any overlapping\n");
  wprint(w,"windows.\n");
  getch();
  color(YEL_F+RED_B);
  w2 = wopen(5,5,20,35,2);
  getch();
  w3 = wopen(5,40,20,75,1);
  getch();
  whline(w2,2);
  whline(w2,8);
  wvline(w3,4);
  wvline(w3,10);
  wvline(w3,16);
  wk(w2);
  wk(w3);
  getch();
  wactivate(w);
  getch();
  wactivate(w2);
  getch();
  wactivate(w);
  wactivate(w3);
  getch();
  wcloseall();
  wcolor(WHT_F+BLU_B,CYN_F+WHT_B);
  w = wopen(5,10,20,70,4);
  wprint(w,"\tOf course there is a wprintf() function...\n");
  getch();
  for ( w2 = 0; w2 < 50; ++w2 )
      wprintf(w,"Counting up...%03d.\n",w2);
  wprintf(w,"Counting up...%03d.",w2);
  getch();
  wk();
  wprint(w,"\nWe can even set the scrolling boundaries...\n");
  getch();
  wcls(w);
  wcenter(w,0,"Freeze this line.");
  wcenter(w,wrow(w),"Freeze this also.");
  wfreeze(w,1,wrow(w)-1);
  whome(w);
  for ( w2 = 0; w2 < 50; ++w2 )
      wprintf(w,"Counting up...%03d.\n",w2);
  wprintf(w,"Counting up...%03d.",w2);
  getch();
  wclose(w);
 }

utilities()
 {
  int x;
  while ( 1 )
   {
    color(MAG_F+BLK_B);
    mcolor(CYN_F,RED_F+WHT_B);
    x = pop_menu(10,10,3,tools," Tools ",1);
    if ( x == -1 )
       return(-1);
    else if ( x == 0 )
       checkc();
    else if ( x == 1 )
       fline();
    else if ( x == 2 )
       headers();
   }
 }

checkc()
 {
  int w;
  color(BOLD+MAG_F+BLK_B);
  w = wopen(7,4,19,66,2);
  wcenter(w,1,"CheckC");
  whline(w,3);
  wcenter(w,5,"CheckC is a small C code checker that checks for unmatched");
  wcenter(w,6,"( ) { } [ ] \" \' in C source code.  Working with multiple");
  wcenter(w,7,"files at once and correctly counting special cases, such");
  wcenter(w,8,"as escape characters, makes this utility a valuable");
  wcenter(w,9,"addition to your C debugging tools.");
  wk(w);
  getch();
  wclose(w);
 }

fline()
 {
  int w;
  color(BOLD+GRN_F+BLK_B);
  w = wopen(7,6,19,74,2);
  wcenter(w,1,"FLine");
  whline(w,3);
  wcenter(w,5,"FLine is a small utility that scans source code and places");
  wcenter(w,6,"and lines begining with a non-whitespace character into the");
  wcenter(w,7,"output file specified on the command line.  This provides");
  wcenter(w,8,"a useful reference file containing all function declerations,");
  wcenter(w,9,"preproccessor statements, global variables, and comments.");
  wk(w);
  getch();
  wclose(w);
 }

headers()
 {
  int w;
  color(BOLD+RED_F+BLK_B);
  w = wopen(5,7,16,76,2);
  wcenter(w,1,"Header Files");
  whline(w,3);
  wcenter(w,5,"The C Spot Run library contains a number of header files");
  wcenter(w,6,"designed to make programming easier.  Color, special keys,");
  wcenter(w,7,"sound, and character codes are all covered in the different");
  wcenter(w,8,"header files included here.");
  wk(w);
  getch();
  wclose(w);
 }

directory()
 {
  color(WHT_F+RED_B);
  if ( num_drives() < 3 )
     dirwin("A:","*.*");
  else
     dirwin("C:","*.*");
 }


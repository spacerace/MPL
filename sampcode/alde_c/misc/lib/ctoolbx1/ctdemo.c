
/*                             CTDEMO.C
 *
 *
 *   This is a simple demo program to test some of the C Tool Box
 *   functions.  Current plans call for further expansion of the library.
 *   The final library developed will hopefully include data entry
 *   forms and menus built around windows.  It is hoped to provide
 *   full data editing and verification with the window forms.
 *
 *   This version requires the use of a color graphics adapter if you
 *   beyond the window portion of the program.
 *
 *
 *   This program and its supporting code (with the exception of the
 *   Unicorn Library) are placed in the public domain by the author.
 *   You have full rights to use this code in any manner you may see
 *   fit with the exception of using any or all for profit.  A club
 *   or users group may however charge a small handling fee not to
 *   exceed $7.00.  The author retains all rights to the Library
 *   subject to the shareware notice contained with the documentation for
 *   that library.
 *
 *   This will be released under the shareware concept as was the previous
 *   Unicorn library.
 *
 *
 *                            David A. Miller
 *                            Unicorn Software
 *                            8018 NE 123rd St
 *                            Kirkland, WA 98034
 *                            (206) 823-4656 after 6PM PST
 *                            Please, no collect calls
 *
 *    Release date 22 August 1987
 */

#include <alloc.h>
#include <stdio.h>
#include <windows.h>
#include <ctb.h>
#include <border.h>
#include <bdrdef.h>
#include <colors.h>


extern ct_mode_;    /* These are global variables where the current video  */
extern ct_col_;     /* mode and number of screen columns are stored        */
extern v_start;

BOX *title, *first, *second, *third, *fourth, *fifth, *sixth, *small;
BOX *mine;



/*  The user is allowed up to 249 notes for background music.  The last
 *  note (the 250th if all are used) MUST be 0xff or the function will
 *  not terminate properly.
 */
                                /* Short segment for test purposes */
/*
int melody[250] = {26, 29, 31, 33, 0xff};
int beat[250]   = { 6, 12,  6,  7, 1};
*/

                        /* The below defines notes for Greensleeves */

int melody[250] = {26, 29, 31, 33, 35, 33, 31, 28, 24, 26, 28, 29, 26,100,
                   26, 25, 26, 28, 25, 21, 26, 29, 31, 33, 35, 33, 31,
                   28, 24, 26, 28, 29, 28, 26, 25, 23, 25, 26,100, 26, 36,
                  100, 36, 35, 33, 31, 28, 24, 26, 28, 29, 26,100, 26, 25, 26,
                   28, 25, 21, 36,100, 36, 35, 33, 31, 28, 24, 26, 28, 29,
                   28, 26, 25, 23, 25, 26,100, 26, 0xff};

int beat[250] =   { 6, 12,  6,  7,  4,  6, 12,  6,  7,  4,  6, 12,  6,  1,
                    7,  4,  6, 12,  6, 12,  6, 12,  6,  7,  4,  6, 12,
                    6,  7,  4,  6,  7,  4,  7,  4,  6,  6, 18,  1, 18, 12,
                    1,  6,  6,  6, 12,  6,  7,  4,  6, 12,  6,  1,  7,  4,  6,
                   12,  6, 20, 20,  1,  7,  4,  6, 12,  6,  7,  4,  6,  7,
                    4,  6,  6,  6,  6, 20,  1, 16,  1};



int start = 9;


main()
{
   char a[80], *wgets(), ch, name[60];
   int  i, j, w, h, row, col;
   unsigned *p, *mem;

   sm(3);
   ctinit();

/*  makebox sets up the definition of a box by allocating memory for a box
 *  structure and placing data into it.  The form is
 *  makebox(row, col, height, width, bfore, bback, ffore, fback);
 *
 *  savebox copies the screen area under the box and stores a pointer to
 *  that memory in the box structure.
 *
 *  draw box actually draws the box on the screen using direct video memory
 *  writes for speed.
 *
 *  boxtitle places a title in the border area of the box.  The form is
 *  boxtitle(BoxPointer, 0/1, " Title ", fore, back);
 *
 */


   first = makebox(0,0,23,79,14, 4, 14, 1);
   savebox(first);
   drawbox(first);
   boxtitle(first, 0, 72, " Box 1 " , YELLOW, 99);


   ct_wvstr(6, 68, LTWHITE, 99, "Vertical Strings", first);
   ct_wvstr(6, 72, LTWHITE, 99, "are possible    ", first);



   ct_str( 8, 10,  4, 1, "        мммммм                                      ");
   ct_str( 9, 10,  4, 1, "        лллллм                                      ");
   ct_str(10, 10, 11, 1, "        Г                                           ");
   ct_str(11, 10, 11, 1, "        Г                                           ");
   ct_str(12, 10,  7, 1, "лл  лл  лл                              лл  лл  лл  ");
   ct_str(13, 10,  7, 1, " лллллллл                                лллллллл   ");
   ct_str(14, 10,  7, 1, " лллл  лл   лл  лл  лл  лл  лл  лл  лл   ллл  ллл   ");
   ct_str(15, 10,  7, 1, " лллллллллллллллллллллллллллллллллллллллллллллллл   ");
   ct_str(16, 10,  7, 1, " ллллллллл  ллллллллллллллл  лллллллллл  лллллллл   ");
   ct_str(17, 10,  7, 1, " ллллллллллллллллллААААААлллллллллллллллллллллллл   ");
   ct_str(18, 10,  7, 1, " ллллллллллллллллллААААААлллллллллллллллллллллллл   ");
   ct_str(19, 10,  7, 1, " ллллллллллллллллллААААААлллллллллллллллллллллллл   ");
   ct_str(20, 10,  7, 1, " ллллллллллллллллллААААААлллллллллллллллллллллллл   ");
   ct_str(21, 5, GREEN, 1, "лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл");
   ct_str(22, 5, GREEN, 1, "лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл");

   if (ct_mode_ != 7)
   {
      ct_str(14, 15, 14, 1, "лл");
      ct_str(16, 37, 14, 1, "лл");

      ct_str(8,  18,  4, 1, "ммммм");
      ct_str(9,  18, 11, 4, " CTB ");
      ct_str(8,  18,  7, 4, "п");

      ct_str(17, 29,  8, 6, "АААААА");
      ct_str(18, 29,  8, 6, "АААААА");
      ct_str(19, 29,  8, 6, "АААААА");
      ct_str(20, 29,  8, 6, "АААААА");
   }

   title = makebox(1, 25, 2, 27, 14, 1, 14, 1);
   makebdr(title, space);
   savebox(title);
   drawbox(title);
   ct_wstr(1,5,99,99, "Unicorn Software ", title);


   second = makebox(3, 25, 4, 26, 9, 1, 9, 1);
   makebdr(second, space);
   savebox(second);
   drawbox(second);
   ct_wstr(1,9,99,99, "Presents", second);
   ct_wstr(2,6,15 + BLINK, 99, "The C Tool Box", second);

   for(i = 0; i <= 10000; i++)
     for(j = 0; j <= 30; j++)
          ;

   third = makebox(12, 10, 4, 59, 9,6,9,6);
   makebdr(third, bdr1);
   savebox(third);
   drawbox(third);
   boxtitle(third, 1, 2, " Box 3 ", 98, 98);

   ct_wstr(1,15,99,99, "What is your name?", third);
   setcurps(0, 13, 45);
   gets(name);
   ct_wstr(2,15,99,99,"Hello ", third);
   ct_wstr(2,22,99,99, name, third);
   ct_wstr(3,5,99,99, "Would you be so kind as to press my enter key?", third);
   setcurps(0, 15, 67);
   gets(a);

   
   fourth = makebox(18, 10, 3, 59, 7,4,7, 4);
   makebdr(fourth, bdr21);
   savebox(fourth);
   drawbox(fourth);
   boxtitle(fourth, 0, 10, " Fourth Box ", 99, 99);

   ct_wstr(1,5,99,99, "Thank you ", fourth);
   ct_wstr(1,16,99,99, name, fourth);
   ct_wstr(2, 5, 99,99, "Now if you would press it again please", fourth);

   setcurps(0, 20, 57);
   gets(a);


   fifth = makebox(6,10, 13, 59, LTMAGENTA, 5, 9, 5);
   makebdr(fifth, bdr12);
   savebox(fifth);
   drawbox(fifth);
   boxtitle(fifth, 0, 10, "Ў Fifth Box Џ", 99, 99);

   ct_wstr(1,3,99,99," Unicorn Software is happy to provide this tool box of", fifth);
   ct_wstr(2,3,99,99,"functions in the form of shareware.  The library may  ", fifth);
   ct_wstr(3,3,99,99,"be registered with the author.  Upon registration you ", fifth);
   ct_wstr(4,3,99,99,"will be sent the complete manual on disk and full     ", fifth);
   ct_wstr(5,3,99,99,"source code to the library.  The manual runs well over", fifth);
   ct_wstr(6,3,99,99,"200 pages.  If you wish, a preprinted manual is also  ", fifth);
   ct_wstr(7,3,99,99,"available.                                            ", fifth);

   ct_wstr(9, 3,YELLOW,99,   "                David A.  Miller                     ", fifth);
   ct_wstr(10,3,WHITE,99,    "                Unicorn Software                     ", fifth);
   ct_wstr(11,3,LTMAGENTA,99,"     8018 NE 123rd St.  Kirkland, WA  98034          ", fifth);


   gets(a);


   sixth = makebox(5,5, 10, 59, 4, 3, 15, 3);
   makebdr(sixth, bdr12);
   savebox(sixth);
   drawbox(sixth);
   boxtitle(sixth, 0, 10, "Ў Sixth Box Џ", 99, 99);

   ct_wstr(1,3,99,99,"The tool box contains most of the previous Unicorn    ", sixth);
   ct_wstr(2,3,99,99,"Library functions ( over 200) plus a fast window      ", sixth);
   ct_wstr(3,3,99,99,"library, background sound and much more.  The entire  ", sixth);
   ct_wstr(4,3,99,99,"library encompasses graphics, text handling, mouse and", sixth);
   ct_wstr(5,3,99,99,"joystick control graphics and daisy wheel control     ", sixth);
   ct_wstr(6,3,99,99,"functions, background sound functions and more.  An   ", sixth);
   ct_wstr(7,3,99,99,"EGA version of the library should be released         ", sixth);
   ct_wstr(8,3,99,99,"before the end of 1987.  <Press ENTER at each pause>  ", sixth);

   setcurps(0,0,0);
   gets(a);
   resbox(sixth);
   gets(a);
   resbox(fifth);
   gets(a);
   resbox(fourth);
   gets(a);
   resbox(third);
   gets(a);
   resbox(second);
   gets(a);
   resbox(first);


   back16_8(12, 14);   /* Switch to using highlighted background colors */

   printf("\tMode is %d and columns are %d and start is %X Hex\n", ct_mode_, ct_col_, v_start);
   if (gcomtype() == 0xff)
      printf("\tThis computer is a PC or very close compatible\n\n");
   if (gcomtype() == 0xfc)
      printf("\tThis computer is an AT or very close compatible\n\n");
   if (gcomtype() == 0xfe)
      printf("\tThis computer is an XT or very close compatible\n\n");
   if (gcomtype() == 0xfd)
      printf("\tThis computer is an PCjr or very close compatible\n\n");
   if (gcomtype() == 0xf9)
      printf("\tThis computer is an PC convertible or very close compatible\n\n");

   printf("\tNote that it is possible to use the highlighted colors in the\n");
   printf("\ttext mode for background color if you give up the blinking \n");
   printf("\ttext attribute.  Functions are available to switch between\n");
   printf("\tblink and high-intensity background modes.  If you wish to\n");
   printf("\tgo further in this demo you MUST have a graphics adapter\n");
   printf("\tinstalled.  Please be aware also that music will be played\n");
   printf("\tin the background mode while other things occur on screen.\n");
   printf("\tWhile this is similar to BASIC in some respects you have \n");
   printf("\ta limit of 249 notes with this library, not the limit of \n");
   printf("\t32 notes BASIC allows you.  Even this limit is user changable.\n");



   printf("\n\t\t\tPress any key to continue\n");
   waitkey();






   /* Draw bar graphs next */

   sm(4);
   palette(2, 5, 14);

   hollow('B', 5,  70, 3);
   hollow('A', 5,  90, 3);
   hollow('R', 5, 110, 3);
   hollow('G', 5, 140, 3);
   hollow('R', 5, 160, 3);
   hollow('A', 5, 180, 3);
   hollow('P', 5, 200, 3);
   hollow('H', 5, 220, 3);

   vbar3d(150, 80, 100, 12, 16, 2, 3);
   vbar3d(150, 100, 140, 8, 26, 2, 3);
   vbar3d(150, 40, 180, 25, 8, 2, 3);

   usrfont('I', 153,  90, 1, 1, 2, 1);
   usrfont('T', 153,  95, 1, 1, 2, 1);
   usrfont('E', 153, 100, 1, 1, 2, 1);
   usrfont('M', 153, 105, 1, 1, 2, 1);
   usrfont('1', 153, 115, 1, 1, 2, 1);

   usrfont('I', 153, 135, 1, 1, 2, 1);
   usrfont('T', 153, 140, 1, 1, 2, 1);
   usrfont('E', 153, 145, 1, 1, 2, 1);
   usrfont('M', 153, 150, 1, 1, 2, 1);
   usrfont('2', 153, 160, 1, 1, 2, 1);

   usrfont('I', 153, 180, 1, 1, 2, 1);
   usrfont('T', 153, 185, 1, 1, 2, 1);
   usrfont('E', 153, 190, 1, 1, 2, 1);
   usrfont('M', 153, 195, 1, 1, 2, 1);
   usrfont('3', 153, 205, 1, 1, 2, 1);

   script('O', 158,  66, 1);
   script('t', 158,  82, 1);
   script('h', 158,  98, 1);
   script('e', 158, 114, 1);
   script('r', 158, 130, 1);
   script('g', 158, 156, 1);
   script('r', 158, 172, 1);
   script('a', 158, 188, 1);
   script('p', 158, 204, 1);
   script('h', 158, 220, 1);

   usrfont('S', 189, 30,  1, 1, 1, 0);
   usrfont('T', 189, 40,  1, 1, 1, 0);
   usrfont('Y', 189, 50,  1, 1, 1, 0);
   usrfont('L', 189, 60,  1, 1, 1, 0);
   usrfont('E', 189, 70,  1, 1, 1, 0);
   usrfont('S', 189,  80, 1, 1, 1, 0);
   usrfont('W', 189, 100, 1, 1, 2, 0);
   usrfont('I', 189, 110, 1, 1, 2, 0);
   usrfont('L', 189, 120, 1, 1, 2, 0);
   usrfont('L', 189, 130, 1, 1, 2, 0);
   usrfont('B', 189, 150, 1, 1, 1, 0);
   usrfont('E', 189, 160, 1, 1, 1, 0);
   usrfont('A', 189, 180, 1, 1, 1, 0);
   usrfont('V', 189, 190, 1, 1, 1, 0);
   usrfont('A', 189, 200, 1, 1, 1, 0);
   usrfont('I', 189, 210, 1, 1, 1, 0);
   usrfont('L', 189, 220, 1, 1, 1, 0);
   usrfont('A', 189, 230, 1, 1, 1, 0);
   usrfont('B', 189, 240, 1, 1, 1, 0);
   usrfont('L', 189, 250, 1, 1, 1, 0);
   usrfont('E', 189, 260, 1, 1, 1, 0);
   waitkey();



   sm(4);
   backclr(1);

   gothic('U',10,start+0,3);
   gothic('n',10,start+25,3);
   gothic('i',10,start+45,3);
   gothic('c',10,start+60,3);
   gothic('o',10,start+80,3);
   gothic('r',10,start+100,3);
   gothic('n',10,start+120,3);

   gothic('S',10,start+150,3);
   gothic('o',10,start+170,3);
   gothic('f',10,start+190,3);
   gothic('t',10,start+205,3);
   gothic('w',10,start+220,3);
   gothic('a',10,start+240,3);
   gothic('r',10,start+260,3);
   gothic('e',10,start+280,3);

   raschar('P', 50, start + 70, 2, 2, 3);
   raschar('r', 50, start + 90, 2, 2, 3);
   raschar('e', 50, start + 110, 2, 2, 3);
   raschar('s', 50, start + 130, 2, 2, 3);
   raschar('e', 50, start + 150, 2, 2, 3);
   raschar('n', 50, start + 170, 2, 2, 3);
   raschar('t', 50, start + 190, 2, 2, 3);
   raschar('s', 50, start + 210, 2, 2, 3);

   setsound();   /* Play the greensleeves melody in the background */
   
   unicorn();
   atsay(24, 3, 2, 0,"Copyright (c) 1986 David A. Miller");
   cntue();
   waitkey();
   sm(4);       /* this resets the mode which in effect is a
                   clear screen command.                      */
   backclr(9);

   printf("\n\n A demonstration of IBM PC graphics\n");
   printf(" using the 4 color med. resolution mode\n");
   printf(" and the C Tool Box library of\n");
   printf(" functions.\n");
   printf(" This library is designed to work with \n");
   printf(" the Turbo C small model. It has\n");
   printf(" been written using Turbo C  1.0 \n");
   printf(" and the Microsoft Assembler 4.0.\n");
   printf("    Not all library functions are\n");
   printf(" illustrated in this demo.  The\n");
   printf(" library is capable of many graphics\n");
   printf(" concepts including points, lines,\n");
   printf(" circles, ellipses (these may be \n");
   printf(" rotated from the vertical or\n");
   printf(" horizontal) and colorful print\n");
   printf(" including different fonts.  We\n");
   printf(" hope you enjoy the C TOOL BOX\n");
   printf(" Version 1.00 for Turbo C  ver 1.0\n");
   printf(" small model.\n");
   printf("  Copyright (C) 1986 David A. Miller\n");
   atsay(23,8,2,0,"Press any key to continue");
   waitkey();
   sm(4);       /* this resets the mode which in effect is a
                   clear screen command.                      */
   backclr(14);



   pg1();
   waitkey();
   sm(4);
   backclr(1);



   pg2();
   cntue();
   waitkey();
   sm(4);
   backclr(1);

   pg3();
   cntue();
   waitkey();
   sm(4);
   backclr(9);

   atom();
   cntue();
   waitkey();
   sm(4);
   backclr(1);

   printf("   The next screen will demonstrate\n\n");
   printf("    T U R T L E  F U N C T I O N S");

   printf("\n\n\n\n\n   The delay function is used to");
   printf("\n   slow down the display of lines\n");
   cntue();
   waitkey();
   palette(0,5,8);

   pg5();
   cntue();
   waitkey();
   sm(4);
   backclr(1);

   script('P', 5, 100, 3);
   script('a', 5, 116, 3);
   script('l', 5, 132, 3);
   script('e', 5, 148, 3);
   script('t', 5, 164, 3);
   script('t', 5, 180, 3);
   script('e', 5, 196, 3);

   redalert();

   printf("\n\n\n\nThe following page illustrates\n");
   printf("the palette command.  This command\n");
   printf("allows the programmer to control the\n");
   printf("color set and background color with\n");
   printf("one command.  There are a total of 6\n");
   printf("sets of three colors that may be \n");
   printf("displayed.  Three of these are normal;\n");
   printf("while the other three are bright\n");
   printf("versions of the basic three.\n");
   printf("\n\n     Press any key to continue\n");
   waitkey();

   pg6();
   cntue();
   waitkey();
   sm(4);
   backclr(9);


   pg7();
   cntue();
   waitkey();
   sm(4);
   backclr(9);


   pg8();
   atsay(24,7,3,0, "Press any key to continue");
   waitkey();
   sm(4);
   backclr(9);

   pgf();
   waitkey();
   sm(3);
   backclr(1);



   sm(3);
   printf("\n\n\n\n");
   printf("\tPlease address any comments, sugestions or criticisms to the\n");
   printf("\tauthor - we do listen, many of the functions in the library\n");
   printf("\tcurrently are a result of listening to users.  \n");
   printf("\tThis Lirary is designed as a library for Turbo C from \n");
   printf("\tBorland International.  A Microsoft C version 4.0 and 5.0\n");
   printf("\tversion of the library are also available.  Other versions\n");
   printf("\tare possible if there is enough interest in them.\n\n");
   printf("\tAll currently registered owners of the Unicorn Library will be\n");
   printf("\table to update for an update fee.  A preprinted manual will also\n");
   printf("\tbe made available for those who so wish. Registered owners, as with\n");
   printf("\tthe Unicorn Library will, of course, receive ALL source code for the\n");
   printf("\tlibrary.\n\n");
   printf("                             David A.  Miller\n");
   printf("                             Unicorn Software\n");
   printf("                             8018 NE 123rd St\n");
   printf("                             Kirkland, WA 98034\n");
   printf("                             (206) 823 4656 after 6:30PM \n");
   printf("                             (no collect calls PLEASE)  \n");



}


pg1()
{
   atsay(10,5,2,0, "The next page will demonstrate");
   atsay(11,5,2,0, "some of the simple graphics");
   atsay(12,5,2,0, "primitives including dots,");
   atsay(13,5,2,0, "lines, filled boxes and");
   atsay(14,5,2,0, "rectangles. ");
   atsay(20,5,3,0, "Press any key to continue");
}

pg2()
{
   int i;

   for(i=0; i <= 20; i++)
      point(20, 10+i*2, i % 4);
   rect(50,20,80,50,1);
   boxfill(54,24,76,46,2);
   rect1(50,80,80,110,2,1);
   rect2(50,140,80,170,3,1);
   circle(65,240,50,1,1,3);
   paint(65,240,2,3);
   line(120,20,190,300,1);
   line(190,20,120,300,2);
   line(155,20,155,300,3);
}

pg3()
{
   atsay(7,5,3,0, "The following screen will further");
   atsay(8,5,3,0, "demonstrate the circle function");
   atsay(9,5,3,0, "and introduce the rotated ellipse");
   atsay(10,5,3,0, "function.  The latter allows the");
   atsay(11,5,3,0, "user to draw an ellipse with a");
   atsay(12,5,3,0, "specified aspect and angle of");
   atsay(13,5,3,0, "rotation.");
}


pg5()
{

   turnright(150);     /*  Double Triangle  */

   pencolor(1);

   forward(50);

   delay(500);
   turnright(120);
   forward(50);
   delay(500);
   turnright(120);
   forward(100);
   delay(500);
   turnleft(120);
   forward(50);
   delay(500);
   turnleft(120);
   forward(50);

   turnright(180);
   forward(50);

   pencolor(0);

   forward(50);
   turnright(120);
   pencolor(2);
   delay(500);
   forward(100);
   turnright(60);
   delay(500);
   forward(100);
   turnright(60);
   delay(500);
   forward(100);
   turnright(60);
   delay(500);
   forward(100);
   turnright(60);
   delay(500);
   forward(100);
   turnright(60);
   delay(500);
   forward(97);

   turnright(72);
   forward(5);
   pencolor(0);
   forward(48);
   turnright(96);

   turnleft(72);
   pencolor(3);
   forward(90);
   turnright(72);
   forward(90);
   turnright(72);
   forward(90);
   turnright(72);
   forward(90);
   turnright(73);
   forward(90);
   pencolor(0);

   paint(90,160,3,1);
   paint(110,160,3,1);

   paint(40,160,2,3);
   paint(182,160,1,2);

 }


pg6()
{
   palette(2,4,0);
   house();
   palette(0,5,0);
   house();
   palette(0,4,0);
   house();
}


house()
{
   int i;

   for(i=0;i<=120;i++)                 /* Make some stars in the sky */
      point(rand() / 165,rand() / 103, rand() / 8192);

   line(180,0,180,319,1);        /* grass line */
   paint(183,10,1,1);            /*   grass */
   line(150,50,120,110,1);
   line(120,110,152,160,1);

   line(140,70,180,70,1);
   line(140,140,180,140,1);
   line(180,70,180,140,1);

   line(160,80,180,80,1);
   line(160,95,180,95,1);
   line(160,80,160,95,1);


   line(150,110,170,110,1);       /* Window */
   line(150,120,170,120,1);
   line(150,130,170,130,1);
   line(150,110,150,130,1);
   line(160,110,160,130,1);
   line(170,110,170,130,1);
   paint(155,115,2,1);             /* color the widow panes */
   paint(155,125,2,1);
   paint(165,115,2,1);
   paint(165,125,2,1);
   paint(130,110,3,1);            /* color the house  */

   line(130,100,130,120,2);       /* lines in the house */
   line(161,75,161,79,2);
   line(155,86,155,97,2);
   line(165,100,165,107,2);
   line(161,72,161,78,2);
   line(142,70,142,93,2);
   line(147,100,147,116,2);

   paint(170,90,2,1);             /*  color the door   */
   circle(170,90,2,1,1,1);        /*    knob           */
   paint(170,90,2,1);             /*   fill it         */
   line(114,124,128,124,2);       /*   chimney         */
   line(114,136,136,136,2);
   line(114,124,114,136,2);
   line(128,124,136,136,2);
   paint(120,130,2,2);            /*  fill chimney       */
   line(116,125,116,135,3);       /*  horizontal bricks  */
   line(118,125,118,135,3);
   line(120,125,120,135,3);
   line(122,125,122,135,3);
   line(124,125,124,135,3);
   line(126,125,126,135,3);
   line(128,125,128,135,3);
   line(130,128,130,135,3);
   line(132,132,132,135,3);
   line(134,134,134,135,3);

   line(116,130,118,130,3);       /*  vertical bricks */
   line(118,133,120,133,3);
   line(118,127,120,127,3);
   line(120,130,122,130,3);
   line(122,127,124,127,3);
   line(122,133,124,133,3);
   line(124,130,126,130,3);
   line(126,127,128,127,3);
   line(126,133,128,133,3);

   line(128,130,130,130,3);
   line(130,133,132,133,3);

   line(132,130,134,130,3);

   boxfill(140,190,180,210,3);      /*  Tree Trunk   */
   line(168,190,172,188,3);
   line(172,188,180,177,3);
   line(168,210,172,212,3);
   line(172,212,180,223,3);
   line(180,177,180,223,3);         /*  base line of tree  */
   paint(178,185,3,3);              /*  paint the tree     */
   paint(178,215,3,3);

   line(174,190,178,183,2);        /*   root lines in tree */
   line(172,210,178,213,2);
   line(174,198,178,190,2);
   line(176,203,178,207,2);
   circle(150,190,3,3,1,3);
   paint(150,189,3,3);
   circle(160,210,3,1,3,3);
   paint(160,211,3,3);
   point(168,205,2);
   point(171,203,2);
   point(168,193,2);
   point(172,195,2);

   circle(135,216,10,1,2,1);
   paint(135,216,1,1);
   circle(105,180,9,1,1,1);
   paint(105,180,1,1);
   circle(128,180,9,1,3,1);
   paint(128,180,1,1);
   circle(125,228,5,1,2,1);
   paint(125,228,1,1);
   circle(94,200,10,1,3,1);
   paint(94,200,1,1);

   circle(117,200,30,3,5,1);
   paint(116,200,1,1);
   line(110,190,117,200,3);
   line(120,180,128,197,3);
   line(115,200,125,180,3);

   circle(74,100,10,1,1,2);       /*   cloud  */
   paint(74,100,2,2);
   circle(62,130,12,1,2,2);
   paint(62,130,2,2);
   circle(72,70,9,1,2,2);
   paint(72,70,2,2);
   circle(32,58,8,1,2,2);
   paint(32,58,2,2);
   circle(20,68,22,1,2,2);
   paint(20,68,2,2);
   circle(35,130,12,1,2,2);
   paint(35,130,2,2);
   circle(21,95,10,1,2,2);
   paint(21,95,2,2);
   circle(33,125,7,1,2,2);
   paint(33,125,2,2);

   circle(50,100,41,3,7,2);            /*   cloud  */
   paint(50,100,2,2);               /*  fill it */


   line(150,194,167,194,1);
   line(142,197,153,197,1);
   line(165,195,174,195,1);
   line(147,200,153,200,1);
   line(150,205,164,205,1);
   line(153,201,164,201,1);
   line(163,202,172,202,1);


   circle(50,270,15,1,1,3);      /*  moon */
   paint(50,270,03,03);            /*  fill it  */
   point(52,273,1);
   point(43,260,1);
   point(52,262,1);
   point(53,262,1);
   point(52,261,1);
   point(53,273,1);
   point(40,270,1);
   point(60,268,1);
   point(45,281,1);
   point(45,282,1);
   point(44,281,1);
   point(44,272,0);
   point(43,272,0);
   point(43,271,0);
   point(47,265,0);
   point(46,265,0);
   point(46,264,0);
   point(53,275,0);
   point(54,276,0);

   delay(3000);
}


pg7()
{

   rhollow(20, 100, 'F',0,  0,3, .15);
   rhollow(20, 100, 'O',0, 22,3, .15);
   rhollow(20, 100, 'N',0, 44,3, .15);
   rhollow(20, 100, 'T',0, 66,3, .15);
   rhollow(20, 100, 'S',0, 88,3, .15);

   gliss(100,1000,300);

   atsay(6,2,2,0,"  Many different Fonts are Possible");
   atsay(7,2,3,0, "      Including rotated fonts");

   usrfont('T',80,60,2,2,3,0);
   usrfont('H',80,80,2,2,3,0);
   usrfont('I',80,100,2,2,3,0);
   usrfont('N',80,120,2,2,3,0);
   usrfont('L',88,140,1,1,3,0);
   usrfont('E',88,150,1,1,3,0);
   usrfont('T',88,160,1,1,3,0);
   usrfont('T',88,170,1,1,3,0);
   usrfont('E',88,180,1,1,3,0);
   usrfont('R',88,190,1,1,3,0);
   usrfont('S',88,200,1,1,3,0);

   usrfont('S',135,146,1,1,3,1);
   usrfont('U',135,152,1,1,3,1);
   usrfont('B',135,158,1,1,3,1);
   usrfont('/',130,164,1,1,3,0);
   usrfont('S',127,174,1,1,3,1);
   usrfont('U',127,180,1,1,3,1);
   usrfont('P',127,186,1,1,3,1);
   usrfont('E',127,192,1,1,3,1);
   usrfont('R',127,198,1,1,3,1);
   usrfont('S',130,209,1,1,2,0);
   usrfont('C',130,219,1,1,2,0);
   usrfont('R',130,229,1,1,2,0);
   usrfont('I',130,239,1,1,2,0);
   usrfont('P',130,249,1,1,2,0);
   usrfont('T',130,259,1,1,2,0);
   usrfont('S',130,269,1,1,2,0);



   usrfont('X',175,65,3,3,1,0);
   usrfont('2',175,97,2,2,1,1);
   usrfont('+',175,105,3,3,1,0);
   usrfont('X',175,134,3,3,1,0);
   usrfont('-',175,160,3,3,1,0);
   usrfont('4',175,185,3,3,1,0);
   usrfont('=',175,210,3,3,1,0);
   usrfont('0',175,230,3,3,1,0);

}



pg8()
{
   printf("   Below are a few Unit Conversions\n\n\n\n\n");
   printf("%8.6f inches    = %f cm\n",4.0, INCH_CM(4.0));
   printf("%8.6f cm       = %f inches\n",10.0, CM_INCH(10.0));
   printf("%8.6f degrees  = %f radians\n",90.0, DEG_RAD(90.0));
   printf("%8.6f radians   = %f degrees\n",1.0, RAD_DEG(1.0));
   printf("%8.6f ounces    = %f grams\n",4.0, OZ_GM(4.0));
   printf("%8.6f grams    = %f ounces\n",50.0, GM_OZ(50.0));
   printf("%8.6f gallons  = %f liters\n",10.0, GAL_LT(10.0));
   printf("%8.6f liters   = %f gallons\n",50.0, LT_GAL(50.0));
   printf("%8.6f pounds    = %f kilograms\n",4.0, LB_KG(4.0));
   printf("%8.6f kilogram = %f pounds\n",50.0, KG_LB(50.0));

}



pgf()
{


   rgothic(25, 15, 'U', 0,   0,  3, .2);
   rgothic(25, 15, 'n', 0,  16,  3, .2);
   rgothic(25, 15, 'i', 0,  32,  3, .2);
   rgothic(25, 15, 'c', 0,  48,  3, .2);
   rgothic(25, 15, 'o', 0,  64,  3, .2);
   rgothic(25, 15, 'r', 0,  80,  3, .2);
   rgothic(25, 15, 'n', 0,  96,  3, .2);

   rgothic(5, 155, 'S', 0,   0,  3, -.17);
   rgothic(5, 155, 'o', 0,  18,  3, -.17);
   rgothic(5, 155, 'f', 0,  32,  3, -.17);
   rgothic(5, 155, 't', 0,  48,  3, -.17);
   rgothic(5, 155, 'w', 0,  64,  3, -.17);
   rgothic(5, 155, 'a', 0,  80,  3, -.17);
   rgothic(5, 155, 'r', 0,  96,  3, -.17);
   rgothic(5, 155, 'e', 0, 112,  3, -.17);


   atsay(7,5,3,0, "We Hope you have enjoyed seeing");
   atsay(8,5,3,0, "some of the features contained");
   atsay(9,5,3,0, "in the C TOOL BOX for the");
   atsay(10,5,3,0, "IBM PC medium resolution mode");
   atsay(11,5,3,0, "graphics.  The next release");
   atsay(12,5,3,0, "(Dec 87) will contain even");
   atsay(13,5,3,0, "more features.  Currently");
   atsay(14,5,3,0, "registered users will receive.");
   atsay(15,5,3,0, "a FREE UPDATE when released.");

   atsay(17,5,2,0, "       Developed by");
   atsay(18,5,2,0, "      David A. Miller");

   gliss(3000, 200, 100);

   atsay(24,4,2,0, " Press any key to return to Dos");
}




atom()
{
  ellipser(100,160,0,50,25,2);
  ellipser(100,160,0,25,50,2);
  ellipser(100,160,48,50,25,1);
  ellipser(100,160,48,25,50,1);
               
  circle(100,160,70,1,1,1);
  circle(100,160,66,1,1,1);
  paint(100,228,2,1);
  circle(100,160,4,1,1,3);
  paint(100,160,3,3);
}


cntue()
{
   atsay(15,1,2,0, "Press any");
   atsay(16,2,2,0, "Key to");
   atsay(17,1,2,0, "continue");
}

unicorn()
{
   line(66,100,107,156, 3);      /*   horn  */
   line(66,100,113,148, 3);


   line(157,170,158,154, 3);
   line(158,154,172,138, 3);
   line(172,138,173,125, 3);
   line(173,125,166,115, 3);
   line(166,115,151,117, 3);
   line(151,117,130,138, 3);
   line(130,138,120,140, 3);
   line(120,140,98,168, 3);     /* this line ends at the ear  */
   line(98,168,85,165,  3);
   line(85,165,89,172, 3);
   line(89,172,97,180, 3);
   line(97,180,121,224, 3);     /* outline after ear      */
   line(121,224,156,255, 3);
   line(156,255,198,266, 3);    /*  very bottom line  */

   line(157,170,170,180, 3);     /* part to left going down  */
   line(170,180,198,180, 3);     /* very bottom line */

   line(198,180,198,266,3);      /*   line across base of head  */
   paint(188,182,3,3);           /*   fill in the head!!  */
   paint(111,148, 3,3);        /*   fill in the horn    */
   line(148,185,157,170, 2);      /* head starting at bottom  */

   line(152,130,140,142, 2);     /* in head near mouth    */
   line(140,142,133,156, 2);

   line(103,167,123,173,2);     /* between ear and eye   */

   line(173,125,160,138,2);       /*  mouth   */

   line(167,145,181,151,3);       /*  goatee   */
   line(181,151,175,141,3);
   line(175,141,183,139,3);
   line(183,139,173,133,3);
   paint(172,141,3,3);             /*  color the goatee  */

   line(102,149,107,142, 2);       /* lines across the horn   */
   line(97,140, 100,135, 2);
   line(91,134,94,130, 2);
   line(86,127,88,122, 2);
   line(81,121,83,119, 2);
   line(76,113,77,111, 2);

   line(98,180,140,182,2);        /*  mane     */
   line(106,160,118,168,2);
   line(140,182,149,198,2);
   line(130,205,170,208,2);
   line(170,208,180,220,2);
   line(154,232,190,235,2);
   line(190,235,194,245,2);

   ellipser(122,158,42,7,3,2);
   paint(122,158,2,2);
   ellipser(156,123,72,4,2,2);
   paint(156,123,2,2);

   circle(123,157,2,1,1,1);
   paint(123,157,1,1);
   point(123,157,3);

}


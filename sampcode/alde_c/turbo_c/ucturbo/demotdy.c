/*                               Demo.c
*
*    This program is designed to illustate some of the features contained
*  in the Unicorn Library.  It uses the 16 color medium resolution mode of
*  Tandy 1000 computer.  A second version of the library is available
*  to be used with the 4 color medium resolution mode (mode 4) on the
*  IBM-PC computer.
*
*    This program was written to test the Unicorn Library and to illustrate
*  the use of the library functions.  Although it includes some art work
*  please keep in mind that the author claims little or no artistic
*  talent.  In spite of this, the program does illustrate how to call
*  various functions contained in the library.
*
*    The library was originally developed to be used in a public school
*  district in the development of software.  In addition it was used as
*  a learning tool by the author.  The author has had no formal training
*  in C or Assembler and was responsible for software development for
*  the district.  The learning experience was worthwhile and the author
*  feels that many others may also benefit by examining the source code.
*
*    This library is an upgraded version of an earlier release but
`*  further improvements will occur in future releases.  This library
*  now includes a paint command and many functions formally written in
*  the C language have now been written in assembler to improve speed.
*
*    This program and library are both
*                 Copyright (c) 1986 by David A. Miller
*
*  The right to use and give away both are hereby granted as long as the
*  copyright notice is not removed from either, and the restrictions
*  listed in the user agreement list elsewhere are followed.
*
*                           David A. Miller
*                           8018 NE 123rd St.
*                           Kirkland, WA  98034
*                           (206) 823-4656
*                           no collect calls please!
*
*/


#include "uc.h"


main()
{
   int start = 9;   /*  define title start column  */


   /* The following must be declared to return proper values */

   sm(9);
   backclr(9);

   gothic('U',10,start+0,14);
   gothic('n',10,start+25,14);
   gothic('i',10,start+45, 14);
   gothic('c',10,start+60, 14);
   gothic('o',10,start+80,14);
   gothic('r',10,start+100,14);
   gothic('n',10,start+120,14);

   gothic('S',10,start+155,14);
   gothic('o',10,start+175,14);
   gothic('f',10,start+195,14);
   gothic('t',10,start+210,14);
   gothic('w',10,start+225,14);
   gothic('a',10,start+245,14);
   gothic('r',10,start+265,14);
   gothic('e',10,start+285,14);

   raschar('P',50, start + 70, 2, 2, 11);
   raschar('r',50, start + 90, 2, 2, 11);
   raschar('e',50, start + 110, 2, 2, 11);
   raschar('s',50, start + 130, 2, 2, 11);
   raschar('e',50, start + 150, 2, 2, 11);
   raschar('n',50, start + 170, 2, 2, 11);
   raschar('t',50, start + 190, 2, 2, 11);
   raschar('s',50, start + 210, 2, 2, 11);

   victory();

/* opening to 5th symphony */
/*
   soundon();  

   note(1,3,11,10);
   delay(200);
   note(1,3,11,0);
   delay(50);

   note(1,3,11,10);
   delay(200);
   note(1,3,11,0);
   delay(50);

   note(1,3,11,10);
   delay(200);
   note(1,3,11,0);
   delay(50);

   note(1,3,7,10);
   delay(1200);

   note(1,3,9,10);
   delay(200);
   note(1,3,9,0);
   delay(50);

   note(1,3,9,10);
   delay(200);
   note(1,3,9,0);
   delay(50);

   note(1,3,9,10);
   delay(200);
   note(1,3,9,0);
   delay(50);

   note(1,3,6,10);
   delay(1200);


   setatten(1,15);
   setatten(2,15);
   setatten(3,15);
*/
   unicorn();
   atsay(24,3,8,0,"Copyright (c) 1986 David A. Miller");
  


   greenslv();

   cntue();
   waitkey();
   sm(9);       /* this resets the mode which in effect is a
                   clear screen command.                      */
   backclr(9);

   burble(200, 300);

   printf("\n\n\n\n\n A demonstration of Tandy-1000\n");
   printf(" graphics using the 16 color medium\n");
   printf(" resolution mode and the Unicorn\n");
   printf(" library of functions.  This library\n");
   printf(" uses the Lattice C large model.\n");
   printf("   Not all library functions are \n");
   printf(" illustrated in this demo.  The \n");
   printf(" library is capable of many graphics\n");
   printf(" concepts including points, lines, \n");
   printf(" circles, ellipses (also rotated),\n");
   printf(" and colorful print including \n");
   printf(" different fonts.\n\n");
   printf("    We hope you enjoy the demo\n");
   printf("       and using the library!\n\n");
   printf("   Copyright (C) 1986 David A. Miller\n");
   atsay(24,10,8,0,"Press any key to continue");

   waitkey();
   gliss(100, 1500, 3);

   sm(9);       /* this resets the mode which in effect is a
                   clear screen command.                      */
   backclr(14);



   pg1();
   waitkey();
   sm(9);
   backclr(1);



   pg2();
   cntue();
   waitkey();
   burble(300, 450);
   sm(9);
   backclr(1);

   pg3();
   cntue();
   waitkey();
   sm(9);
   backclr(9);

   atom();
   redalert();
   cntue();
   waitkey();
   sm(9);
   backclr(1);

   printf("   The next screen will demonstrate\n\n\n");
   printf("   T u r t l e    F u n c t i o n s");
   printf("\n\n\n\n\n   The delay function is used to");
   printf("\n   slow down the display of lines\n");
   cntue();
   burble(200, 350);
   waitkey();
   sm(9);
   backclr(7);

   pg5();
   cntue();
   waitkey();
   sm(9);
   backclr(1);

   script('C',5,110,15);
   script('o',5,126,14);
   script('l',5,142,13);
   script('o',5,158,12);
   script('r',5,174,11);
   script('s',5,190,10);


   printf("\n\n\n\n   The following page illustrates\n");
   printf("   the full 16 color color-set.\n");
   printf("\n\n        Press any key to continue\n");
   waitkey();

   pg6();
   cntue();
   waitkey();
   gliss(1200, 400, 4);
   sm(9);
   backclr(9);


   pg7();
   cntue();
   waitkey();
   sm(9);
   backclr(9);


   pg8();
   atsay(24,5,8,0,"Press any key to continue");
   waitkey();
   sm(9);
   backclr(1);


   pgf();
   waitkey();
   sm(3);
   backclr(1);

}  /*  End of main program */



pg1()
{
   atsay(10,05,8,0, "The next page will demonstrate");
   atsay(11,05,8,0, "some of the simple graphics");
   atsay(12,05,8,0, "primitives including dots,");
   atsay(13,05,8,0, "lines, filled boxes and");
   atsay(14,05,8,0, "rectangles. ");
   atsay(20,05,7,0, "Press any key to continue");
}

pg2()
{
   int i;

   for(i=0; i <= 20; i++)
      point(20, 10+i*2,i % 16);
   box(50,20,80,50,15);
   boxfill(54,24,76,46,4);
   box1(50,80,80,110,12,14);
   box2(50,140,80,170,13,9);
   circle(65,240,50,1,1,13);
   paint(65,240,4,13);
   line(120,20,190,300,14);
   line(190,20,120,300,12);
   line(155,20,155,300,4);
   line(120,20,120,300,5);
   line(190,20,190,300,15);
}

pg3()
{
   atsay(7,05,12,0,  "The following screen will further");
   atsay(8,05,12,0,  "demonstrate the circle function");
   atsay(9,05,12,0,  "and introduce the rotated ellipse");
   atsay(10,05,12,0, "function.  The latter allows the");
   atsay(11,05,12,0, "user to draw an ellipse with a");
   atsay(12,05,12,0, "specified aspect and angle of");
   atsay(13,05,12,0, "rotation.");
}

pg5()
{
   turnright(150);

   pencolor(14);

   forward(50);

   delay(300);
   turnright(120);
   forward(50);
   delay(300);
   turnright(120);
   forward(100);
   delay(300);
   turnleft(120);
   forward(50);
   delay(300);
   turnleft(120);
   forward(50);

   turnright(180);
   forward(50);

   pencolor(0);

   forward(50);
   turnright(120);
   pencolor(14);
   delay(300);
   forward(100);
   turnright(60);
   delay(300);
   forward(100);
   turnright(60);
   delay(300);
   forward(100);
   turnright(60);
   delay(300);
   forward(100);
   turnright(60);
   delay(300);
   forward(100);
   turnright(60);
   delay(300);
   forward(97);

   turnright(72);
   forward(5);

   pencolor(0);

   forward(48);
   turnright(96);

   turnleft(72);
   pencolor(4);
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

   paint(90,160,4,14);
   paint(110,160,4,14);


   paint(40,160,15,4);
   paint(182,160,14,15);

 }




pg6()
{
   int i;
   sm(9);
   backclr(0);
   for(i=0;i<=150;i++)
      point(rand()/165,rand()/103,rand()/2048);

   line(180,0,180,319,1);        /* grass line */
   paint(183,10,2,1);            /*   grass */
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
   paint(155,115,14,1);             /* color the widow panes */
   paint(155,125,14,1);
   paint(165,115,14,1);
   paint(165,125,14,1);
   paint(130,110,3,1);            /* color the house  */

   line(130,100,130,120,11);       /* lines in the house */
   line(161,75,161,79,11);
   line(155,86,155,97,11);
   line(165,100,165,107,11);
   line(161,72,161,78,11);
   line(142,70,142,93,11);
   line(147,100,147,116,11);

   paint(170,90,6,1);             /*  color the door   */
   circle(170,90,2,1,1,8);        /*    knob           */
   paint(170,90,8,8);             /*   fill it         */
   line(114,124,128,124,4);       /*   chimney         */
   line(114,136,136,136,4);
   line(114,124,114,136,4);
   line(128,124,136,136,4);
   paint(120,130,4,4);            /*  fill chimney       */
   line(116,125,116,135,8);       /*  horizontal bricks  */
   line(118,125,118,135,8);
   line(120,125,120,135,8);
   line(122,125,122,135,8);
   line(124,125,124,135,8);
   line(126,125,126,135,8);
   line(128,125,128,135,8);
   line(130,128,130,135,8);
   line(132,132,132,135,8);
   line(134,134,134,135,8);

   line(116,130,118,130,8);       /*  vertical bricks */
   line(118,133,120,133,8);
   line(118,127,120,127,8);
   line(120,130,122,130,8);
   line(122,127,124,127,8);
   line(122,133,124,133,8);
   line(124,130,126,130,8);
   line(126,127,128,127,8);
   line(126,133,128,133,8);

   line(128,130,130,130,8);
   line(130,133,132,133,8);

   line(132,130,134,130,8);

   boxfill(140,190,180,210,6);      /*  Tree Trunk   */
   line(168,190,172,188,6);
   line(172,188,180,177,6);
   line(168,210,172,212,6);
   line(172,212,180,223,6);
   line(180,177,180,223,6);         /*  base line of tree  */
   paint(178,185,6,6);              /*  paint the tree     */
   paint(178,215,6,6);

   line(174,190,178,183,8);        /*   root lines in tree */
   line(172,212,178,213,8);
   line(174,198,178,190,8);
   line(176,203,178,207,8);
   circle(150,190,3,3,1,6);
   paint(150,189,6,6);
   circle(160,210,3,1,3,6);
   paint(160,211,6,6);
   point(168,205,8);
   point(171,203,8);
   point(168,193,8);
   point(172,195,8);

   circle(135,216,10,1,2,2);
   paint(135,216,2,2);
   circle(105,180,9,1,1,2);
   paint(105,180,2,2);
   circle(128,180,9,1,3,2);
   paint(128,180,2,2);
   circle(125,228,5,1,2,2);
   paint(125,228,2,2);
   circle(104,200,10,1,3,2);
   paint(104,200,2,2);

   circle(117,200,30,3,5,2);
   paint(116,200,2,2);
   line(110,190,117,200,10);
   line(120,180,128,197,10);




   circle(74,100,10,1,1,8);       /*   cloud  */
   paint(74,100,8,8);
   circle(62,130,12,1,2,8);
   paint(62,130,8,8);
   circle(72,70,9,1,2,8);
   paint(72,70,8,8);
   circle(32,58,8,1,2,8);
   paint(32,58,8,8);
   circle(20,68,22,1,2,8);
   paint(20,68,8,8);
   circle(35,130,12,1,2,8);
   paint(35,130,8,8);
   circle(21,95,10,1,2,8);
   paint(21,95,8,8);
   circle(33,125,7,1,2,8);
   paint(33,125,8,8);

   circle(50,100,41,3,7,8);            /*   cloud  */
   paint(50,100,8,8);               /*  fill it */


   line(150,194,167,194,8);
   line(142,197,153,197,8);
   line(165,195,174,195,8);
   line(147,200,153,200,8);
   line(150,205,164,205,8);
   line(153,201,164,201,8);
   line(163,202,172,202,8);


   circle(50,270,15,1,1,7);      /*  moon */
   paint(50,270,7,7);            /*  fill it  */
   point(52,273,8);
   point(43,260,8);
   point(52,262,8);
   point(53,262,8);
   point(52,261,8);
   point(53,273,8);
   point(40,270,8);
   point(60,268,8);
   point(45,281,8);
   point(45,282,8);
   point(44,281,8);
   point(44,272,15);
   point(43,272,15);
   point(43,271,15);
   point(47,265,15);
   point(46,265,15);
   point(46,264,15);
   point(53,272,15);
   point(54,276,15);

   delay(2000);



}

pg7()
{
   hollow('F',15,100,14);
   hollow('O',15,120,14);
   hollow('N',15,140,14);
   hollow('T',15,160,14);
   hollow('S',15,180,14);

   printf("Many different fonts possible");
   usrfont('T',80,60,2,2,4,0);
   usrfont('H',80,80,2,2,4,0);
   usrfont('I',80,100,2,2,4,0);
   usrfont('N',80,120,2,2,4,0);
   usrfont('L',88,140,1,1,4,0);
   usrfont('E',88,150,1,1,4,0);
   usrfont('T',88,160,1,1,4,0);
   usrfont('T',88,170,1,1,4,0);
   usrfont('E',88,180,1,1,4,0);
   usrfont('R',88,190,1,1,4,0);
   usrfont('S',88,200,1,1,4,0);

   usrfont('S',135,146,1,1,14,1);
   usrfont('U',135,152,1,1,14,1);
   usrfont('B',135,158,1,1,14,1);
   usrfont('/',130,164,1,1,14,0);
   usrfont('S',127,174,1,1,14,1);
   usrfont('U',127,180,1,1,14,1);
   usrfont('P',127,186,1,1,14,1);
   usrfont('E',127,192,1,1,14,1);
   usrfont('R',127,198,1,1,14,1);
   usrfont('S',130,209,1,1,14,0);
   usrfont('C',130,219,1,1,14,0);
   usrfont('R',130,229,1,1,14,0);
   usrfont('I',130,239,1,1,14,0);
   usrfont('P',130,249,1,1,14,0);
   usrfont('T',130,259,1,1,14,0);
   usrfont('S',130,269,1,1,14,0);



   usrfont('X',175,65,3,3,14,0);
   usrfont('2',175,97,2,2,14,1);
   usrfont('+',175,105,3,3,14,0);
   usrfont('X',175,134,3,3,14,0);
   usrfont('-',175,160,3,3,14,0);
   usrfont('4',175,185,3,3,14,0);
   usrfont('=',175,210,3,3,14,0);
   usrfont('0',175,230,3,3,14,0);

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
   int start = 9;
   gothic('U',10,start+0,14);
   gothic('n',10,start+25,14);
   gothic('i',10,start+45, 14);
   gothic('c',10,start+60, 14);
   gothic('o', 10,start+80,14);
   gothic('r',10,start+100,14);
   gothic('n',10,start+120,14);

   gothic('S',10,start+150,14);
   gothic('o',10,start+170,14);
   gothic('f',10,start+190,14);
   gothic('t',10,start+205,14);
   gothic('w',10,start+220,14);
   gothic('a',10,start+240,14);
   gothic('r',10,start+260,14);
   gothic('e',10,start+280,14);


   atsay(7,04,14,0, "hopes you have enjoyed seeing");
   atsay(8,04,14,0, "some of the features contained");
   atsay(9,04,14,0, "in the Unicorn library for the");
   atsay(10,04,14,0, "Tandy 1000 16 color medium");
   atsay(11,04,14,0, "resolution mode  (mode 9).");
   atsay(14,04,9,0, "      This Library was");
   atsay(16,04,11,0, "        Developed by");
   atsay(18,04,15,0, "      David A. Miller");

   greenslv();

   atsay(24,02,7,0, "    Press any key to return to Dos");
}




atom()
{
  ellipser(100,160,0,50,25,14);
  ellipser(100,160,0,25,50,12);
  ellipser(100,160,48,50,25,13);
  ellipser(100,160,48,25,50,10);

  circle(100,160,70,1,1,1);
  circle(100,160,66,1,1,1);
  paint(100,228,4,1);
  circle(100,160,4,1,1,3);
  paint(100,160,4,3);
}


cntue()
{
   atsay(15,1,8,0, "Press any");
   atsay(16,2,8,0, "Key to");
   atsay(17,1,8,0, "continue");
}




unicorn()
{
   line(66,100,107,156, 15);      /*   horn  */
   line(66,100,113,148, 15);

   line(157,170,158,154, 15);
   line(158,154,172,138, 15);
   line(172,138,173,125, 15);
   line(173,125,166,115, 15);
   line(166,115,151,117, 15);
   line(151,117,130,138, 15);
   line(130,138,120,140, 15);
   line(120,140,98,168, 15);     /* this line ends at the ear  */
   line(98,168,85,165 , 15);
   line(85,165,89,172, 15);
   line(89,172,97,180, 15);
   line(97,180,121,224, 15);     /* outline after ear      */
   line(121,224,156,255, 15);
   line(156,255,198,266, 15);    /*  very bottom line  */

   line(157,170,170,180, 15);     /* part to left going down  */
   line(170,180,198,180, 15);     /* very bottom line */

   line(198,180,198,266,15);      /*   line across base of head  */
   paint(188,182,15,15);           /*   fill in the head!!  */
   paint(111, 148, 15,15);        /*   fill in the horn    */
   line(148,185,157,170, 8);      /* head starting at bottom  */

   line(152,130,140,142, 8);     /* in head near mouth    */
   line(140,142,133,156, 8);

   line(103,167,123,173,8);     /* between ear and eye   */

   line(173,125,160,138,8);       /*  mouth   */

   line(167,145, 181,151,15);       /*  goatee   */
   line(181,151,175,141,15);
   line(175,141,183,139,15);
   line(183,139,173,133,15);
   line(133,173,145,167,15);
   paint(172,141,15,15);             /*  color the goatee  */

   line(102,149,107,142, 8);       /* lines across the horn   */
   line(97,140, 100,135, 8);
   line(91,134,94,130, 8);
   line(86,127,88,122, 8);
   line(81,121,83,119, 8);
   line(76,113,77,111, 8);

   line(98,180,140,182,8);        /*  mane     */
   line(106,160,118,168,8);
   line(140,182,149,198,8);
   line(130,205,170,208,8);
   line(170,208,180,220,8);
   line(154,232,190,235,8);
   line(190,235,194,245,8);

   ellipser(122,158,42,7,3,8);
   paint(122,158,8,8);
   ellipser(156,123,72,4,2,8);
   paint(156,123,8,8);

   circle(123,157,2,1,1,9);
   paint(123,157,9,9);
   point(123,157,7);
}




victory()
{
  playibm(7,3,320);
  playibm(1,4,320);
  playibm(4,4,320);
  playibm(7,4,480);
  playibm(7,4,160);
  playibm(7,4,320);
  playibm(4,4,480);
  playibm(4,4,160);
  playibm(4,4,320);
  playibm(12,3,320);
  playibm(4,4,320);
  playibm(12,3,320);
  playibm(7,3,960);
}



greenslv()
{
   playibm(6,3,400);

/* 1 */
   playibm(9,3,800);
   playibm(11,3,400);

/* 2 */
   playibm(1,4,500);
   playibm(3,4,300);
   playibm(1,4,400);

/* 3 */
   playibm(11,3,800);
   playibm(8,3,400);

/* 4 */
   playibm(4,3,500);
   playibm(6,3,300);
   playibm(8,3,400);

/* 5 */
   playibm(9,3,800);
   playibm(6,3,400);

/* 6 */
   playibm(6,3,500);
   playibm(5,3,300);
   playibm(6,3,400);

/* 7 */
   playibm(8,3,800);
   playibm(5,3,400);

/* 8 */
   playibm(1,3,800);
   playibm(6,3,400);

/* 9 */
   playibm(9,3,800);
   playibm(11,3,400);

/* 10 */
   playibm(1,4,500);
   playibm(3,4,300);
   playibm(1,4,400);

/* 11 */
   playibm(11,3,800);
   playibm(8,3,400);

/* 12 */
   playibm(4,3,500);
   playibm(6,3,300);
   playibm(8,3,400);

/* 13 */
   playibm(9,3,500);
   playibm(8,3,300);
   playibm(6,3,400);

/* 14 */
   playibm(5,3,400);
   playibm(3,3,400);
   playibm(5,3,400);

/* 15 */
   playibm(6,3,1200);
   playibm(1,1,1);
/* 16 */
   playibm(6,3,1200);

/* 17 */
   playibm(4,4,800);
   playibm(1,1,1);

/* 18 */
   playibm(4,4,400);
   playibm(3,4,400);
   playibm(1,4,400);

/* 19 */
   playibm(11,3,800);
   playibm(8,3,400);

/* 20 */
   playibm(4,3,500);
   playibm(6,3,300);
   playibm(8,3,400);

/* 21 */
   playibm(9,3,800);
   playibm(6,3,400);

/* 22 */
   playibm(6,3,400);
   playibm(5,3,400);
   playibm(6,3,400);

/* 23 */
   playibm(8,3,800);
   playibm(5,3,400);

/* 24 */
   playibm(1,3,1600);
   playibm(1,1,1);

/* 25 */
   playibm(4,4,1600);
   playibm(1,1,1);

/* 26 */
   playibm(4,4,400);
   playibm(3,4,400);
   playibm(1,4,400);

/* 27 */
   playibm(11,3,800);
   playibm(8,3,400);
   playibm(1,1,1);

/* 28 */
   playibm(4,3,500);
   playibm(6,3,300);
   playibm(8,3,400);

/* 29 */
   playibm(9,3,500);
   playibm(8,3,300);
   playibm(6,3,400);

/* 30 */
   playibm(5,3,400);
   playibm(3,3,400);
   playibm(5,3,400);

/* 31 */
   playibm(6,3,1600);
   playibm(1,1,1);

/* 32 */
   playibm(6,3,1200);
}







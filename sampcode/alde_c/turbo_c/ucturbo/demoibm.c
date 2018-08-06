/*                      Demo4.c
*
*    This program is designed to illustate some of the features contained
*  in the UC4 library.  It uses the 4 color medium resolution mode of the
*  IBM PC and compatibles.  A second version of the library is available
*  to be used with the 16 color medium resolution mode (mode 9) on the
*  Tandy 1000 computer.
*/

#include <uc.h>

int i;

main()
{

   int start = 9;

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
   victory();
   unicorn();
   greenslv();
   atsay(24, 3, 2, 0,"Copyright (c) 1986 David A. Miller");
   cntue();
   waitkey();
   sm(4);       /* this resets the mode which in effect is a
                   clear screen command.                      */
   backclr(9);

   printf("\n\n\n  A demonstration of IBM PC graphics\n");
   printf(" using the 4 color med. resolution mode\n");
   printf(" and the unicorn library of functions.\n");
   printf(" This library is designed to work with \n");
   printf(" the Microsoft C small model. It has\n");
   printf(" been written using Microsoft C 4.0 \n");
   printf(" and the Microsoft Assembler 4.0.\n");
   printf("    Not all library functions are\n");
   printf(" illustrated in this demo.  The\n");
   printf(" library is capable of many graphics\n");
   printf(" concepts including points, lines,\n");
   printf(" circles, ellipses (these may be \n");
   printf(" rotated from the vertical or\n");
   printf(" horizontal and colorful print\n");
   printf(" including different fonts.  We\n");
   printf(" hope you enjoy the Unicorn library!\n\n");
   printf(" Version 4.0 pre-release for Microsoft\n");
   printf(" C small model.\n");
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

   burble(200, 300);

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
   greenslv();
   waitkey();
   sm(3);
   backclr(1);

}  /*  End of main program */



pg1()
{
   burble(200,300);

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
   box(50,20,80,50,1);
   boxfill(54,24,76,46,2);
   box1(50,80,80,110,2,1);
   box2(50,140,80,170,3,1);
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

   burble(100,200);

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
   palette(0,4,0);
   house();

}

house()
{
   for(i=0;i<=120;i++)
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

   hollow('F',15,100,3);
   hollow('O',15,124,3);
   hollow('N',15,148,3);
   hollow('T',15,172,3);
   hollow('S',15,196,3);

   gliss(100,1000,300);

   atsay(6,2,2,0,"Many different Fonts are Possible");

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
   int start = 9;

  
   gothic('U',10,start+0,3);
   gothic('n',10,start+25,3);
   gothic('i',10,start+45,3);
   gothic('c',10,start+65,3);
   gothic('o',10,start+85,3);
   gothic('r',10,start+105,3);
   gothic('n',10,start+125,3);

   gothic('S',10,start+155,3);
   gothic('o',10,start+175,3);
   gothic('f',10,start+195,3);
   gothic('t',10,start+215,3);
   gothic('w',10,start+235,3);
   gothic('a',10,start+255,3);
   gothic('r',10,start+275,3);
   gothic('e',10,start+295,3);

   atsay(7,5,3,0, "We Hope you have enjoyed seeing");
   atsay(8,5,3,0, "some of the features contained");
   atsay(9,5,3,0, "in the Unicorn library for the");
   atsay(10,5,3,0, "IBM PC medium resolution mode");
   atsay(11,5,3,0, "graphics.");
   atsay(14,5,2,0, "       Developed by");
   atsay(15,5,2,0, "      David A. Miller");

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

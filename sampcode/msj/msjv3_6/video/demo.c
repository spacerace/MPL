/* ==================================================================
              DEMO.C - Program demonstrating the use of 
              video functions.

              Copyright (c) 1988 Microsoft Systems Journal

              Compile with:  cl /c demo.c
                             link /noi demo+cvideo+asmvideo;
================================================================== */

#include "video.h"
#include <time.h>

struct VideoInfo video;
int buffer[3][510];

main()
{
   char TextColor = 0x07;
   char BorderColor = 0x0F;
   char WinColor1, WinColor2, WinColor3;
   int i, j;

   /* ----- Initialize video structure ----- */
   GetVideoParms(&video);

   /* ----- Set attributes for monochrome or color ----- */
   if (video.ColorFlag) {
       WinColor1 = 0x1F;
       WinColor2 = 0x4F;
       WinColor3 = 0x6F;
   }
   else {
       WinColor1 = 0x0F;
       WinColor2 = 0x70;
       WinColor3 = 0x0F;
   }

   /* ----- Clear the screen and fill it with text ----- */
   ClrScr(TextColor, &video);
   for (i=0; i<video.rows; i++)
       for (j=0; j<=52; j+=26)
           DispString("Microsoft Systems Journal", i, j, TextColor, 
                      &video);
   delay(2.0);

   /* ----- Open a window ----- */
   SaveRegion(5, 2, 14, 34, buffer[0], &video);
   ClrRegion(6, 3, 13, 33, WinColor1);
   TextBox(5, 2, 14, 34, BorderColor, &video);
   for (i=6; i<14; i++)
       DispString("Open the first window here...", i, 4, WinColor1, 
                  &video);
   delay(2.0);

   /* ----- Open a second window ----- */
   SaveRegion(2, 48, 12, 74, buffer[1], &video);
   ClrRegion(3, 49, 11, 73, WinColor2);
   TextBox(2, 48, 12, 74, BorderColor, &video);
   for (i=3; i<12; i++)
       DispString("Then the second here...", i, 50, WinColor2, 
                  &video);
   delay(2.0);

   /* ----- Open a third window overlapping the first two ----- */
   SaveRegion(9, 25, 22, 60, buffer[2], &video);
   ClrRegion(10, 26, 21, 59, WinColor3);
   TextBox(9, 25, 22, 60, BorderColor, &video);
   for (i=10; i<22; i++)
       DispString("And finally a third window here.", i, 27,
                  WinColor3, &video);
   delay(4.0);

   /* ----- Close all windows and exit ----- */
   RestRegion(9, 25, 22, 60, buffer[2], &video);
   delay(1.0);
   RestRegion(2, 48, 12, 74, buffer[1], &video);
   delay(1.0);
   RestRegion(5, 2, 14, 34, buffer[0], &video);
   delay(1.0);
   ClrScr(TextColor, &video);
}
/* ----- Timed delay function ----- */
delay(double sec)
{
   time_t StartTime, EndTime;

   time(&StartTime);
   time(&EndTime);
   while (difftime(EndTime, StartTime) < sec)
       time(&EndTime);
}

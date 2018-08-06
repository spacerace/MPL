main()
{
   cls();
   printf("\n\nSound testing program!\n\n\n");

   printf("Sound a tone\n");
   ctone(1000, 500);
   printf("Sound another tone\n");
   ctone(500, 1000);

   printf("Here is a red alert \n");
   redalert();

   printf("Here is a broad spectrum white noise\n");
   wnoise(100,3000, 9500);
   printf("burble using 200, 300\n");
   burble(200,300);
   printf("burble using 400, 500\n");
   burble(400, 500);
   printf("high frequency white noise\n");
   wnoise(1000, 1500, 9500);
   printf("low frequency white noise\n");
   wnoise(100, 400, 9500);
   printf("Machine gun sound\n");
   mgun();

   printf("An ascending glissando\n");
   gliss(100,1000,300);
   printf("A descending glissando\n");
   gliss(3000, 200, 100);
   printf("\n\nEnd of the sound test program!");
}






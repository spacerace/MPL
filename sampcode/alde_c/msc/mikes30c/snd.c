/************************************************************************
  Demo of the sound.asm routine for Microsoft C 3.0.  Some very low 
  freqs may cause divide error.

        void sound(duration, frequency);

     Enjoy!!   Mike's "C" Board 619-722-8724
************************************************************************/

main()
{
   int i, j;

   for(j = 0; j < 3; j++) {
      for(i = 100; i < 650; i += 25)
         sound(25, i);
      for(i = 650; i > 99; i -= 25)
         sound(25, i);
      }

}

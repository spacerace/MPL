                                         /* Chapter 11 - Program 7 */
main()
{
union {
   int index;
   struct {
      unsigned int x : 1;
      unsigned int y : 2;
      unsigned int z : 2;
   } bits;
} number;

   for (number.index = 0;number.index < 20;number.index++) {
      printf("index = %3d, bits = %3d%3d%3d\n",number.index,
              number.bits.z,number.bits.y,number.bits.x);
   }
}

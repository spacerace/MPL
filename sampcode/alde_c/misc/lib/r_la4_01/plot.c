/* plot.c */
/* plots coordinates on screen */
#define HEIGHT 5
#define WIDTH 10

main()
{
char matrix[HEIGHT][WIDTH];
int x, y;

   for(y = 0; y < HEIGHT; y++)      /* fill matrix with periods */
      for(x = 0; x < WIDTH; x++)
         matrix[y][x] = '.';
   printf("Enter coordinates in form x,y (4,2).\n");
   printf("Use negative numbers to quit.\n");

   while(x >= 0)  {        /* until neg coordinates */
      for(y = 0; y < HEIGHT; y++) {    /*print matrix*/
         for(x = 0; x < WIDTH; x++)
            printf("%c ", matrix[y][x]);
         printf("\n\n");
      }
      printf("Coordinates: ");
      scanf("%d,%d", &x, &y);    /* get coordinates */
      if (x > 9 || y > 4) {
         printf("\n\nLimits: x = 9, y = 4");
         break;
      }
      matrix[y][x] = '\xb0';     /* put gray box there */
   }
}


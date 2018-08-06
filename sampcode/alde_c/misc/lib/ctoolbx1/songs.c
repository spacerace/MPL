extern j[250], jb[250], s[250], sb[250], w[250], wb[250], d[250], db[250];

int melody[250];
int beat[250];


main()
{

   int choice = 0, jx, i = 0;

      printf("\n\nThe following is a demonstration of the background\n");
      printf("music capability of the C Tool Box Library version 1.00\n");
      printf("It is simply written with no safeguards but does\n");
      printf("illustrate a method of obtaining more than one song\n");
      printf("played with the current command structure.  The \n");
      printf("background sound capability will be improved and \n");
      printf("restructured somewhat for the next major release.\n\n");

      printf("You may make a selection of songs to play\n");
      printf("but once a song starts to play do NOT attempt\n");
      printf("to rerun the program and start a new one.  Let\n");
      printf("the first song finish before starting a new one\n");
      printf("\n\nChoose\n\n   1.  Jingle Bells\n");
      printf("   2.  Sugar Plum Fairy\n");
      printf("   3.  We Wish You a Merry Xmas\n");
      printf("   4.  Deck the Halls\n");
      printf("   5.  End the Program\n");

      scanf("%d", &choice);

      if (choice == 1)
      {
         jx = 1;
         while (jx != 255)
         {
           melody[i] =  jx = j[i];
           beat[i]   = jb[i];
           i++;
         }
         jingle();
      }

      if (choice == 2)
      {
         jx = 1;
         while (jx != 255)
         {
           melody[i] = jx = s[i];
           beat[i]   = sb[i];
           i++;
         }
         spf();
      }

      if (choice == 3)
      {
         jx = 1;
         while (jx != 255)
         {
           melody[i] = jx = w[i];
           beat[i]   = wb[i];
           i++;
         }
         wwxmas();
      }

      if (choice == 4)
      {
         jx = 1;
         while (jx != 255)
         {
           melody[i] = jx = d[i];
           beat[i]   = db[i];
           i++;
         }
         deck();
      }
      printf("\n\n\We at Unicorn Software wish you and yours the\n");
      printf("very best of holiday seasons.\n\n");
}

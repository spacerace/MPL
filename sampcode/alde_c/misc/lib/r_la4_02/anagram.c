/* ANAGRAM.C - From page 584 of "Microsoft C Programming for    */
/* the IBM" by Robert Lafore. Creates all possible letter ar-   */
/* rangements in a given word. The version in the book doesn't  */
/* work correctly. The line I changed is marked in the comments */
/* in the source code. The printf() was added because in the    */
/* recursive function rotate is called before the original      */
/* word is printed.                                             */
/****************************************************************/

char word[40];
int length;
main()
{

   printf("\n\nType word: ");
   gets(word);
   length = strlen(word);
   printf("%s\n", word);               /*** ADDED TO SOURCE ***/
   permute(0);
}

/* permute */  /* prints all permutations of word */
               /* word begins at position startperm */
permute(startperm)
int startperm;
{
int j;

   if(length - startperm < 2)       /* exit if one char */
      return;
   for(j = startperm; j < length - 1; j++)  {   /*# chars in word-1*/
      permute(startperm + 1);       /*permutes all but first*/
      rotate(startperm);            /*rotate all chars*/
      printf("%s\n", word);
   }
   permute(startperm + 1); /*restore word to*/
   rotate(startperm);               /*form at entry*/
}

/* rotate() */    /*rotates word one char left*/
                  /*word begins at char pos startrot*/
rotate(startrot)
int startrot;
{
int j;
char ch;

   ch = word[startrot];             /*save first char*/
   for(j = startrot; j < length - 1; j++) /*move other chars left*/
      word[j] = word[j + 1];              /* one position */
   word[length - 1] = ch;
}


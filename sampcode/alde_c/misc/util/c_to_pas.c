/* C to Pascal program -- filter to replace C punctuation and certain
   key words with their Pascal equivalents.

  C form          Pascal form
 -======-        -===========-
    "                 '
    {               BEGIN
    }                END
  <tab>        < 2 blank spaces >
    ()            < nothing >
    &&               AND
    ||                OR
comment start        {
comment end          }
    ==               =
    !=               <>
    =                :=
 printf            writeln
 scanf             readln
 while             WHILE

Usage:  ctopas <infile >outfile

Copyright 1984 Ted Carnevale.
Permission granted for personal nonprofit use.
All other rights reserved.
*/

#include "stdio.h"
#include "ctype.h"
#define EOF -1
#define EOS '\0'

main()
{
   char c, *letter, word[100];
   int wordlnth;

   letter=word;
   wordlnth=0;
   while((c=getchar()) != EOF) {
      if(isalpha(c)) letter[wordlnth++]=c;
      else {
         if(wordlnth>0) {            /* word ready to check */
            letter[wordlnth]='\0';
            wtest(word);             /* pass or replace it */
            wordlnth=0;              /* reset index */
         }
         ctest(c);                   /* process following character */
      }
   }
}       /* Note:  the last word in the file will be missed if it is
immediately followed by EOF with no intervening nonalpha characters.
This is not a problem for Pascal or C source files.  However,
a general purpose workd filter would have to check for a nonzero
wordlength after EOF is reached.   */

wtest(word)
char *word;
{
   char *swapword;

   swapword=word;
   switch(word[0]) {       /* test first letter, then rest of word */
      case 'p':  if(strcmp(word,"printf\0")==0) swapword="writeln\0";
        break;
      case 's':  if(strcmp(word,"scanf\0")==0) swapword="readln\0";
        break;
      case 'w':  if(strcmp(word,"while\0")==0) swapword="WHILE\0";
        break;
      default:  break;       /* pass unchanged */
   }
   swap(swapword);
}

ctest(c)
char c;
{
   switch(c) {
   case '"':  putchar('\'');
     break;
   case '{':  swap("BEGIN\0");
     break;
   case '}':  swap("END;\0");
     break;
   case '\t':  swap("  \0");
     break;
   case '&':  swapif('&','&',"AND  \0");
     break;
   case '|':  swapif('|','|',"OR  \0");
     break;
   case '(':  swapif('(',')',"\0");
     break;
   case '/':  swapif('/','*',"{\0");
     break;
   case '*':  swapif('*','/',"}\0");
     break;
   case '!':  swapif('!','=',"<>\0");
     break;
   case '<':
   case '>':  putchar(c);      /* <x and >x are passed unchanged */
              c=getchar();
              putchar(c);
     break;
   case '=':  identassign();   /* == -> =, = -> := */
     break;
   default :  putchar(c);
     break;

   }
}

swap(s)
char *s;
{
   while(*s != EOS) putchar(*s++);
}

swapif(first, second, replacement)
char first, second, *replacement;
{
   char c;

   if((c=getchar()) == second) swap(replacement);
   else {
      putchar(first);
      putchar(c);
   }

}

identassign()
{
   char c;

   if((c=getchar()) != '=') {          /* assignment */
      putchar(':');
      putchar('=');
   }
   putchar(c);
}

/* end of ctp.c */

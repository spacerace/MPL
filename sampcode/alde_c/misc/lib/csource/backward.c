                                         /* Chapter 5 - Program 6 */
#include "stdio.h"      /* Prototypes for standard Input/Output   */
#include "string.h"     /* Prototypes for string operations       */

void forward_and_backwards(char line_of_char[],int index);

main()
{
char line_of_char[80];
int index = 0;

   strcpy(line_of_char,"This is a string.\n");

   forward_and_backwards(line_of_char,index);

}

void forward_and_backwards(char line_of_char[],int index)
{
   if (line_of_char[index]) {
      printf("%c",line_of_char[index]);
      index++;
      forward_and_backwards(line_of_char,index);
   }
   printf("%c",line_of_char[index]);
}

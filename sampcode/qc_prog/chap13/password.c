/*  password.c -- requires a password to complete the   */
/*                program; illustrates a use of getch() */

#include <stdio.h>
#include <conio.h>
#include <string.h>
#define GUESS_LIMIT 4
#define WORD_LIMIT 10  /* maximum length of password */
#define TRUE 1
#define FALSE 0
char *Password = "I'mOk";
main()
{
     int g_count = 0;           /* guesses taken */
     int w_count;               /* letters accepted */
     int in_count;              /* letters entered  */
     char entry[WORD_LIMIT + 1];
     char ch;
     int correct, go_on;

     do
          {
          puts("Enter the secret password.");
          in_count = w_count = 0;
          /* the following loop accepts no more chars */
          /* than entry[] will hold, but keeps track  */
          /* of total number typed                    */
          while ((ch = getch()) != '\r')
               {
               if (w_count < WORD_LIMIT)
                    entry[w_count++] = ch;
               in_count++;
               }
          entry[w_count] = '\0';
          if (in_count != w_count)
               correct = FALSE;    /* too many chars */
          else
               correct = (strcmp(entry, Password) == 0);
          g_count++;
          go_on = !correct && g_count < GUESS_LIMIT;
          if (go_on)
               puts("\nNo good; try again.");
          } while (go_on);
     if (!correct)
          {
          puts("Sorry, no more guesses.  Bye.");
          return(1);
          }
     puts("Welcome to Swiss bank account 2929100.");
     puts("Your current balance is $10,232,862.61.");
     return(0);
}

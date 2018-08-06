/* GET.C - From page 20 of Micro Cornucopia, No. 40,            */
/* March/April 1988.                                            */
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>     /* Has prototypes for "un-headered" functs */
#include <string.h>     /* Need for string functions */

#define DATAFILE "get.dat" /* Name of directory-info file */
#define MAXBUFF 256
#define MATCH 0
#define FAIL 1

char buff[MAXBUFF],     /* Input buffer for file reads  */
     path[MAXBUFF],     /* Buffer for full path name    */
     drive[3];          /* Buffer for disk drive        */
   /*** Prototypes for non-standard functions ***/
int search_file(FILE *fp, char *where);
int do_switch(void);
FILE *open_data_file(char *name);

int main(int argc, char **argv)
{
int flag;
FILE *fpin;

   if (argc != 2) {
      fprintf(stderr, "Usage: GET directory_abbreviation_name");
      exit(0);
   }
   fpin = open_data_file(DATAFILE);    /* Open the data file */
   flag = search_file(fpin, argv[1]);  /* Find the pseudo-name */
   fclose(fpin);                       /* We're done reading */

   if (flag == FAIL) {
      fprintf(stderr, "\nCannot find %s in data file.\n", argv[1]);
      exit(0);
   }

   flag = do_switch();           /* Try the change */
   if (flag != MATCH) {
      fprintf(stderr, "\nCannot find directory %s\n", path);
      exit(0);
   }
}

/*
   do_switch()
   Function to do the work.
   Argument list:    void
   Return value:     int      0 if successful, 1 on error
*/

int do_switch(void)
{
int flag;
int res;
res = system(drive);       /* Function to issue command to command.com */
if(res < 0)       /* and places us on the correct drive.      */
   perror("System call error");
flag = chdir(path);  /* We should be there now.                  */
if(flag < 0)
   perror("Change directory error");
system("DIR");
return flag;
}

/*
   search_file()
   Function reads the data file that holds the information about the
   directory locations, lookeng for a match on the string passed in.
   The format for the input file is:

   abbreviation  drive   full_pathname\n

   where:
      abbreviation  --  the abbreviation for the full path name
             drive  --  the disk drive designator for the search
     full_pathname  --  the full MSDOS pathname desired
                \n  --  a newline terminates the data for each
                        possible directory
   Example:

   tut   c  \lessons\prog\tutorial

   When the user types: go tut, the user is moved to the \lessons\prog\
                        \tutorial directory on drive C:

   Argument list:    FILE *fp       a FILE pointer to the open file.
                     char *where    a string constant that is the short
                                    name for the directory desired.
   Return value:     int            0 if successful, 1 on no match
*/

int search_file(FILE *fp, char *where)
{
char *ptr, temp[MAXBUFF];
int c, i;

   i = 0;
   while ((c = fgetc(fp)) != EOF) {
      if (c != '\n') {
         buff[i++] = (char) c;
         continue;
      }
      buff[i] = '\0';
      strcpy(temp, buff);
      ptr = strtok(temp, " ");
      if (strcmp(ptr, where) == MATCH) {
         ptr = strtok((char *) 0, " ");      /* Get the disk drive */
         strcpy(drive, ptr);
/*       strcat(drive, ":");   */ /* Removed from book version-doesn't work*/
         ptr = strtok((char *) 0, " ");      /* Get full path name */
         strcpy(path, ptr);
         return MATCH;                    /* ... and we're done */
      }
      i = 0;                  /* No match, start over */
   }
   return FAIL;
}

/*
         open_data_file()
   Function attempts to open the data file that holds the information
   about the directory locations. It does this by searching the PATH
   environment variable. The program assumes that the dat file is in
   the PATH directory.

   Argument list:    char *name     a string constant that is the name
                                    of the file containing the directory
                                    information.
   Return value:     FILE *fp       a FILE pointer to the open file if
                                    successful, program aborts on error.
*/

FILE *open_data_file(char *name)
{
char *ptr, p[MAXBUFF], temp[MAXBUFF];
FILE *fpin;

   ptr = getenv("PATH");            /* Find out where go.dat is.        */
                                    /* NOTE: PATH must be in caps       */
   if (ptr == NULL) {
      fprintf(stderr, "\nPATH not set. Program and data file must \
be on PATH.\n");
      exit(0);
   }
   strcpy(p, ptr);
   ptr = strtok(p, ";");            /* Find first PATH                  */
   while (ptr != NULL) {
      strcpy(temp, ptr);            /* Save a copy of substring         */
/*    strcat(temp, "\\");  */ /* Form a path and file name-doesn't work*/
      strcat(temp, name);
      if ((fpin = fopen(temp, "r")) == NULL) {
         temp[0] = '\0';            /* Start over again         */
         ptr = strtok((char *) 0, ";");   /* Try next path      */
      }
      else
         break;               /* Must have a good fpin          */
   }
   if (fpin == NULL) {
      fprintf(stderr, "\nCannot find %s on default path(s).\n", name);
      exit(0);
   }
   else
      return fpin;
}


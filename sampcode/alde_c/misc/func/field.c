/*
 *    field.c
 *      adapted from the unix utility of the same name by :
 *
 *      Torensoft
 *      1405 Prince of Wales
 *      Suite 602
 *      Ottawa ON Canada
 *      K2C 3J9
 *
 *      Uses prototypes as defined in Ansi X3J11
 */

#include <stdio.h>
#include <ctype.h>

#define VERSION "field V0.1"
#define MAXF 3000
#define MAXL 20000
#define IFS '\t'
#define TRUE 0
#define FALSE 1


int arguments [MAXF];     /* numerical equivalent of command line parameters */
int num_columns;          /* number of columns to print */
int num_fields;           /* number of fields in the current record */
char *fp [MAXF];          /* pointers into line at field boundaries */
char line [MAXL];         /* current line buffer */
int separator = IFS;      /* the field separator */
int check = FALSE;        /* true if you should check for field errors */
int error = FALSE;        /* true if a check error occurs */

/* prototypes for functions in this file */
void explain (char *string);
int fixup (int value);
void putf (int n);


main (argcount, argvalue)
int argcount;
char *argvalue [];
{
        char *cp;
        char **ap;
        int  c;
        int  f;          /* the field counter -- -1 initially */
        int  fc;
        char *parameter;   /* pointer to each command line parameter */

        num_columns = 0;
        f = -1;

        if (argcount <= 1) {
           explain ("no parameters selected.");
        }

        /*
         * read parameters into arguments.
         */
        while (argcount > 1) {
              if (num_columns >= MAXF) {
                 fprintf (stderr, "field: you cannot have more than %d fields.\n", MAXF);
                 exit (1);
              }
              parameter = argvalue [1];
              if (isdigit (parameter [0])) {
                 if ((arguments [num_columns++] = atoi (parameter)) <= 0) {
                    explain ("all fields are numbered from 1.");
                 }

              } else if (parameter [0] == '-') {
                     switch (tolower (parameter [1])) {
                            case 't' : separator = fixup (parameter [2]); break;
                            case 'c' : check = TRUE; break;
                            default  : explain ("bad option."); break;
                     }

              } else {
                explain ("all fields are numbers.");
              }
              argcount --; argvalue ++;
        }

        if (num_columns <= 0) {
           explain ("you must select at least one field.");
        }

        /*
         * read and copy the input.
         */
        num_columns --;
        cp = line;
        ap = fp;
        *ap++ = cp;
        while (1) {
              c = getchar ();
              if (c == '\n' || c == EOF) {
                 if (cp == line && c == EOF){
                    break;
                 }
                 *cp++ = '\0';
                 num_fields = ap - fp;

                 /*
                  * check to see if all lines have the same number of fields.
                  * if f is negative then this is the first line we are processing.
                  */
                 if (f < 0) {
                    f = num_fields;
                 } else {
                    if (f != num_fields) {
                       error = TRUE;
                    }
                 }

                 /*
                  * print this line.
                  */
                 for (fc = 0; fc <= num_columns; fc++) {
                     putf (arguments [fc] - 1);
                     if (fc != num_columns) {
                        putchar (separator);
                     }
                 }
                 if (c == EOF) {
                    break;
                 }
                 putchar ('\n');
                 cp = line;
                 ap = fp;
                 *ap++ = cp;
              } else if (c == separator) {
                 *cp++ = '\0';
                 if (ap >= fp + MAXF) {
                    fprintf (stderr, "field: some lines have more than %d fields in them.\n", MAXF);
                    exit (1);
                 }
                 *ap++ = cp;
              } else {
                 if (cp >= line + MAXL) {
                    fprintf (stderr, "field: some lines have more than %d characters in them.\n", MAXL);
                    exit (1);
                 }
                 *cp++ = c;
              }


        }

        /*
         * print the error message if we have to.
         */
        if (error && check) {
           fprintf (stderr, "field: not all lines had the same number of fields.\n");
        }

        exit (0);
}


/*
 * output field n from the current line to the standard output.
 */

void putf (n)
int n;
{
        char *cp = fp [n];
        int c;

        if (n < 0 || n >= num_fields) {
           return;
        }
        while (c = *cp++) {
              putchar (c);
        }
}

void explain (string)
char *string;
{
        fprintf (stderr, "\n%s\n\n", string);
        fprintf (stderr, "%s:\nusage: [-tc -c] n [n n ...]\n", VERSION);
        fprintf (stderr, "Selects fields or columns from a file.\n");
        fprintf (stderr, "Fields are numbered from 1 and a field may be requested more than once.\n");
        fprintf (stderr, "Options:\n-tc\tSet the field separator to c (default TAB).\n");
        fprintf (stderr, "-c\tCheck that all input records contain the same number of fields.\n\n");
        exit (2);
}

/*
 * compensates for someone typing "-t " to get space for a separator.
 * this is needed since "-t " is changed to "-t" on the command line
 */

int fixup (value)
int value;
{
        if (value == 0) {
           value = ' ';
        }
        return (value);
}
   
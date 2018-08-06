/*                      Show Text File Utility

        This program displays a text file on the screen and redisplays
        it according to the user's use of the page-up, page-down,
        up-arrow, down-arrow, 'u', and 'd' keys. It exits when the user
        types either 'q' or ESC.
*/

#include <stdio.h>

#define CALIBRATE       /* If defined, display pages 21 times for timing. */

/*#define VANILLA*/     /* Define for vanilla, TTY style output. */
/*#define ANSI*/        /* Define for ANSI.SYS style output. */
#define ROMBIOS         /* Define for ROM BIOS style output. */

#define MAXLINES 10000  /* Maximum number of lines in file. */

#define LINESIZE 200    /* Maximum number of characters in a line. */

FILE *inFile;           /* File identifier for file to be displayed. */

int curLine;            /* Where to start displaying from. */

struct cmdType {        /* Command table entry form. */
 char cmdKey;           /* The key code or ASCII value. */
 int (*cmdFunc)();      /* The function to call. */
 int cmdParm;           /* The parameter to pass to it. */
};

int upLines(), quitIt();

struct cmdType keyTab[] = {     /* Normal key table. */
        'u', upLines, 20,       /* 'u' key: scroll backward 20 lines. */
        'U', upLines, 20,       /* 'U' key: scroll backward 20 lines. */
        'd', upLines, -20,      /* 'd' key: scroll forward 20 lines. */
        'D', upLines, -20,      /* 'D' key: scroll forward 20 lines. */
        'q', quitIt, 0,         /* 'q' key: quit. */
        'Q', quitIt, 0,         /* 'Q' key: quit. */
        033, quitIt, 0,         /* ESC key: quit. */
        0, quitIt, 0            /* End-of-table. */
};

struct cmdType zKeyTab[] = {    /* Zero-prefix key table. */
        72, upLines, 1,         /* Up-arrow key: scroll backward 1 line. */
        73, upLines, 20,        /* Up-page key: scroll backward 20 lines. */
        80, upLines, -1,        /* Down-arrow key: scroll forward 1 line. */
        81, upLines, -20,       /* Down-page key: scroll forward 20 lines. */
        0, quitIt, 0            /* End-of-table. */
};

long lineStart[MAXLINES];       /* Offsets from start of file to lines. */
long *nextStart;                /* Pointer to next available line start. */

#define TRUE 1
#define FALSE 0

/*      main(argc,argv)

        Function: Display a text file on the screen.

        Algorithm: Start at the beginning of the file. Display a page, 
        reading the file as needed. Then repeatedly wait for keyboard
        input, and take the appropriate action based on it.
*/

main(argc,argv)

int argc;
char *argv[];

{
        int i;

        /* Check that the user gave us the right number of command
           line parameters. */
        if (argc != 2) {
                /* If not, tell him what's wrong and exit. */
                puts("Usage: show <filename>\n");
                exit(1);
        };

        /* Open the input file, if possible. */
        if ((inFile = fopen(argv[1],"r")) == NULL) {
                /* If it wouldn't open, let the user know and exit. */
                puts("Couldn't open input file.\n");
                exit(1);
        };

        /* Start at the first line in the file. */
        curLine = 0;
        /* Set the file offset for that line in the line start array. */
        lineStart[0] = 0L;
        /* Set nextStart to indicate that this is the only entry we've
           filled in in lineStart so far. */
        nextStart = &lineStart[1];

        /* Repeatedly display a page, and then get and execute commands. */
        do {

#ifdef CALIBRATE
                /* If we're going to calibrate, repeat the following page
                   display 20 times. */
                for (i = 20; i > 0; i--)
#endif CALIBRATE

                        showPage();
        } while (!nextCommand());
}

/*      nextCommand()

        Function: Get and execute the next keyboard command. Return
        TRUE if it's time to quit.

        Algorithm: Get the keyboard input using getch. It then looks
        the character up in the keyTab or zKeyTab tables and executes 
        the function found there (if any). Finally, it returns the value 
        returned by the executed function.
*/

nextCommand()

{
        char c;                 /* The input from the user. */
        struct cmdType *cPtr;   /* A pointer into the command table. */

        /* Get the command from the user. */
        c = getch();
        /* If it isn't an extended code, use the ASCII command table. */
        if (c != 0) cPtr = keyTab;
        /* Otherwise, get the second character (the scan code), and use
           the scan code command table. */
        else {
                c = getch();
               cPtr = zKeyTab;
        };

        /* Search thru the command table for the user input value. */
        for (; cPtr->cmdKey != 0; cPtr++)
                if (cPtr->cmdKey == c)
                        /* If we found it, execute it. */
                        return((*cPtr->cmdFunc)(cPtr->cmdParm));

        /* If we didn't find it in the table, don't do anything at all. */
        return(FALSE);
}

/*      quitIt(dummy)

        Function: Quit command.

        Algorithm: Just return TRUE.
*/

quitIt(dummy)

int dummy;

{
         return(TRUE);
}

/*      upLines(num)

        Function: Move the window up by num lines (may be negative).

        Algorithm: Just adjust curLine.

        Comments: The actual change in the display will be done by
        the showPage function.
*/

upLines(num)

int num;

{
        /* Adjust the curLine global. */
        curLine -= num;
        /* But make sure we don't move back past the beginning of the file. */
        if (curLine < 0) curLine = 0;
        /* Return FALSE -- i.e., don't quit. */
        return(FALSE);
}

/*      showPage()

        Function: Display the page starting at curLine.

        Algorithm: First, clear the screen. Second, make sure that
        the line start array has the first line to be displayed.
        Then, starting at the first line on the screen, read and
        display each visible line.
*/

showPage()

{
        int i,j;
        long *lineStPtr;        /* Pointer into the line start array. */
        char line[LINESIZE];    /* Buffer into which to read lines. */

        /* Clear the screen. */
        clrScreen();

        /* Make sure the first line offset is in the line start array. */
        while (&lineStart[curLine] >= nextStart)
                readLine(nextStart,line);

        /* For each line to be displayed... */
        for (lineStPtr = &lineStart[curLine], i = 24; i; i--, lineStPtr++) {
                /* Read the line in. */
                readLine(lineStPtr,line);
                /* And display it. */
                dispLine(line);
        };
}

/*      readLine(lStPtr,lBuf)

        Function: Read the line whose line start array entry is pointed
        to by lStPtr into the buffer pointed to by lBuf.

        Algorithm: Use fseek to position the file to the line requested,
        then read it in with fgets. Finally, set the next entry in the
        line start array to the file position after the read.

        Comments: The line start array entry pointed to by lStPtr must
        have been filled in already or the results of calling this
        function are undefined.
*/

readLine(lStPtr,lBuf)

long *lStPtr;
char *lBuf;

{
        /* Position the file to the line requested. */
        fseek(inFile,*lStPtr,0);
        /* Read in the line. */
        fgets(lBuf,LINESIZE,inFile);
        /* Record the offset to the next line in the file. */
        if (lStPtr == (nextStart-1)) *nextStart++ = ftell(inFile);
}

/*      clrScreen(), dispLine(lBuf)

        Function: Clear the screen (clrScreen()) or display a line (dispLine())

        Algorithm: The algorithm depends on which of VANILLA, ANSI, and
        ROMBIOS are defined. See the main text of the chapter for more
        details.
*/

clrScreen()

{
#ifdef VANILLA
        int i;

        /* Write blank lines to scroll everything off the screen. */
        for (i = 0; i < 25; i++) dispLine("\n");
#endif VANILLA

#ifdef ANSI
        /* Send the ANSI clear screen sequence. */
        fputs("\033[2J",stdout);
#endif ANSI

#ifdef ROMBIOS
        /* Call the ROM BIOS to clear the screen for us. */
        scrClr();
#endif ROMBIOS
}

dispLine(lBuf)

char *lBuf;

{
#ifdef VANILLA
        /* Use the regular standard output routines. */
        fputs(lBuf,stdout);
#endif VANILLA

#ifdef ANSI
        /* Use the regular standard output routine. */
        fputs(lBuf,stdout);
#endif ANSI

#ifdef ROMBIOS
        /* Call the ROM BIOS. */
        scrPuts(lBuf); scrPuts("\r");
#endif ROMBIOS
}


/*             RAM Sort Main Program

        This program reads the standard input into RAM, sorts
        it by line, and writes it back out again.
*/

#include <stdio.h>
#include <string.h>
#include "rsort.h"

/*      main(argc,argv)

        Function: Read the standard input into memory, sort it, and
        write it to the standard output. If there's a switch "/+<n>",
        then sort starting at column <n>.

        Algorithm: Call readLines to read it in, offLines to adjust
        the sort column if the appropriate switch is present, sortLines
        to sort it, and writeLines to write it out. If CALIBRATE is
        defined, skip the reading and writing and fill the buffer up
        with hard-to-sort stuff; this is used to check the timing.
*/

main(argc,argv)

int argc;
char *argv[];

{
        int i;

        /* Read in the lines, or fill up the buffer with test data. */
#ifdef CALIBRATE
        fillLines();
        puts("Starting sort...");
#ifdef HIST
        startH();
#endif HIST
#else CALIBRATE
        readLines();
#endif CALIBRATE

        /* Check for a starting column parameter. */
        for (i = 1; i < argc; i++)
                if ((argv[i][0] == '/') && (argv[i][1] == '+'))
                        /* Column sort switch found, so set the
                           columns to sort from. */
                        offLines(atoi(&argv[i][2])-1);

        /* Sort the lines. */
        sortLines();

        /* Write the lines back out again. */
#ifdef CALIBRATE
#ifdef HIST
        endH();
#endif HIST
        puts("done.\n");
#else CALIBRATE
        writeLines();
#endif CALIBRATE
}

/*      fillLines()

        Function: Fill the buffer with fake (and hard-to-sort) data.

        Algorithm: Fill up ten-character lines until out of buffer space.

        Comments: Remember, this is intended to calibrate the speed
        of the sort, not to test it. This isn't necessarily the worst
        case for the program, just a case that takes time.
*/

fillLines()

{
        char *bPtr;             /* Pointer into buffer. */
        char fakeIn[10];        /* Current fill string. */
        char *fPtr;             /* Pointer into fakeIn. */

        /* Start with "ZZZZZZZZZZ". */
        for (fPtr = fakeIn; fPtr < &fakeIn[9]; *fPtr++ = 'Z');
        fakeIn[9] = 0;

        /* Fill in the buffer. */
        for (bPtr = buffer, nextLPtr = lPtr;
             (bPtr < &buffer[BUFSIZE-10]) &&
              (nextLPtr < &lPtr[LPTRSIZE]);
             nextLPtr++) {
                /* Set the line pointer entry for this line. */
                nextLPtr->start = nextLPtr->sortAt = bPtr;
                /* Get the next string ("Z...ZZ" -> "Z...ZY", etc.). */
                for (fPtr = &fakeIn[8]; fPtr >= fakeIn; fPtr--)
                        if ((*fPtr)-- == 'A') *fPtr = 'Z';
                        else break;
                /* Copy it into the buffer. */
                strcpy(bPtr,fakeIn);
                bPtr += 10;
        };
}

/*      readLines()

        Function: Read all the lines on the standard input into
        the buffer array, and set the lPtr array to point to each
        line in the buffer.

        Algorithm: Repeatedly call gets() until we've read the
        whole thing in. But make sure we don't overflow any of
        the arrays in the process.

        Comments: Note that there is no way with gets() to ensure
        that it doesn't overflow the buffer. For example, suppose
        we've almost filled up the buffer, and then get a very long
        line. Whatever is in memory after the buffer will get
        overwritten. This is a flaw in the standard I/O library,
        but not one we'll take the time to fix here. We will simply
        exit ASAP so it can't do very much damage.
*/

readLines()

{
        char *bPtr;   /* Pointer into buffer. */

        /* Fill in the buffer from standard input. */
        for (bPtr = buffer, nextLPtr = lPtr;
             (bPtr < &buffer[BUFSIZE]) &&
              (nextLPtr < &lPtr[LPTRSIZE]) &&
              (gets(bPtr) != NULL);
             nextLPtr->start = nextLPtr->sortAt = bPtr, nextLPtr++,
              bPtr += strlen(bPtr)+1);
        /* Check for errors. */
        if (bPtr >= &buffer[BUFSIZE])
                fatal("Input too large for RAM sort");
        if (nextLPtr >= &lPtr[LPTRSIZE])
                fatal("Input has too many lines");
}

/*      writeLines()

        Function: Write all the lines pointed to by the lPtr array
        (presumably they're in the buffer) to the standard output.

        Algorithm: Cycle thru the lines and call puts() for each one.

        Comments: This routine does not depend on the lines being in
        the buffer; they could be anywhere in the data segment. We
        don't use this feature - or rather lack of restriction - here,
        but it could be useful at some future date.
*/

writeLines()

{
        struct linePtr *lPPtr;  /* Line pointer pointer. */

        /* Write out each line. */
        for (lPPtr = lPtr; lPPtr < nextLPtr; puts((lPPtr++)->start));
}

/*      offLines(offset)

        Function: Start search from offset characters after the
        start of the line.

        Algorithm: Go thru the line pointer table and bump the
        sortAt field to point offset bytes past the start field;
        make sure, however, that it doesn't end up pointing past
        the end of the line.
*/

offLines(offset)

int offset;

{
        struct linePtr *lPPtr;  /* Line pointer pointer. */
        int i;                  /* Byte count. */
        char *cp;               /* Pointer into line. */

        /* If there's nothing to do, don't do it. */
        if (offset <= 0) return;

        /* Update each line in the array. */
        for (lPPtr = lPtr; lPPtr < nextLPtr; lPPtr++) {
                /* Find the byte, correcting for end-of-line. */
                for (i = offset, cp = lPPtr->start;
                     (i > 0) && (*cp != 0); i--, cp++);
                /* Set the sortAt pointer. */
                lPPtr->sortAt = cp;
        };
}

/*      fatal(s)

        Function: Report fatal errors and then exit to the system.
        s is a pointer to a string that describes the error.

        Algorithm: Calls fputs() to write the error message to the
        standard error output stream. Then call exit() to abort
        execution.

        Comments: The C library has perror() and abort() routines,
        but this is slightly more portable.
*/

fatal(s)

char *s;

{
        fputs("Fatal error: ",stderr);
        fputs(s,stderr);
        fputs(".\n",stderr);
        exit(1);
}


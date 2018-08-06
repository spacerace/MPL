/* Compile with DeSmet 2.2 or later */

#define MAXSIZE 255     /* maximum line size */
#define CR '\015'       /* code for carriage return (in octal) */

main(argc,argv)         /* UNTAB4.C - Feb. 25, 1984 - V. T. Bly */
int  argc;              /* Contact:     Vincent T. Bly          */
char *argv[];           /*              P. O. Box 409           */
{                       /*              Ft. Belvoir, VA  22060  */
/*
FUNCTION:   Take, as input, a file with tab stops expanded to 4 spaces.
            Create a new file with all tabs replaced by spaces.  The
            new file will have the original filename with the extension
            ".tab".  Example: input file - my.c, output file - my.tab.
            The original file is not altered.

PURPOSE:    To allow files created by the DeSmet SEE editor (including
            all C source files from C-Ware) to by used with the IBM
            Personal Editor.  Files created with editors such as SEE or
            Vedit treat nonstandard tabs differently than other editors
            such as IBM's Personal Editor.  By processing these files
            (with tabs set to 4) with UNTAB4, the ".tab" file can be
            loaded and displayed by any editor.

TO USE:     Type UNTAB4 followed by the filename (including extension)
            of the file you wish to untab.  The file you specify should
            have been created by SEE or a similar editor with tabs set
            to 4.  All C source files from C-ware are supplied this way.
            The new ".tab" file created will be larger than the original,
            since all tabs have been replaced by spaces.  You can shrink
            it back by loading it into the Personal Editor and resaving
            it.  This, again, will substitute tabs for spaces, but in a
            different way than that used by SEE.
*/
    int   s_file, d_file, e, i, j, k, l;
    int   tab = 4;
    char  *tabname, c, inbuff[256], outbuff[256];

    if (argc < 2) {
        puts("** Need a filename **\t");
        exit(1);
    }
    tabname = tabext(argv[1]);                  /* get destination file name */
    s_file = fopen(argv[1], "r");               /* open source file */
    if (s_file == 0) {                          /* abort if can't open file */
        printf("* Cannot Open: %s *\n", argv[1]);
        exit(1);
    }
    d_file = fopen(tabname, "w");               /* open destination file */
    if (d_file == 0) {                          /* abort if can't open file */
        printf("* Cannot Open: %s *\n", tabname);
        exit(1);
    }
    do {
        e = fgets(inbuff, MAXSIZE, s_file);     /* get a line from the file */
        i = 0;  k = 0;
        while ((c = inbuff[i++]) != '\0') {     /* while not end of line */
            if (c == '\t') {                    /* if character is a tab */
                l = k;
                for (j = 0; j < (tab - (l % tab)); j++)
                    outbuff[k++] =' ';          /* expand tab to "tab" spaces */
            } else if (c != CR) {               /* else, if not a CR, */
                outbuff[k++] = c;               /* output character */
            }
        }
        outbuff[k] = '\0';                      /* output end of line char. */
        fputs(outbuff, d_file);                 /* write line to output file */
        puts("* ");                             /* display "*" on screen */
    } while (e != 0);                           /* loop until end of file */
    close(s_file);                              /* close files */
    close(d_file);
}

tabext(oldname)             /* Create a new filename w/.tab extension */
char  *oldname;
{
    int   l;
    char  *index(), *p;
    static char tabname[13];

    l = strlen(oldname);            /* get length of oldname */
    p = index(oldname, '.');        /* get pointer to '.' in oldname */
    if (p > 0)
        l = p - oldname;            /* now l = # of chars up to extension */
    strncpy(tabname, oldname, 8);   /* copy name up to extension */
    strncpy(tabname + l, ".tab", 5);/* add ".tab" to form tabname */
    return(tabname);
}

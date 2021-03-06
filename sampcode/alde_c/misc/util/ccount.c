/*
	Title:		ccount

	Function:	Counts number of lines of code, lines with comments,
			and total lines in C source programs.

	Usage:		ccount <file1> [<file2> ...]
			ccount		(prints help info)

	History:	May 1983 - Created by Deb Brown, Mitre
			(Based on Tom Anderson's cnest.)
*/

#include <stdio.h>

#define TRUE -1
#define FALSE 0

main (argc, argv)
unsigned int	argc;
char   *argv[];

{
    char    c;
    char    prev_char;
    unsigned register int   in_comment;
    unsigned register int   line_number;
    unsigned register int   file_index;
    unsigned register int   count_code;
    unsigned register int   count_com;
    unsigned register int   some_comment;
    unsigned register int   some_code;
    unsigned register int   total_code = 0;
    unsigned register int   total_com = 0;
    unsigned register int   total_lines = 0;
    char    status;
    FILE   *chk_file;

    status = 0;

if (argc == 1) {
	printf ("Usage: %s <file1> [<file2> ...]\n", argv [0]);
	printf ("Output: lines_of_code lines_of_comments total_lines\n");
	exit ();
}

    for (file_index = 1; file_index < argc; ++file_index)
    {
	line_number = 1;
	in_comment = FALSE;
	count_code = 0;
	count_com = 0;
	prev_char = ' ';
	if (argc == 1)
	    chk_file = stdin;
	else
	{
	    if ((chk_file = fopen (argv[file_index], "r")) == (FILE *) NULL)
	    {
		fprintf (stderr, "%s: Can't access %s", argv[0], argv[file_index]);
		status = 2;
		continue;
	    }
	}

	while ((c = fgetc (chk_file)) != EOF)
	{
	    switch (c)
	    {
		case '\n':
		    line_number++;
		    if (some_comment)
			++count_com;
		    if (some_code)
			++count_code;
		    some_comment = FALSE;
		    some_code = FALSE;
		    break;
		case '/':
		    if (prev_char == '*')
		    {
			in_comment = FALSE;
			some_comment = TRUE;
		    }
		    break;
		case '*':
		    if (prev_char == '/')
		    {
			in_comment = TRUE;
			some_comment = TRUE;
		    }
		    break;
case ' ':
case '\t':
	break;
		default:
		    if (in_comment)
			some_comment = TRUE;
		    else
			some_code = TRUE;
		    break;
	    }
	    prev_char = c;
	}
	fclose (chk_file);
	total_code += count_code;
	total_com += count_com;
	total_lines += (--line_number);
if (argc > 1)
	printf ("%7d %7d %7d %s\n",
	     count_code, count_com, line_number, argv [file_index]);
else
	printf ("%7d %7d %7d\n", count_code, count_com, line_number);
    }
if (argc > 2)
    printf ("\n%7d %7d %7d Total\n", total_code, total_com, total_lines);
    exit (status);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
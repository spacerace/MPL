#include <stdio.h>
#include <sgtty.h>
instruct()
{
	static char *instructions[] =
	{
		"Rekey : Usage - rekey fname\n",
		"Where fname is the name of the key redefinition file you\n",
		"wish to create.\n\n",
		"Hit the key you want to redefine, then type the string you\n",
		"want the key defined as, followed by a carriage return.\n",
		"Use the following escapes to insert control chars in the string:\n",
		"                \\n = crlf sequence\n",
		"                \\t = tab\n",
		"                \\b = backspace\n",
		"                \\r = carriage return\n",
		"                \\f = form feed\n",
		"                \\\\  = backslash\n",
		"Hit the escape key to end this program\n",
		"to clear all keyboard definitions\n",
		"To redefine the keyboard, use the command\n",
		"               type fname<cr>\n",
		"where fname is the name of the file you created with rekey\n",
		NULL
	};
	register char **instructor = instructions;
	while (*instructor)
	{
		write(2,*instructor,strlen(*instructor));
		instructor++;
	}
}

/*
** returns null if normal character, returns extended character otherwise.
** character read is stuffed into *c
*/
ext_getc(c)
	register int *c;
{
	register int returnval;
	struct sgttyb gtty;
	ioctl(stdin,TIOCGETP,&gtty);
	gtty.sg_flags |= RAW;
	ioctl(stdin,TIOCSETP,&gtty);
	if (*c = getc(stdin))
		returnval = NULL;
	else
		returnval = *c = getc(stdin);
	if (*c == 3 || *c == 26 || *c == 27)
	{
		*c = EOF;
		return EOF;
	}
	gtty.sg_flags &= (RAW ^ 0xFFFF);
	ioctl(stdin,TIOCSETP,&gtty);
	return returnval;
}

char keybuf[128];
char tmpbuf[128];

map_string(to,from)
	char* to;
	char* from;
{
	while (*from)
	{
		if (*from == '\\')
		{
			switch (*++from)
			{
			case 'n':
				*to++ = '\r';
				*to++ = '\n';
				break;
			case 't':
				*to++ = 9;
				break;
			case 'b':
				*to++ = 8;
				break;
			case 'r':
				*to++ = '\r';
				break;
			case 'f':
				*to++ = '\f';
				break;
			default:
				*to++ = *from;
			}
		} else
		{
			*to++ = *from;
		}
		from++;
	}
}

define_key(op)
	FILE *op;
{
	int c;
	char *string;
	register int is_extended;
	if (EOF == (is_extended = ext_getc(&c)))
		return EOF;
	if (is_extended)
	{
		if (c >= 59 && c <= 68)
		{
			printf("F%d =",c-58);
		} else if (c >= 84 && c <= 93)
		{
			printf("Shift F%d =",c-83);
		} else if (c >= 94 && c <= 103)
		{
			printf("Ctrl F%d =",c-103);
		} else if (c >= 104 && c <= 113)
		{
			printf("Alt F%d =",c-103);
		} else
		{
			switch (c)
			{
			case 71:
				printf("Home=");
				break;
			case 72:
				printf("%c=",24);
				break;
			case 73:
				printf("Pg Up=");
				break;
			case 75:
				printf("Left Arrow="); /* cursor left */
				break;
			case 77:
				printf("Right Arrow="); /* cursor right */
				break;
			case 79:
				printf("End=");
				break;
			case 80:
				printf("%c=",25); /* cursor down */
				break;
			case 81:
				printf("Pg Dn=");
				break;
			case 82:
				printf("Ins=");
				break;
			case 83:
				printf("Del=");
				break;
			case 119:
				printf("Ctrl Home=");
				break;
			case 132:
				printf("Ctrl Pg Up=");
				break;
			case 115:
				printf("Ctrl Left Arrow="); /* cursor left */
				break;
			case 116:
				printf("Ctrl Right Arrow="); /* cursor right */
				break;
			case 117:
				printf("Ctrl End=");
				break;
			case 118:
				printf("Ctrl Pg Dn=");
				break;
			default:
				break;
			}
		}
		if (NULL == gets(tmpbuf))
			return EOF;
		map_string(keybuf,tmpbuf);	/* map newlines to crlfs */
		/* output the redefines string to file */
		fprintf(op,"\033[0;%d",c);
		string = keybuf;			/* control string */
		while (*string)
		{
			fprintf(op,";%d",*string);	/* spit each decimal character */
			++string;
		}
		fprintf(op,"p");				/* terminate the control string */

	}
}

main(argc,argv)
	int argc;
	char *argv[];
{
	FILE *op;
	if (argc == 1)
		instruct();
	if (!--argc)	/* if no args are left */
		exit(0);
	if (NULL == (op = fopen(*(++argv),"w")))
	{
		fprintf(stderr,"rekey : cant open %s\n",*argv);
		exit(1);
	}
	while (EOF != define_key(op))
		;
	exit(0);
}

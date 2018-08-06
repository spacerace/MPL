/*
*	Wildcard matching routine for MSDOS V2.0, Lattice C V2.0.
*	    Ross Nelson
*	    10 November 1983
*
*	Requires "find1st" and "findnext" from FIND.ASM.  User passes
*	in a legal MSDOS wildcard, an attribute bit mask, and a buffer.
*	On the first call, the wildcard is passed to DOS, and the
*	routine returns 0 if there was no match.  If a match occurs,
*	the first matching file name in the directory is copied into
*	the buffer.  Subsequent calls ignore the first two parameters
*	and copy the next succeding files to the buffer at the third
*	parameter.  When no further matches exist the function returns
*	FALSE (0).  Typical usage is:
*
*		if (!wildcard(pattern, 0x10, filename))
*			/* error, no match */
*		else
*			do {
*				/* process filename */
*			} while (wildcard(0, 0, filename));
*
*
*/

#include <ctype.h>

struct dta {
	char reserve[21];
	char attrib;
	unsigned time;
	unsigned date;
	long size;
	char name[13];
	};

static struct dta info;
static int repeat = 0;
static char dir[64];

int wildcard (wc, attr, file)
char *wc, attr, *file;
{
	if (!repeat) {
		get_header (wc, dir);	/* save directory prefix */
		repeat = find1st (wc, attr, &info);
		}
	else
		repeat = findnext (&info);
	if (repeat) {
		strcpy (file, dir);	/* directory prefix */
		strcat (file, info.name);  /* file name */
		while (*file) {
			*file = tolower (*file);
			file++;
			}
		}
	return (int) repeat;
}

static get_header (s, head)
char *s, *head;
{
char *ptr;

	/* Strip device or directory header e.g. a: or \etc\ */
	ptr = s + strlen (s);
	while (s < ptr--)
		if ((*ptr == '\\') || (*ptr == '/') || (*ptr == ':'))
			break;
	while (s <= ptr)
		*head++ = *s++;
	*head = '\0';
}

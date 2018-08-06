/*
 *	getkey -- return a code for single combo keystrokes
 *	- returns a unique code for each keystroke or combination
 *	- ignores "Ctrl-Break" input
 */

#include <dos.h>
#include <local\std.h>
#include <local\doslib.h>
#include <local\keydefs.h>

int
getkey()
{
	int ch;

	/* normal key codes */
	if ((ch = bdos(KEYIN, 0, 0) & LOBYTE) != '\0')
		return (ch);

	/* convert scan codes to unique internal codes */
	return ((bdos(KEYIN, 0, 0) & LOBYTE) | XF);
}

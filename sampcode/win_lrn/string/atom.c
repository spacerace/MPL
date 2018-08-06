/*	Atom.c 
 *
 * This program is designed to be used in conjunction with atom2. These
 * two programs will demonstrate the use of atoms. Atoms are integers
 * that uniquely identify character strings. Atoms are stored in tables.
 * These two programs will share certain global atom tables.
 *
 */
#include "windows.h"
#include "stdio.h"
#include "process.h"

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
       int     IDForAtom;
		 int		IDForAtom2;
		 int		ActualAtomSize;
		 int		AtomMaxSize = 20;
		 int     MB_Return;
       HANDLE  hToAtom;
       BOOL	   bTableAdded;
       char    szTempBuff[80];
		 char		szMainBuff[80];

   bTableAdded = InitAtomTable(101);
      /* Only neccessary if you want more than 37 entries in your atom
       * table. NOTE: 101 is the number of possible entries and should
       * be PRIME
       */

	/* Deal with local atoms	*/
	IDForAtom = AddAtom((LPSTR)"This is an Example String");

	IDForAtom = FindAtom((LPSTR)"This is an Example String");
	ActualAtomSize = GetAtomName(IDForAtom, (LPSTR)szTempBuff, AtomMaxSize);
	hToAtom = GetAtomHandle(IDForAtom);

	IDForAtom = DeleteAtom(IDForAtom);

	/* Deal with global atoms	*/
	IDForAtom2 = GlobalAddAtom((LPSTR)"This is another");

	sprintf ( szMainBuff, "%s%s%d%s%d", szTempBuff,"ID is : ",IDForAtom,
		"\nID2 is : ", IDForAtom2);
	MB_Return = MessageBox(NULL, (LPSTR)szMainBuff, (LPSTR)"Atom Example", MB_OK);
	MB_Return = MessageBox(NULL, (LPSTR)"Continue test by Running Atom2", (LPSTR)"Atom Example", MB_OK);
}

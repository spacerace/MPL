/*	Atom2.c 
 *
 * This program is designed to be used in conjunction with atom. These
 * two programs will demonstrate the use of atoms. Atoms are integers
 * that uniquely identify character strings. Atoms are stored in tables.
 * These two programs will share certain global atom tables.
 *
 * NOTE: Run atom first.
 *
 */
#include "windows.h"

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
	int		IDForAtom;
	int		ActualAtomSize;
	int		AtomMaxSize = 20;
	int		MB_Return;
	HANDLE	hToAtom;
	BOOL		bTableAdded;
	char		szMainBuff[80];
	char		szbuff[20];

	IDForAtom = GlobalFindAtom((LPSTR)"This is another");
	ActualAtomSize = GlobalGetAtomName(IDForAtom, (LPSTR)szbuff, AtomMaxSize);

	sprintf ( szMainBuff, "%s%s%s%d","string retrieved: ",szbuff," ID is : ",IDForAtom);
	MB_Return = MessageBox(NULL, (LPSTR)szMainBuff, (LPSTR)"Atom2 Example", MB_OK);
	IDForAtom = GlobalDeleteAtom(IDForAtom);
}

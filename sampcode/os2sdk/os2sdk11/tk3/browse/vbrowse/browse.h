/*
    VIO File Browser Headers
    Created by Microsoft Corporation, 1989
*/

/*
    Constants
*/
#define NUM_DATA_LINES	4096
#define	MAXLINELENGTH	256

#define ESC		1
#define LINE_UP		0x48
#define LINE_DOWN	0x50
#define PAGE_UP		0x49
#define PAGE_DOWN	0x51
#define HOME_KEY	0x47
#define END_KEY		0x4f

#define BOTTOM		(sTopLine - sRows + 1)

/*
    Function prototypes
*/
/* Toplevel Routines */
int	cdecl main(int, char *[]);
VOID	ManipulateFile(VOID);
SHORT	ReadFile(VOID);

/* Line Manipulation Routines */
SHORT	StoreLine(char *);
SHORT	RetrieveLine(char **, USHORT);

/* Screen Control Routines */
VOID	DisplayScreen(USHORT, BOOL);
VOID	Refresh (USHORT, USHORT, USHORT);

/* User Input Routines */
CHAR	GetKbdInput(VOID);

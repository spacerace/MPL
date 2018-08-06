/*
	CDLL.C

	C source code for the dynlink library CDLL.LIB.  Requires
	the module CINIT.ASM.

	Compile with:  C> cl /c /Asnu /Gs cdll.c
	Assemble CINIT.ASM with:  C> masm /Mx cinit.asm;
	Link with:  C> link /NOI /NOD cdll+cinit,cdll.dll,,os2,cdll
	Create CDLL.LIB with:  C> implib cdll.lib cdll.def

	Copyright (C) 1988 Ray Duncan
*/

int _acrtused = 0;	    /* don't link startup */

#define STDOUT 1            /* standard output handle */

#define API unsigned extern far pascal 

API DosWrite(unsigned, void far *, unsigned, unsigned far *);

static char funcmsg[] = "\nCDLL.MYFUNC is executing\n";
static char initmsg[] = "\nCDLL.C_INIT is executing\n";


/*
    MYFUNC is exported for use by appliation programs;
    it displays a message and returns the sum of 2 numbers.
*/

int far pascal MYFUNC(int a, int b)
{
    unsigned wlen;          /* receives length written */

                            /* display message that 
                               MYFUNC is executing */
    DosWrite(STDOUT,funcmsg,sizeof(funcmsg)-1,&wlen);
    
    return(a+b);            /* return function result */
}


/*
    C_INIT is called from the entry point in CINIT.ASM when
    a client process dynamically links to the library.
*/

int far pascal C_INIT(void)
{
    unsigned wlen;          /* receives length written */

                            /* display message that 
                               C_INIT is executing */
    DosWrite(STDOUT,initmsg,sizeof(initmsg)-1,&wlen);

    return(1);              /* return success code */
}


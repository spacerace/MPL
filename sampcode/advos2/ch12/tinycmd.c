/*
	TINYCMD.C

	A simple command interpreter for OS/2.

	Compile with:  C> cl tinycmd.c

	Usage is:  C> tinycmd

	Copyright (C) 1988 Ray Duncan
*/

#include <stdio.h>
#include <string.h>
#include <memory.h>
                                        /* macro to return number of
                                           elements in a structure */
#define DIM(x) (sizeof(x) / sizeof(x[0]))

#define INPSIZE 80                      /* maximum input length */

#define API unsigned extern far pascal	/* OS/2 API prototypes */

API DosExecPgm(char far *, int, int, char far *, char far *, 
               int far *, char far *);
API DosGetVersion(char far *);

unsigned intrinsic(char *);             /* local function prototypes */
void extrinsic(char *);
void cls_cmd(void);
void ver_cmd(void);
void exit_cmd(void);

struct _commands {                      /* intrinsic commands table */
    char *name;                         /* command name */
    int  (*fxn)();                      /* command function */
    } 
      commands[] = { "CLS",   cls_cmd,  /* built-in commands */         
                     "VER",   ver_cmd,
                     "EXIT",  exit_cmd, };

main(int argc, char *argv[])
{   
    char input[INPSIZE];                /* keyboard input buffer */

    while(1)                            /* main interpreter loop */
    {   
        printf("\n>> ");                /* display prompt */
        memset(input, 0, INPSIZE);      /* initialize input buffer */
        gets(input);                    /* get keyboard entry */
        strtok(input, " ;,=\t");        /* break out first token */
	strupr(input);			/* and fold to uppercase */

        if(! intrinsic(input))          /* if intrinsic command,
                                           run its function */
	    extrinsic(input);		/* else assume EXE file */
    }
}

/*      
    Try to match first token of command line with intrinsic
    command table.  If a match is found, run the associated 
    routine and return true; otherwise, return false.
*/
unsigned intrinsic(char *input)
{   
    int i;                              /* scratch variable */

    for(i=0; i < DIM(commands); i++)    /* search command table */
    {   
        if(! strcmp(commands[i].name, input))
        { 
            (*commands[i].fxn)();       /* if match, run routine */
            return(1);                  /* and return true */
        }
    }
    return(0);                          /* no match, return false */
}

/* 
    Append .EXE to first token and attempt to execute program, 
    passing address of argument strings block and null pointer 
    for environment (child process inherits TINYCMD's environment).
*/
void extrinsic(char *input)
{   
    char pbuff[INPSIZE];                /* buffer for program name */
    unsigned status;                    /* value from DosExecPgm */
    int cinfo[2];                       /* child process info */

    strcpy(pbuff, input);               /* copy first command token 
                                           to local buffer */
    strcat(pbuff, ".EXE");              /* append .EXE to token */

                                        /* try to execute program */
    if(DosExecPgm(NULL, 		/* object name buffer pointer */
		  0,			/* object name buffer length */
                  0,                    /* synchronous exec. mode */
                  input,                /* argument strings */
                  NULL,                 /* use parent's environment */
                  cinfo,                /* receives termination info */
		  pbuff))		/* program name pointer */

        printf("\nBad command or filename\n");
}

/*      
    These are the subroutines for the intrinsic commands.
*/
void cls_cmd(void)                      /* CLS command */
{   
    printf("\033[2J");                  /* ANSI escape sequence */
}                                       /* to clear screen */

void ver_cmd(void)                      /* VER command */
{   
    char verinfo[2];                    /* receives version info */

    DosGetVersion(verinfo);		/* get and display version */
    printf("\nOS/2 version %d.%02d\n", verinfo[0], verinfo[1]);
}

void exit_cmd(void)                     /* EXIT command */
{   
    exit(0);                            /* terminate TINYCMD */
}


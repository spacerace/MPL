/*
	SHELL.C		Simple extendable command interpreter
				for MS-DOS version 2.0 and later

	Copyright 1988 Ray Duncan

	Compile:	C>CL SHELL.C

	Usage:		C>SHELL
*/

#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <signal.h>

									/* macro to return number of
									   elements in a structure */
#define dim(x) (sizeof(x) / sizeof(x[0]))

unsigned intrinsic(char *);			/* function prototypes */
void extrinsic(char *);
void get_cmd(char *);
void get_comspec(char *);
void break_handler(void);
void cls_cmd(void);
void dos_cmd(void);
void exit_cmd(void);

struct cmd_table {					/* intrinsic commands table */
		           char *cmd_name;
           		   int  (*cmd_fxn)(); 
				 }   commands[] =

               	 { "CLS",   cls_cmd,
                   "DOS",   dos_cmd,
                   "EXIT",  exit_cmd, };

static char com_spec[64];			/* COMMAND.COM filespec */


main(int argc, char *argv[])
{   
	char inp_buf[80];             	/* keyboard input buffer */

    get_comspec(com_spec);			/* get COMMAND.COM filespec */

									/* register new handler
							     	   for Ctrl-C interrupts */
	if(signal(SIGINT, break_handler) == (int(*)()) -1)
    {	
		fputs("Can't capture Control-C Interrupt", stderr);
	    exit(1); 
    }

    while(1)                       	/* main interpreter loop */
    {	
		get_cmd(inp_buf);           /* get a command */
        if (! intrinsic(inp_buf) )	/* if it's intrinsic,
                                       run its subroutine */
           extrinsic(inp_buf);		/* else pass to COMMAND.COM */
        }
}


/*      
	Try and match user's command with intrinsic command 
	table.  If a match is found, run the associated routine 
	and return true, else return false.
*/

unsigned intrinsic(char *input_string)
{   
	int i, j;						/* some scratch variables */

									/* scan off leading blanks */
    while(*input_string == '\x20') input_string++ ;

									/* search command table */
    for(i=0; i < dim(commands); i++)
    {	
		j = strcmp(commands[i].cmd_name, input_string);

        if(j == 0) 					/* if match, run routine */
        { 
			(*commands[i].cmd_fxn)();  
            return(1);				/* and return true */
        }
	}
    return(0);						/* no match, return false */
}


/* 
	Process an extrinsic command by passing it
    to an EXEC'd copy of COMMAND.COM.
*/

void extrinsic(char *input_string)
{   
	int status;						

    status = system(input_string);	/* call EXEC function */

    if(status) 						/* if failed, display
									   error message */
	    fputs("\nEXEC of COMMAND.COM failed\n", stderr);
}


/* 
	Issue prompt, get user's command from standard input,
    fold it to upper case.
*/

void get_cmd(char *buffer)
{   
	printf("\nsh: ");                   /* display prompt */
    gets(buffer);                       /* get keyboard entry */
    strupr(buffer);                     /* fold to upper case */
}       


/*	
	Get the full path and file specification for COMMAND.COM
	from the "COMSPEC=" variable in the environment.
*/

void get_comspec(char *buffer)
{   
	strcpy(buffer, getenv("COMSPEC"));

    if(buffer[0] == NULL)		
    {   
		fputs("\nNo COMSPEC in environment\n", stderr);
 	    exit(1);
    }
}


/*
	This Ctrl-C handler keeps SHELL from losing control.
	It just re-issues the prompt and returns.
*/

void break_handler(void)
{   
	signal(SIGINT, break_handler);		/* reset handler */
    printf("\nsh: ");                   /* display prompt */
}	


/*      
	These are the subroutines for the intrinsic commands.
*/

void cls_cmd(void)         				/* CLS command */
{   
	printf("\033[2J");  				/* ANSI escape sequence */
}										/* to clear screen */

void dos_cmd(void)         				/* DOS command */
{   
	int status;
										/* run COMMAND.COM */
    status = spawnlp(P_WAIT, com_spec, com_spec, NULL);

    if (status)	
		fputs("\nEXEC of COMMAND.COM failed\n",stderr); 
}

void exit_cmd(void)        				/* EXIT command */
{   
	exit(0);							/* terminate SHELL */
}

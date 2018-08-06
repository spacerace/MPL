$LIST
c       FOREXEC.INC - interface file for C library routines

c       This file has been included
c       with your FORTRAN 4.0 to show you how easy it is to call routines
c       written in Microsoft C (version 3.0 or higher).
c
c       The Microsoft FORTRAN 4.0, and C 4.0 releases
c       have been designed so that libraries or subprograms can be written
c       and used in either one of these languages.
c
c       Try compiling and running the demonstration program DEMOEXEC.FOR
c       to see some actual examples.

c       C function
c
c               int system(string)
c                       char *string;
c
c       The system() function passes the given C string (which ends with a
c	CHAR(0)) to the DOS command interpreter (COMMAND.COM), which interprets
c	and executes the string as an MS-DOS command.  This allows MS-DOS 
c	commands (i.e., DIR or DEL), batch files, and programs to be executed.
c
c       Example usage in FORTRAN
c
c       integer*2 system                (the return type must be declared)
c       ...
c       i = system('dir *.for'c)        (notice the C literal string '...'c)
c             OR
c	i = system('dir *.for'//char(0))
c
c       The interface to system is given below.  The [c] attribute is given
c       after the function name.  The argument string has the attribute
c       [reference] to indicate that the argument is passed by reference.
c       Normally, arguments are passed to C procedures by value.

        INTERFACE TO FUNCTION SYSTEM[C] 
     +          (STRING)
        INTEGER*2 SYSTEM
        CHARACTER*1 STRING[REFERENCE]
        END


c       C function
c
c       int spawnlp(mode,path,arg0,arg1,...,argn)
c               int mode;               /* spawn mode */
c               char *path;             /* pathname of program to execute */
c               char *arg0;             /* should be the same as path */
c               char *arg1,...,*argn;   /* command line arguments */
c                                       /* argn must be NULL */
c
c       The spawnlp (to be referenced in FORTRAN as spawn) creates and
c       executes a new child process.  There must be enough memory to load
c       and execute the child process.  The mode argument determines which
c       form of spawn is executed. When calling from FORTRAN, the mode 
c	argument must be set to zero.
c
c           Value       Action
c
c             0         Suspend parent program and execute the child program.
c                       When the child program terminates, the parent program
c                       resumes execution.  The return value from spawn is -1
c                       if an error has occurred or if the child process has
c                       run, the return value is the child process'return
c                       code.
c
c       The path argument specifies the file to be executed as the child
c       process.  The path can specify a full path name (from the root
c       directory \), a partial path name (from the current working directory),
c       or just a file name.  If the path argument does not have a filename
c       extension or end with a period (.), the spawn call first appends
c       the extension ".COM" and searches for the file; if unsuccessful, the
c       extension ".EXE" is tried.  The spawn routine will also search for
c       the file in any of the directories specified in the PATH environment
c       variable (using the same procedure as above).
c
c       Example usage in FORTRAN
c
c       integer*2 spawn                 (the return type must be declared)
c       ...
c       i = spawn(0, loc('exemod'c), loc('exemod'c),
c    +          loc('demoexec.exe'c), int4(0))          (execute as a child)
c
c       The interface to _spawnlp is given below.  The [c] attribute is given
c       after the function name.  The [varying] attribute indicates that a
c       variable number of arguments may be given to the function.  The
c       [alias] attribute has to be used because the C name for the function
c       _spawnlp has 8 characters.  By default, names in FORTRAN are only 
c	significant to 6 characters, so we 'alias' the FORTRAN name spawn
c	to the actual C name _spawnlp. 
c	When using the [alias] attribute, remember that the name is passed 
c	EXACTLY as you type it. This means that if you call a C routine using
c	the [alias] attribute, you MUST add an underscore character before
c	the name of the function.

c	Notice in the example above that the C 
c	strings are passed differently than those in the system function.  This 
c	is because the string arguments to spawn are undeclared in the 
c	interface below and assumed to be passed by value.  The C spawnlp 
c	function is expecting the addresses of the strings (not the actual 
c	characters), so we use the LOC() function to pass the address 
c	(remember that functions with the [c] attribute pass arguments by 
c	value).  The last parameter to the spawn routine must be a C NULL 
c	pointer which is a 32-bit integer 0, so we use the INT4(0) function 
c	to pass this number by value as the last parameter.

        interface to function spawn
     +          [c,varying,alias:'_spawnlp']
     +          (mode)
        integer*2 mode,spawn
        end

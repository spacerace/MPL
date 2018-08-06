Companion Disk for ADVANCED MS-DOS PROGRAMMING Second Edition
--------------------------------------------------------------------------

The source files and executable files available on this companion disk
are placed in directories by chapter.  All MASM and C programs have
instructions for assembly or compilation and for linking at the beginning
of each file.  All MASM programs are supplied with appropriate MAKE files.

Before you begin working with this disk, make a backup copy using the
DOS DISKCOPY command.  If you have a question or comment about this disk,
contact Ray Duncan via MCI Mail (user name LMI), CompuServe
(user ID 72406,1577), or BIX (user name rduncan).

If this disk proves defective, contact Consumer Sales, Microsoft Press,
16011 NE 36th Way, Box 97017, Redmond, WA  98073-9717.


Disk Contents
-------------

\CHAP03\    

    HELLO-C             Make file for .COM version of HELLO program
    HELLO-C.ASM         Source file for .COM version of HELLO program
    HELLO-C.COM         Executable .COM file for HELLO program

    HELLO-E             Make file for .EXE version of HELLO program
    HELLO-E.ASM         Source file for .EXE version of HELLO program
    HELLO-E.EXE         Executable .EXE file for HELLO program

\CHAP05\    

    BREAK.ASM           Ctrl-C and Ctrl-Break handler for C programs
    TRYBREAK            Make file for TRYBREAK.EXE
    TRYBREAK.C          Source file for TRYBREAK.EXE
    TRYBREAK.EXE        Executable demo of TRYBREAK.C + BREAK.ASM

    MOUDEMO.C           Source file for MOUDEMO.EXE
    MOUDEMO.EXE         Executable simple mouse demo program

\CHAP07\    

    TALK                Make file for TALK.EXE
    TALK.ASM            Source file for TALK.EXE
    TALK.EXE            Executable simple communications program

\CHAP08\    

    DUMP                Make file for DUMP.EXE  
    DUMP.ASM            Source file for DUMP.EXE
    DUMP.EXE            Executable file-dump utility

    DUMP.C              C-language version of file-dump utility

    INT24.ASM           Critical-error handler
    TRYINT24            Make file for TRYINT24.EXE
    TRYINT24.ASM        Source file for TRYINT24.EXE
    TRYINT24.EXE        Executable demo of TRYINT24.ASM + INT24.ASM

\CHAP11\    

    EXTMEM              Make file for EXTMEM.EXE
    EXTMEM.ASM          Source file for EXTMEM.EXE
    EXTMEM.EXE          Executable demo of extended memory usage
                        via Int 15H Function 87H

\CHAP12\    

    SHELL               Make file for SHELL.EXE 
    SHELL.ASM           Source file for SHELL.EXE
    SHELL.EXE           Executable simple user shell

    SHELL.C             C-language version of simple user shell

\CHAP13\    

    ZERODIV.ASM         Source file for ZERODIV.COM
    ZERODIV.COM         Executable divide-by-zero interrupt handler

    TEST0DIV            Make file for TEST0DIV.COM and ZERODIV.COM
    TEST0DIV.ASM        Source file for TEST0DIV.COM
    TEST0DIV.COM        Executable demo program that forces
                        divide-by-zero faults (load ZERODIV.COM first)

\CHAP14\    

    DRIVER              Make file for DRIVER.SYS        
    DRIVER.ASM          Source file for DRIVER.SYS
    DRIVER.SYS          Executable simple device-driver template

\CHAP15\    

    PROTO.ASM           Source file for MASM filter prototype
    PROTO.C             Source file for C filter prototype

    CLEAN               Make file for CLEAN.EXE
    CLEAN.ASM           Source file for CLEAN.EXE
    CLEAN.EXE           Executable document file filter

    CLEAN.C             C-language version of document file filter

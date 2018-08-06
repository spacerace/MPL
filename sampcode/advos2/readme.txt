Companion Disk for ADVANCED OS/2 PROGRAMMING
--------------------------------------------------------------------------

The source files and executable files available on this companion disk
are placed in directories by chapter.  All MASM and C programs have
instructions for assembly or compilation and for linking at the beginning
of each file.  All MASM programs are supplied with appropriate MAKE files.

Programs marked with an asterisk (*) are not published in the book or
are in the book only in fragmentary form.

Before you begin working with this disk, make a backup copy using the
DOS DISKCOPY command.  If you have a question or comment about this disk,
contact Ray Duncan via MCI Mail (user name LMI), CompuServe
(user ID 72406,1577), or BIX (user name rduncan).

If this disk proves defective, contact Consumer Sales, Microsoft Press,
16011 NE 36th Way, Box 97017, Redmond, WA  98073-9717.


Disk Contents
-------------

Directory \CH03\    

    HELLO.ASM           MASM source file for HELLO.EXE, a
                        trivial OS/2 application program that
                        displays "Hello World!" on the standard
                        output and then terminates.
    HELLO.DEF           Module definition file for HELLO.EXE.
    HELLO               Make file for HELLO.EXE.

    ARGC.ASM            MASM subroutine to return the number of
                        command line arguments.  Illustrates
                        use of DosGetEnv.
    ARGV.ASM            MASM subroutine to return address and
                        length of specific command line argument.
                        Illustrates use of DosGetEnv.

    SHOWARGS.ASM      * MASM source code for SHOWARGS.EXE, a simple 
                        MASM application which uses ARGC and ARGV
                        to display its command line arguments.
    SHOWARGS.DEF      * Module definition file for SHOWARGS.EXE.
    SHOWARGS          * Make file for SHOWARGS.EXE.

    GETENV.ASM        * MASM subroutine to find environment variable
                        and return the address and length of the
                        associated "value" string.  Illustrates
                        use of DosGetEnv.
    GETENV2.ASM       * Alternative version of GETENV subroutine
                        which uses API function DosScanEnv, shorter
                        but slower.

    SHOWENV.ASM       * MASM source code for SHOWENV.EXE, an 
                        interactive demonstration program that
                        prompts the user for the name of an environment
                        variable and then displays the associated
                        "value" string.
    SHOWENV.DEF       * Module definition file for SHOWENV.EXE.
    SHOWENV           * Make file for SHOWENV.EXE.

Directory \CH05\    

    MOUDEMO.C           C source file for MOUDEMO.EXE a simple C
                        demonstration of the Mouse API that tracks and
                        displays the mouse position.

Directory \CH07\    

    SHOWCOM.C         * Simple C demonstration of DosDevIOCtl
                        functions for COM port control; obtains
                        and displays the current COM port baud
                        rate, word length, parity, etc.

Directory \CH08\    

    DUMP.C              Source code for C version of DUMP.EXE,
                        a utility program that displays the contents
                        of a file in hex and ASCII on the standard
                        output.

    DUMP.ASM            Source code for MASM version of DUMP.EXE.
    DUMP.DEF            Module definition file for MASM version
                        of DUMP.EXE.
    DUMP                Make file for MASM version of DUMP.EXE.
    ARGC.ASM            ARGC subroutine used by MASM version of DUMP.
    ARGV.ASM            ARGV subroutine used by MASM version of DUMP.

Directory \CH09\    
                                                                                
    SCHEX.ASM         * MASM source code for SCHEX.EXE, a simple
                        demonstration of the use of DosFindFirst and
                        DosFindNext.  Finds and displays all files
                        with the extension .ASM in the current directory.
    SCHEX.DEF         * Module definition file for SCHEX.EXE.
    SCHEX             * Make file for SCHEX.EXE.
        
    VOLNAME.ASM       * MASM source code for VOLNAME.EXE, a simple
                        demonstration of the use of DosQFSInfo.
    VOLNAME.DEF       * Module definition file for VOLNAME.EXE.
    VOLNAME           * Make file for VOLNAME.EXE.

    WHEREIS.C           Source code for C version of WHEREIS.EXE, a
                        utility program to find and display all files
                        matching the command line parameter (which may
                        include wildcards).

    WHEREIS.ASM         Source code for MASM version of WHEREIS.EXE.
    WHEREIS.DEF         Module definition file for MASM version 
                        of WHEREIS.EXE.
    WHEREIS             Make file for MASM version of WHEREIS.EXE.
    ARGC.ASM            ARGC subroutine used by MASM version of WHEREIS.
    ARGV.ASM            ARGV subroutine used by MASM version of WHEREIS.

    QFN.C             * C subroutine to fully qualify a filename.
                        Illustrates use of DosChDir, DosSelectDisk,
                        DosQCurDisk, DosQCurDir.

    TRYQFN.C          * C source code for TRYQFN.EXE, a demonstration 
                        program which illustrates use of QFN.C.  

    QFN.ASM           * MASM subroutine to fully qualify a filename.
                        Illustrates use of DosChDir, DosSelectDisk,
                        DosQCurDisk, DosQCurDir.

    TRYQFN.ASM        * MASM source code for TRYQFN.EXE, a demonstration
                        program which illustrates use of QFN.ASM.     
    TRYQFN.DEF        * Module definition file for TRYQFN.EXE.
    TRYQFN            * Make file for MASM version of TRYQFN.EXE.

Directory \CH10\    

    DASD.ASM          * MASM source code for DASD.EXE, a simple 
                        demonstration of the use of direct disk access
                        in a well-behaved OS/2 application.  Reads
                        the boot sector for the current drive and then 
                        writes it into a file named BOOT.BIN.
    DASD.DEF          * Module definition file for DASD.EXE.
    DASD              * Make file for DASD.EXE.

Directory \CH12\    

    BEEPER.ASM        * MASM source code for BEEPER.EXE, a simple
                        demonstration of a multithreaded application.
                        The main thread starts up a secondary thread
                        which sounds a tone at 1-second intervals; the
                        primary thread waits until any key is pressed
                        and then terminates the program.
    BEEPER.DEF        * Module definition file for BEEPER.EXE.
    BEEPER            * Make file for BEEPER.EXE.

    EXEC1.ASM         * MASM source code for EXEC1.EXE, demonstrating
                        the use of DosExecPgm to run CHKDSK as a
                        synchronous child process.
    EXEC1.DEF         * Module definition file for EXEC1.EXE.
    EXEC1             * Make file for EXEC1.EXE.        

    EXEC2.ASM         * MASM source code for EXEC2.EXE, demonstrating
                        the use of DosExecPgm and DosCwait to run 
                        CHKDSK as an asynchronous child process.
    EXEC2.DEF         * Module definition file for EXEC2.EXE.
    EXEC2             * Make file for EXEC2.EXE.        

    EXEC3.ASM         * MASM source code for EXEC3.EXE, a simple 
                        demonstration of DosStartSession.  Launches
                        CMD.EXE to display a disk directory listing
                        in a background, related child session.
    EXEC3.DEF         * Module definition file for EXEC3.EXE.
    EXEC3             * Make file for EXEC3.EXE.

    DUMBTERM.C          Source code for C version of DUMBTERM.EXE,
                        a simple terminal emulator program that
                        demonstrates the use of multiple threads.

    DUMBTERM.ASM        Source code for MASM version of DUMBTERM.EXE.
    DUMBTERM.DEF        Module definition file for MASM version
                        of DUMBTERM.EXE.
    DUMBTERM            Make file for MASM version of DUMBTERM.EXE.

    TINYCMD.C           Source code for C version of TINYCMD.EXE, a
                        simple table-driven command line interpreter.

    TINYCMD.ASM         Source code for MASM version of TINYCMD.EXE.
    TINYCMD.DEF         Module definition file for MASM version
                        of TINYCMD.EXE.
    TINYCMD             Make file for MASM version of TINYCMD.EXE.

Directory \CH13\    

    SIGNAL.C          * Source code for C version of SIGNAL.EXE, a 
                        simple demonstration of signal handling.
                        Registers a signal handler for Ctrl-C signals
                        and then sleeps for 10 seconds; when Ctrl-C
                        is detected, the handler displays a message.

    SIGNAL.ASM        * Source code for MASM version of SIGNAL.EXE.
    SIGNAL.DEF        * Module definition file for MASM version
                        of SIGNAL.EXE.
    SIGNAL            * Make file for MASM version of SIGNAL.EXE.

Directory \CH14\C\  

    PORTS.C             Source code for C version of PORTS.EXE,
                        a simple demonstration program that reads
                        and displays ports 00H-0FFH.
    PORTS.DEF           Module definition file for C version
                        of PORTS.EXE.
    PORTS               Make file for C version of PORTS.EXE.
    PORTIO.ASM          MASM source code for IOPL segment used by
                        PORTS.EXE.  Contains RPORT and WPORT routines 
                        to read and write I/O ports.

Directory \CH14\MASM\    

    PORTS.ASM           Source code for MASM version of PORTS.EXE,
                        a simple demonstration program that reads
                        and displays ports 00H-0FFH.
    PORTS.DEF           Module definition file for MASM version
                        of PORTS.EXE.
    PORTS               Make file for MASM version of PORTS.EXE.
    PORTIO.ASM          MASM source code for IOPL segment used by
                        PORTS.EXE.  Contains RPORT and WPORT routines 
                        to read and write I/O ports.

Directory \CH15\   

    CLOCK.ASM         * MASM source code for CLOCK.EXE, a simple
                        demonstration of the use of semaphores and
                        periodic timers.  Displays the time at
                        1-second intervals in the upper right
                        corner of the screen.
    CLOCK.DEF         * Module definition file for CLOCK.EXE.
    CLOCK             * Make file for CLOCK.EXE.

    TOUCH.C           * C source code for TOUCH.EXE, a utility
                        program to stamp files with time and date.
                        Illustrates use of DosGetDateTime,
                        DosFindFirst, DosFindNext, DosSetFileInfo.

Directory \CH16\   

    PROTO.C             Source code for C version of PROTO.EXE, a
                        prototype filter program.

    PROTO.ASM           Source file for MASM version of PROTO.EXE,
                        a prototype filter program.
    PROTO.DEF           Module definition file for MASM version
                        of PROTO.EXE.
    PROTO               Make file for MASM version of PROTO.EXE.

    LC.C              * Source code for C version of LC.EXE, a
                        filter to translate uppercase letters
                        to lowercase.

    LC.ASM            * Source code for MASM version of LC.EXE.
    LC.DEF            * Module definition file for MASM version
                        of LC.EXE.
    LC                * Make file for MASM version of LC.EXE.

    CLEAN.C           * C source code for CLEAN.EXE, a simple
                        filter which removes control characters
                        except for linefeeds and carriage returns,
                        and expands tabs to spaces.

    FIND.C              C source code for FIND.EXE, a simple
                        filter which searches text streams for a
                        pattern and sends all lines containing
                        a match for the pattern to the standard
                        output.

    EXECSORT.ASM        MASM source code for EXECSORT.EXE, a simple
                        demonstration of the use of a filter as
                        a child process.  EXECSORT runs SORT.EXE
                        as a child to sort the contents of MYFILE.DAT
                        and sends the output to MYFILE.SRT.
    EXECSORT.DEF        Module definition file for EXECSORT.EXE.
    EXECSORT            Make file for EXECSORT.EXE.

    MYFILE.DAT          Sample data file for EXECSORT.EXE and SORT.EXE.

Directory \CH17\   

    TEMPLATE.ASM        MASM source code for TEMPLATE.SYS, a skeleton
                        OS/2 device driver.  Does nothing (except
                        initialize and display an error message), but
                        also does no harm.
    TEMPLATE.DEF        Module definition file for TEMPLATE.SYS.
    TEMPLATE            Make file for TEMPLATE.SYS.

    TINYDISK.ASM        MASM source code for TINYDISK.SYS, a simple
                        OS/2 RAM Disk device driver.  Illustrates
                        bimodal operation and proper use of PhysToVirt 
                        and VirtToPhys.
    TINYDISK.DEF        Module definition file for TINYDISK.SYS.
    TINYDISK            Make file for TINYDISK.SYS.

Directory \CH18\   

    STAMPER.ASM       * MASM source code for STAMPER.EXE, a simple
                        device monitor program which inserts ASCII
                        date or time into keyboard data stream on
                        command.  Monitors the keyboard for Alt-D, 
                        Alt-T, and Alt-X hot keys.
    STAMPER.DEF       * Module definition file for STAMPER.EXE.
    STAMPER           * Make file for STAMPER.EXE.

    SNAP.C              Source code for C version of SNAP.EXE, a more
                        complicated device monitor example.  Monitors
                        keyboard data stream for hot key and copies
                        current screen contents into a file on command.
                        Illustrates use of multiple threads, file I/O, 
                        video pop-ups, system semaphores, spawning of
                        child processes, etc.

    SNAP.ASM            Source code for MASM version of SNAP.EXE.
    SNAP.DEF            Module definition file for MASM version
                        of SNAP.EXE.
    SNAP                Make file for MASM version of SNAP.EXE. 

Directory \CH19\   

    ASMHELP.ASM         MASM source code for ASMHELP.DLL, a sample
                        DLL that supplies MASM applications with
                        command line scanning services.
    ASMHELP.DEF         Module definition file for ASMHELP.DLL.
    ASMHELP             Make file for ASMHELP.DLL.

    SHOWARGS.ASM        MASM source code for SHOWARGS.EXE, a 
                        demonstration program that dynlinks to,
                        and uses the services of, the sample
                        dynlink library ASMHELP.DLL.
    SHOWARGS.DEF        Module definition file for SHOWARGS.EXE.
    SHOWARGS            Make file for SHOWARGS.EXE.

    CDLL.C              C source code for body of CDLL.DLL, a
                        a simple sample dynlink library.
    CDLL.DEF            Module definition file for CDLL.DLL.
    CDLL.LIB            Import library for CDLL.DLL.
    CDLL                Make file for CDLL.DLL.
    CINIT.ASM           MASM source code for initialization entry
                        point of CDLL.DLL.

    TESTCDLL.C        * C source code for a demonstration program
                        that dynlinks to, and uses the services
                        of, the sample dynlink library CDLL.DLL.

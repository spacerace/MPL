ECHO OFF
CLS
REM This batch file builds a version of SHOW that can be run from
REM either DOS or OS/2. OS/2 programs normally contain a short DOS
REM program (called a stub) which displays an error message when the
REM program is run from DOS. The stub version of SHOW works by
REM replacing the normal stub program with a copy of the entire DOS
REM version of SHOW. For small programs, this results in a much
REM smaller .EXE file than a bound program.
REM
REM This is a batch file rather than a make file because a stub
REM program should not contain debug information. You normally build
REM it once after the program is debugged. First assemble and link
REM the real mode version of show.
REM
ECHO ml showr.asm showutil.asm pagerr.asm
     ml showr.asm showutil.asm pagerr.asm
IF ERRORLEVEL 1 GOTO Exit
REM
REM Next assemble the protect mode files.
REM
ECHO ml /c showp.asm showutil.asm pagerp.asm
     ml /c showp.asm showutil.asm pagerp.asm
IF ERRORLEVEL 1 GOTO Exit
REM
REM Finally, link the protect mode object files. The definition file
REM SHOW.DEF contains the command to replace the normal stub file
REM with SHOWR.EXE:
REM
ECHO link showp.obj showutil.obj pagerp.obj,show.exe,,,show.def
     link showp.obj showutil.obj pagerp.obj,show.exe,,,show.def
:Exit

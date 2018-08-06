REM	makefile for glob routines by John Plocher
REM	glob is a routine which expands DOS style wildcards into an argv array

REM	Microsoft C 3.0 and 4.0		-Must pack structures in glob.c-

cl -O -c -Zp glob.c
cl -O chd.c glob -o chd


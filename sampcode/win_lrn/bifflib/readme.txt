
This directory contains examples which read or write Microsoft
Excel's Binary File Format (BIFF).  'DUMPER.EXE' is a program which
displays all the BIFF records of a given type.  'FINDER.EXE' allows
the user to find the contents of some cell in a stored worksheet.
'EXAMPLE.EXE' creates a BIFF file, storing data contained in an array
of structures.  All of these programs utilize 'LIB.C', which is a
library of routines which allow the developer to easily access a BIFF
file.  These routines are designed to aid the developer who needs to 
utilize Microsoft Excel's Binary File Format.
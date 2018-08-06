@echo off
cls
echo !This batch file compiles and links the example test routine for the
echo !	INT 13  Interface.
echo !
echo !If you DO NOT want to proceed, hit CTRL BREAK.
pause
cl /AS /Zp /c  rw13.c
cl /AS /Zp     test.c rw13.obj 
echo !Compile and link complete.   Type TEST to execute.

